//------------------------------------------------------------------------------
//  nrprendertarget.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "renderpath/nrprendertarget.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nRpRenderTarget::nRpRenderTarget() :
    format(nTexture2::X8R8G8B8),
    relSize(1.0f),
    width(0),
    height(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpRenderTarget::~nRpRenderTarget()
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRpRenderTarget::Validate()
{
    if (!this->refTexture.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        const nDisplayMode2& mode = gfxServer->GetDisplayMode();
        int w, h;
        if ((this->width > 0) && (this->height > 0))
        {
            // absolute size defined         
            w = this->width;
            h = this->height;
        }
        else
        {
            // use relative width
            w = int(mode.GetWidth() * this->relSize);
            h = int(mode.GetHeight() * this->relSize);
        }
        this->refTexture = nGfxServer2::Instance()->NewRenderTarget(this->name.Get(), w, h, this->format, nTexture2::RenderTargetColor);
    }
}
