//------------------------------------------------------------------------------
//  nrenderpath.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nrenderpath.h"
#include "gfx2/nshader2.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nRenderPath::nRenderPath() :
    isOpen(false),
    inPass(false),
    inPhase(false),
    curPassIndex(0),
    curPhaseIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRenderPath::~nRenderPath()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRenderPath::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->filename.IsEmpty());
    n_assert(this->passArray.Empty());

    if (!this->ParseXmlFile())
    {
        return false;
    }
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPath::Close()
{
    n_assert(this->isOpen);
    this->passArray.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Parse the render path xml file and build pass array.
*/
bool
nRenderPath::ParseXmlFile()
{
    n_assert(!this->filename.IsEmpty());

    this->passArray.Clear();

    nString mangledPath = nFileServer2::Instance()->ManglePath(this->filename.Get());
    TiXmlDocument doc(mangledPath.Get());
    if (!doc.LoadFile())
    {
        n_error("nRenderPath: Failed to open XML file '%s'", this->filename.Get());
        return false;
    }

    TiXmlHandle docHandle(&doc);
    TiXmlElement* elmRenderPath = docHandle.FirstChildElement("RenderPath").Element();
    n_assert(elmRenderPath);
    this->name       = elmRenderPath->Attribute("name");
    this->featureSet = elmRenderPath->Attribute("featureset");
    this->shaderPath = elmRenderPath->Attribute("shaderpath");

    int curShaderIndex = 0;
    TiXmlElement* elmPass;
    for (elmPass = elmRenderPath->FirstChildElement("Pass");
         elmPass != 0;
         elmPass = elmPass->NextSiblingElement("Pass"))
    {
        Pass newPass;
        Pass& pass = this->passArray.PushBack(newPass);
        pass.name = elmPass->Attribute("name");
        pass.refShader = this->LoadShader(elmPass->Attribute("shader"));

        TiXmlElement* elmPhase;
        for (elmPhase = elmPass->FirstChildElement("Phase");
             elmPhase != 0;
             elmPhase = elmPhase->NextSiblingElement("Phase"))
        {
            Phase newPhase;
            Phase& phase = pass.phaseArray.PushBack(newPhase);
            phase.name      = elmPhase->Attribute("name");
            phase.refShader = this->LoadShader(elmPhase->Attribute("shader"));
            const char* fourccString = elmPhase->Attribute("fourcc");
            if (fourccString)
            {
                phase.fourcc = n_strtofourcc(fourccString);
            }
            phase.sortingOrder  = Phase::StringToSortingOrder(elmPhase->Attribute("sorting"));
            phase.lightsEnabled = Phase::StringToBool(elmPhase->Attribute("lights"));
            
            TiXmlElement* elmSeq;
            for (elmSeq = elmPhase->FirstChildElement("Sequence");
                 elmSeq != 0;
                 elmSeq = elmSeq->NextSiblingElement("Sequence"))
            {
                Sequence newSeq;
                Sequence& seq = phase.sequenceArray.PushBack(newSeq);
                seq.refShader = this->LoadShader(elmSeq->Attribute("shader"));

                // initialize the shader's priority index (for bucket sorting)
                seq.refShader->SetShaderIndex(curShaderIndex++);
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load and initialize one shader.
*/
nShader2*
nRenderPath::LoadShader(const nString& shaderFile)
{
    nShader2* shd = nGfxServer2::Instance()->NewShader(shaderFile.Get());
    n_assert(shd);
    if (!shd->IsValid())
    {
        shd->SetFilename(shaderFile);
        if (!shd->Load())
        {
            shd->Release();
            n_error("nRenderPath: Could not load shader '%s'!", shaderFile.Get());
            return 0;
        }
    }
    return shd;
}

//------------------------------------------------------------------------------
/**
    Begin a scene pass.
*/
void
nRenderPath::BeginPass(uint index)
{
    n_assert(!this->inPass);
    n_assert(index < this->GetNumPasses());

    this->inPass = true;
    this->curPassIndex = index;
    nShader2* shader = this->GetPass().GetShader();
    n_assert(shader);

    // note: pass shaders must save/restore shader state
    nGfxServer2::Instance()->SetShader(shader);
    int num = shader->Begin(true);
    n_assert(1 == num);
    shader->BeginPass(0);
}

//------------------------------------------------------------------------------
/**
    End a scene pass.
*/
void
nRenderPath::EndPass()
{
    n_assert(this->inPass);
    nShader2* shader = this->GetPass().GetShader();
    n_assert(shader);
    shader->EndPass();
    shader->End();
    this->inPass = false;
}

//------------------------------------------------------------------------------
/**
    Begin a phase.
*/
void
nRenderPath::BeginPhase(uint index)
{
    n_assert(this->inPass);
    n_assert(!this->inPhase);
    n_assert(index < this->GetNumPhases());

    this->inPhase = true;
    this->curPhaseIndex = index;
    nShader2* shader = this->GetPhase().GetShader();
    n_assert(shader);

    // note: phase shaders must save/restore shader state
    nGfxServer2::Instance()->SetShader(shader);
    int num = shader->Begin(true);
    n_assert(1 == num);
    shader->BeginPass(0);
}

//------------------------------------------------------------------------------
/**
    End a phase.
*/
void
nRenderPath::EndPhase()
{
    n_assert(this->inPhase);
    nShader2* shader = this->GetPhase().GetShader();
    n_assert(shader);
    shader->EndPass();
    shader->End();
    this->inPhase = false;
}

//------------------------------------------------------------------------------
/**
    Returns pointer to sequence shader defined by sequence index.
*/
nShader2*
nRenderPath::GetSequenceShader(uint index)
{
    n_assert(this->inPhase);
    return this->GetPhase().sequenceArray[index].GetShader();
}













