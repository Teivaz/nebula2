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

