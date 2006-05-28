#ifndef N_LW_TEXTURE_EXPORT_H
#define N_LW_TEXTURE_EXPORT_H
//----------------------------------------------------------------------------
#include "util/nstring.h"
#include "gfx2/nshaderstate.h"

class nLWShaderExportSettings;
class nMaterialNode;

//----------------------------------------------------------------------------
/**
    @class nLWTextureExport
    @brief Handles exporting of textures from a Lightwave surface.

    @note Each Lightwave surface that has a Nebula 2 shader associated with it 
    will be converted to an nMaterialNode that will reference the exported 
    textures.

    Right now "exporting" just copies the textures to the appropriate export
    directory without converting them to DDS... that will be added in sometime
    in the future.
*/
class nLWTextureExport
{
public:
    nLWTextureExport(nLWShaderExportSettings*);
    ~nLWTextureExport();

    bool Run();
    void CopyTextureParamsTo(nMaterialNode*);

private:
    class Texture
    {
    public:
        Texture();
        Texture(nShaderState::Param, const nString& sourceTexture);

        nShaderState::Param param;
        nString sourceTexture;
        nString exportedTexture;
    };

    nArray<Texture> textures;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWTextureExport::Texture::Texture():
    param(nShaderState::InvalidParameter)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWTextureExport::Texture::Texture(nShaderState::Param p, 
                                   const nString& sourceTexture) :
    param(p),
    sourceTexture(sourceTexture)
{
    // empty
}

//----------------------------------------------------------------------------
#endif // N_LW_TEXTURE_EXPORT_H
