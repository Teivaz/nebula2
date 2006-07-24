//-----------------------------------------------------------------------------
//  nmaxnode.cc
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxnode.h"
#include "export2/nmaxutil.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/nkernelserver.h"
#include "scene/nscenenode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxNode::nMaxNode()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxNode::~nMaxNode()
{
}

//-----------------------------------------------------------------------------
/**
    Create Nebula object of the given class name and set the created object to
    current working directory.

    The created object has its name as 3dsmax node.

    @param nodeName 3dsmax node name which to be used for Nebula object's name
    @param classname Nebula object class name.

    @return pointer to the created Nebula object.
*/
nSceneNode*
nMaxNode::CreateNebulaObject(const char* classname, const char* objectname)
{
    n_assert(classname);

    nSceneNode* sceneNode = 0;

    nKernelServer* kernelServer = nKernelServer::Instance();

    nString objectName = nMaxUtil::CorrectName(objectname);

    // append (material) id if the given node has multi-material.
    //if (useID)
    //{
    //    if (id<0)
    //        n_maxlog("");
    //    nodeName += "_";
    //    nodeName.AppendInt(id);
    //}

    sceneNode = (nSceneNode*)(kernelServer->NewNoFail(classname, objectName.Get()));
    if (sceneNode)
    {
        nRoot* cwd = kernelServer->Lookup(sceneNode->GetFullName().Get());
        kernelServer->PushCwd(cwd);

        n_maxlog(Medium, "Created '%s' Nebula object.", sceneNode->GetFullName().Get());
    }
    else
    {
        n_maxlog(Error, "Failed to create Nebula object for [%s] node.", objectname);
    }

    return sceneNode;
}