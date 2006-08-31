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
#include "export2/nmaxcustattrib.h"

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

    @note
    A static model object are exported to separate mesh files(.n3d2 or .nvx2 file) 
    rather than a skinned model objects are exported to one mesh file.

    -30-Aug-06  kims  Changed to make it possible to export multiple meshes which 
                      have different vertex components at once.
*/
bool nMaxScene::Postprocess()
{
    //this->UnInitializeNodes(this->sceneRoot);

    // all meshes which have no joint indicies are collected to this storage.
    nArray<nMaxMesh*> nonSkinnedMeshArray;

    // a directory which mesh is exported.
    nString globalMeshPath;

    // append meshes to one master mesh
    int nummeshes = this->meshArray.Size();
    if ( nummeshes > 0)
    {
        //if (!nMaxOptions::Instance()->UseIndivisualMesh())
        //{
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

                nMeshBuilder& meshBuilder = mesh->GetMeshBuilder();

                // assume that if the given mesh has joint indicies, it is skinned mesh
                if (meshBuilder.HasVertexComponent(nMeshBuilder::Vertex::Component::JINDICES))
                {
                    // append retrieved meshes to a global mesh.
                    // we need it to do skin partitioning.
                    int baseGroupIndex = this->globalMeshBuilder.Append(meshBuilder);
                    mesh->SetBaseGroupIndex(baseGroupIndex);
                }
                else
                {
                    nonSkinnedMeshArray.PushBack(mesh);
                }
            }
        //}
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
    //if (!nMaxOptions::Instance()->UseIndivisualMesh())
    //{
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
    //}

    bbox3 rootBBox;

    //if (!nMaxOptions::Instance()->UseIndivisualMesh())
    {
        if (this->globalMeshBuilder.GetNumVertices())
        {
            nString meshFileName;
            meshFileName = this->GetMeshFileNameToSave(globalMeshPath);

            ProcessOnMeshBuilder(this->globalMeshBuilder, isShadowMesh, meshFileName);

            // specifies bounding box.
            rootBBox = globalMeshBuilder.GetBBox();

            // save mesh data.
            this->globalMeshBuilder.Save(nKernelServer::Instance()->GetFileServer(), meshFileName.Get());
        }
    }
    //else

    if (nonSkinnedMeshArray.Size())
    {
        // save meshses to each of its specified direcotry.

        nMaxMesh* mesh = 0;
        nString meshFileName;

        // save for each mehses.
        for (int i=0; i<nonSkinnedMeshArray.Size(); i++)
        {
            mesh = nonSkinnedMeshArray[i];

            nMeshBuilder& meshBuilder = mesh->GetMeshBuilder();

            // use a 3dsmax's node name for a mesh file name.
            nString meshname, meshFileName;
            meshname = mesh->GetNodeName();
            meshFileName += nMaxUtil::RelacePathToAssign(nMaxUtil::Mesh, mesh->GetMeshPath(), meshname);
            meshFileName += nMaxOptions::Instance()->GetMeshFileType();

            ProcessOnMeshBuilder(meshBuilder, (mesh->GetType() == nMaxMesh::Shadow ? true : false), meshFileName);

            meshBuilder.Save(nKernelServer::Instance()->GetFileServer(), meshFileName.Get());
        }
    }

    nMaxBoneManager *bm = nMaxBoneManager::Instance();
    if (nMaxBoneManager::Instance()->GetNumBones() > 0) 
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
    // save node to export.
    //

    INode* sceneRoot = nMaxInterface::Instance()->GetInterface()->GetRootNode();

    TiXmlDocument xmlDoc;
    nMaxCustAttrib custAttrib;

    // HACK: sceneRoot is root node of 
    if (!custAttrib.Convert(sceneRoot, xmlDoc))
    {
        n_maxlog(Warning, "A directory for gfxlib is not specified.");
    }

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
            const char* scenePath = child->Attribute("value");
            if (scenePath)
            {
                this->sceneDir = scenePath;

                //HACK: if the path has "<<NULL>>" for its value,
                //      we convert it to the default mesh export directory.
                //      See nMaxCustAttrib::StringToXml() function in the nmaxcustattrib.cc file.
                if (this->sceneDir == "<<NULL>>")
                {
                    this->sceneDir = nFileServer2::Instance()->ManglePath(nMaxOptions::Instance()->GetGfxLibAssign());
                }
            }
        }
    }

    // assign gfx object name.
    nString gfxname, gfxObjFileName;
    gfxname = nMaxOptions::Instance()->GetSaveFileName();

    gfxObjFileName += nMaxUtil::RelacePathToAssign(nMaxUtil::Gfx, this->sceneDir, gfxname);
    gfxObjFileName += ".n2";

    // specify boundbox of the top level exported node.
    nKernelServer* ks = nKernelServer::Instance();

    nString exportedNodeName;
    exportedNodeName += "/";
    exportedNodeName += this->exportRoot->GetName();

    nTransformNode* exportNode = static_cast<nTransformNode*>(ks->Lookup(exportedNodeName.Get()));
    exportNode->SetLocalBox(rootBBox);

    // save gfx object.
    if (!exportNode->SaveAs(gfxObjFileName.Get()))
    {
        n_maxlog(Error, "Failed to Save % file.", gfxObjFileName.Get());
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

//-----------------------------------------------------------------------------
/**
    Do some processing on the meshbuilder object as the followings:
      - cleanup mesh data
      - build tangent normal
      - check geometry errors
      - scale a mesh

    @param meshBuilder
    @param isShadowMesh

*/
void nMaxScene::ProcessOnMeshBuilder(nMeshBuilder& meshBuilder, bool isShadowMesh, nString meshName)
{
    // remove redundant vertices.
    meshBuilder.Cleanup(0);

    if (!isShadowMesh)
    {
        //
        // build mesh tangents and normals (also vertex normal if it does not exist)
        //
        const nMeshBuilder::Vertex& v = meshBuilder.GetVertexAt(0);

        if (nMaxOptions::Instance()->ExportNormals() || nMaxOptions::Instance()->ExportTangents())
        {
            // build triangle normals, vertex normals and tangents.
            n_maxlog(Low, "Build tangents and normals...");

            if (false == v.HasComponent(nMeshBuilder::Vertex::UV0))
            {
                n_maxlog(Error, "The tangents require a valid uv-mapping in texcoord layer 0.");
            }
            n_maxlog(Low, "  - Build triangle normals, tangents, and binormals...");
            meshBuilder.BuildTriangleNormals();

            if (false == v.HasComponent(nMeshBuilder::Vertex::NORMAL))
            {
                // build vertex normals by averaging triangle normals.
                n_maxlog(Low, "  - Build vertex normals...");
                meshBuilder.BuildVertexNormals();
            }
            if (nMaxOptions::Instance()->ExportTangents())
            {
                n_maxlog(Low, "  - Build vertex tangents...");
                // XXX: One day, we may want to make this configurable so that blendshape export can pass false.
                meshBuilder.BuildVertexTangents(true);
            }
            n_maxlog(Low, "Building mesh tangents and normals done.");
        }
    }
    else
    {
        // shadow mesh needs to call CreateEdges() which should be called after Cleanup()
        meshBuilder.CreateEdges();
    }

    //
    // check the mesh for geometry error.
    //
    nArray<nString> geomErrorMsgArray;
    geomErrorMsgArray = meshBuilder.CheckForGeometryError();
    if (geomErrorMsgArray.Size())
    {
        nString errlogfilename;
        errlogfilename = meshName;
        errlogfilename.StripExtension();
        errlogfilename += ".error";

        n_maxlog(Warning, "Warning: The exported mesh file has geometry errors.");
        n_maxlog(Warning, "    - See the file '%s' for the details.", errlogfilename.Get());

        nFile* errFile = nFileServer2::Instance()->NewFileObject();
        if (errFile->Open(errlogfilename.Get(), "w"))
        {
            // put the geometry error message to log dialog.
            for (int i=0; i<geomErrorMsgArray.Size(); i++)
            {
                errFile->PutS(geomErrorMsgArray[i].Get());
            }

            errFile->Close();
            errFile->Release();
        }
        else
        {
            n_maxlog(Error, "Error: Failed to open error log file '%s for the geometry errrs.", errlogfilename.Get());
            errFile->Release();
        }
    }

    //
    // do geometry scaling.
    //
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
