//------------------------------------------------------------------------------
//  nrenderpath2.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "renderpath/nrenderpath2.h"
#include "renderpath/nrprendertarget.h"
#include "renderpath/nrpxmlparser.h"

nRenderPath2* nRenderPath2::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nRenderPath2::nRenderPath2() :
    isOpen(false),
    inBegin(false),
    sequenceShaderIndex(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nRenderPath2::~nRenderPath2()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the XML document. This will just load the XML document and
    initialize the shader path. The rest of the initialization happens
    inside nRenderPath2::Open(). This 2-step approach is necessary to
    prevent a shader initialization chicken/egg problem 
*/
bool
nRenderPath2::OpenXml()
{
    this->xmlParser.SetRenderPath(this);
    if (!this->xmlParser.OpenXml())
    {
        return false;
    }
    n_assert(!this->shaderPath.IsEmpty());
    n_assert(!this->name.IsEmpty());
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the XML document. This method should be called after
    nRenderPath2::Open() to release the memory assigned to the XML
    document data.
*/
void
nRenderPath2::CloseXml()
{
    this->xmlParser.CloseXml();
}

//------------------------------------------------------------------------------
/**
    Open the render path. This will parse the xml file which describes
    the render path and configure the render path object from it.
*/
bool
nRenderPath2::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->inBegin);

    this->sequenceShaderIndex = 0;
    if (!xmlParser.ParseXml())
    {
        return false;
    }
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the render path. This will delete all embedded objects.
*/
void
nRenderPath2::Close()
{
    n_assert(this->isOpen);
    n_assert(!this->inBegin);
    this->name.Clear();
    this->passes.Clear();
    this->renderTargets.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering the render path. This will validate all embedded objects.
    Returns the number of scene passes in the render path.
    After begin, each pass should be "rendered" recursively.
*/
int
nRenderPath2::Begin()
{
    n_assert(!this->inBegin);
    this->Validate();
    this->inBegin = true;
    return this->passes.Size();
}

//------------------------------------------------------------------------------
/**
    Finish rendering the render path.
*/
void
nRenderPath2::End()
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
    Validate the render path. This will simply invoke Validate() on all
    render targets and pass objects.
*/
void
nRenderPath2::Validate()
{
    int passIndex;
    int numPasses = this->passes.Size();
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        this->passes[passIndex].Validate();
    }
}

//------------------------------------------------------------------------------
/**
    Find a render target by name.
*/
nRpRenderTarget*
nRenderPath2::FindRenderTarget(const nString& n) const
{
    int i;
    int num = this->renderTargets.Size();
    for (i = 0; i < num; i++)
    {
        if (this->renderTargets[i].GetName() == n)
        {
            return &(this->renderTargets[i]);
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPath2::AddRenderTarget(nRpRenderTarget& rt)
{
    rt.Validate();
    this->renderTargets.Append(rt);
}
