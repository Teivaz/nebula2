//------------------------------------------------------------------------------
//  nexportanalyzer.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "tools/nexportanalyzer.h"
#include "kernel/nfileserver2.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ntexture2.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"
#include "scene/nscenenode.h"
#include "scene/nabstractshadernode.h"
#include "sql/nsqlserver.h"
#include "sql/nsqldatabase.h"
#include "sql/nsqlquery.h"
#include <time.h>

//------------------------------------------------------------------------------
/**
*/
nExportAnalyzer::nExportAnalyzer() :
    textures(1024, 1024),
    meshes(1024, 1024),
    anims(1024, 1024),
    objects(1024, 1024),
    levels(100, 100)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nExportAnalyzer::~nExportAnalyzer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::Run()
{
    bool success = true;

    // get a date-and-time string (argh...)
    time_t ltime;
    time(&ltime);
    char buf[256];
#if defined(_MSC_VER) && (_MSC_VER > 1400)
    struct tm now;
    _localtime64_s(&now, &ltime);
    strftime(buf, sizeof(dateAndTime), "%Y-%m-%d_%H-%M-%S", &now);
#else
    struct tm * now = localtime(&ltime);
    strftime(buf, sizeof(dateAndTime), "%Y-%m-%d_%H-%M-%S", now);
#endif

    // initialize time stamp, target directory
    this->dateAndTime = buf;
    this->rootDir = "proj:statistics/";
    this->rootDir.Append(this->dateAndTime);
    nFileServer2::Instance()->MakePath(this->rootDir);

    // read data
    success &= this->ParseTextures();
    success &= this->ParseMeshes();
    success &= this->ParseAnims();
    success &= this->ParseObjects();
    success &= this->ParseLevels();

    // write analysis files as comma-separated-files
    this->WriteTextureInfo();
    this->WriteMeshInfo();
    this->WriteAnimInfo();
    this->WriteObjectInfo();
    this->WriteLevelInfo();
    this->WriteSummary();

    return success;
}

//------------------------------------------------------------------------------
/**
    Recursively count the number of objects in a Nebula hierarchy which
    match a given class.
*/
int
nExportAnalyzer::RecurseCountByClass(nClass* cl, nRoot* root, int count)
{
    n_assert(cl);
    n_assert(root);

    if (root->IsA(cl))
    {
        count++;
    }
    nRoot* cur;
    for (cur = root->GetHead(); cur; cur = cur->GetSucc())
    {
        count = this->RecurseCountByClass(cl, cur, count);
    }
    return count;
}

