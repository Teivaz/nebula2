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

#include "kernel/npersistserver.h"
#include "variable/nvariableserver.h"
#include "scene/ntransformnode.h"
#include "scene/nskinshapenode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxScene::nMaxScene() :
    sceneRoot (0),
    nohBase ("export"),
    boneManager(0)
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxScene::~nMaxScene()
{
    // release scene base object.
    nRoot* sceneRoot = nKernelServer::Instance()->Lookup(this->nohBase.Get());
    if (sceneRoot)
    {
        sceneRoot->Release();
    }

    // clean up any exist instance of nMaxMesh
    for (int i=0;i<this->meshArray.Size(); i++)
    {
        nMaxMesh* mesh = this->meshArray[i];
        n_delete(mesh);
    }

    // remove bone manager.
    n_delete(this->boneManager);
}

//-----------------------------------------------------------------------------
/**
    Export given 3dsmax scene.
*/
bool nMaxScene::Export()
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

    if(!this->Begin(rootNode))
    {
        return false;
    }

    //int numChildren = rootNode->NumberOfChildren();

    //for (int i=0; i<numChildren; i++)
    //{
    //    INode* child = rootNode->GetChildNode(i);
    //    // export nodes.
    //    this->ExportNodes(child);
    //}
    if (!this->ExportNodes(rootNode))
    {
        return false;
    }

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

    //this->InitializeNodes(root);

// begin build bone array
    // Build bone list.
    n_maxlog(Midium, "Start to build bone list.");

    this->boneManager = nMaxBoneManager::Instance();
    
    //for (int i=0;i<this->topLevelNodes.Size(); i++)
    //{
    //    INode* inode = this->topLevelNodes[i];
    //    
    //    boneList->BuildBoneList(-1, inode);
    //}
    //for (int i=0; i<root->NumberOfChildren(); i++)
    //{
    //    boneList->BuildBoneList(-1, root->GetChildNode(i));
    //}
    //boneList->BuildBoneList(-1, root);

    this->boneManager->BuildBoneList(root);

    n_maxlog(Midium, "Found %d bones", this->boneManager->GetNumBones());
// end build bone array

    this->globalMeshBuilder.Clear();

    // Disable physique modifier to get skin in the initial pose.
    // ...

    return true;
}

void SetFlags(ReferenceMaker *pRefMaker, int iStat)
{
    int i;

    for (i = 0; i < pRefMaker->NumRefs(); i++) 
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

    // Be sure to make the cwd to be root.
    nRoot* nohRoot = ks->Lookup("/");
    ks->PushCwd(nohRoot);

    // Make the scene base object which to be exported in final export stage.
    // All nodes which exported should be under this node as child node of this.
    nString nodeName = nMaxUtil::CorrectName(this->nohBase);

    nRoot* baseObj = ks->NewNoFail("ntransformnode", nodeName.Get());
    if (baseObj)
    {
        nRoot* cwd = ks->Lookup(baseObj->GetFullName().Get());
        ks->PushCwd(cwd);

        n_maxlog(Midium, "Created '%s' scene base object.", baseObj->GetFullName().Get());
    }
    else
    {
        n_maxlog(Error, "Failed to create scene base object in NOH.");
        return false;
    }

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

    return true;
}

