//-----------------------------------------------------------------------------
//  nmaxtexture.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxtexture.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "export2/nmaxoptions.h"
#include "scene/nshapenode.h"

#include "kernel/nfileserver2.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxTexture::nMaxTexture()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxTexture::~nMaxTexture()
{
}

//-----------------------------------------------------------------------------
/**
    Export textures.

    function for native 3DS Max standard material.
*/
void nMaxTexture::Export(Texmap* texmap, int subID, nShapeNode* shapeNode)
{
    if (IsClassID(texmap, BMTEX_CLASS_ID))
    {
        StdUVGen* uvGen = ((BitmapTex*)texmap)->GetUVGen();

        // get texture name.
        nString mapFileName = ((BitmapTex*)texmap)->GetMapName();

        //FIXME: if no textures is assigned for this material, we should assign default one.

        // get shader parameter.
        nShaderState::Param param = this->GetShaderParamFromStdMapSlot(uvGen, subID);

        // set the texture to the given node.
        nString textureName;
        textureName += nMaxOptions::Instance()->GetTextureAssign();
        textureName += mapFileName.ExtractFileName();
        shapeNode->SetTexture(param, textureName.Get());

        // copy textures.
        if (CopyTexture(mapFileName.Get()))
        {
            n_maxlog(Midium, "'%s' is copied.", mapFileName.Get());
        }

        // get uv transform if it exist.
        // ...
    }
    else
    {
        // we only support bitmap texture.
        n_maxlog(Error, "Only bitmap texture is supported : %s", shapeNode->GetFullName());
    }

    for (int i=0; i<texmap->NumSubTexmaps(); i++)
    {
        Texmap* subTexmap = texmap->GetSubTexmap(i);
        this->Export(subTexmap, i, shapeNode);
    }
}

//-----------------------------------------------------------------------------
/**
    function for Nebula2 custom material.
*/
void nMaxTexture::Export(Texmap* texmap, nShaderState::Param param, nShapeNode* shapeNode)
{
    if (IsClassID(texmap, BMTEX_CLASS_ID))
    {
        // get texture name.
        nString mapFileName = ((BitmapTex*)texmap)->GetMapName();

        //FIXME: if no textures is assigned for this material, we should assign default one.

        // set the texture to the given node.
        nString textureName;
        textureName += nMaxOptions::Instance()->GetTextureAssign();
        textureName += mapFileName.ExtractFileName();
        shapeNode->SetTexture(param, textureName.Get());

        // copy textures.
        if (CopyTexture(mapFileName.Get()))
        {
            n_maxlog(Midium, "'%s' is copied.", mapFileName.Get());
        }

        // get uv transform if it exist.
        // ...
    }
    else
    {
        // we only support bitmap texture.
        n_maxlog(Error, "Only bitmap texture is supported : %s", shapeNode->GetFullName());
    }
}

