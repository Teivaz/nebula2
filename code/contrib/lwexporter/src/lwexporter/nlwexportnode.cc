//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportnode.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "scene/nshapenode.h"
#include "scene/nskinshapenode.h"
#include "lwwrapper/nlwcallbacktemplates.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwwrapper/nlwlayoutmonitor.h"
#include "lwexporter/nlwtextureexport.h"
#include "lwexporter/nlwexportnodetype.h"
#include "lwexporter/nlwmeshexport.h"
#include "lwexporter/nlwskinanimexport.h"
#include "lwexporter/nlwexporter.h"

//----------------------------------------------------------------------------
/**
*/
nLWExportNode::nLWExportNode(nLWObjectExportSettings* settings,
                             nLWCmdExec* cmdExec) :
    nodeType(0),
    settings(settings),
    children(0, 0),
    cmdExec(cmdExec)
{
    n_assert(settings);
    if (settings)
    {
        nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
        n_assert(exporterSettings);
        if (exporterSettings)
        {
            this->nodeType = exporterSettings->GetExportNodeType(settings->GetExportNodeType());
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWExportNode::~nLWExportNode()
{
    this->ClearChildren();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNode::ClearChildren()
{
    for (int i = 0; i < this->children.Size(); i++)
    {
        n_delete(this->children[i]);
        this->children[i] = 0;
    }

    // array only holds pointers so we can safely Reset() instead of Clear()
    this->children.Reset();
}

//----------------------------------------------------------------------------
/**
    @brief Check if this node will export it's descendants.
    @return true if descendants will be exported, false otherwise.
*/
bool
nLWExportNode::GetExportHierarchy()
{
    n_assert(this->settings);
    if (this->settings)
        return this->settings->GetExportSceneNodeHierarchy();

    return true;
}

//----------------------------------------------------------------------------
/**
    @brief Collect all of the descendants of this node.
*/
void
nLWExportNode::CollectDescendants(const nLWGlobals::ItemInfo* itemInfo,
                                  const nLWSettingsRegistry* registry)
{
    n_assert(itemInfo);
    n_assert(registry);

    n_assert(this->settings);
    if (!this->settings)
        return;

    this->ClearChildren();

    // the user explicitly told us not to export the descendants
    if (!this->settings->GetExportSceneNodeHierarchy())
        return;

    // figure out how many children the node has so we can allocate storage in one go
    LWItemID parentItemId = this->settings->GetItemId();
    int numChildren = 0;
    LWItemID childItemId = itemInfo->Get()->firstChild(parentItemId);
    while (childItemId != LWITEM_NULL)
    {
        numChildren++;
        childItemId = itemInfo->Get()->nextChild(parentItemId, childItemId);
    }
    this->children.Reallocate(numChildren, 0);

    // collect child nodes and recurse for each
    childItemId = itemInfo->Get()->firstChild(parentItemId);
    while (childItemId != LWITEM_NULL)
    {
        switch (itemInfo->Get()->type(childItemId))
        {
            case LWI_OBJECT:
            {
                nLWObjectExportSettings* settings = registry->Get(childItemId);
                if (settings)
                {
                    nLWExportNode* expNode = n_new(nLWExportNode(settings, this->cmdExec));
                    expNode->CollectDescendants(itemInfo, registry);
                    this->children.PushBack(expNode);
                }
                break;
            }

            default:
                break;
        }
        childItemId = itemInfo->Get()->nextChild(parentItemId, childItemId);
    }
}

//----------------------------------------------------------------------------
/**
    @brief Get the number of immediate child nodes of this node.
*/
int
nLWExportNode::GetNumChildren()
{
    return this->children.Size();
}

//----------------------------------------------------------------------------
/**
    @brief Get the total number of descendants of this node.
*/
int
nLWExportNode::GetNumDescendants()
{
    int count = 0;
    for (int i = 0; i < this->children.Size(); i++)
    {
        count += this->children[i]->GetNumDescendants() + 1;
    }
    return count;
}

//----------------------------------------------------------------------------
/**
    @brief Check if this node is an ancestor of the given node.
    @note This only works if CollectChildren() has been called for this node.
*/
bool
nLWExportNode::IsAncestorOf(nLWExportNode* otherNode)
{
    if (-1 != this->children.FindIndex(otherNode))
        return true;

    for (int i = 0; i < this->children.Size(); i++)
    {
        if (this->children[i]->IsAncestorOf(otherNode))
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
/**
    Entry point from exporter.
*/
void
nLWExportNode::Export(nLWLayoutMonitor* monitor)
{
    // get the name of the node item
    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
    {
        n_printf("[ERROR] Invalid nLWItemInfoGlobal in nLWExportNode::Export().\n");
        return;
    }

    nString rawItemName(itemInfo.Get()->name(this->settings->GetItemId()));
    nString itemName(rawItemName);
    itemName.ReplaceIllegalFilenameChars('_');
    nString monitorMsg;
    monitorMsg.Format("[INFO] Started exporting %s as %s\n", rawItemName.Get(),
                      itemName.Get());

    if (monitor)
    {
        monitor->Step(0, monitorMsg);
    }
    n_printf(monitorMsg.Get());

    // create scene node(s) and export mesh
    nSceneNode* sceneNode = 0;
    if (this->nodeType)
    {
        nString sceneNodeType = this->nodeType->GetSceneNodeType();
        if ("nshapenode" == sceneNodeType)
        {
            sceneNode = this->ExportShapeNode(itemName, monitor);
            if (!sceneNode)
            {
                // TODO: notify user
                return;
            }
        }
        else if ("nskinshapenode" == sceneNodeType)
        {
            sceneNode = this->ExportSkinShapeNode(itemName, monitor);
            if (!sceneNode)
            {
                // TODO: notify user
                return;
            }
        }
        else
        {
            this->ExportResourcesOnly(itemName, monitor);
        }
    }

    // recursively export child nodes
    if ((this->GetNumChildren() > 0))
    {
        if (sceneNode)
        {
            nKernelServer::Instance()->PushCwd(sceneNode);
        }
        for (int i = 0; i < this->children.Size(); i++)
        {
            this->children[i]->Export(monitor);
        }
        if (sceneNode)
        {
            nKernelServer::Instance()->PopCwd();
        }
    }

    // TODO: update the bounding box for the sceneNode if it has
    // children (SetLocalBox())

    monitorMsg.Format("[INFO] Finished exporting %s\n", itemName.Get());
    if (monitor)
    {
        monitor->Step(1, monitorMsg);
    }
    n_printf(monitorMsg.Get());
}

//----------------------------------------------------------------------------
/**
    @brief Create a visual node sub-tree to represent a static mesh object
           in Nebula 2.
    @return Root scene node of the visual node sub-tree.

    If the object has multiple surfaces multiple nShapeNode(s) will be created
    under a single nTransformNode, one nShapeNode per surface. If the object
    consists of only one surface then only one nShapeNode will be created.
    Any mesh and texture data the sub-tree require will also be exported.
*/
nSceneNode*
nLWExportNode::ExportShapeNode(const nString& topNodeName,
                               nLWLayoutMonitor* monitor)
{
    nKernelServer* ks = nKernelServer::Instance();
    nSceneNode* topSceneNode = 0;
    nString monitorMsg;

    nLWMeshExport meshExport(topNodeName, this->settings, monitor);
    if (!meshExport.Run())
    {
        return 0;
    }

    if (meshExport.GetNumGroups() > 1)
    {
        // a bunch of shape nodes grouped under a transform node,
        // one shape node per material
        topSceneNode = static_cast<nSceneNode*>(ks->NewNoFail("ntransformnode",
                                                              topNodeName.Get()));
        topSceneNode->SetLocalBox(meshExport.GetBBox());
        ks->PushCwd(topSceneNode);
        for (int i = 0; i < meshExport.GetNumGroups(); i++)
        {
            nLWShaderExportSettings* shaderSettings = meshExport.GetShaderForGroup(i);
            if (shaderSettings && shaderSettings->Valid())
            {
                nString nodeName;
                nodeName.Format("%s_%d", topNodeName.Get(), i);
                nShapeNode* shapeNode = static_cast<nShapeNode*>(ks->NewNoFail("nshapenode",
                                                                               nodeName.Get()));
                shaderSettings->CopyNonTextureParamsTo(shapeNode);
                nLWTextureExport textureExport(shaderSettings);
                if (textureExport.Run())
                {
                    textureExport.CopyTextureParamsTo(shapeNode);
                }
                shapeNode->SetLocalBox(meshExport.GetGroupBBox(i));
                shapeNode->SetMesh(meshExport.GetMeshFileName().Get());
                shapeNode->SetGroupIndex(i);
            }
        }
        ks->PopCwd();
    }
    else
    {
        // just the one shape node
        nLWShaderExportSettings* shaderSettings = meshExport.GetShaderForGroup(0);
        if (shaderSettings && shaderSettings->Valid())
        {
            nShapeNode* shapeNode = static_cast<nShapeNode*>(ks->NewNoFail("nshapenode",
                                                                           topNodeName.Get()));
            // set the shader and shader params
            shaderSettings->CopyNonTextureParamsTo(shapeNode);
            nLWTextureExport textureExport(shaderSettings);
            if (textureExport.Run())
            {
                textureExport.CopyTextureParamsTo(shapeNode);
            }
            shapeNode->SetLocalBox(meshExport.GetBBox());
            shapeNode->SetMesh(meshExport.GetMeshFileName().Get());
            shapeNode->SetGroupIndex(0);

            topSceneNode = shapeNode;
        }
    }

    return topSceneNode;
}

//----------------------------------------------------------------------------
/**
    @brief Create a visual node sub-tree to represent a bone-animated mesh
           object in Nebula 2.
    @return Root scene node of the visual node sub-tree.

    The root of the visual node sub-tree will be an nTransformNode, with one
    nSkinAnimator below it and one or more nSkinShapeNode(s)
    (one nSkinShapeNode per surface). Any mesh, texture and animation data the
    sub-tree requires will also be exported.
*/
nSceneNode*
nLWExportNode::ExportSkinShapeNode(const nString& topNodeName,
                                   nLWLayoutMonitor* monitor)
{
    nKernelServer* ks = nKernelServer::Instance();

    // write out mesh file
    nLWMeshExport meshExport(topNodeName, settings, monitor);
    if (!meshExport.Run())
    {
        return 0;
    }

    // one skin animator and one or more skin shape nodes grouped under
    // a transform node
    nObject* object = ks->NewNoFail("ntransformnode", topNodeName.Get());
    nSceneNode* topSceneNode = static_cast<nSceneNode*>(object);
    if (!topSceneNode)
    {
        return 0;
    }

    // TODO: the bounding box is wrong if the node has child nodes
    //topSceneNode->SetLocalBox(meshExport.GetBBox());
    ks->PushCwd(topSceneNode);

    // create the skin animator node
    nLWSkinAnimExport skinAnimExport(settings, this->cmdExec);
    if (!skinAnimExport.Run(monitor))
    {
        return 0;
    }

    for (int i = 0; i < meshExport.GetNumGroups(); i++)
    {
        nLWShaderExportSettings* shaderSettings = meshExport.GetShaderForGroup(i);
        if (shaderSettings && shaderSettings->Valid())
        {
            nString nodeName("skin_");
            nodeName.AppendInt(i);
            object = ks->NewNoFail("nskinshapenode", nodeName.Get());
            nSkinShapeNode* skinShapeNode = static_cast<nSkinShapeNode*>(object);
            // Testing:
            //nSkinShapeNode* skinShapeNode = static_cast<nSkinShapeNode*>(ks->NewNoFail("ncharskeletonnode",
            //                                                             nodeName.Get()));
            shaderSettings->CopyNonTextureParamsTo(skinShapeNode);
            nLWTextureExport textureExport(shaderSettings);
            if (textureExport.Run())
            {
                textureExport.CopyTextureParamsTo(skinShapeNode);
            }
            skinShapeNode->SetLocalBox(meshExport.GetGroupBBox(i));
            skinShapeNode->SetMesh(meshExport.GetMeshFileName().Get());
            skinShapeNode->SetGroupIndex(i);
            // use the skin animator we prepared earlier
            skinShapeNode->SetSkinAnimator("../skinanimator");
            // specify mesh fragments
            const nArray<int>& groupFrags = meshExport.GetGroupFragments(i);
            skinShapeNode->BeginFragments(groupFrags.Size());
            for (int groupFragIdx = 0; groupFragIdx < groupFrags.Size(); groupFragIdx++)
            {
                int meshFragIdx = groupFrags[groupFragIdx];
                skinShapeNode->SetFragGroupIndex(meshFragIdx, meshFragIdx);

                const nArray<int>& fragJoints = meshExport.GetFragmentJoints(meshFragIdx);
                skinShapeNode->BeginJointPalette(meshFragIdx, fragJoints.Size());
                for (int fragJointIdx = 0; fragJointIdx < fragJoints.Size(); fragJointIdx++)
                {
                    skinShapeNode->SetJointIndex(meshFragIdx, fragJointIdx, fragJoints[fragJointIdx]);
                }
                skinShapeNode->EndJointPalette(meshFragIdx);
            }
            skinShapeNode->EndFragments();
        }
    }

    ks->PopCwd();

    return topSceneNode;
}

//----------------------------------------------------------------------------
/**
    @brief Export just the raw resources for an object, without creating a
           visual node sub-tree.

    Only certain mesh data is handled here at the moment (for collision
    meshes), will probably be expanded in the future to handle textures
    and animations.
*/
void
nLWExportNode::ExportResourcesOnly(const nString& topNodeName,
                                   nLWLayoutMonitor* monitor)
{
    nLWExporterSettings* exps = nLWExporterSettings::Instance();
    n_assert(exps);
    if (!exps)
        return;

    nLWExportNodeType* exportType = 0;
    exportType = exps->GetExportNodeType(this->settings->GetExportNodeType());

    if (exportType->GetVertexComponentMask() != 0)
    {
        // write out mesh data
        nLWMeshExport meshExport(topNodeName, settings, monitor);
        if (!meshExport.Run())
        {
            return;
        }
    }

    // TODO: textures, animations
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