//-----------------------------------------------------------------------------
/**
    Uninitialize for nebula specifics.
    (Be sure to call after calling Postprocess())

*/
bool nMaxScene::CloseNebula()
{
    if (!this->varServer->Release())
        return false;

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

    //// append meshes to one master mesh
    //if (nMaxOptions::Instance()->GroupMeshes())
    //{
    //    int numMeshes = this->meshArray.Size();
    //    if ( numMeshes > 0)
    //    {
    //        nMeshBuilder masterMesh;
    //        nMaxMesh* mesh;

    //        for (int i=0; i<numMeshes; i++)
    //        {
    //            mesh = this->meshArray[i];

    //            // get individual mesh then append it to master mesh.
    //        }

    //        // save mater mesh.
    //    }
    //    else
    //    {
    //        // no meshes to merge
    //    }
    //}

    if (!nMaxOptions::Instance()->UseIndivisualMesh())
    {
        // if the global mesh is skinned mesh, 
        // create skin animator and partition the mesh.
        nMeshBuilder::Vertex& v = this->globalMeshBuilder.GetVertexAt(0);
        if (v.HasComponent(nMeshBuilder::Vertex::WEIGHTS) && 
            v.HasComponent(nMeshBuilder::Vertex::JINDICES))
        {
            nMaxSkinPartitioner skinPartitioner;
            skinPartitioner.Partitioning(this->meshArray, this->globalMeshBuilder);
            //this->PartitionMesh();
        }
    }

    bbox3 rootBBox;

    if (!nMaxOptions::Instance()->UseIndivisualMesh())
    {
        // remove redundant vertices.
        this->globalMeshBuilder.Cleanup(0);

        // build mesh tangents.
        nMaxMesh::BuildMeshTangentNormals(globalMeshBuilder);
        
        // specifies bounding box.
        rootBBox = globalMeshBuilder.GetBBox();

        // save mesh data.
        nString filename;
        filename += nMaxOptions::Instance()->GetMeshesAssign();
        filename += nMaxOptions::Instance()->GetSaveFileName();
        filename += nMaxOptions::Instance()->GetMeshFileType();

        this->globalMeshBuilder.Save(nKernelServer::Instance()->GetFileServer(), filename.Get());
    }
    else
    {
        for (int i=0; i<this->meshArray.Size(); i++)
        {
            const nMeshBuilder& localMeshBuilder = meshArray[i]->GetLocalMeshBuilder();
            bbox3 localBox = localMeshBuilder.GetBBox();

            rootBBox.extend(localBox);
        }
    }

// begin animation save
    nString animFilename;
    animFilename += nMaxOptions::Instance()->GetSaveFileName();
    animFilename += nMaxOptions::Instance()->GetAnimFileType();
    animFilename = nMaxOptions::Instance()->GetAnimPath() + animFilename;

    //FIXME: we should add error handling code.
    if (!this->ExportAnimation(animFilename))
        return false;
// end animation save

    //FIXME: should check skinned mesh with different way
    //       to work with more than two skinned mesh in a scene.
    // if the exported scene has skinned mesh.
    if (nMaxBoneManager::Instance()->GetNumBones() > 0)
    {
        //nString animatorName;
        //animatorName += "/";
        //animatorName += this->nohBase;
        //animatorName += "/skinanimator";
        //this->CreateSkinAnimator(animatorName, animFilename);
        nSkinAnimator* createdAnimator = NULL;

        nMaxSkinAnimator skinAnimator;
        createdAnimator = (nSkinAnimator*)skinAnimator.Export("skinanimator", animFilename.Get());
        if (createdAnimator)
        {
            nKernelServer::Instance()->PopCwd();
        }
    }

    // save node.
    nString exportNodeName;
    exportNodeName += "/";
    exportNodeName += this->nohBase;

    nKernelServer* ks = nKernelServer::Instance();

    nTransformNode* exportNode = static_cast<nTransformNode*>(ks->Lookup(exportNodeName.Get()));
    exportNode->SetLocalBox(rootBBox);

    nString tmp = nMaxOptions::Instance()->GetSaveFilePath().Get();
    
    nString filename;
    filename += nMaxOptions::Instance()->GetGfxLibPath();
    filename += tmp.ExtractFileName();

    if (!exportNode->SaveAs(filename.Get()))
    {
        n_maxlog(Error, "Failed to Save % file.", filename.Get());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Recursively export the scene. Call with the scene root.

    @param inode 3dsmax node.
*/
bool nMaxScene::ExportNodes(INode* inode)
{
    n_assert(inode);

    //TODO: check any errors exist in stack. 
    //      if there, return false then exit export.

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
                nMaxCamera camera;
                camera.Export(inode, obj);
            }
            break;

        case LIGHT_CLASS_ID:
            ExportLightObject(inode);
            break;

        case GEOMOBJECT_CLASS_ID:
            if (obj->IsRenderable())
            {
                //FIXME: need more appropriate way.
                //Check the given node is hidden and we even export hidden node or not.
                //bool exportHidden = true;
                //bool exportHidden = inode->IsNodeHidden() ? true : false;
                bool exportHidden = nMaxOptions::Instance()->ExportHiddenNodes();

                // export only renderable geometry objects from the scene.
                if ((!inode->IsNodeHidden() || exportHidden ) &&
                    !nMaxBoneManager::Instance()->IsBone(inode) && 
                    !nMaxBoneManager::Instance()->IsFootStep(inode))
                    //!nMaxBoneManager::Instance()->FindBoneIDByNode(inode))
                {
                    createdNode = ExportGeomObject(inode);
                }
            }
            break;
        case HELPER_CLASS_ID:
            {
                nMaxDummy dummyNode;
                createdNode = dummyNode.Export(inode);
            }
            break;

        default:
            break;
        }
    }

    // if there's any create nebula object node and 3dsmanx node for that has xform
    // we neeed to export it.
    if (createdNode)
    {
        //HACK: is that sure the 'createNode' param is nTransformNode type?
        this->ExportXForm(inode, createdNode, animStart);
    }

    // export nodes recursively.
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
*/
void nMaxScene::ExportLightObject(INode* inode)
{
}

