//------------------------------------------------------------------------------
//  nnodelist.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nnodelist.h"

nNodeList* nNodeList::Singleton = 0;

//-----------------------------------------------------------------------------
/**
*/
nNodeList::nNodeList(void):
    refScriptServer("/sys/servers/script"),
    numElements(0),
    isOpen(false),
    lightStageEnabled(true),
    hardpointObjectsCnt(0),
    character3Set(0)
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

    // if we have a character3set, we must delete it
    if(this->character3Set)
    {
        delete this->character3Set;
    };

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

    this->hardpointObjectsCnt = 0;
    this->hardpointJointIndex.Clear();
    this->hardpointPtr.Clear();

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
    if (this->lightStageEnabled)
    {
        if (!this->GetStageScript().IsEmpty())
        {
            nRoot* node = nKernelServer::Instance()->Lookup("/usr/scene/default");
            if (node)
            {
                nKernelServer::Instance()->PushCwd(node);
                nKernelServer::Instance()->Load(this->GetStageScript().Get());
                nKernelServer::Instance()->PopCwd();
            }
        }
        else
            n_printf("WARNING: No light stage set.");
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
    nString nodeName;
    nodeName.Format("%s.%d", tmpString.Get(), numElements);

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
    Get Pointer to the first nCharacter2 Object from the current rendercontexts
*/
nCharacter2*
nNodeList::GetCharacter()
{
    nVariable::Handle charHandle = nVariableServer::Instance()->GetVariableHandleByName("charPointer");
    const nVariable* charVar = 0;

    int i;
    for( i = 0 ; i < renderContexts.Size() ; i++)
    {
        charVar = renderContexts.At(i).FindLocalVar(charHandle);
        if( 0 != charVar ) break;
    };

    // break if there is no characterVariable in the current context
    if(charVar == 0) return 0;

    nCharacter2* curCharacter = (nCharacter2*) charVar->GetObj();
    n_assert(curCharacter);
    return curCharacter;
};

//------------------------------------------------------------------------------
/**
    Loads an object and attaches it to the given animator
*/
void
nNodeList::LoadObjectAndAttachToHardpoint(const nString& objPath,int jointIndex)
{
    // load Object inside an individual Node
    nString tmpString = objPath.ExtractFileName();
    tmpString.StripExtension();
    nString nodeName;
    nodeName.Format("%s.%d", tmpString.Get(), numElements);

    AddEntry(nodeName);

    // load new object
    nKernelServer* kernelServer = nKernelServer::Instance();
    kernelServer->PushCwd(this->refUsrScene);
    kernelServer->SetCwd( this->refNodes.Back() );
    kernelServer->Load(objPath.Get());
    kernelServer->PopCwd();

    refNodes.Back()->RenderContextCreated(&this->renderContexts.Back());

    // Add the Object to the list of objects to animate
    this->hardpointObjectsCnt++;
    this->hardpointJointIndex.PushBack(jointIndex);
    this->hardpointPtr.PushBack(refNodes.Back());

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

    // Apply animation on the Node that was attached to a Hardpoint
    if(this->hardpointObjectsCnt > 0)
    {
        nCharacter2* curChar = GetCharacter();
        if( 0 != curChar )
        {
            nCharSkeleton& skel = curChar->GetSkeleton();
            int i;
            for( i = 0 ; i < hardpointObjectsCnt ; i++ )
            {
                nCharJoint joint = skel.GetJointAt(this->hardpointJointIndex.At(i));
                nSceneNode* currentNode = this->hardpointPtr.At(i);
                if(currentNode->HasTransform())
                {
                    nTransformNode* transformNode = (nTransformNode*) currentNode;
                    transformNode->SetTransform(joint.GetMatrix());
                };
            };
        };
    };
}
//------------------------------------------------------------------------------
/**
    Transfers globals vars for a specific rendering context
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
