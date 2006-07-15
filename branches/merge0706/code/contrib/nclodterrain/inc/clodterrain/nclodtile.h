#ifndef N_CLODTILE_H
#define N_CLODTILE_H
//------------------------------------------------------------------------------
/**
 *   @class nCLODTile
 *   @ingroup NCLODTerrainContribModule
 *   @brief nCLODTile manages the terrain texture resource applied to a specific terrain chunk
 */
#include "resource/nresource.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"

class nGfxServer2;
class nVariableServer;

//------------------------------------------------------------------------------
class nCLODTile : public nResource
{
public:
    /// constructor
    nCLODTile();
    /// destructor
    virtual ~nCLODTile();

    /// configure tile size
    void Configure(unsigned int tilepixelsize, nFile *tqtfile);

    /// How many textures in here?
    unsigned int GetNumTextures() { return texturecount; }

    /// clod tiles support asynchronous loading
    virtual bool CanLoadAsync() const;

    /// issue a load request from an open file
    virtual bool Load();

    /// dump the loaded texture into the appropriate shader
    bool ToShader(nShader2 *puthere, unsigned int textureindex = 0);

protected:
    /// load resource
    virtual bool LoadResource();
    /// unload resource
    virtual void UnloadResource();

    nAutoRef<nGfxServer2> gfx2; // gfx server - we get the texture from here
    nFile *tqtFile;         // open file containing our terrain texture data
    unsigned long dataOffset, dataSize;     // offset in TQT file where our data resides
    unsigned int pixelsize; // pixel size of the texture
    unsigned int texturecount;

    typedef nArray<nRef<nTexture2> > texArray;
    texArray chunkTextures; // actual gfxserver-specific texture instances
};

#endif
