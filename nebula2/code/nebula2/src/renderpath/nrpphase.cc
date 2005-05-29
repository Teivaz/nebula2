//------------------------------------------------------------------------------
//  nrpphase.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "renderpath/nrpphase.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nRpPhase::nRpPhase() :
    inBegin(false),
    sortingOrder(FrontToBack),
    lightsEnabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpPhase::~nRpPhase()
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
int
nRpPhase::Begin()
{
    n_assert(!this->inBegin);

    // note: save/restore state for phase shaders!
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nShader2* shd = this->refShader;
    if (!this->technique.IsEmpty())
    {
        shd->SetTechnique(this->technique.Get());
    }
    gfxServer->SetShader(shd);
    int numShaderPasses = shd->Begin(true);
    n_assert(1 == numShaderPasses); // assume 1-pass phase shader!
    shd->BeginPass(0);

    this->inBegin = true;
    return this->sequences.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
nRpPhase::End()
{
    n_assert(this->inBegin);

    nShader2* shd = this->refShader;
    shd->EndPass();
    shd->End();

    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nRpPhase::Validate()
{
    // invoke validate on sequences
    int i;
    int num = this->sequences.Size();
    for (i = 0; i < num; i++)
    {
        this->sequences[i].Validate();
    }

    // validate shader
    nShader2* shd = 0;
    if (!this->refShader.isvalid())
    {
        n_assert(!this->shaderPath.IsEmpty());
        shd = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
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


