//------------------------------------------------------------------------------
//  nrpsequence.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "renderpath/nrpsequence.h"
#include "renderpath/nrenderpath2.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nRpSequence::nRpSequence()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpSequence::~nRpSequence()
{
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRpSequence::Validate()
{
    // validate shader
    nShader2* shd = 0;
    if (!this->refShader.isvalid())
    {
        n_assert(!this->shaderPath.IsEmpty());
        shd = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
        shd->SetShaderIndex(nRenderPath2::Instance()->GetSequenceShaderAndIncrement());
        this->refShader = shd;
    }
    else
    {
        shd = this->refShader;
    }

    n_assert(shd);
    if (!shd->IsLoaded())
    {
        shd->SetFilename(this->shaderPath);
        if (!shd->Load())
        {
            shd->Release();
            n_error("nRpPass: could not load shader '%s'!", this->shaderPath.Get());
        }
    }
}    
