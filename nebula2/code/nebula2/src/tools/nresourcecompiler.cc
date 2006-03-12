//------------------------------------------------------------------------------
//  nresourcecompiler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nresourcecompiler.h"
#include "kernel/nfileserver2.h"
#include "scene/nshapenode.h"
#include "scene/nskinanimator.h"
#include "kernel/npersistserver.h"
#include "tools/nmeshbuilder.h"
#include "tools/nanimbuilder.h"
#include "tools/napplauncher.h"

//------------------------------------------------------------------------------
/**
*/
nResourceCompiler::nResourceCompiler() :
    kernelServer(0),
    refResourceServer("/sys/servers/resource"),
    isOpen(false),
    rootNode(0),
    shapeNodeClass(0),
    shaderNodeClass(0),
    skinAnimatorClass(0),
    binaryFlag(false),
    dataFile(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nResourceCompiler::~nResourceCompiler()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nResourceCompiler::Open(nKernelServer* ks)
{
    n_assert(!this->isOpen);
    n_assert(0 != ks);
    n_assert(0 == this->kernelServer);
    n_assert(this->meshes.Empty());
    n_assert(this->textures.Empty());
    n_assert(this->anims.Empty());
    n_assert(0 == this->rootNode);
    n_assert(0 == this->dataFile);

    // initialize Nebula runtime
    this->kernelServer = ks;
    
    // initialize proj assign
    nFileServer2* fileServer = kernelServer->GetFileServer();
    fileServer->SetAssign("proj", this->GetPath(ProjectDirectory));

    // create resource root node
    this->rootNode = kernelServer->New("nroot", "/rsrc");

    // lookup Nebula classes
    this->shaderNodeClass   = kernelServer->FindClass("nabstractshadernode");
    this->shapeNodeClass    = kernelServer->FindClass("nshapenode");
    this->skinAnimatorClass = kernelServer->FindClass("nskinanimator");
    n_assert(this->shaderNodeClass);
    n_assert(this->shapeNodeClass);
    n_assert(this->skinAnimatorClass);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nResourceCompiler::Close()
{
    n_assert(this->isOpen);
    n_assert(this->rootNode);

    if (this->dataFile)
    {
        this->dataFile->Release();
        this->dataFile = 0;
    }

    this->meshes.Clear();
    this->textures.Clear();
    this->anims.Clear();
    this->rootNode->Release();
    this->rootNode = 0;
    this->kernelServer = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nResourceCompiler::Compile(const nArray<nString>& objs)
{
    n_assert(this->isOpen);
    nFileServer2* fileServer = kernelServer->GetFileServer();

    // open temporary data file
    this->dataFilePath = this->GetPath(ScratchDirectory);
    this->dataFilePath.Append("/nrb_data.tmp");
    this->dataFile = fileServer->NewFileObject();
    if (!this->dataFile->Open(this->dataFilePath.Get(), "wb"))
    {
        this->SetError("nResourceCompiler::Open(): could not open tmp file '%s'\n", this->dataFilePath.Get());
        this->Close();
        return false;
    }

    // step 1: load Nebula objects
    n_printf("\n* LOADING OBJECTS:\n");
    n_printf("==================\n");
    if (!this->LoadObjects(objs))
    {
        return false;
    }

    // step 2: extract resource names from Nebula objects
    this->ExtractResourceAttrs(this->rootNode);

    // step 3: process meshes
    n_printf("\n* PROCESSING MESHES:\n");
    n_printf("====================\n");
    if (!this->ProcessMeshes())
    {
        return false;
    }

    // step 4: process anims
    n_printf("\n* PROCESSING ANIMS:\n");
    n_printf("===================\n");
    if (!this->ProcessAnims())
    {
        return false;
    }

    // step 5: process textures
    n_printf("\n* PROCESSING TEXTURES:\n");
    n_printf("======================\n");
    if (!this->ProcessTextures())
    {
        return false;
    }
    
    // close temporary data file
    this->dataFile->Close();

    // step 6: save Nebula objects
    if (!this->SaveObjectHierarchy())
    {
        this->SetError("Error: Failed to save Nebula2 object to %s.n2\n", this->GetPath(BaseFilename));
        return false;
    }

    // step 7: save resource bundle
    n_printf("\n* Creating resource file '%s.nrb'...\n\n", this->GetPath(BaseFilename));
    if (!this->SaveResourceBundle())
    {
        this->SetError("Error: Failed to write resource bundle file to %s.nrb\n", this->GetPath(BaseFilename));
        return false;
    }

    // delete tmp files
    fileServer->DeleteFile(this->dataFilePath.Get());

    return true;
}

//------------------------------------------------------------------------------
/**
*/
int
nResourceCompiler::GetMeshDataSize() const
{
    int size = 0;
    int i;
    for (i = 0; i < this->meshes.Size(); i++)
    {
        size += this->meshes[i].GetDataLength();
    }
    return size;
}

//------------------------------------------------------------------------------
/**
*/
int
nResourceCompiler::GetAnimDataSize() const
{
    int size = 0;
    int i;
    for (i = 0; i < this->anims.Size(); i++)
    {
        size += this->anims[i].GetDataLength();
    }
    return size;
}

//------------------------------------------------------------------------------
/**
*/
int
nResourceCompiler::GetTextureDataSize() const
{
    int size = 0;
    int i;
    for (i = 0; i < this->textures.Size(); i++)
    {
        size += this->textures[i].GetDataLength();
    }
    return size;
}

//------------------------------------------------------------------------------
/**
    This loads all Nebula objects.
*/
bool
nResourceCompiler::LoadObjects(const nArray<nString>& objs)
{
    n_assert(this->isOpen);
    n_assert(this->kernelServer);
    n_assert(this->rootNode);

    kernelServer->PushCwd(this->rootNode);
    int i;
    int num = objs.Size();
    for (i = 0; i < num; i++)
    {
        n_printf("-> loading '%s'\n", objs[i].Get());
        if (0 == kernelServer->Load(objs[i].Get()))
        {
            this->SetError("ERROR: Failed to load object '%s'\n", objs[i].Get());
            return false;
        }
    }
    kernelServer->PopCwd();
    return true;
}

//------------------------------------------------------------------------------
/**
    Recursively extract resource attributes from Nebula2 object hierarchy.
*/
void
nResourceCompiler::ExtractResourceAttrs(nRoot* curNode)
{
    // meshes
    if (curNode->IsA(this->shapeNodeClass))
    {
        nShapeNode* shapeNode = (nShapeNode*) curNode;
        ResEntry entry(nResource::Mesh, shapeNode->GetMesh());
        entry.SetFlags(shapeNode->GetMeshUsage());
        if (!this->meshes.Find(entry))
        {
            this->meshes.Append(entry);
        }
    }

    // textures
    if (curNode->IsA(this->shaderNodeClass))
    {
        nAbstractShaderNode* shdNode = (nAbstractShaderNode*) curNode;
        int num = shdNode->GetNumTextures();
        int i;
        for (i = 0; i < num; i++)
        {
            ResEntry entry(nResource::Texture, shdNode->GetTextureAt(i));
            if (!this->textures.Find(entry))
            {
                this->textures.Append(entry);
            }
        }
    }

    // animations
    if (curNode->IsA(this->skinAnimatorClass))
    {
        nSkinAnimator* skinAnimator = (nSkinAnimator*) curNode;
        ResEntry entry(nResource::Animation, skinAnimator->GetAnim().Get());
        if (!this->anims.Find(entry))
        {
            this->anims.Append(entry);
        }
    }

    // recurse
    nRoot* curChild;
    for (curChild = curNode->GetHead(); curChild; curChild = curChild->GetSucc())
    {
        this->ExtractResourceAttrs(curChild);
    }
}

//------------------------------------------------------------------------------
/**
    Save the Nebula object hierarchy.
*/
bool
nResourceCompiler::SaveObjectHierarchy()
{
    nPersistServer* persistServer = kernelServer->GetPersistServer();
    if (this->GetBinaryFlag())
    {
        persistServer->SetSaverClass("nbinscriptserver");
    }
    else
    {
        persistServer->SetSaverClass("ntclserver");
    }
    return this->rootNode->SaveAs(this->GetPath(BaseFilename));
}

//------------------------------------------------------------------------------
/**
    Load each mesh, append it as binary nvx2 file to the temporary 
    data file, and record its start offset and length.
    FIXME: small meshes should be combined into optimally sized
    meshes.
*/
bool
nResourceCompiler::ProcessMeshes()
{
    n_assert(this->dataFile);
    n_assert(this->kernelServer);

    int i;
    int num = this->meshes.Size();
    for (i = 0; i < num; i++)
    {
        ResEntry& resEntry = this->meshes[i];
        nMeshBuilder meshBuilder;
        const char* name = resEntry.GetName();
        n_assert(name);

        n_printf("-> %s\n", name);

        // load mesh
        if (!meshBuilder.Load(kernelServer->GetFileServer(), name))
        {
            this->SetError("Error: Failed to load mesh '%s'\n", name);
            return false;
        }

        // record start offset in temp data file
        resEntry.SetDataOffset(this->dataFile->GetSize());
        
        // append mesh data to temp data file
        if (!meshBuilder.SaveNvx2(this->dataFile))
        {
            this->SetError("Error: Writing data of mesh '%s' failed!\n", resEntry.GetName());
            return false;
        }

        // record length of mesh data
        resEntry.SetDataLength(this->dataFile->GetSize() - resEntry.GetDataOffset());
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load each animation, and append it to the temporary data file.
*/
bool
nResourceCompiler::ProcessAnims()
{
    n_assert(this->dataFile);
    n_assert(this->kernelServer);

    int i;
    int num = this->anims.Size();
    for (i = 0; i < num; i++)
    {
        ResEntry& resEntry = this->anims[i];
        nAnimBuilder animBuilder;
        const char* name = resEntry.GetName();
        n_assert(name);

        n_printf("-> %s\n", name);

        // load anim
        if (!animBuilder.Load(kernelServer->GetFileServer(), name))
        {
            this->SetError("Error: Failed to load anim '%s'\n", name);
            return false;
        }

        // record start offset in temp data file
        resEntry.SetDataOffset(this->dataFile->GetSize());

        // append anim data to temp data file
        if (!animBuilder.SaveNax2(this->dataFile))
        {
            this->SetError("Error: Writing data of animation '%s' failed!\n", resEntry.GetName());
            return false;
        }

        // record length of anim data
        resEntry.SetDataLength(this->dataFile->GetSize() - resEntry.GetDataOffset());
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Append the texture file (assumed to be dds) to the data file and
    record its start offset and length.
*/
bool
nResourceCompiler::ProcessTextures()
{
    n_assert(this->kernelServer);
    nFileServer2* fileServer = this->kernelServer->GetFileServer();

    int i;
    int num = this->textures.Size();
    for (i = 0; i < num; i++)
    {
        ResEntry& resEntry = this->textures[i];
        const char* name = resEntry.GetName();
        n_assert(name);

        n_printf("-> %s\n", name);

        // record start offset in temp data file
        resEntry.SetDataOffset(this->dataFile->GetSize());

        // append image file to temp data file
        nFile* srcFile = fileServer->NewFileObject();
        if (srcFile->Open(name, "rb"))
        {
            this->dataFile->AppendFile(srcFile);
            srcFile->Close();
        }
        else
        {
            this->SetError("Error: Failed to file '%s'!\n", name);
            srcFile->Release();
            return false;
        }
        srcFile->Release();

        // record length of data
        resEntry.SetDataLength(this->dataFile->GetSize() - resEntry.GetDataOffset());
    }        
    return true;
}

//------------------------------------------------------------------------------
/**
    Write a single resource entry to a file.
*/
bool
nResourceCompiler::WriteResEntry(nFile* file, const ResEntry& resEntry, int dataBlockOffset)
{
    n_assert(file);

    // write type
    switch (resEntry.GetType())
    {
        case nResource::Mesh:
            file->PutInt('MESH');
            break;

        case nResource::Animation:
            file->PutInt('MANI');       // a memory-animation vs. streamed animation
            break;

        case nResource::Texture:
            file->PutInt('TXTR');
            break;

        default:
            n_error("nResourceCompiler::WriteResEntry(): Unsupported resource type!");
            break;
    }

    // convert filename to resource identifier
    nString resId = this->refResourceServer->GetResourceId(resEntry.GetName());

    // write resource identifier
    file->Write(resId.Get(), resId.Length());

    // write data offset and data length
    file->PutInt(resEntry.GetDataOffset() + dataBlockOffset);
    file->PutInt(resEntry.GetDataLength());
    file->PutInt(resEntry.GetFlags());
    return true;
}

//------------------------------------------------------------------------------
/**
    Write the resource bundle file consisting of header and data block.
*/
bool
nResourceCompiler::SaveResourceBundle()
{
    n_assert(this->isOpen);
    nFileServer2* fileServer = kernelServer->GetFileServer();

    nFile* dstFile = fileServer->NewFileObject();

    // open destination file
    nString dstFileName = this->GetPath(BaseFilename);
    dstFileName.Append(".nrb");
    if (!dstFile->Open(dstFileName.Get(), "wb"))
    {
        dstFile->Release();
        return false;
    }

    // write header
    int numTocEntries = this->meshes.Size() + this->anims.Size() + this->textures.Size();
    dstFile->PutInt('NRB0');
    dstFile->PutInt(numTocEntries);

    // compute offset of datablock (header + numTocEntries * sizeof(tocEntry))
    int dataOffset = 2 * sizeof(int) + numTocEntries * (3 * sizeof(int) + 32);

    // write toc entries
    int i;
    for (i = 0; i < this->meshes.Size(); i++)
    {
        this->WriteResEntry(dstFile, this->meshes[i], dataOffset);
    }
    for (i = 0; i < this->anims.Size(); i++)
    {
        this->WriteResEntry(dstFile, this->anims[i], dataOffset);
    }
    for (i = 0; i < this->textures.Size(); i++)
    {
        this->WriteResEntry(dstFile, this->textures[i], dataOffset);
    }

    // append the data block to the nrb file
    bool success = this->dataFile->Open(this->dataFilePath.Get(), "rb");
    n_assert(success);
    dstFile->AppendFile(this->dataFile);
    this->dataFile->Close();
    
    // cleanup
    dstFile->Close();
    dstFile->Release();
    return true;
}
