//------------------------------------------------------------------------------
//  nresourcebundle_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "resource/nresourcebundle.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"

nNebulaClass(nResourceBundle, "nresource");

//------------------------------------------------------------------------------
/**
*/
nResourceBundle::nResourceBundle() :
    refAnimServer("/sys/servers/anim"),
    bundleResources(128, 128)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nResourceBundle::~nResourceBundle()
{
    // empty, nResource class calls Unload() if necessary
}

//------------------------------------------------------------------------------
/**
    Load the resource bundle. This creates resource objects for the
    resources in the bundle and loads them.
*/
bool
nResourceBundle::LoadResource()
{
    n_assert(!this->IsLoaded());
    bool success = false;
    nString filename = this->GetFilename();

    // open the resource bundle file
    nFile* hdrFile = nFileServer2::Instance()->NewFileObject();
    if (hdrFile->Open(filename.Get(), "rb"))
    {
        // open a second file handle to the data block
        nFile* dataFile = nFileServer2::Instance()->NewFileObject();
        success = dataFile->Open(filename.Get(), "rb");
        n_assert(success);

        // read header and check magic number
        int magic = hdrFile->GetInt();
        int numEntries = hdrFile->GetInt();

        if (magic != 'NRB0')
        {
            n_printf("nResourceBundle: magic number mismatch in file '%s'\n", this->GetFilename().Get());
            hdrFile->Close();
            dataFile->Close();
            hdrFile->Release();
            dataFile->Release();
            return false;
        }

        // read toc entries
        int entryIndex;
        for (entryIndex = 0; entryIndex < numEntries; entryIndex++)
        {
            const int nameFieldLen = 32;
            char name[nameFieldLen + 1] = { 0 };
            
            int type = hdrFile->GetInt();
            hdrFile->Read(name, nameFieldLen);
            int dataOffset = hdrFile->GetInt();
            int dataSize   = hdrFile->GetInt();
            int flags      = hdrFile->GetInt();

            if (!this->LoadResource(type, name, dataOffset, dataSize, flags, dataFile))
            {
                n_printf("WARNING: Failed to load resource '%s'\n", name);
            }
        }
        hdrFile->Close();
        dataFile->Close();
        dataFile->Release();
    }
    hdrFile->Release();
    this->SetState(Valid);
    return success;
}

//------------------------------------------------------------------------------
/**
    Unload all resource belonging to the bundle.
*/
void
nResourceBundle::UnloadResource()
{
    if (this->IsLoaded())
    {
        int i;
        int num = this->bundleResources.Size();
        for (i = 0; i < num; i++)
        {
            if (this->bundleResources[i].isvalid())
            {
                this->bundleResources[i]->Release();
                this->bundleResources[i].invalidate();
            }
        }
        this->SetState(Unloaded);
    }
}

//------------------------------------------------------------------------------
/**
    Load a single resource object.

    @param  type            a resource type fourcc
    @param  resId           a 32 character resource id
    @param  dataOffset      offset of resource data in dataFile
    @param  dataSize        byte-size of resource data in dataFile
    @param  flags           additional resource specific flags
    @param  dataFile        an open file object
    @return                 true if successful
*/
bool
nResourceBundle::LoadResource(int type, 
                              const char* resId, 
                              int dataOffset, 
                              int dataSize, 
                              int flags,
                              nFile* dataFile)
{
    switch (type)
    {
        case 'MESH':
            return this->LoadMesh(resId, dataOffset, dataSize, flags, dataFile);

        case 'MANI':
            return this->LoadAnimation(resId, dataOffset, dataSize, flags, dataFile);
            
        case 'TXTR':
            return this->LoadTexture(resId, dataOffset, dataSize, flags, dataFile);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Load a mesh resource, called by the generic LoadResource method.
*/
bool
nResourceBundle::LoadMesh(const char* resId, 
                          int dataOffset, 
                          int dataSize, 
                          int flags, 
                          nFile* dataFile)
{
    n_assert(resId && dataFile);
    n_assert(dataOffset > 0);
    n_assert(dataSize > 0);

    // create a new mesh object
    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(resId);
    n_assert(mesh);
    if (!mesh->IsLoaded())
    {
        mesh->SetUsage(flags);
        if (!mesh->Load(dataFile, dataOffset, dataSize))
        {
            n_printf("nResourceBundler: failed to load mesh data '%s'!\n", resId);
            mesh->Release();
            return false;
        }
    }
    this->bundleResources.Append(nRef<nResource>(mesh));
    return true;
}

//------------------------------------------------------------------------------
/**
    Load a animation resource, called by the generic LoadResource method.
*/
bool
nResourceBundle::LoadAnimation(const char* resId, 
                               int dataOffset, 
                               int dataSize, 
                               int /*flags*/, 
                               nFile* dataFile)
{
    n_assert(resId && dataFile);
    n_assert(dataOffset > 0);
    n_assert(dataSize > 0);

    // create a new animation object
    nAnimation* anim = this->refAnimServer->NewMemoryAnimation(resId);
    n_assert(anim);
    if (!anim->IsLoaded())
    {
        if (!anim->Load(dataFile, dataOffset, dataSize))
        {
            n_printf("nResourceBundler: failed to load anim data '%s'!\n", resId);
            anim->Release();
            return false;
        }
    }
    this->bundleResources.Append(nRef<nResource>(anim));
    return true;
}

//------------------------------------------------------------------------------
/**
    Load a texture resource, called by the generic LoadResource method.
*/
bool
nResourceBundle::LoadTexture(const char* resId, 
                             int dataOffset, 
                             int dataSize, 
                             int /*flags*/, 
                             nFile* dataFile)
{
    n_assert(resId && dataFile);
    n_assert(dataOffset > 0);
    n_assert(dataSize > 0);

    // create a new animation object
    nTexture2* tex = nGfxServer2::Instance()->NewTexture(resId);
    n_assert(tex);
    if (!tex->IsLoaded())
    {
        if (!tex->Load(dataFile, dataOffset, dataSize))
        {
            n_printf("nResourceBundler: failed to load texture data '%s'!\n", resId);
            tex->Release();
            return false;
        }
    }
    this->bundleResources.Append(nRef<nResource>(tex));
    return true;
}