//-----------------------------------------------------------------------------
/**
*/
nSceneNode* nMaxScene::ExportGeomObject(INode* inode)
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
        //ExportMesh();
        nMaxMesh* mesh = n_new(nMaxMesh);

        bool useIndivisualMesh = nMaxOptions::Instance()->UseIndivisualMesh();
        createdNode = mesh->Export(inode, &this->globalMeshBuilder, useIndivisualMesh);

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

    DWORD flag = tm.GetIdentFlags();

    AffineParts ap;
    decomp_affine(tm, &ap);

    //FIXME: ugly type casting.
    nTransformNode* tn = static_cast<nTransformNode*>(sceneNode);

    bool bXForm = false;

    // we only export xform if there's actual xform modifier exist
    // to prevent redundant call of SetPosition() or SetQuat().
    if (flag & POS_IDENT)
    {
        vector3 trans (-ap.t.x, ap.t.z, ap.t.y);
        tn->SetPosition(trans);

        bXForm = true;
    }
    
    if (flag & ROT_IDENT)
    {
        quaternion rot (-ap.q.x, ap.q.z, ap.q.y, -ap.q.w);
        tn->SetQuat(rot);

        bXForm = true;
    }

    if (bXForm)
    {
        n_maxlog(High, "Exported XForm of the node '%s'", inode->GetName());
    }
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxScene::ExportAnimation(const nString &filename)
{
    nAnimBuilder animBuilder;

    if (!CreateAnimation(animBuilder))
    {
        n_maxlog(Error, "Failed to create animation.");
        return false;
    }

    if (animBuilder.Save(nKernelServer::Instance()->GetFileServer(), filename.Get()))
    {
        n_maxlog(Low, "'%s' animation file saved.", filename.Get());
    }
    else
    {
        n_maxlog(Error, "Failed to save '%s' animation file.", filename.Get());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxScene::CreateAnimation(nAnimBuilder &animBuilder)
{
    //FIXME: should be get from nMaxOption.
    int sampleRate = 2;
    float keyDuration = (float)sampleRate / GetFrameRate();
    int sceneFirstKey = 0;

    int numBones = nMaxBoneManager::Instance()->GetNumBones();

    typedef nArray<nMaxSampleKey> Keys;
    Keys keys;
    keys.SetFixedSize(numBones);

    nArray<Keys> keysArray;
    keysArray.SetFixedSize(numBones+1);

    for (int boneIndex=0; boneIndex<numBones; boneIndex++)
    {
        const nMaxBoneManager::Bone &bone = nMaxBoneManager::Instance()->GetBone(boneIndex);
        INode* boneNode = bone.node;

        nMaxControl::GetSampledKey(boneNode, keysArray[boneIndex], sampleRate, nMaxTM);
    }

    nMaxNoteTrack& noteTrack = nMaxBoneManager::Instance()->GetNoteTrack();
    int numAnimStates = noteTrack.GetNumStates();

    for (int state=0; state<numAnimStates; state++)
    {
        const nMaxAnimState& animState = noteTrack.GetState(state);

        nAnimBuilder::Group animGroup;

        TimeValue stateStart = animState.firstFrame * GetTicksPerFrame();
        TimeValue stateEnd   = animState.duration * GetTicksPerFrame();

        int numClips = animState.clipArray.Size();

        int firstKey     = animState.firstFrame / sampleRate;
        int numStateKeys = animState.duration / sampleRate;
        int numClipKeys  = numStateKeys / numClips;

        // do not add anim group, if the number of the state key or the clip keys are 0.
        if (numStateKeys <= 0 || numClipKeys <= 0)
            continue;

        animGroup.SetLoopType(nAnimBuilder::Group::REPEAT);
        animGroup.SetKeyTime(keyDuration);
        animGroup.SetNumKeys(numClipKeys);

        for (int clip=0; clip<numClips; clip++)
        {
            int numBones = nMaxBoneManager::Instance()->GetNumBones();

            for (int boneIdx=0; boneIdx<numBones; boneIdx++)
            {
                nArray<nMaxSampleKey> tmpSampleArray = keysArray[boneIdx];

                nAnimBuilder::Curve animCurveTrans;
                nAnimBuilder::Curve animCurveRot;
                nAnimBuilder::Curve animCurveScale;

                animCurveTrans.SetIpolType(nAnimBuilder::Curve::LINEAR);
                animCurveRot.SetIpolType(nAnimBuilder::Curve::QUAT);
                animCurveScale.SetIpolType(nAnimBuilder::Curve::LINEAR);

                for (int clipKey=0; clipKey<numClipKeys; clipKey++)
                {
                    nAnimBuilder::Key keyTrans;
                    nAnimBuilder::Key keyRot;
                    nAnimBuilder::Key keyScale;

                    int key_idx = firstKey - sceneFirstKey + clip * numClipKeys + clipKey;
                    n_iclamp(key_idx, 0, tmpSampleArray.Size());

                    nMaxSampleKey& skey = tmpSampleArray[key_idx];

                    keyTrans.Set(vector4(-skey.pos.x, skey.pos.z, skey.pos.y, 0.0f));
                    animCurveTrans.SetKey(clipKey, keyTrans);

                    keyRot.Set(vector4(-skey.rot.x, skey.rot.z, skey.rot.y, -skey.rot.w));
                    animCurveRot.SetKey(clipKey, keyRot);

                    keyScale.Set(vector4(skey.scale.x, skey.scale.z, skey.scale.y, 0.0f));
                    animCurveScale.SetKey(clipKey, keyScale);
                }

                animGroup.AddCurve(animCurveTrans);
                animGroup.AddCurve(animCurveRot);
                animGroup.AddCurve(animCurveScale);
            }
        }

        animBuilder.AddGroup(animGroup);
    }

    n_maxlog(Midium, "Optimizing animation curves...");
    int numOptimizedCurves = animBuilder.Optimize();
    n_maxlog(Midium, "Number of optimized curves : %d", numOptimizedCurves);

    animBuilder.FixKeyOffsets();

    return true;
}