//------------------------------------------------------------------------------
/**
    Recursively scans a Nebula2 object hierarchy for nAbstractShaderNodes,
    and checks if the used textures exist. Returns an array of the filenames
    of missing textures.
*/
bool
nExportAnalyzer::RecurseCheckTexturesExist(nRoot* root, nArray<nString>& failedArray)
{
    n_assert(root);

    // check if root is an abstract shader node, if yes, check its textures
    bool retval = true;
    if (root->IsA("nabstractshadernode"))
    {
        nAbstractShaderNode* abstractShaderNode = (nAbstractShaderNode*) root;
        int i;
        int num = abstractShaderNode->GetNumTextures();
        for (i = 0; i < num; i++)
        {
            nString texName = abstractShaderNode->GetTextureAt(i);
            if (!nFileServer2::Instance()->FileExists(texName))
            {
                failedArray.Append(texName);
                retval = false;
            }
        }
    }

    // recurse
    nRoot* cur;
    for (cur = root->GetHead(); cur; cur = cur->GetSucc())
    {
        bool success = this->RecurseCheckTexturesExist(cur, failedArray);
        if (!success)
        {
            retval = false;
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Recursively list all files matching pattern.
*/
nArray<nString>
nExportAnalyzer::ListFiles(const nString& dir, const nString& pattern, nArray<nString>* gatheredFiles)
{
    nFileServer2* fileServer = nFileServer2::Instance();

    // first, list files
    nArray<nString> files = fileServer->ListMatchingFiles(dir, pattern);
    if (gatheredFiles)
    {
        gatheredFiles->AppendArray(files);
    }
    else
    {
        gatheredFiles = &files;
    }

    // then list directories and recurse
    nArray<nString> dirs = fileServer->ListDirectories(dir);
    int dirIndex;
    for (dirIndex = 0; dirIndex < dirs.Size(); dirIndex++)
    {
        if (dirs[dirIndex] != "CVS")
        {
            this->ListFiles(dirs[dirIndex], pattern, gatheredFiles);
        }
    }
    return files;
}

//------------------------------------------------------------------------------
/**
    Takes a full path and returns the category/filename string.
*/
nString
nExportAnalyzer::ExtractCategoryFilename(const nString& path)
{
    nString fname = path.ExtractFileName();
    fname.StripExtension();
    nString category = path.ExtractLastDirName();
    nString name;
    name.Format("%s/%s", category.Get(), fname.Get());
    return name;
}

//------------------------------------------------------------------------------
/**
    Find texture info by name, check against existing array of indices.
*/
int
nExportAnalyzer::FindUniqueTextureInfo(const nString& name, const nArray<int>& indices)
{
    int i;
    for (i = 0; i < this->textures.Size(); i++)
    {
        if ((name == this->textures[i].name) && (-1 == indices.FindIndex(i)))
        {
            return i;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find unique mesh info index by name.
*/
int
nExportAnalyzer::FindUniqueMeshInfo(const nString& name, const nArray<int>& indices)
{
    int i;
    for (i = 0; i < this->meshes.Size(); i++)
    {
        if ((name == this->meshes[i].name) && (-1 == indices.FindIndex(i)))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Find unique anim info index by name.
*/
int
nExportAnalyzer::FindUniqueAnimInfo(const nString& name, const nArray<int>& indices)
{
    int i;
    for (i = 0; i < this->anims.Size(); i++)
    {
        if ((name == this->anims[i].name) && (-1 == indices.FindIndex(i)))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Find unique object info index by name.
*/
int
nExportAnalyzer::FindUniqueObjectInfo(const nString& name, const nArray<int>& indices)
{
    int i;
    for (i = 0; i < this->objects.Size(); i++)
    {
        if ((name == this->objects[i].name) && (-1 == indices.FindIndex(i)))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Returns a concatenated string of all texture names.
*/
nString
nExportAnalyzer::ConcatTextureNames(const nArray<int>& indices)
{
    nString str;
    int i;
    for (i = 0; i < indices.Size(); i++)
    {
        str.Append(this->textures[indices[i]].name);
        str.Append(" ");
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Returns a concatenated string of all meshes names.
*/
nString
nExportAnalyzer::ConcatMeshNames(const nArray<int>& indices)
{
    nString str;
    int i;
    for (i = 0; i < indices.Size(); i++)
    {
        str.Append(this->meshes[indices[i]].name);
        str.Append(" ");
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Returns a concatenated string of all anim names.
*/
nString
nExportAnalyzer::ConcatAnimNames(const nArray<int>& indices)
{
    nString str;
    int i;
    for (i = 0; i < indices.Size(); i++)
    {
        str.Append(this->anims[indices[i]].name);
        str.Append(" ");
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Returns a concatenated string of all object names.
*/
nString
nExportAnalyzer::ConcatObjectNames(const nArray<int>& indices)
{
    nString str;
    int i;
    for (i = 0; i < indices.Size(); i++)
    {
        str.Append(this->objects[indices[i]].name);
        str.Append(" ");
    }
    return str;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ParseTextures()
{
    bool retval = true;
    this->textures.Clear();
    nArray<nString> files = this->ListFiles("proj:export/textures", "*.dds");
    n_printf("Parse %i textures...", files.Size());

    int i;
    for (i = 0; i < files.Size(); i++)
    {
        retval &= this->ReadTextureInfo(files[i]);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ParseMeshes()
{
    bool retval = true;
    this->meshes.Clear();
    nArray<nString> files = this->ListFiles("proj:export/meshes", "*.nvx2");
    n_printf("Parse %i meshes...", files.Size());

    int i;
    for (i = 0; i < files.Size(); i++)
    {
        retval &= this->ReadMeshInfo(files[i]);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ParseAnims()
{
    bool retval = true;
    this->anims.Clear();
    nArray<nString> files = this->ListFiles("proj:export/anims", "*.nax2");
    n_printf("Parse %i animations...", files.Size());

    int i;
    for (i = 0; i < files.Size(); i++)
    {
        retval &= this->ReadAnimInfo(files[i]);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ParseObjects()
{
    bool retval = true;
    this->objects.Clear();
    nArray<nString> files = this->ListFiles("proj:export/gfxlib", "*.n2");
    n_printf("Parse %i graphic objects...", files.Size());

    int i;
    for (i = 0; i < files.Size(); i++)
    {
        retval &= this->ReadObjectInfo(files[i]);
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ParseLevels()
{
    bool retval = true;
    this->levels.Clear();

    // open the world database
    nSqlServer* sqlServer = (nSqlServer*) nKernelServer::Instance()->New("nsqlite3server", "/sys/servers/sql");
    nSqlDatabase* db = nSqlServer::Instance()->NewDatabase("proj:export/db/world.db3");
    if (db)
    {
        // get Graphics attribute from all game objects
        nSqlQuery* query = db->CreateQuery("SELECT _Level,Graphics FROM '_Entities' WHERE _Type='INSTANCE'");
        query->Execute();

        // for each object in the database...
        int rowIndex;
        int numRows = query->GetNumRows();
        n_printf("Parse %i levels...", query->GetNumRows());

        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            nSqlRow sqlRow = query->GetRow(rowIndex);
            if (sqlRow.HasColumn("_Level") && sqlRow.HasColumn("Graphics"))
            {
                this->ReadGameObjectInfo(sqlRow.Get("_Level"), sqlRow.Get("Graphics"));
            }
        }
        db->Release();
        return true;
    }
    sqlServer->Release();
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ReadTextureInfo(const nString& path)
{
    n_assert(path.IsValid());
    nString name = this->ExtractCategoryFilename(path);

    // create a texture object
    nTexture2* tex = nGfxServer2::Instance()->NewTexture(path);
    tex->SetFilename(path);
    if (tex->Load())
    {
        // create a new texture info structure and append to textures array
        TextureInfo texInfo;
        texInfo.filename = path;
        texInfo.name = name;
        texInfo.fileSize = nFileServer2::Instance()->GetFileSize(path);
        texInfo.ramSize = tex->GetByteSize();
        texInfo.type = tex->GetType();
        texInfo.format = tex->GetFormat();
        texInfo.width = tex->GetWidth();
        texInfo.height = tex->GetHeight();
        texInfo.numMipLevels = tex->GetNumMipLevels();
        texInfo.useCount = 0;
        this->textures.Append(texInfo);

        tex->Unload();
        tex->Release();
        return true;
    }
    else
    {
        n_printf("Texture '%s' FAILED!\n", name.Get());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ReadMeshInfo(const nString& path)
{
    n_assert(path.IsValid());
    nString name = this->ExtractCategoryFilename(path);

    // create a mesh object
    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(path);
    mesh->SetFilename(path);
    if (mesh->Load())
    {
        // create a new mesh info structure and append to meshes array
        MeshInfo meshInfo;
        meshInfo.filename = path;
        meshInfo.name = name;
        meshInfo.fileSize = nFileServer2::Instance()->GetFileSize(path);
        meshInfo.ramSize = mesh->GetByteSize();
        meshInfo.numVertices = mesh->GetNumVertices();
        meshInfo.numIndices = mesh->GetNumIndices();
        meshInfo.numEdges = mesh->GetNumEdges();
        meshInfo.numGroups = mesh->GetNumGroups();
        meshInfo.vertexComponents = mesh->GetVertexComponents();
        meshInfo.useCount = 0;
        this->meshes.Append(meshInfo);

        mesh->Unload();
        mesh->Release();
        return true;
    }
    else
    {
        n_printf("Mesh: '%s' FAILED!\n", name.Get());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ReadAnimInfo(const nString& path)
{
    n_assert(path.IsValid());
    nString name = this->ExtractCategoryFilename(path);


    // create an animation object
    nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(path);
    anim->SetFilename(path);
    if (anim->Load())
    {
        n_printf("ok.\n");

        // create a new anim info structure and append to anim array
        AnimInfo animInfo;
        animInfo.filename = path;
        animInfo.name = name;
        animInfo.fileSize = nFileServer2::Instance()->GetFileSize(path);
        animInfo.ramSize = anim->GetByteSize();
        animInfo.numGroups = anim->GetNumGroups();
        animInfo.numKeys = 0;
        animInfo.useCount = 0;
        int i;
        for (i = 0; i < animInfo.numGroups; i++)
        {
            const nAnimation::Group& g = anim->GetGroupAt(i);
            animInfo.numKeys += g.GetNumCurves() * g.GetNumKeys();
        }
        this->anims.Append(animInfo);

        anim->Unload();
        anim->Release();
        return true;
    }
    else
    {
        n_printf("Animation '%s' FAILED!\n", name.Get());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nExportAnalyzer::ReadObjectInfo(const nString& path)
{
    n_assert(path.IsValid());
    nString name = this->ExtractCategoryFilename(path);

    // create a Nebula2 object
    nKernelServer* kernelServer = nKernelServer::Instance();
    kernelServer->PushCwd(kernelServer->Lookup("/"));
    nSceneNode* o = (nSceneNode*) kernelServer->Load(path.Get());
    kernelServer->PopCwd();
    n_assert(o->IsA("nscenenode"));
    if (o)
    {
        nClass* rsrcClass = kernelServer->FindClass("nresource");
        ObjectInfo objectInfo;
        objectInfo.filename = path;
        objectInfo.name = name;
        objectInfo.useCount = 0;
        objectInfo.fileSize = nFileServer2::Instance()->GetFileSize(path);
        objectInfo.meshSize = 0;
        objectInfo.textureSize = 0;
        objectInfo.animSize = 0;
        objectInfo.error = false;

        // load resources for Nebula2 hierarchy into ram, but only
        // if all required textures exist, otherwise the program
        // would fail hard
        if (this->RecurseCheckTexturesExist(o, objectInfo.missingTextures))
        {
            n_assert(0 == objectInfo.missingTextures.Size());
            o->PreloadResources();
        }
        else
        {
            objectInfo.error = true;
        }

        // fill out a new object info structure
        objectInfo.numMaterials = this->RecurseCountByClass(kernelServer->FindClass("nmaterialnode"), o);

        // scan used textures...
        nRoot* rsrc = kernelServer->Lookup("/sys/share/rsrc/tex");
        n_assert(0 != rsrc);
        nRoot* cur;
        for (cur = rsrc->GetHead(); cur; cur = cur->GetSucc())
        {
            n_assert(cur->IsA(rsrcClass));
            nTexture2* tex = (nTexture2*) cur;
            nString fname = tex->GetFilename();
            if (fname.IsValid() && (!fname.MatchPattern("*/skin.dds")))
            {
                nString rsrcName = this->ExtractCategoryFilename(fname);
                int texIndex = this->FindUniqueTextureInfo(rsrcName, objectInfo.textureIndices);
                if (-1 != texIndex)
                {
                    objectInfo.textureIndices.Append(texIndex);
                    objectInfo.textureSize += this->textures[texIndex].ramSize;
                    this->textures[texIndex].useCount++;
                }
            }
        }

        // scan used meshes...
        rsrc = kernelServer->Lookup("/sys/share/rsrc/mesh");
        n_assert(0 != rsrc);
        for (cur = rsrc->GetHead(); cur; cur = cur->GetSucc())
        {
            n_assert(cur->IsA(rsrcClass));
            nMesh2* mesh = (nMesh2*) cur;
            nString fname = mesh->GetFilename();
            if (fname.IsValid())
            {
                nString rsrcName = this->ExtractCategoryFilename(fname);
                int meshIndex = this->FindUniqueMeshInfo(rsrcName, objectInfo.meshIndices);
                if (-1 != meshIndex)
                {
                    objectInfo.meshIndices.Append(meshIndex);
                    objectInfo.meshSize += this->meshes[meshIndex].ramSize;
                    this->meshes[meshIndex].useCount++;
                }
            }
        }

        // scan used anims
        rsrc = kernelServer->Lookup("/sys/share/rsrc/anim");
        n_assert(0 != rsrc);
        nClass *animClass = kernelServer->FindClass("nanimation");
        for (cur = rsrc->GetHead(); cur; cur = cur->GetSucc())
        {
            n_assert(cur->IsA(rsrcClass));
            nAnimation* anim = (nAnimation*) cur;
            nString fname = anim->GetFilename();
            if (fname.IsValid())
            {
                nString rsrcName = this->ExtractCategoryFilename(fname);
                int animIndex = this->FindUniqueAnimInfo(rsrcName, objectInfo.animIndices);
                if (-1 != animIndex)
                {
                    objectInfo.animIndices.Append(animIndex);
                    objectInfo.animSize += this->anims[animIndex].ramSize;
                    this->anims[animIndex].useCount++;
                }
            }
        }
        this->objects.Append(objectInfo);

        // release object
        o->UnloadResources();
        o->Release();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Find pointer to level info by level name, return -1 if not found.
*/
int
nExportAnalyzer::FindLevelInfo(const nString& name)
{
    int i;
    int num = this->levels.Size();
    for (i = 0; i < num; i++)
    {
        if (this->levels[i].name == name)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    This takes a game object from the database and writes the information
    into the levels array.
*/
void
nExportAnalyzer::ReadGameObjectInfo(const nString& levelName, const nString& graphicsResource)
{
    // check if level exists or must be created
    int levelIndex = this->FindLevelInfo(levelName);
    if (-1 == levelIndex)
    {
        // doesn't exist yet, create new one
        LevelInfo newLevelInfo;
        newLevelInfo.name = levelName;
        newLevelInfo.error = false;
        newLevelInfo.numObjects = 0;
        newLevelInfo.numUniqueObjects = 0;
        newLevelInfo.textureSize = 0;
        newLevelInfo.animSize = 0;
        newLevelInfo.meshSize = 0;
        this->levels.Append(newLevelInfo);
        levelIndex = this->levels.Size() - 1;
    }
    LevelInfo& levelInfo = this->levels[levelIndex];
    levelInfo.numObjects++;
    int objIndex = this->FindUniqueObjectInfo(graphicsResource, levelInfo.objectIndices);
    if (-1 != objIndex)
    {
        levelInfo.objectIndices.Append(objIndex);
        levelInfo.numUniqueObjects++;
        int i;
        for (i = 0; i < this->objects[objIndex].textureIndices.Size(); i++)
        {
            int textureIndex = this->objects[objIndex].textureIndices[i];
            if (-1 == levelInfo.textureIndices.FindIndex(textureIndex))
            {
                levelInfo.textureIndices.Append(textureIndex);
                levelInfo.textureSize += this->textures[textureIndex].ramSize;
            }
        }
        for (i = 0; i < this->objects[objIndex].meshIndices.Size(); i++)
        {
            int meshIndex = this->objects[objIndex].meshIndices[i];
            if (-1 == levelInfo.meshIndices.FindIndex(meshIndex))
            {
                levelInfo.meshIndices.Append(meshIndex);
                levelInfo.meshSize += this->meshes[meshIndex].ramSize;
            }
        }
        for (i = 0; i < this->objects[objIndex].animIndices.Size(); i++)
        {
            int animIndex = this->objects[objIndex].animIndices[i];
            if (-1 == levelInfo.animIndices.FindIndex(animIndex))
            {
                levelInfo.animIndices.Append(animIndex);
                levelInfo.animSize += this->anims[animIndex].ramSize;
            }
        }
        this->objects[objIndex].useCount++;
    }
    else
    {
        // the object was either already added, or wasn't found at all
        // just do a check whether the object doesn't exist, if the graphics
        // resource is made of 3 components then it's a character3 object
        // and must be limited to the first 2
        bool exists = false;
        nArray<nString> tokens;
        graphicsResource.Tokenize("/", tokens);
        if (tokens.Size() == 3)
        {
            // character3 special case
            exists = true;
        }
        else
        {
            int i;
            int num = this->objects.Size();
            for (i = 0; i < num; i++)
            {
                if (this->objects[i].name == graphicsResource)
                {
                    exists = true;
                    break;
                }
            }
        }
        if (!exists)
        {
            // set an error
            levelInfo.error = true;
            if (-1 == levelInfo.missingObjects.FindIndex(graphicsResource))
            {
                levelInfo.missingObjects.Append(graphicsResource);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Write out texture info into comma separated file.
*/
void
nExportAnalyzer::WriteTextureInfo()
{
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(this->rootDir + "/textures.csv", "wa"))
    {
        file->PutS("Name;Filename;FileSize;RamSize;UseCount;Type;Format;Width;Height;MipLevels\n");
        int i;
        int num = this->textures.Size();
        for (i = 0; i < num; i++)
        {
            const TextureInfo& texInfo = this->textures[i];
            nString line;
            line.Format("%s ;%s ;%d ;%d ;%d ;%s ;%s ;%d ;%d ;%d \n",
                texInfo.name.Get(),
                texInfo.filename.Get(),
                texInfo.fileSize,
                texInfo.ramSize,
                texInfo.useCount,
                nTexture2::TypeToString(texInfo.type),
                nTexture2::FormatToString(texInfo.format),
                texInfo.width,
                texInfo.height,
                texInfo.numMipLevels);
            file->PutS(line);
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
    Write out mesh information into comma-separated file.
*/
void
nExportAnalyzer::WriteMeshInfo()
{
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(this->rootDir + "/meshes.csv", "wa"))
    {
        file->PutS("Name;Filename;FileSize;RamSize;UseCount;Vertices;Indices;Edges;Groups;Components\n");
        int i;
        int num = this->meshes.Size();
        for (i = 0; i < num; i++)
        {
            const MeshInfo& meshInfo = this->meshes[i];

            nString compStr;
            int c = meshInfo.vertexComponents;
            if (c & nMesh2::Coord) compStr += "Coord ";
            if (c & nMesh2::Coord4) compStr += "Coord4 ";
            if (c & nMesh2::Normal) compStr += "Normal ";
            if (c & nMesh2::Uv0) compStr += "Uv0 ";
            if (c & nMesh2::Uv1) compStr += "Uv1 ";
            if (c & nMesh2::Uv2) compStr += "Uv2 ";
            if (c & nMesh2::Uv3) compStr += "Uv3 ";
            if (c & nMesh2::Color) compStr += "Color ";
            if (c & nMesh2::Tangent) compStr += "Tangent ";
            if (c & nMesh2::Binormal) compStr += "Binormal ";
            if (c & nMesh2::Weights) compStr += "Weights ";
            if (c & nMesh2::JIndices) compStr += "JIndices ";

            nString line;
            line.Format("%s ;%s ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%s \n",
                meshInfo.name.Get(),
                meshInfo.filename.Get(),
                meshInfo.fileSize,
                meshInfo.ramSize,
                meshInfo.useCount,
                meshInfo.numVertices,
                meshInfo.numIndices,
                meshInfo.numEdges,
                meshInfo.numGroups,
                compStr.Get());
            file->PutS(line);
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
    Write out anim info into comma separated file.
*/
void
nExportAnalyzer::WriteAnimInfo()
{
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(this->rootDir + "/anims.csv", "wa"))
    {
        file->PutS("Name;Filename;FileSize;RamSize;UseCount;NumGroups;NumKeys\n");
        int i;
        int num = this->anims.Size();
        for (i = 0; i < num; i++)
        {
            const AnimInfo& animInfo = this->anims[i];
            nString line;
            line.Format("%s ;%s ;%d ;%d ;%d ;%d ;%d \n",
                animInfo.name.Get(),
                animInfo.filename.Get(),
                animInfo.fileSize,
                animInfo.ramSize,
                animInfo.useCount,
                animInfo.numGroups,
                animInfo.numKeys);
            file->PutS(line);
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
    Write out object info into comma separated file.
*/
void
nExportAnalyzer::WriteObjectInfo()
{
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(this->rootDir + "/objects.csv", "wa"))
    {
        file->PutS("Status;Name;Filename;FileSize;UseCount;Complexity;NumTextures;NumMeshes;NumAnims;TextureSize;MeshSize;AnimSize;Textures;Meshes;Anims;MissingTextures\n");
        int i;
        int num = this->objects.Size();
        for (i = 0; i < num; i++)
        {
            const ObjectInfo& objectInfo = this->objects[i];
            nString texturesString = this->ConcatTextureNames(objectInfo.textureIndices);
            nString meshesString = this->ConcatMeshNames(objectInfo.meshIndices);
            nString animsString = this->ConcatAnimNames(objectInfo.animIndices);
            nString line;
            line.Format("%s ;%s ;%s ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%s ;%s ;%s ;%s \n",
                objectInfo.error ? "ERROR" : "OK",
                objectInfo.name.Get(),
                objectInfo.filename.Get(),
                objectInfo.fileSize,
                objectInfo.useCount,
                objectInfo.numMaterials,
                objectInfo.textureIndices.Size(),
                objectInfo.meshIndices.Size(),
                objectInfo.animIndices.Size(),
                objectInfo.textureSize,
                objectInfo.meshSize,
                objectInfo.animSize,
                texturesString.Get(),
                meshesString.Get(),
                animsString.Get(),
                nString::Concatenate(objectInfo.missingTextures, " ").Get());
            file->PutS(line);
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
    Write out level info into comma separated file.
*/
void
nExportAnalyzer::WriteLevelInfo()
{
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(this->rootDir + "/levels.csv", "wa"))
    {
        file->PutS("Status;Name;NumObjects;NumUniqueObjects;NumTextures;TextureSize;NumMeshes;MeshSize;NumAnims;AnimSize;Objects;MissingObjects;\n");
        int i;
        int num = this->levels.Size();
        for (i = 0; i < num; i++)
        {
            const LevelInfo& levelInfo = this->levels[i];
            nString objectString = this->ConcatObjectNames(levelInfo.objectIndices);
            nString missingString = nString::Concatenate(levelInfo.missingObjects, " ");
            nString line;
            line.Format("%s ;%s ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%s; %s \n",
                levelInfo.error ? "ERROR" : "OK",
                levelInfo.name.Get(),
                levelInfo.numObjects,
                levelInfo.numUniqueObjects,
                levelInfo.textureIndices.Size(),
                levelInfo.textureSize,
                levelInfo.meshIndices.Size(),
                levelInfo.meshSize,
                levelInfo.animIndices.Size(),
                levelInfo.animSize,
                objectString.Get(),
                missingString.Get());
            file->PutS(line);
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
    Append a summary...
*/
void
nExportAnalyzer::WriteSummary()
{
    bool create = false;
    if (!nFileServer2::Instance()->FileExists("proj:statistics/summary.csv"))
    {
        create = true;
    }

    // Nebula2 doesn't allow to open in append mode
    nString mangledPath = nFileServer2::Instance()->ManglePath("proj:statistics/summary.csv");
    FILE* fp = fopen(mangledPath.Get(), "a");
    n_assert(0 != fp);
    if (create)
    {
        fputs("Date;NumObjects;NumTextures;TextureSize;NumMeshes;MeshSize;NumAnims;AnimSize;NumErrors\n", fp);
    }
    int numErrors = 0;
    int textureSize = 0;
    int meshSize = 0;
    int animSize = 0;
    int i;
    for (i = 0; i < this->textures.Size(); i++)
    {
        textureSize += this->textures[i].fileSize;
    }
    for (i = 0; i < this->meshes.Size(); i++)
    {
        meshSize += this->meshes[i].fileSize;
    }
    for (i = 0; i < this->anims.Size(); i++)
    {
        animSize += this->anims[i].fileSize;
    }
    for (i = 0; i < this->objects.Size(); i++)
    {
        if (this->objects[i].error)
        {
            numErrors++;
        }
    }
    for (i = 0; i < this->levels.Size(); i++)
    {
        if (this->levels[i].error)
        {
            numErrors++;
        }
    }
    nString line;
    line.Format("%s ;%d ;%d ;%d ;%d ;%d ;%d ;%d ;%d\n",
        this->dateAndTime.Get(),
        this->objects.Size(),
        this->textures.Size(),
        textureSize,
        this->meshes.Size(),
        meshSize,
        this->anims.Size(),
        animSize,
        numErrors);
    fputs(line.Get(), fp);
    fclose(fp);
}
