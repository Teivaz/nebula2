//------------------------------------------------------------------------------
//  nguiresource.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiresource.h"
#include "gui/nguiserver.h"

//------------------------------------------------------------------------------
/**
*/
bool
nGuiResource::Load()
{
    n_assert(!this->IsValid());
    n_assert(!this->texName.IsEmpty());
    char texPath[N_MAXPATH];

    n_strncpy2(texPath, this->texName.Get(), sizeof(texPath));
    if (!this->refTexture.isvalid())
    {
        this->refTexture = nGfxServer2::Instance()->NewTexture(texPath);
    }
    if (!this->refTexture->IsValid())
    {
        this->refTexture->SetFilename(texPath);
        if (!this->refTexture->Load())
        {
            n_error("nGuiResource: could not load texture %s!", this->texName.Get());
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiResource::Unload()
{
    n_assert(this->IsValid());
    this->refTexture->Release();
    this->refTexture.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
const rectangle&
nGuiResource::GetRelUvRect()
{
    nGuiServer* guiServer = nGuiServer::Instance();

    if (!this->IsValid())
    {
        this->Load();
    }

    vector2 corrUvPos(this->absUvRect.v0.x, this->absUvRect.v0.y);
    vector2 absUvSize = this->absUvRect.size();
    vector2 corrUvSize(absUvSize.x - 1.0f, absUvSize.y - 1.0f);

    float texHeight = float(this->GetTextureHeight()) * guiServer->GetTexelMappingRatio();
    float texWidth  = float(this->GetTextureWidth()) * guiServer->GetTexelMappingRatio();

    this->relUvRect.v0.x = (corrUvPos.x + 0.5f) / texWidth;
    this->relUvRect.v0.y = 1.0f - ((corrUvPos.y + corrUvSize.y + 1.0f) / texHeight);
    this->relUvRect.v1.x = (corrUvPos.x + corrUvSize.x + 1.0f) / texWidth;
    this->relUvRect.v1.y = 1.0f - ((corrUvPos.y + 0.5f) / texHeight);

    return this->relUvRect;
}


