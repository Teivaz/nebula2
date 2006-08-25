//-----------------------------------------------------------------------------
//  nmaxscene.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "export2/nmaxutil.h"

#include "tools/nmeshbuilder.h"

#include "export2/nmaxmesh.h"
#include "export2/nmaxscene.h"
#include "export2/nmaxoptions.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxcamera.h"
#include "export2/nmaxlight.h"
#include "export2/nmaxdummy.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxskinanimator.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxskinpartitioner.h"
#include "export2/nmaxanimator.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/npersistserver.h"
#include "variable/nvariableserver.h"
#include "scene/ntransformnode.h"
#include "scene/nskinshapenode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxScene::nMaxScene() :
    sceneRoot (0),
    exportRoot(0),
    boneManager(0)
{
}

//-----------------------------------------------------------------------------
/**
    @note
        Be careful, the destructor is called when the log dialog box is closed.
        So put all necessary tasks for releasing, deallocating resources etc.
        under the End() function.
*/
nMaxScene::~nMaxScene()
{
    //empty.
}

//-----------------------------------------------------------------------------
/**
    Export given 3dsmax scene.

    @param inode node to export. If it is Null, the plugin exports whole scene.

    @return Return false if it fails to export the given node.
*/
bool nMaxScene::Export(INode* inode)
{
    n_maxlog(Low, "Start exporting.");

    // retrieves the root node.
    INode* rootNode = nMaxInterface::Instance()->GetRootNode();
    if (rootNode)
    {
        this->sceneRoot = rootNode;
    }
    else
    {
        n_maxlog(Error, "No root node exist.");
        return false;
    }
    
    // prepares export and do needed preprocess.
    if(!this->Begin(rootNode))
    {
        return false;
    }

    // if the given node is NULL, it passes scene root node.
    INode* node = (inode ? inode : rootNode);

    // recursively exports nodes in the scene.
    if (!this->ExportNodes(node))
    {
        return false;
    }

    // release resources and save the results.
    if (!this->End())
    {
        return false;
    }
    
    n_maxlog(Low, "End exporting.");

    return true;
}

