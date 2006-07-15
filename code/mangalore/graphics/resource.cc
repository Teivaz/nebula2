//------------------------------------------------------------------------------
//  graphics/resource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/resource.h"
#include "kernel/nkernelserver.h"
#include "scene/ntransformnode.h"

namespace Graphics
{
ImplementRtti(Graphics::Resource, Foundation::RefCounted);
ImplementFactory(Graphics::Resource);

//------------------------------------------------------------------------------
/**
*/
Resource::Resource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Resource::~Resource()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Set the name of the graphics resource.

    @param  n   graphics resource name
*/
void
Resource::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
    Get the name of the graphics resource.

    @return     name of graphics resource
*/
const nString&
Resource::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
    Return true if the resource is loaded.

    @return     true if resource is loaded
*/
bool
Resource::IsLoaded() const
{
    return this->refNode.isvalid();
}

//------------------------------------------------------------------------------
/**
    Find root node for an existing sub-node by looking at the resource name
    and extracting the category and object names. May return 0.
*/
nRoot*
Resource::FindRootNode()
{
    nKernelServer* kernelServer = nKernelServer::Instance();

    // extract category and object names
    nArray<nString> tokens;
    this->name.Tokenize("/", tokens);
    n_assert(tokens.Size() >= 2);
    nString catName = tokens[0];
    nString objName = tokens[1];
    nString nodeName;
    nodeName.Format("%s/%s", catName.Get(), objName.Get());

    // find the root node...
    nRoot* rsrcPool = Foundation::Server::Instance()->GetResourcePool(Foundation::Server::GraphicsPool);
    kernelServer->PushCwd(rsrcPool);
    nRoot* rootNode = kernelServer->Lookup(nodeName.Get());
    kernelServer->PopCwd();
    return rootNode;
}

//------------------------------------------------------------------------------
/**
    Load the resource. The routine will fail hard if loading fails. If the 
    resource already exists in memory, it's refcount will be incremented.
*/
void
Resource::Load()
{
    n_assert(!this->name.IsEmpty());
    n_assert(!this->IsLoaded());
    Foundation::Server* fndServer = Foundation::Server::Instance();
    nKernelServer* kernelServer = nKernelServer::Instance();

    // split the resource name into category, object and optional node
    nArray<nString> tokens;
    this->name.Tokenize("/", tokens);
    n_assert(tokens.Size() >= 2);

    nString catName = tokens[0];
    nString objName = tokens[1];

    // ----------------------------------------------------------------------------
    // FIXME : special case handling for characters
    //         should be generalized to support more than 1 subdirectory
    nString catDir = catName;
    nString nebulaPath = this->name;
    bool char3mode = false;
    if((tokens.Size() == 3) && (catName == "characters") && (tokens[2] == "skeleton"))
    {
        // this seems to be a character
        catDir = tokens[0] + nString("/") + tokens[1];
        objName = tokens[2];
        catName = tokens[1] + "_" + tokens[2];
        tokens.Clear();
        tokens.Append(catName);
        tokens.Append(objName);
        nebulaPath = catName + "/" + objName;
        char3mode = true;
    };
    // ----------------------------------------------------------------------------


    // first check if the resource is already in memory
    nRoot* rsrcPool = fndServer->GetResourcePool(Foundation::Server::GraphicsPool);
    kernelServer->PushCwd(rsrcPool);
    this->refNode = (nTransformNode*) kernelServer->Lookup(nebulaPath.Get());
    kernelServer->PopCwd();
    if (this->refNode.isvalid())
    {
        n_assert(!this->refRootNode.isvalid());
        this->refNode->AddRef();
        if(char3mode)
        {
            this->refRootNode = this->refNode;
        }
        else
        {
            this->refRootNode = this->FindRootNode();
        };
        n_assert(this->refRootNode.isvalid());
        this->refRootNode->AddRef();
    }
    else
    {
        // create category node if necessary
        nRoot* category = 0;
        if (catName.IsValid())
        {
            category = rsrcPool->Find(catName.Get());
            if (!category)
            {
                kernelServer->PushCwd(rsrcPool);
                category = kernelServer->New("nroot", catName.Get());
                kernelServer->PopCwd();
                n_assert(category);
            }
        }

        // make sure root node doesn't exist, this may happen if the
        // same graphics object is loaded without using sub-nodes
        nRoot* oldRoot;
        while ((oldRoot = this->FindRootNode()))
        {
            oldRoot->Release();
        }

        // try to load Nebula object
        char fileName[N_MAXPATH];
        if (category)
        {
            snprintf(fileName, sizeof(fileName), "gfxlib:%s/%s.n2", catDir.Get(), objName.Get());
            kernelServer->PushCwd(category);
        }
        else
        {
            snprintf(fileName, sizeof(fileName), "gfxlib:%s.n2", objName.Get());
        }
        nRoot* obj = (nTransformNode*) kernelServer->Load(fileName);
        if (category)
        {
            kernelServer->PopCwd();
        }
        if (obj)
        {
            this->refRootNode = (nTransformNode*) obj;
            this->refNode = (nTransformNode*) obj;

            // resolve deep hierarchy node if needed
            int index;
            for (index = 2; index < tokens.Size(); index++)
            {
                this->refNode = (nTransformNode*) this->refNode->Find(tokens[index].Get());
            }
        }

        // error loading object?
        if (!this->refNode.isvalid())
        {
            n_error("Resource: Failed to load resource '%s'\n", this->GetName().Get());
        }

        // preload resources
        this->refNode->PreloadResources();
    }
}

//------------------------------------------------------------------------------
/**
    Unload the resource. This will simply decrement the ref count of
    the Nebula node.
*/
void
Resource::Unload()
{
    n_assert(this->IsLoaded());
    this->refNode->Release();
    this->refNode.invalidate();
    if (this->refRootNode.isvalid())
    {
        this->refRootNode->Release();
        this->refRootNode.invalidate();
    }
}

} // namespace Graphics
