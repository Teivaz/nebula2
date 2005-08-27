//------------------------------------------------------------------------------
/**
    @class nNodeList
    @ingroup Tools

    Implements the Root Node of an Object and a depending render context

    (C) 2004 RadonLabs GmbH
*/
#include "tools/nnodelist.h"

nNodeList* nNodeList::Singleton = 0;

//-----------------------------------------------------------------------------
/**
*/
nNodeList::nNodeList(void):
    refScriptServer("/sys/servers/script"),
    numElements(0),
    isOpen(false),
    lightStageEnabled(true)
{
    n_assert(nNodeList::Singleton == 0);
    this->Singleton = this;
}

//------------------------------------------------------------------------------
/**
    Initializes the List and creates a default entry for the default light
*/
void 
nNodeList::Open ()
{
    if (this->isOpen) 
    {
        return;
    }

    n_assert(this->refScriptServer.isvalid());
    
    this->timeHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
    
    this->Clear();
    this->AddDefaultEntry();
    
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
    Releases all Resources
*/
void 
nNodeList::Close ()
{
    n_assert(this->IsOpen());

    this->Clear(); // empty the list
            
    if (this->refUsrScene.isvalid()) 
    {
        this->refUsrScene->Release();
    }
        
    // verify if everything is clean
    n_assert(this->refUsrScene == 0);
    n_assert(renderContexts.Size() == 0);
    n_assert(refNodes.Size() == 0);

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Empty the Node-List and recreate /usr/scene and a default entry + light
*/
void 
nNodeList::Clear() 
{        
    uint index;
    for (index = 0; index < numElements; index++)
    {
        this->refNodes[index]->RenderContextDestroyed(&(this->renderContexts[index]));
    }        
    this->numElements = 0;
    
    this->renderContexts.Clear();
    this->refNodes.Clear();
    
    if (this->refUsrScene.isvalid()) 
    {
        this->refUsrScene->Release();
    }

    // create the /usr/scene object
    if (!this->refUsrScene.isvalid())
    {
        this->refUsrScene = (nSceneNode*) nKernelServer::Instance()->New("nscenenode", "/usr/scene");
    }
    
    n_assert(this->refUsrScene.isvalid());
} 
//------------------------------------------------------------------------------
/**
    Creates the /usr/scene/default node with an additional light-source
*/
void 
nNodeList::AddDefaultEntry()
{
    this->AddEntry("default");
            
    // source the light stage...
    // load the stage, normally this is "proj:stdlight.tcl", if not exists
    // then try "home:stdlight.tcl"
    if (this->lightStageEnabled)
    {
        nString result;
        this->refScriptServer->RunScript("home:bin/stdlight.tcl", result);
    }
}
//------------------------------------------------------------------------------
/**
    Creates new entry in /usr/scene/
*/
void 
nNodeList::AddEntry(const nString& name)
{
    nString nodePath = "/usr/scene/" + name;
    
    // Create new node in /usr/scene
    nTransformNode* object = (nTransformNode*) nKernelServer::Instance()->New("ntransformnode", nodePath.Get());
        
    refNodes.Append(object);
    
    // Create new RenderContext & connect with the new Object
    nRenderContext newRenderContext;
    AddDefaultVariables(newRenderContext);
    newRenderContext.SetRootNode(object);
    renderContexts.Append(newRenderContext);
    object->RenderContextCreated(&this->renderContexts[numElements]);
    
    // number of elements inside the array
    numElements++;
}
//------------------------------------------------------------------------------
/**
    Loads and initialized object in /usr/scene/
*/
void 
nNodeList::LoadObject(const nString& objPath) 
{ 
    // load Object inside an individual Node
    nString tmpString = objPath.ExtractFileName();
    tmpString.StripExtension();
    nString nodeName = tmpString + "." + ((int)numElements);
    
    AddEntry(nodeName);

    // load new object
    nKernelServer* kernelServer = nKernelServer::Instance();
    kernelServer->PushCwd(this->refUsrScene);
    kernelServer->SetCwd( this->refNodes.Back() );
    kernelServer->Load(objPath.Get());
    kernelServer->PopCwd();
    
    refNodes.Back()->RenderContextCreated(&this->renderContexts.Back());

    // reset time
    nTimeServer::Instance()->ResetTime();
}
//------------------------------------------------------------------------------
/**
    Updates global varibles for all rendercontexts
*/
void 
nNodeList::Trigger(double time, uint frameId)
{   
    n_assert ( this->refUsrScene.isvalid() )
    
    // Update all Variables
    uint index;
    for( index=0 ; index < numElements ; index++ )
        TransferGlobalVars(renderContexts[index],time,frameId);
}
//------------------------------------------------------------------------------
/**
    Transfers globals vars for a spefific rendering context
*/
void 
nNodeList::TransferGlobalVars(nRenderContext &context,double time,uint frameId)
{ 
    context.GetVariable(timeHandle)->SetFloat((float)time);
    context.SetFrameId(frameId);
    
    // Transfer global variables
    const nVariableContext& globalContext = nVariableServer::Instance()->GetGlobalVariableContext();
    int numGlobalVars = globalContext.GetNumVariables();
    
    int globalVarIndex;
    for (globalVarIndex = 0; globalVarIndex < numGlobalVars; globalVarIndex++)
    {
        const nVariable& globalVar = globalContext.GetVariableAt(globalVarIndex);
        nVariable* var = context.GetVariable(globalVar.GetHandle());
        if (var)
        {
            *var = globalVar;
        }
        else
        {
            nVariable newVar(globalVar);
            context.AddVariable(newVar);
        }
    }
}
//------------------------------------------------------------------------------
/**
    Creates default variables for a given render context
*/
void 
nNodeList::AddDefaultVariables(nRenderContext& context)
{
    static const nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    nVariable::Handle oneHandle  = nVariableServer::Instance()->GetVariableHandleByName("one");
    nVariable::Handle windHandle = nVariableServer::Instance()->GetVariableHandleByName("wind");
    context.AddVariable(nVariable(this->timeHandle, 0.5f));
    context.AddVariable(nVariable(oneHandle, 1.0f));
    context.AddVariable(nVariable(windHandle, wind));
}
//------------------------------------------------------------------------------
