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
    if (this->IsDynamic())
    {
        // dynamic brush, create as render target
        this->refTexture = nGfxServer2::Instance()->NewRenderTarget(
            0,                                          // Need an unique name.
            int(this->absUvRect.v1.x),                  // Width.
            int(this->absUvRect.v1.y),                  // Height.
            nTexture2::X8R8G8B8,                        // Format.
            nTexture2::CreateEmpty | nTexture2::Dynamic | nTexture2::RenderTargetColor);
    }
    else
    {
        // static brush, load texture from disk
        n_assert(!this->texName.IsEmpty());
        if (!this->refTexture.isvalid())
        {
            this->refTexture = nGfxServer2::Instance()->NewTexture(this->texName);
        }
        if (!this->refTexture->IsValid())
        {
            this->refTexture->SetFilename(this->texName);
            if (!this->refTexture->Load())
            {
                n_error("nGuiResource: could not load texture %s!", this->texName.Get());
                return false;
            }
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

