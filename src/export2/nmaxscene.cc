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

#include "variable/nvariableserver.h"
#include "util/nkeyarray.h"
#include "tools/nmeshbuilder.h"

#include "export2/nmaxdummy.h"
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
#include "export2/nmaxcustattrib.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/npersistserver.h"
#include "variable/nvariableserver.h"
#include "scene/ntransformnode.h"
#include "scene/nskinshapenode.h"
//begin nskynode
#include "scene/nskynode.h"
#include "scene/nskystate.h"
//#include "scene/nanimator.h"
//#include "scene/ntransformanimator.h"
//#include "scene/nshaderanimator.h"
//#include "scene/nblendshapeanimator.h"
//#include "scene/nfloatanimator.h"
//#include "scene/nintanimator.h"
//#include "scene/nvectoranimator.h"
//end nskynode

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
    this->globalShadowMeshBuilder.Clear();
    this->globalSkinnedMeshBuilder.Clear();
    this->globalSkinnedShadowMeshBuilder.Clear();

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

    // shadow meshes.
    for (int i=0;i<this->shadowMeshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->shadowMeshArray[i];
        n_delete(mesh);
    }

    // skinned shadow meshes.
    for (int i=0;i<this->skinnedShadowMeshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->skinnedShadowMeshArray[i];
        n_delete(mesh);
    }

    // collision meshes
    for (int i=0; i<this->collisionMeshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->collisionMeshArray[i];
        n_delete(mesh);
    }

    // sky meshes.
    for (int i=0; i<this->skyMeshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->skyMeshArray[i];
        n_delete(mesh);
    }

    // dummy nodes used for sky state.
    for (int i=0; i<this->skyStateArray.Size(); i++)
    {
        nMaxDummy* dummy = this->skyStateArray[i];
        n_delete(dummy);
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

    @note
    A static model object are exported to separate mesh files(.n3d2 or .nvx2 file) 
    rather than a skinned model objects are exported to one mesh file.

    -30-Aug-06  kims  Changed to make it possible to export multiple meshes which 
                      have different vertex components at once.
*/
bool nMaxScene::Postprocess()
{
    //this->UnInitializeNodes(this->sceneRoot);

    ProcessOnSkyNode();

    nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    nString globalMeshPath;

    // append meshes to one master mesh
    nMaxMesh* mesh;
    
    // static meshes.
    if (!this->meshArray.Empty())
    {
        int nummeshes = this->meshArray.Size();
        for (int i=0; i<nummeshes; i++)
        {
            mesh = this->meshArray[i];

            //TODO: check the mesh has same vertex component.
            //...

            // append retrieved meshes to a global mesh.
            int baseGroupIndex = this->globalMeshBuilder.Append(mesh->GetMeshBuilder());
            mesh->SetBaseGroupIndex(baseGroupIndex);

            // if we consolidate all meshes to one mesh, use the first mesh's path for saving it.
            if (i==0 && globalMeshPath.IsEmpty())
            {
                globalMeshPath = mesh->GetMeshPath();
            }
        }
    }

    // skinned meshes.
    if (!this->skinnedMeshArray.Empty())
    {
        int nummeshes = this->skinnedMeshArray.Size();
        for (int i=0; i<nummeshes; i++)
        {
            mesh = this->skinnedMeshArray[i];

            //TODO: check the mesh has same vertex component.
            //...

            // append retrieved meshes to a global mesh.
            int baseGroupIndex = this->globalSkinnedMeshBuilder.Append(mesh->GetMeshBuilder());
                    mesh->SetBaseGroupIndex(baseGroupIndex);

            // if we consolidate all meshes to one mesh, use the first mesh's path for saving it.
            if (i==0 && globalMeshPath.IsEmpty())
            {
                globalMeshPath = mesh->GetMeshPath();
            }
        }
    }

    // static shadow meshes.
    if (!this->shadowMeshArray.Empty())
    {
        int nummeshes = this->shadowMeshArray.Size();
        for (int i=0; i<nummeshes; i++)
        {
            mesh = this->shadowMeshArray[i];

            //TODO: check the mesh has same vertex component.
            //...

            // append retrieved meshes to a global mesh.
            int baseGroupIndex = this->globalShadowMeshBuilder.Append(mesh->GetMeshBuilder());
            mesh->SetBaseGroupIndex(baseGroupIndex);

            // if we consolidate all meshes to one mesh, use the first mesh's path for saving it.
            if (i==0 && globalMeshPath.IsEmpty())
            {
                globalMeshPath = mesh->GetMeshPath();
            }
        }
    }

    // skinned shadow meshes.
    if (!this->skinnedShadowMeshArray.Empty())
    {
        int nummeshes = this->skinnedShadowMeshArray.Size();
        for (int i=0; i<nummeshes; i++)
        {
            mesh = this->skinnedShadowMeshArray[i];

            //TODO: check the mesh has same vertex component.
            //...

            // append retrieved meshes to a global mesh.
            int baseGroupIndex = this->globalSkinnedShadowMeshBuilder.Append(mesh->GetMeshBuilder());
            mesh->SetBaseGroupIndex(baseGroupIndex);

            // if we consolidate all meshes to one mesh, use the first mesh's path for saving it.
            if (i==0 && globalMeshPath.IsEmpty())
            {
                globalMeshPath = mesh->GetMeshPath();
            }
        }
    }

    // collision meshes.
    if (!this->collisionMeshArray.Empty())
    {
        nString meshPath;
        nString meshFileName;

        for (int i=0; i<collisionMeshArray.Size(); i++)
        {
            mesh = this->collisionMeshArray[i];
            nMeshBuilder& meshBuilder = mesh->GetMeshBuilder();

            meshPath = mesh->GetMeshPath();

            // Unlike other types of mesh, collision meshes are individually saved.
            meshFileName = this->GetMeshFileNameToSave(meshPath, nMaxMesh::Collision, false);
            meshBuilder.Save(fileServer, meshFileName.Get());
        }
    }

    // if the global mesh has skinned animation, it might be needed to be partitioning. 
    if (nMaxBoneManager::Instance()->GetNumBones() > 0)
    {
        if (this->globalSkinnedMeshBuilder.GetNumVertices())
        {
            nMaxSkinPartitioner skinPartitioner;
            skinPartitioner.Partitioning(this->skinnedMeshArray, this->globalSkinnedMeshBuilder);
        }

        if (this->globalSkinnedShadowMeshBuilder.GetNumVertices())
        {
            // skinned shadow mesh also need to be partitionning.
            nMaxSkinPartitioner skinPartitioner;
            skinPartitioner.Partitioning(this->skinnedShadowMeshArray, this->globalSkinnedShadowMeshBuilder);
        }
    }

    bbox3 rootBBox;
    nString meshFileName;

    // export staic mesh object.
    if (this->globalMeshBuilder.GetNumVertices())
    {
        meshFileName = this->GetMeshFileNameToSave(globalMeshPath, nMaxMesh::Shape, false);

        ProcessOnMeshBuilder(this->globalMeshBuilder, false, meshFileName);

        // specify bounding box.
        rootBBox = globalMeshBuilder.GetBBox();

        // save mesh data.
        this->globalMeshBuilder.Save(fileServer, meshFileName.Get());
    }

    // export skinned mesh object.
    if (this->globalSkinnedMeshBuilder.GetNumVertices())
    {
        meshFileName = this->GetMeshFileNameToSave(globalMeshPath, nMaxMesh::Shape, true);

        ProcessOnMeshBuilder(this->globalSkinnedMeshBuilder, false, meshFileName);

        rootBBox = globalSkinnedMeshBuilder.GetBBox();

        this->globalSkinnedMeshBuilder.Save(fileServer, meshFileName.Get());
    }

    // export static shadow mesh object.
    if (this->globalShadowMeshBuilder.GetNumVertices())
    {
        meshFileName = this->GetMeshFileNameToSave(globalMeshPath, nMaxMesh::Shadow, false);

        // remove useless components if those are.
        this->globalShadowMeshBuilder.ForceVertexComponents(nMeshBuilder::Vertex::COORD);

        ProcessOnMeshBuilder(this->globalShadowMeshBuilder, true, meshFileName);

        rootBBox = globalShadowMeshBuilder.GetBBox();

        this->globalShadowMeshBuilder.Save(fileServer, meshFileName.Get());
    }

    // export skinned shadow mesh object.
    if (this->globalSkinnedShadowMeshBuilder.GetNumVertices())
    {
        meshFileName = this->GetMeshFileNameToSave(globalMeshPath, nMaxMesh::Shadow, true);

        // remove useless components if those are.
        this->globalSkinnedShadowMeshBuilder.ForceVertexComponents(
            nMeshBuilder::Vertex::COORD | nMeshBuilder::Vertex::WEIGHTS | nMeshBuilder::Vertex::JINDICES);

        ProcessOnMeshBuilder(this->globalSkinnedShadowMeshBuilder, true, meshFileName);

        rootBBox = globalSkinnedShadowMeshBuilder.GetBBox();

        this->globalSkinnedShadowMeshBuilder.Save(fileServer, meshFileName.Get());
    }

    // create skin animators
    nMaxBoneManager *bm = nMaxBoneManager::Instance();
    if (bm->GetNumBones() > 0) 
    {
        for (int skelIndex = 0; skelIndex < bm->GetNumSkeletons(); skelIndex++) 
        {
            // export .anim2 and skin animator, if the exported scene has skinned mesh.
            nString animFilename = this->GetAnimFileNameToSave(skelIndex);

            if (!nMaxBoneManager::Instance()->Export(skelIndex, animFilename.Get()))
            {
                n_maxlog(Error, "Failed to export animation %s", animFilename.Get());
                return false;
            }

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

    //
    // save node to export.(save .n2 file on disk)
    //

    INode* sceneRoot = nMaxInterface::Instance()->GetInterface()->GetRootNode();

    TiXmlDocument xmlDoc;
    nMaxCustAttrib custAttrib;

    // HACK: sceneRoot is root node in the current 3dsmax viewport.
    if (!custAttrib.Convert(sceneRoot, xmlDoc))
    {
        TiXmlHandle xmlHandle(&xmlDoc);

        // parameter block name for gfx directory setting.
        const char* dirParamName = "SceneDirSetting";
        TiXmlElement* e;
        e = xmlHandle.FirstChild(dirParamName).Element();
        if (e)
        {
            // find parameter with the given its name.
            TiXmlElement* child;
            child = xmlHandle.FirstChild(dirParamName).FirstChild("gfxDir").Child("", 0).Element();
            if (child)
            {
                nString scenePath = child->Attribute("value");

                // The directory parameter has "" for default string. It is absolutely necessary in Max6.
                // Without that, the exporter is not usable as the panels that have those controls in them don't work.
                if (scenePath == "")
                {
                    // The given scene (which to be exported) has already scene option custom attribute
                    // but the directory is not specified so we just use default gfx assign value for it.
                    // (the directory path which is specified in "Directory Setting Dialog".)
                    this->sceneDir = nFileServer2::Instance()->ManglePath(nMaxOptions::Instance()->GetGfxLibAssign());
                }
                else
                {
                    this->sceneDir = scenePath;
                }
            }
        }
    }
    else
    {
        n_maxlog(Low, "A scene option custom attribute is not specified.");
    }

    // we only save .n2 gfx object in the case that there are nodes to export exist.
    // we don't need to save .n2 file when only export collision meshes.
    nString exportedNodeName;
    exportedNodeName += "/";
    exportedNodeName += this->exportRoot->GetName();

    nKernelServer* ks = nKernelServer::Instance();
    nTransformNode* exportNode = static_cast<nTransformNode*>(ks->Lookup(exportedNodeName.Get()));
    if (exportNode->GetHead())
    {
        // assign gfx object name.
        nString gfxname, gfxObjFileName;
        gfxname = nMaxOptions::Instance()->GetSaveFileName();

        gfxObjFileName += nMaxUtil::RelacePathToAssign(nMaxUtil::Gfx, this->sceneDir, gfxname);
        gfxObjFileName += ".n2";

        // specify boundbox of the top level exported node.
        exportNode->SetLocalBox(rootBBox);

        // save gfx object.
        if (!exportNode->SaveAs(gfxObjFileName.Get()))
        {
            n_maxlog(Error, "Failed to Save % file.", gfxObjFileName.Get());
            return false;
        }
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
nString nMaxScene::GetMeshFileNameToSave(nString& meshPath, nMaxMesh::Type type, bool isSkinned) const
{
    nString meshname, meshFileName;

    // use a scene name for a mesh name.
    meshname = nMaxOptions::Instance()->GetSaveFileName();

    meshFileName += nMaxUtil::RelacePathToAssign(nMaxUtil::Mesh, meshPath, meshname);

    meshFileName += nMaxMesh::AppendMeshPostfixByType(type, isSkinned);

    meshFileName += nMaxOptions::Instance()->GetMeshFileType();

    return meshFileName;
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
        if (nString(clazz->GetName()) != "nskinshapenode")
        {
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
nSceneNode* nMaxScene::ExportNodesHook(SClass_ID sID, INode* inode, Object* obj)
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportDummy(INode* inode, Object* obj) 
{
    nSceneNode* createdNode = 0;

    nMaxDummy* dummy = n_new(nMaxDummy);
    createdNode =  dummy->Export(inode);
    AddDummyByType(dummy);

    return createdNode;
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

    // 3dsmax mesh object
    {
        // we consider this INode is mesh object
        nMaxMesh* mesh = n_new(nMaxMesh);
        createdNode = mesh->Export(inode);

        // add the mesh to array for later merging.
        //this->meshArray.Append(mesh);
        AddMeshByType(mesh);
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxScene::AddMeshByType(nMaxMesh* mesh)
{
    if (0 == mesh)
        return;

    switch (mesh->GetType())
    {
    case nMaxMesh::Shape:
        {
            if (mesh->IsPhysique() || mesh->IsSkinned())
                this->skinnedMeshArray.Append(mesh);
            else
                this->meshArray.Append(mesh);
        }
        break;
    case nMaxMesh::Shadow:
        {
            if (mesh->IsPhysique() || mesh->IsSkinned())
                this->skinnedShadowMeshArray.Append(mesh);
            else
                this->shadowMeshArray.Append(mesh);
        }
        break;
    case nMaxMesh::Swing:
        {
            // in the case of a mesh for nsiwngnode, we just add it to the
            // static mesh array.
            this->meshArray.Append(mesh);
        }
        break;
    case nMaxMesh::Collision:
        {
            this->collisionMeshArray.Append(mesh);
        }
        break;
    case nMaxMesh::Sky:
        {
            this->skyMeshArray.Append(mesh);
        }
        break;
    default:
        n_maxlog(Warning, "Unknown mesh type : %s.", mesh->GetNodeName().Get());
        n_delete(mesh);
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxScene::AddDummyByType(nMaxDummy* dummy)
{
    if (0 == dummy)
        return;

    switch(dummy->GetType())
    {
    case nMaxDummy::Lod:
        //
        break;

    case nMaxDummy::SkyNode:
        //
        break;

    case nMaxDummy::SkyState:
        {
            nFourCC key = nVariableServer::Instance()->StringToFourCC(dummy->GetNodeName().Get());
            skyStateArray.Add(key, dummy);
        }
        break;

    default:
        n_maxlog(Warning, "Unknown dummy type : %s.", dummy->GetNodeName().Get());
        n_delete(dummy);
        break;
    }
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

//-----------------------------------------------------------------------------
/**
    Do some processing on the meshbuilder object as the followings:
      - cleanup mesh data
      - build tangent and vertex normal
      - check geometry errors
      - scale a mesh

    Called in nMaxScene::Postprocess() function.

    @param meshBuilder  - A reference to nMeshBuilder needed for building tangent, 
                          normal and edges if it is necessary.
    @param isShadowMesh - true if the given mesh is used for shadow.
    @param meshName     - mesh file name.

*/
void nMaxScene::ProcessOnMeshBuilder(nMeshBuilder& meshBuilder, bool isShadowMesh, nString meshName)
{
    // remove redundant vertices.
    meshBuilder.Cleanup(0);

    if (!isShadowMesh)
    {
        // build mesh tangents and normals (also vertex normal if it does not exist)
        if (!nMaxMesh::BuildMeshTangentNormals(meshBuilder))
        {
            n_maxlog(Error, "Failed to build tangent and normal.");
        }
    }
    else
    {
        // NOTE: shadow mesh needs to call CreateEdges() and this should be done after calling Cleanup()
        meshBuilder.CreateEdges();
    }

    // check the mesh for geometry error.
    nMaxMesh::CheckGeometryErrors(meshBuilder, meshName);

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
            meshBuilder.Transform(m);
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxScene::ProcessOnSkyNode()
{
    // skynode post process
    n_assert(this->exportRoot);
    for (nRoot *child = this->exportRoot->GetHead(); child; child = child->GetSucc())
    {
        if (!child->IsA("nskynode"))
            continue;

        nSkyNode *skyNode = (nSkyNode*)child;

        // add sky elements to skynode if exist
        for (nRoot *child = skyNode->GetHead(); child; child = child->GetSucc())
        {
            if (!child->IsA("nscenenode"))
                continue;

            nSceneNode *sceneNode = (nSceneNode*)child;

            // sky element.
            nSkyNode::ElementType elementType = nSkyNode::InvalidElement;
            if (sceneNode->IsA("nshapenode"))
            {
                nShapeNode *skyElement = (nShapeNode *)sceneNode;
                
                // find element's type by assigned shader.
                nString shadername = skyElement->GetShader();
                shadername.ToLower();

                if (shadername == "sun")
                    elementType = nSkyNode::SunElement;
                else
                if (shadername == "sky")
                    elementType = nSkyNode::SkyElement;
                else
                if (shadername == "cloud")
                    elementType = nSkyNode::CloudElement;
                else
                if (shadername == "stars")
                    elementType = nSkyNode::StarElement;
                else
                    elementType = nSkyNode::GenericElement;

                // specify element.
                skyNode->AddElement(elementType, sceneNode->GetName());
            }
            else
            if (sceneNode->IsA("nlightnode"))
            {
                elementType = nSkyNode::LightElement;

                skyNode->AddElement(elementType, sceneNode->GetName());
            }
            else
            if (sceneNode->IsA("nskystate"))
            {
                // see if the child is nskystate.
                if (sceneNode->IsA("nskystate"))
                {
                    // find proper nMaxDummy with the given child node name in the skyStateMap.

                    //HACK: Be sure that there are no nskystate objects which has same name!
                    //      even we use fourcc, each of first four characters are different.
                    const char* skyStateName = sceneNode->GetName();
                    nFourCC key = nVariableServer::Instance()->StringToFourCC(skyStateName);

                    nMaxDummy* dummy = 0;
                    if (skyStateArray.Find(key, dummy))
                    {
                        if (dummy)
                        {
                            // retrieve picked element's name form the skystate.
                            nString destName = dummy->GetElemOfState();
                            float stateTime  = dummy->GetSkyStateTime();

                            // add element and state to the skynode.
                            skyNode->AddState(destName, nString(skyStateName), stateTime);
                        }
                    }
                    else
                    {
                        // cannnot find skystate
                        n_maxlog(Error, "Failed to find sky state: %s", skyStateName);
                    }
                }
            }
            else
            {
                n_maxlog(Warning, "Invalid Node under nSkyNode: %s", sceneNode->GetName());
            }

            // link each of elements if it is neccessary.
            for (int i=0; i<this->skyMeshArray.Size(); i++)
            {
                nMaxMesh* mesh = skyMeshArray[i];

                if (mesh && sceneNode->GetName() == mesh->GetNodeName())
                {
                    // found sky element to link
                    if (!mesh->GetSkyElementNameToLink().IsEmpty())
                        skyNode->LinkTo(sceneNode->GetName(), mesh->GetSkyElementNameToLink());
                }
            }

        }// end of for each sky elements.
    }// end of for each nebula objects.
}