//-----------------------------------------------------------------------------
/**
    Begin scene.
*/
bool nMaxScene::Begin(INode *rootNode)
{
    // preprocess.
    if(!this->Preprocess(rootNode))
    {
        n_maxlog(Error, "Failed to preprocess of the scene");
        return false;
    }

    if (!this->OpenNebula())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Do any preprocessing for this scene. 
    This is called before the scene is exported.

    @param root scene root node which retrieved from core interface.
*/
bool nMaxScene::Preprocess(INode* root)
{
    this->CollectTopLevelNodes(root);
    if (this->topLevelNodes.Size() <= 0)
    {
        n_maxlog(Warning, "The scene has no nodes to export.");
        return false;
    }

    //this->InitializeNodes(root);

    // Build bone list.
    n_maxlog(Medium, "Start to build bone list.");

    this->boneManager = nMaxBoneManager::Instance();
    if (!this->boneManager->BuildBones(root))
    {
        n_maxlog(Error, "Failed to build bone list.");
        return false;
    }

    this->globalMeshBuilder.Clear();

    // Disable physique modifier to get skin in the initial pose.
    // ...

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void SetFlags(ReferenceMaker *pRefMaker, int iStat)
{
    for (int i = 0; i < pRefMaker->NumRefs(); i++) 
    {
        ReferenceMaker *pChildRef = pRefMaker->GetReference(i);
        if (pChildRef) 
            SetFlags(pChildRef, iStat);
    }

    switch (iStat) 
    {
    case 0:
        pRefMaker->ClearAFlag(A_WORK1);
        break;
    case 1:
        pRefMaker->SetAFlag(A_WORK1);
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxScene::InitializeNodes(INode* inode)
{
    SetFlags(inode, 0);

    for (int i=0; i<inode->NumberOfChildren(); i++)
        InitializeNodes(inode->GetChildNode(i));

    ObjectState kOState = inode->EvalWorldState(0);
    Object* obj = kOState.obj;
    if (!obj) 
        return;

    if (obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
        SetFlags(obj, 0);

    Object* objectRef = inode->GetObjectRef();

    // Disable Skin Modifier to get the skin in the initial pose.
    Modifier* mod = nMaxUtil::FindModifier(objectRef, SKIN_CLASSID);

    if (mod)
        mod->DisableMod();

    // Disable Physique Modifier to get the skin in the initial pose.
    mod = nMaxUtil::FindModifier(objectRef, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    if (mod)
        mod->DisableMod();
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxScene::UnInitializeNodes(INode* inode)
{
    // Enable Skin Modifier.
    Object* objectRef = inode->GetObjectRef();

    Modifier* mod;

    mod = nMaxUtil::FindModifier(objectRef, SKIN_CLASSID);
    if (mod)
        mod->EnableMod();

    // Enable Physique Modifier.
    mod = nMaxUtil::FindModifier(objectRef,Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    if (mod)
        mod->EnableMod();

    for (int i=0; i<inode->NumberOfChildren(); i++)
        UnInitializeNodes(inode->GetChildNode(i));
}

//-----------------------------------------------------------------------------
/**
    Nebula specific initializations.
*/
bool nMaxScene::OpenNebula()
{
    nKernelServer* ks = nKernelServer::Instance();

    // prepare persistence server.
    nPersistServer* persisitServer = ks->GetPersistServer();
    persisitServer->SetSaverClass(nMaxOptions::Instance()->GetSaveScriptServer());

    // need to create nskinanimator.
    varServer = (nVariableServer*)ks->NewNoFail("nvariableserver", "/sys/servers/variable");

    // Make the scene base object which to be exported in final export stage.
    // All nodes which exported should be under this node as child node of this.
    exportRoot = static_cast<nSceneNode*>(ks->NewNoFail("ntransformnode", "/export"));
    if (exportRoot)
    {
        ks->PushCwd(exportRoot);

        n_maxlog(Medium, "Created '%s' scene base object.", exportRoot->GetFullName().Get());
    }
    else
    {
        n_maxlog(Error, "Failed to create scene base object in NOH.");
        return false;
    }

    // if preview mode was set, switch to ram file server.
    if (nMaxOptions::Instance()->UsePreviewMode())
        ks->ReplaceFileServer("nramfileserver");
    else
        ks->ReplaceFileServer("nfileserver2");

    return true;
}

//-----------------------------------------------------------------------------
/**
    -# save mesh, animation file and .n2 scene file.
    -# do any release job if it is necessary.
*/
bool nMaxScene::End()
{
    // postprocess.
    if (!this->Postprocess())
        return false;

    if (!this->CloseNebula())
        return false;

    // clean up any exist instance of nMaxMesh
    for (int i=0;i<this->meshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->meshArray[i];
        n_delete(mesh);
    }

    // remove bone manager.
    n_delete(this->boneManager);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Uninitialize for nebula specifics.

    @note
        Be sure to call after calling Postprocess().
*/
bool nMaxScene::CloseNebula()
{
    // release export base object.
    if (this->exportRoot)
    {
        if (!exportRoot->Release())
        {
            n_maxlog(Error, "Failed to release Nebula object %s", exportRoot->GetName());
            return false;
        }

        exportRoot = 0;
    }

    // release variable server.
    if (!this->varServer->Release())
    {
        n_maxlog(Error, "Failed to release Nebula object %s", this->varServer->GetName());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Do any postprocessing for this scene. 
    This is called after the scene is exported.
*/
bool nMaxScene::Postprocess()
{
    //this->UnInitializeNodes(this->sceneRoot);

    nString globalMeshPath;

    // append meshes to one master mesh
    int nummeshes = this->meshArray.Size();
    if ( nummeshes > 0)
    {
        if (!nMaxOptions::Instance()->UseIndivisualMesh())
        {
            nMaxMesh* mesh;

            for (int i=0; i<nummeshes; i++)
            {
                mesh = this->meshArray[i];

                // if we consolidate all meshes to one mesh,
                // use the first mesh's path for saving it.
                if (i==0)
                {
                    globalMeshPath = mesh->GetMeshPath();
                }

                //TODO: check the mesh has same vertex component.
                //...

                // append retrieved meshes to a global mesh.
                int baseGroupIndex = this->globalMeshBuilder.Append(mesh->GetMeshBuilder());
                mesh->SetBaseGroupIndex(baseGroupIndex);
            }
        }
    }

    // we assume any one of the meshes are shadow type, consider it to a shadow mesh.
    // FIXME: it would be good to use more intuitive way.
    bool isShadowMesh = false;
    for (int i=0; i<this->meshArray.Size(); i++)
    {
        if (this->meshArray[i]->GetType() == nMaxMesh::Shadow)
        {
            // we don't check anymore.
            isShadowMesh = true;
            break;
        }
    }

    // if the global mesh has skinned animation, it might be needed to be partitioning. 
    if (!nMaxOptions::Instance()->UseIndivisualMesh())
    {
        if (!isShadowMesh)
        {
            if (this->globalMeshBuilder.GetNumVertices())
            {
                if (nMaxBoneManager::Instance()->GetNumBones() > 0)
                {
                    nMaxSkinPartitioner skinPartitioner;
                    skinPartitioner.Partitioning(this->meshArray, this->globalMeshBuilder);
                }
            }
        }
    }

    bbox3 rootBBox;

    if (!nMaxOptions::Instance()->UseIndivisualMesh())
    {
        if (this->globalMeshBuilder.GetNumVertices())
        {
            nString filename;
            filename = this->GetMeshFileNameToSave(globalMeshPath);

            // remove redundant vertices.
            this->globalMeshBuilder.Cleanup(0);

            //FIXME: no need for shadow node
            if (!isShadowMesh)
            {
                // build mesh tangents and normals (also vertex normal if it does not exist)
                nMaxMesh::BuildMeshTangentNormals(globalMeshBuilder);
            }
            else
            {
                // shadow mesh needs to call CreateEdges() which should be called after Cleanup()
                this->globalMeshBuilder.CreateEdges();
            }
                       
            // check the mesh for geometry error.
            nMaxMesh::CheckGeometryErrors(this->globalMeshBuilder, filename.Get());

            // do geometry scaling.
            float geomScale = nMaxOptions::Instance()->GetGeomScaleValue();
            if (geomScale != 0.0f)
            {
                vector3 scale;
                matrix44 m;

                scale.set(geomScale, geomScale, geomScale);
                if (scale.len() != 1.0f)
                {
                    m.scale(scale);
                    this->globalMeshBuilder.Transform(m);
                }
            }

            // specifies bounding box.
            rootBBox = globalMeshBuilder.GetBBox();

            // save mesh data.
            this->globalMeshBuilder.Save(nKernelServer::Instance()->GetFileServer(), filename.Get());
        }
    }
    else
    {
        // save meshses to each of specified direcotry.

        nMaxMesh* mesh = 0;
        nString meshFileName;

        // save for each mehses.
        for (int i=0; i<meshArray.Size(); i++)
        {
            mesh = this->meshArray[i];

            nMeshBuilder& mehsBuilder = mesh->GetMeshBuilder();
            meshFileName = GetMeshFileNameToSave(mesh->GetMeshPath());

            mehsBuilder.Save(nKernelServer::Instance()->GetFileServer(), meshFileName.Get());
        }
    }

    nMaxBoneManager *bm = nMaxBoneManager::Instance();
    if (nMaxBoneManager::Instance()->GetNumBones() > 0) 
    {
        for (int skelIndex = 0; skelIndex < bm->GetNumSkeletons(); skelIndex++) 
        {
            // export .anim2 and skin animator, if the exported scene has skinned mesh.
            nString animFilename = this->GetAnimFileNameToSave(skelIndex);

            //FIXME: we should add error handling code.
            if (!nMaxBoneManager::Instance()->Export(skelIndex, animFilename.Get()))
                return false;

            nSkinAnimator* createdAnimator = NULL;

            nMaxSkinAnimator skinAnimator;
            nString animatorName("skinanimator");
            animatorName.AppendInt(skelIndex);
            createdAnimator = (nSkinAnimator*)skinAnimator.Export(skelIndex, animatorName.Get(), animFilename.Get());
            if (createdAnimator)
            {
                nKernelServer::Instance()->PopCwd();
            }
        }
    }

    // save node.
    nString exportNodeName;
    exportNodeName += "/";
    exportNodeName += this->exportRoot->GetName();

    nKernelServer* ks = nKernelServer::Instance();

    nTransformNode* exportNode = static_cast<nTransformNode*>(ks->Lookup(exportNodeName.Get()));
    exportNode->SetLocalBox(rootBBox);

    nString filename = this->GetFileNameToSave();

    if (!exportNode->SaveAs(filename.Get()))
    {
        n_maxlog(Error, "Failed to Save % file.", filename.Get());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Returns animation file name without extension.
*/
nString nMaxScene::GetAnimFileNameToSaveBase() 
{
    nString animFilename;
    animFilename += nMaxOptions::Instance()->GetSaveFileName();
    animFilename = nMaxOptions::Instance()->GetAnimAssign() + animFilename;

    return animFilename;
}

//-----------------------------------------------------------------------------
/**
*/
nString nMaxScene::GetAnimFileNameToSave(int skelIndex) 
{
    nString animFilename;
    animFilename += this->GetAnimFileNameToSaveBase();
    animFilename.AppendInt(skelIndex);
    animFilename += nMaxOptions::Instance()->GetAnimFileType();

    return animFilename;
}

//-----------------------------------------------------------------------------
/**
*/
nString nMaxScene::GetMeshFileNameToSave(nString& meshPath) 
{

    nString meshname, meshFileName;

    // use a scene name for a mesh name.
    meshname = nMaxOptions::Instance()->GetSaveFileName();

    meshFileName += nMaxUtil::RelacePathToAssign(nMaxUtil::Mesh, meshPath, meshname);
    meshFileName += nMaxOptions::Instance()->GetMeshFileType();

    return meshFileName;
}

//-----------------------------------------------------------------------------
/**
*/
nString nMaxScene::GetFileNameToSave() 
{
    nString tmp = nMaxOptions::Instance()->GetSaveFilePath().Get();

    nString filename;
    filename += nMaxOptions::Instance()->GetGfxLibPath();
    filename += tmp.ExtractFileName();
    return filename;
}

//-----------------------------------------------------------------------------
/**
    Recursively export the scene. Call with the scene root.

    @param inode 3dsmax node.
*/
bool nMaxScene::ExportNodes(INode* inode)
{
    n_assert(inode);
#ifdef _DEBUG
    n_dbgout("Current Node: %s.\n", inode->GetName());
#endif

    //TODO: check any errors exist in stack. 
    //      if there, return false to exit export.

    nSceneNode* createdNode = 0;

    // check the node that we have already exported it.
    if (this->IsExportedNode(inode))
    {
        // already processed this node, so just instant node.
        return true;
    }

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();

    ObjectState objState = inode->EvalWorldState(animStart);
    Object* obj = objState.obj;

    if (obj)
    {
        SClass_ID sID = nMaxUtil::GetSuperClassID(obj);
        while (sID == GEN_DERIVOB_CLASS_ID)
        {
            obj = ((IDerivedObject*)obj)->GetObjRef();
            sID = obj->SuperClassID();
        }

        switch(sID)
        {
        case CAMERA_CLASS_ID:
            {
                createdNode = this->ExportCamera(inode, obj);
            }
            break;

        case LIGHT_CLASS_ID:
            {
                createdNode = this->ExportLight(inode, obj);
            }
            break;

        case GEOMOBJECT_CLASS_ID:
            {
                createdNode = this->ExportGeomObject(inode, obj);
            }
            break;
        case HELPER_CLASS_ID:
            {
                createdNode = this->ExportDummy(inode, obj);
            }
            break;

        default:
            createdNode = this->ExportNodesHook(sID, inode, obj);
            break;
        }
    }

    // if there's any create nebula object node and 3dsmanx node for that has xform
    // we neeed to export it.
    if (createdNode)
    {
        //HACK: is that sure the 'createNode' param is nTransformNode type or its derive class?

        // export animation if it exist.
        nMaxAnimator animator;
        animator.Export(inode);

        
        // HACK: do not export xform if there is skin animation
        nClass* clazz = createdNode->GetClass();
        if (nString(clazz->GetName()) != "nskinshapenode") {
        // export xform.
        this->ExportXForm(inode, createdNode, animStart);
    }
    }

    // recursively export the nodes.
    for (int i=0; i<inode->NumberOfChildren(); i++)
    {
        INode* child = inode->GetChildNode(i);

        if (!ExportNodes(child))
        {
            return false;
        }
    }

    // if any created node is exist, pop cwd and set cwd to the parent.
    if (createdNode)
    {
        nKernelServer::Instance()->PopCwd();
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Export anything else. 

    @note
    To be derived in subclasses.
*/
nSceneNode* nMaxScene::ExportNodesHook(SClass_ID sID, INode* inode, Object* obj) {
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportDummy(INode* inode, Object* obj) 
{
    nMaxDummy dummyNode;
    return dummyNode.Export(inode);
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportCamera(INode* inode, Object* obj) 
{
    nMaxCamera camera;
    camera.Export(inode, obj);
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportLight(INode* inode, Object* obj) {
    nMaxLight light;
    return light.Export(inode, obj);
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportGeomObject(INode* inode, Object* obj) 
{
    nSceneNode* createdNode = 0;
    if (obj->IsRenderable())
    {
        //FIXME: need more appropriate way.
        //Check the given node is hidden and we even export hidden node or not.
        bool exportHidden = nMaxOptions::Instance()->ExportHiddenNodes();

        // export only renderable geometry objects from the scene.
        if ((!inode->IsNodeHidden() || exportHidden ) &&
            !nMaxBoneManager::Instance()->IsBone(inode) && 
            !nMaxBoneManager::Instance()->IsFootStep(inode))
            //!nMaxBoneManager::Instance()->FindBoneIDByNode(inode))
        {
            createdNode = ExportGeomObject2(inode);
        }
    }
    return createdNode;
}

//-----------------------------------------------------------------------------
/**
    Export geometry type of nodes. 

    @note
    A geometry class type of objects are normally meshes or bones in 3dsmax.
*/
nSceneNode* nMaxScene::ExportGeomObject2(INode* inode)
{
    nSceneNode* createdNode = 0;
    
    if (nMaxUtil::IsMorphObject(inode))
    {
        return ExportMorph();
    }

    if (nMaxUtil::IsParticle(inode))
    {
        return ExportParticle();
    }

    {
        // we consider this INode is mesh object
        nMaxMesh* mesh = n_new(nMaxMesh);
        createdNode = mesh->Export(inode);

        // add the mesh to array for later merging.
        this->meshArray.Append(mesh);
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportMorph()
{
    nSceneNode* createdNode = 0;

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportParticle()
{
    nSceneNode* createdNode = 0;

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxScene::IsExportedNode(INode* inode)
{
    return false;
}

//-----------------------------------------------------------------------------
/**
    Retrieves view background color of a 3dsmax's view.
*/
void nMaxScene::ExportBackgroudColor()
{
    Interface* intf = nMaxInterface::Instance()->GetInterface();

    this->backgroudCol = intf->GetBackGround(0, FOREVER);
}

//-----------------------------------------------------------------------------
/**
    Recursively collect top level nodes from scene and append it to array.
    Top level node is a node which does not have any parent (except scene root)

    @param inode pointer to INode. Call by passing scene root node.
*/
void nMaxScene::CollectTopLevelNodes(INode* inode)
{
    n_assert(this->sceneRoot);

    if (NULL == inode)
        return;

    const int numChildren = inode->NumberOfChildren();
    
    for (int i=0; i<numChildren; i++)
    {
        INode* child = inode->GetChildNode(i);

        // if the given node's parent node is scene root,
        // this node is top-level node.
        if (child->GetParentNode() == this->sceneRoot)
        {
            this->topLevelNodes.Append(child);
            n_maxlog(High, "Top Level Node: %s", child->GetName());
        }

        CollectTopLevelNodes(child);
    }
}

//-----------------------------------------------------------------------------
/**
    Find XFrom modifier and export transform of the given node.
*/
void nMaxScene::ExportXForm(INode* inode, nSceneNode* sceneNode, TimeValue &animStart)
{
    n_assert(sceneNode);

    // get local transform of the given node.
    Matrix3 tm = nMaxTransform::GetLocalTM(inode, animStart);

    // scale transformation value. it only effects translation.
    float scl = nMaxOptions::Instance()->GetGeomScaleValue();

    tm.ValidateFlags();
    DWORD flag = tm.GetIdentFlags();

    AffineParts ap;
    decomp_affine(tm, &ap);

    //FIXME: ugly type casting.
    nTransformNode* tn = static_cast<nTransformNode*>(sceneNode);

    bool bXForm = false;

    // we only export xform if there's actual xform modifier exist
    // to prevent redundant call of SetPosition() or SetQuat().
    // note: 'POS_IDENT' means position elements are identity.
    if (!(flag & POS_IDENT))
    {
        vector3 trans (-ap.t.x*scl, ap.t.z*scl, ap.t.y*scl);
        tn->SetPosition(trans);

        bXForm = true;
    }
    
    if (!(flag & ROT_IDENT))
    {
        quaternion rot (-ap.q.x, ap.q.z, ap.q.y, -ap.q.w);
        tn->SetQuat(rot);

        bXForm = true;
    }

    if (!(flag & SCL_IDENT))
    {
        vector3 scale (ap.k.x, ap.k.z, ap.k.y);
        if (ap.f < 0.0f) {
            tn->SetScale(scale * -1);
        } else {
        tn->SetScale(scale);
        }

        bXForm = true;
    }

    if (bXForm)
    {
        n_maxlog(High, "Exported XForm of the node '%s'", inode->GetName());
    }
}