//-----------------------------------------------------------------------------
/**
Copy texture file to texture assign directory.

@param textureName texture filename which to be copied.
@return true, if the file was successfully copied, otherwise false.
*/
bool nMaxTexture::CopyTexture(const char* textureName)
{
    n_assert(textureName);

    // copy texture if it does not exist in texture path.
    nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    const char* from = textureName;

    nString tmp = textureName;
    nString dest = nMaxOptions::Instance()->GetTextureAssign().Get();
    dest += "\\";
    dest += tmp.ExtractFileName();

    const char* to = dest.Get();

    if (!fileServer->FileExists(to))
    {
        // if the texture file does not exist, copy it to texture assign directory.
        if (!fileServer->CopyFile(from, to))
        {
            n_maxlog(Error, "Failed to copy %s to %s", from, to);
            return false;
        }
    }
    else
    {
        // texture exist but force copy it to the directory.
        // if overwrite option is on.
        if (nMaxOptions::Instance()->OverwriteExistTexture())
        {
            if (!fileServer->CopyFile(from, to))
            {
                n_maxlog(Error, "Failed to copy %s to %s", from, to);
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxTexture::GetUvTransform(UVGen* uvGen)
{
    n_assert(uvGen);

    // check that given uvGen is StdUVGen type,
    // if not, we just return.
    StdUVGen* stdUvGen = 0;
    if (uvGen)
    {
        if (uvGen->IsStdUVGen())
            stdUvGen = (StdUVGen*)uvGen;
        else
        {
            n_maxlog(Error, "");
            return;
        }
    }

    // there should be more than one sub-anim.
    if (stdUvGen->NumSubs() < 1)
    {
        n_maxlog(Error, "");
        return;
    }

    // get 'Animatable' from StdUVGen.
    Animatable* subAnim = stdUvGen->SubAnim(0);
    if (subAnim->SuperClassID() != PARAMETER_BLOCK_CLASS_ID)
    {
        n_maxlog(Error, "");
        return;
    }
    // get param block from animatable.
    IParamBlock* pblock = (IParamBlock*)subAnim;

    // we only support these 5 type of transforms at the moment.
    enum {
        UvTransU = 0, // translate of u coord
        UvTransV = 1, // translate of v coord
        UvScaleU = 2, // scale of u
        UvScaleV = 3, // scale of v
        UvRotate = 6, // rotate of uv
    };

    // the number of uv transform type in 3dsmax.
    const int MaxTransformType = 12;

    for (int i=0; i<MaxTransformType; i++)
    {
        // get i'th controller from the param block.
        Control* ctrl = pblock->GetController(i);
        if (ctrl)
        {
            // we have controller, so a transform exist on there.

            // determine transform type.
            switch(i)
            {
            case UvTransU:
                break;
            case UvTransV:
                break;           
            case UvScaleU:
                break;
            case UvScaleV:
                break;
            case UvRotate:
                break;
            }

            // determine animation type (loop or not)

        }
    }// end of for each transform type.
}

//-----------------------------------------------------------------------------
/**
    Retrieves string from the given submap id.
*/
const char* nMaxTexture::SubMapIDToString(int subID)
{
    switch(subID)
    {
    case ID_AM: return "<ambient>";
    case ID_DI: return "<diffuse>";
    case ID_SP: return "<specular>";
    case ID_BU: return "<bump>";
    case ID_OP: return "<opacity>";
    case ID_FI: return "<filter color>";
    case ID_SH: return "<shininess>";
    case ID_SS: return "<shininess strength>";
    case ID_SI: return "<self-illumination>";
    case ID_RL: return "<reflection>";
    case ID_RR: return "<refraction>";
    case ID_DP: return "<displacement>";
    default:
        break;
    }
    return "<unknown>";
}

//-----------------------------------------------------------------------------
/**
    Retrieves Nebula shader parameter from the given texmap and its submap ID.
*/
nShaderState::Param nMaxTexture::GetShaderParamFromStdMapSlot(StdUVGen* uvGen, int subID)
{
    nShaderState::Param shaderParam = nShaderState::InvalidParameter;

    // get its mapping channel it is applied to
    // (map channel 0 is vertex color channel)
    const int mapChannel = uvGen->GetMapChannel();

    //if (IsClassID(texmap, DMTL_CLASS_ID))
    //{
    switch(subID)
    {
    case ID_AM: // ambient
        switch(mapChannel)
        {
        case 1: shaderParam = nShaderState::AmbientMap0; break;
        case 2: shaderParam = nShaderState::AmbientMap1; break;
        case 3: shaderParam = nShaderState::AmbientMap2; break;
        case 4: shaderParam = nShaderState::AmbientMap3; break;
        default:
            n_maxlog(Low, "Use mapping channel [1~4] for %s map. map channel [%d] was used", 
                        SubMapIDToString(subID), mapChannel);
            shaderParam = nShaderState::AmbientMap0;
            break;
        }
        break;

    case ID_DI: // diffuse
        switch(mapChannel)
        {
        case 1: shaderParam = nShaderState::DiffMap0; break;
        case 2: shaderParam = nShaderState::DiffMap1; break;
        case 3: shaderParam = nShaderState::DiffMap2; break;
        case 4: shaderParam = nShaderState::DiffMap3; break;
        default:
            n_maxlog(Low, "Use mapping channel [1~4] for %s map. map channel [%d] was used", 
                SubMapIDToString(subID), mapChannel);
            shaderParam = nShaderState::DiffMap0;
            break;
        }
        break;

    case ID_SP: // specular
        switch(mapChannel)
        {
        case 1: shaderParam = nShaderState::SpecMap0; break;
        case 2: shaderParam = nShaderState::SpecMap1; break;
        case 3: shaderParam = nShaderState::SpecMap2; break;
        case 4: shaderParam = nShaderState::SpecMap3; break;
        default:
            n_maxlog(Low, "Use mapping channel [1~4] for %s map. map channel [%d] was used", 
                SubMapIDToString(subID), mapChannel);
            shaderParam = nShaderState::SpecMap0;
            break;
        }
        break;

    case ID_BU: // bump
        switch(mapChannel)
        {
        case 1: shaderParam = nShaderState::BumpMap0; break;
        case 2: shaderParam = nShaderState::BumpMap1; break;
        case 3: shaderParam = nShaderState::BumpMap2; break;
        case 4: shaderParam = nShaderState::BumpMap3; break;
        default:
            n_maxlog(Low, "Use mapping channel [1~4] for %s map. map channel [%d] was used", 
                SubMapIDToString(subID), mapChannel);
            shaderParam = nShaderState::SpecMap0;
            break;
        }
        break;

    case ID_OP:
    case ID_FI:
    case ID_SH:
    case ID_SS:
    case ID_SI:
    case ID_RL:
    case ID_RR:
    case ID_DP:
        n_maxlog(Error, "Nebula does not support %s map", SubMapIDToString(subID));
        break;

    default:
        n_maxlog(Error, "Unknown submap ID [%d]", subID);
        break;
    }
    //}
    //else
    //{
    //    // we only support standard material
    //}

    return shaderParam;
}

//-----------------------------------------------------------------------------
/**
    Check texture map ID is identical to given class ID.

    @param texmap
    @param classID 
*/
bool nMaxTexture::IsClassID(Texmap* texmap, ulong classID)
{
    if (texmap && texmap->ClassID() == Class_ID(classID, 0))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
/**
    Get map type from given texture map.

    @param 
*/
nMaxTexture::Type nMaxTexture::GetType(Texmap* map)
{
    n_assert (map);

    nMaxTexture::Type type;

    if (map->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
        type = Bitmap;
    else
    if (map->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
        type = Composite;
    else
    if (map->ClassID() == Class_ID(MIX_CLASS_ID, 0))
        type = Mix;
    else
    if (map->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
        type = RGBMultiply;
    else
        type = Unknown;

    return type;
}
