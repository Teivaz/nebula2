#ifndef N_NODELIST_H
#define N_NODELIST_H
//------------------------------------------------------------------------------
/**
    @class nNodeList
    @ingroup Tools
    @brief Manages a list of Scene Objects and the depending rendering contexts

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nscriptserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "scene/nrendercontext.h"
#include "scene/ntransformnode.h"
#include "variable/nvariableserver.h"
#include "scene/nskinanimator.h"
#include "character/ncharacter3set.h"

//------------------------------------------------------------------------------
class nNodeList
{
public:
    /// the default constructor
    nNodeList();
    /// the destructor
    ~nNodeList(){};
    /// Returns the singleton of the instance
    static nNodeList* Instance();
    /// set the light stage script
    void SetStageScript(const nString& name);
    /// get the light stage script
    const nString& GetStageScript() const;
    /// enable/disable the standard light stage
    void SetLightStageEnabled(bool b);
    /// Initialize Class
    void Open ();
    /// Free Resources
    void Close ();
    /// Clear List
    void Clear();
    /// Create Default node "/usr/scene/default" and a stdLight
    void AddDefaultEntry();
    /// AddEntry
    void AddEntry(const nString& name);
    /// Load an object
    void LoadObject(const nString& objPath);
    /// Load an object and animate it by the given hardpoint(jointIndex)
    void LoadObjectAndAttachToHardpoint(const nString& objPath,int jointIndex);
    /// update vars
    void Trigger(double time, uint frameId);

    /// check rendercontext, global vars, etc
    void TransferGlobalVars(nRenderContext &context,double time,uint frameId);
    /// add Default Variables
    void AddDefaultVariables(nRenderContext& context);
    /// currently open?
    bool IsOpen() const;
    /// return number of objects
    uint GetCount()const;
    /// get render context at index
    nRenderContext* GetRenderContextAt(uint index) const;
    /// get transform node at index
    nTransformNode* GetNodeAt(uint index) const;

    /// give a character3 set; from now on owned by this class
    void GiveCharacter3Set(nCharacter3Set* charSet);
    /// get character3set
    nCharacter3Set* GetCharacter3Set();

private:

    // Created Structure
    //
    // -scene+-default-light1
    //       +-loaded.obj.bla.1- ...
    //       +-loaded.obj.bli.2- ...
    //       +-loaded.obj.blu.3- ...

    /// Get Pointer to the first nCharacter2 Object from the current rendercontexts
    nCharacter2*    GetCharacter();
    nCharacter3Set* character3Set;

    static nNodeList* Singleton;
    uint numElements;
    nArray<nRenderContext> renderContexts;
    nArray< nRef<nTransformNode> >  refNodes;
    nAutoRef<nScriptServer> refScriptServer;
    nRef<nSceneNode> refUsrScene;
    bool isOpen;
    nVariable::Handle timeHandle;
    bool lightStageEnabled;
    nString stageScript;
    // FIXME: Quick and dirty
    int                     hardpointObjectsCnt;
    nArray<int>             hardpointJointIndex;
    nArray<nSceneNode*>     hardpointPtr;
};

//-----------------------------------------------------------------------------
/**
*/
inline
uint
nNodeList::GetCount() const
{
    return this->numElements;
}

//------------------------------------------------------------------------------
/**
    returns the singleton of the nNodeList
*/
inline
nNodeList*
nNodeList::Instance()
{
    n_assert(0 != nNodeList::Singleton);
    return nNodeList::Singleton;
}

//------------------------------------------------------------------------------
/**
    returns the render context of Object [index] inside the list
*/
inline
nRenderContext*
nNodeList::GetRenderContextAt(uint index) const
{
    n_assert( index < numElements);
    return &renderContexts[index];
}

//------------------------------------------------------------------------------
/**
    returns the root-node of Object [index] inside the list
*/
inline
nTransformNode*
nNodeList::GetNodeAt(uint index) const
{
    n_assert( index < numElements);
    n_assert( refNodes[index].isvalid() );
    return refNodes[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nNodeList::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNodeList::SetLightStageEnabled(bool b)
{
    this->lightStageEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNodeList::GiveCharacter3Set(nCharacter3Set* charSet)
{
    this->character3Set = charSet;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter3Set*
nNodeList::GetCharacter3Set()
{
    return this->character3Set;
}


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
inline
void
nNodeList::SetStageScript(const nString& script)
{
    this->stageScript = script;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nNodeList::GetStageScript() const
{
    return this->stageScript;
}

//------------------------------------------------------------------------------

#endif
