//------------------------------------------------------------------------------
//  nclodmesh_main.cc
//  (C) 2004 Gary Haussmann
//------------------------------------------------------------------------------
#include "clodterrain/nclodtile.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfile.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nCLODTile, "nresource");

//------------------------------------------------------------------------------
/**
*/
nCLODTile::nCLODTile() :
    gfx2("/sys/servers/gfx"),
    pixelsize(0)
{
    this->SetType(nResource::Other);
    this->SetAsyncEnabled(true);
}

//------------------------------------------------------------------------------
/**
*/
nCLODTile::~nCLODTile()
{
    //n_assert(!this->IsValid());
    if (this->IsLoaded())
    {
        this->Unload();
    }

    for (texArray::iterator curtex = chunkTextures.Begin();
        curtex != chunkTextures.End();
        curtex++)
    {
        if ((*curtex).isvalid())
            (*curtex)->Release();
    }
}

/// configure tile size
void nCLODTile::Configure(unsigned int tilepixelsize, nFile *tqtfile)
{

    this->pixelsize = tilepixelsize;
    this->tqtFile = tqtfile;
    this->texturecount = tqtfile->GetInt();
//    n_assert(texturecount == 1);
    this->dataOffset = tqtfile->GetInt();
    this->dataSize = tqtfile->GetInt();

    this->chunkTextures.SetFixedSize(this->texturecount);
}

/// dump the loaded texture into the appropriate shader
bool nCLODTile::ToShader(nShader2 *puthere, unsigned int textureindex)
{
    if (puthere->IsParameterUsed(nShaderState::DiffMap0))
    {
        puthere->SetTexture(nShaderState::DiffMap0, chunkTextures[textureindex].get());
        return true;
    }
    else
        return false;
}


/// clod meshes support asynchronous loading
bool nCLODTile::CanLoadAsync() const
{
    return true;
}

/// issue a load request from an open file
bool nCLODTile::Load()
{
//  n_assert(!this->tileData.isvalid());
    n_assert(!this->IsLoaded());//n_assert(!this->IsValid());

    // create texture if we haven't already
    if (!chunkTextures[0].isvalid())
    {
        for (unsigned int iindex=0; iindex < this->texturecount; iindex++)
        {
            nString tilename = this->GetName();
            tilename += "_tqt_";
            char buffer[10];
            tilename += itoa(iindex,buffer,10);
            chunkTextures[iindex] = (nTexture2 *)gfx2->NewTexture(tilename.Get());
        }
    }

    return this->nResource::Load();
}

//------------------------------------------------------------------------------
/**
    Load in terrain texture data.
*/
bool nCLODTile::LoadResource()
{
    if (this->IsValid())
        return true;

    n_assert(!this->IsValid());
    n_assert(chunkTextures[0].isvalid());

    //
    // read in data from the terrain file
    //

    unsigned int singledatasize = this->dataSize / this->texturecount;
    n_assert(singledatasize * this->texturecount == this->dataSize);
    tqtFile->Seek(dataOffset, nFile::START);
    char *filedata = new char[singledatasize];
    bool multitexture = (this->texturecount > 1);
    for (unsigned int iindex=0; iindex < this->texturecount; iindex++)
    {
        // read in tile index and image size
        int filetileindex = tqtFile->GetInt();
        int imagesize = tqtFile->GetInt();

        n_assert(filetileindex == iindex);
        // skip if no image
        if (imagesize == 0)
            continue;

        // create an empty texture and fill in from file
        nTexture2 *tileData = chunkTextures[iindex].get();

        tileData->SetUsage(nTexture2::CreateEmpty);
        tileData->SetType(nTexture2::TEXTURE_2D);
        tileData->SetWidth(pixelsize);
        tileData->SetHeight(pixelsize);
        if (multitexture)
            tileData->SetFormat(nTexture2::A4R4G4B4);
        else
            tileData->SetFormat(nTexture2::A8R8G8B8);
        tileData->Load();

        struct nTexture2::LockInfo tilesurf;
        if (tileData->Lock(nTexture2::WriteOnly, 0, tilesurf))
        {
            // read in the data and dump into the texture
            if (!multitexture)
                tqtFile->Read(tilesurf.surfPointer, singledatasize);
            else
            {
                // read in one byte at a time and stuff into the texture buffer
                char *surface = (char *)tilesurf.surfPointer;
                tqtFile->Read(filedata, singledatasize);
                for (unsigned int pixbyte=0; pixbyte < (singledatasize); pixbyte++)
                {
                    unsigned char curbyte = filedata[pixbyte];
                    surface[0] = curbyte;
                    surface[1] = curbyte;
                    surface += 2;
/*                    for (int curbit=0; curbit < 8; curbit++)
                    {
                        unsigned char extendedbit = (curbyte & 128) ? 255 : 0;
                        surface[0] = extendedbit;
                        surface[1] = extendedbit & 127;
                        surface += 2;
                        curbyte <<= 1;
                    }*/
                }
            }
            tileData->Unlock(0);
//            n_printf("loaded texture at %d\n", this->dataOffset);
        }
    }
    delete [] filedata;

    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void nCLODTile::UnloadResource()
{
    //n_assert(this->IsValid());
    n_assert(this->IsLoaded());

    n_assert(chunkTextures[0].isvalid());

    for (unsigned int iindex=0; iindex < this->texturecount; iindex++)
    {
        if (chunkTextures[iindex]->IsValid())
            chunkTextures[iindex]->Unload();
    }

    this->SetState(Unloaded);
}
