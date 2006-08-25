//---------------------------------------------------------------------------
//  nmaxbones.cc
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "export2/nmaxnotetrack.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxcontrol.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/nkernelserver.h"
#include "tools/nanimbuilder.h"

nMaxBoneManager* nMaxBoneManager::Singleton = 0;

//---------------------------------------------------------------------------
/**
*/
nMaxBoneManager::Bone::Bone() :
    parentID(-1),
    id(-1),
    name(""),
    node(0),
    dummy(false)
    
{
}

//---------------------------------------------------------------------------
/**
*/
nMaxBoneManager::nMaxBoneManager()
{
    n_assert(Singleton == 0);
    Singleton = this;
}

//---------------------------------------------------------------------------
/**
*/
nMaxBoneManager::~nMaxBoneManager()    
{
    Singleton = 0;
}

//---------------------------------------------------------------------------
/**
    Retrieves all scene nodes from the given scene.
*/
void nMaxBoneManager::GetNodes(INode* node, nArray<INode*>& nodeArray)
{
    if (0 == node)
        return;

    nodeArray.Append(node);

    // recursively get nodes.
    const int numChildNodes = node->NumberOfChildren();
    for (int i=0; i<numChildNodes; i++)
    {
        GetNodes(node->GetChildNode(i), nodeArray);
    }
}

//---------------------------------------------------------------------------
/**
    @param nodeArray scene nodes array
    @param boneNodeArray array which retrieved bone is contained.
*/
void nMaxBoneManager::GetBoneByModifier(const nArray<INode*>& nodeArray, 
                                        nArray<INode*> &boneNodeArray)
{
    for (int i=0; i<nodeArray.Size(); i++)
    {
        INode* node = nodeArray[i];

        if (!IsGeomObject(node))
            continue;

        Modifier* physique = nMaxUtil::FindPhysique(node);
        Modifier* skin     = nMaxUtil::FindSkin(node);

        if (physique || skin)
        {
            ObjectState os = node->EvalWorldState(0);

            if (os.obj)
            {
                if (physique)
                {
                    ExtractPhysiqueBones(node, physique, &os, boneNodeArray);
                }
                else
                if (skin)
                {
                    ExtractSkinBones(node, skin, boneNodeArray);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxBoneManager::GetBoneByClassID(const nArray<INode*>& nodeArray, 
                                       nArray<INode*> &boneNodeArray)
{
    for (int i=0; i<nodeArray.Size(); i++)
    {
        INode* node = nodeArray[i];

        // check the given node is bone node.
        bool isBone = this->IsBone(node);

        //HACK: if the dummy node is just like a group node or something 
        //      which is not actually bone?
        //      dummy node can be used in various way in 3dsmax.
        //      so it should be considered when it actually used for bone.
        //      A dummy node which used for bone should be added when it has modifier 
        //      in GetBoneByModifier() func

        //bool isDummy = this->IsDummy(node);

        //if (isBone || isDummy)
        if (isBone)
        {
            if (!boneNodeArray.Find(node))
            {
                boneNodeArray.Append(node);
            }
        }
    }
}

//---------------------------------------------------------------------------
/**
    The way collecting nodes of bone type is a bit tricky.

    3dsmax support any type of object to be bone, so it is not enough
    to check an object which type is bone or biped.
    So, we check an object which has any physique or skin modifier and 
    retrieve bones from it via physique(or skin) interface.

    It is accomplished by two ways to collect bones from the given scene.
    First, we check modifier of the geometry node's and collect bones from the
    modifier. Second, we collect bones by its class ID if the class ID is one of
    the bone's or biped's one.
    Functions for each of the ways are GetBoneByModifier() and GetBoneByClassID().

    Next thing to do is find the root bone from collected bones. The closet bone
    node from scene root is selected for root bone. (Note that the root bone is
    not always a child of scene root node)

    Example:
    @verbatim
    scene root
        bone A
            bone B
            bone C
                constraint
                    bone D
    @endverbatim

    In the above example, if the bone A is not used for the animation, the root
    bone is bone B.

    And last we build bone array and indices of its elements which will be used 
    for joint indices of vertices. 
    At this time, only collected bones are added to the bone array.
    An artist can add constraint or something other which is not a bone to a bone 
    hierarchy and it should be filtered out when the bone array is built.

*/
bool nMaxBoneManager::BuildBones(INode* node)
{
    INode* root = nMaxInterface::Instance()->GetInterface()->GetRootNode();
    n_assert(root == node);

    INode* sceneRoot = node;

    // array for scene nodes.
    nArray<INode*> nodeArray;

    // retrieves all scene nodes.
    this->GetNodes(node, nodeArray);

    for (int i=0; i<nodeArray.Size(); i++)
    {
        n_maxlog(Medium, "    nodes in array: %s", nodeArray[i]->GetName());
    }

    // array for bone nodes.
    nArray<INode*> boneNodeArray;

    // retrieves only bones from scene nodes.
    this->GetBoneByModifier(nodeArray, boneNodeArray);

    n_maxlog(Medium, "Actually used number of bones : %d", boneNodeArray.Size());

    this->GetBoneByClassID(nodeArray, boneNodeArray);

    // if there is no bones in the scene, just return.
    if (boneNodeArray.Size() <= 0)
    {
        n_maxlog(Medium, "The scene has no bones.");
        return true;
    }

    // get top most level of the bone in the bone array.
    // this bone will be used as the root bone.
    nArray<INode*> rootBonesNodeArray;
    this->GetRootBones(sceneRoot, boneNodeArray, rootBonesNodeArray);

    for (int i=0; i<rootBonesNodeArray.Size(); i++) {
        this->skeletonsArray.Append(Skeleton());
        this->noteTracksArray.Append(nMaxNoteTrack());
        // build bone array.
        this->ReconstructBoneHierarchy(-1, i, rootBonesNodeArray[i], boneNodeArray);

        // extract animation state from note track.
        for (int ai=0; ai<this->GetNumBones(i); ai++)
        {
            Bone bone = this->GetBone(i, ai);
            n_maxlog(High, "%s ID:%d", bone.name.Get(), bone.id);

            // build animation states.
            INode* boneNode = bone.node;

            this->noteTracksArray[i].GetAnimState(boneNode);
        }

        // if there are no animation states, we add default one.
        // (skin animator needs it at least one)
        if (this->noteTracksArray[i].GetNumStates() <= 0) {
            //FIXME: fix to get proper first frame.
            int firstframe   = 0;
            int duration     = nMaxInterface::Instance()->GetNumFrames();
            float fadeintime = 0.0f;
            this->noteTracksArray[i].AddAnimState(firstframe, duration, fadeintime);
        }

        n_maxlog(Medium, "Found %d bones", this->GetNumBones());
    }

    std::map<INode*, INode*>::iterator iter = nodeToBone.begin();
    std::map<INode*, int>::iterator fj;
    for (; iter != nodeToBone.end(); ++iter) {
        fj = boneToSkel.find(iter->second);
        if (fj != boneToSkel.end()) {
            nodeToSkel[iter->first] = fj->second;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
int nMaxBoneManager::GetSkelForNode(INode* inode) {
    std::map<INode*, int>::iterator fj;
    n_assert(inode);
    fj = nodeToSkel.find(inode);
    if (fj != nodeToSkel.end()) {
       return fj->second;
    }
    return -1;
}
//-----------------------------------------------------------------------------
struct BoneLevel
{
    INode *node;
    int   depth;
};



//-----------------------------------------------------------------------------
/**
*/
int nMaxBoneManager::GetRootBones(INode *sceneRoot, nArray<INode*> &boneNodeArray, nArray<INode*> &rootBonesNodeArray) {
    int i, j, k;

    #ifdef _DEBUG
        //for (i=0; i<boneNodeArray.Size(); i++)
        //{
        //    n_maxlog(Medium, "Before Sort: %s", boneNodeArray[i]->GetName());
        //}
    #endif

    nArray<BoneLevel> boneLevelArray;
    
    INode* bone;
    int depth;

    // evaluate depth of the bone in the hierarchy.
    for(i=0; i<boneNodeArray.Size(); i++)
    {
        bone = boneNodeArray[i];
        depth = 0;

        while(bone != sceneRoot)
        {
            bone = bone->GetParentNode();
            depth++;
        }

        BoneLevel bl;
        bl.node  = boneNodeArray[i];
        bl.depth = depth;
        boneLevelArray.Append(bl);
    }

    n_assert(boneNodeArray.Size() == boneLevelArray.Size());

    // do selection sort.
    BoneLevel tmp;
    for(i=0; i<boneLevelArray.Size()-1; i++)
    {
        k = i;
        for (j=i+1; j<boneLevelArray.Size(); j++)
        {
            // find the bone which has lowest depth value.
            if (boneLevelArray[k].depth > boneLevelArray[j].depth)
            {
                k = j;
            }
        }

        tmp = boneLevelArray[i];
        boneLevelArray[i] = boneLevelArray[k];
        boneLevelArray[k] = tmp;
    }

    #ifdef _DEBUG
        //for (i=0; i<boneLevelArray.Size(); i++)
        //{
        //    n_maxlog(Medium, "After Sort: %s %d", boneLevelArray[i].node->GetName(), boneLevelArray[i].depth);
        //}
    #endif

    // the first node in boneLevelArray is the root bone node.
    depth = boneLevelArray[0].depth;
    for (int i=0; i<boneLevelArray.Size(); i++) {
        if (boneLevelArray[i].depth == depth) {
            rootBonesNodeArray.Append(boneLevelArray[i].node);
        }
    }
    return rootBonesNodeArray.Size();
}


//-----------------------------------------------------------------------------
/**
*/
void nMaxBoneManager::ReconstructBoneHierarchy(int parentID, int skeleton, INode* node, nArray<INode*> &boneNodeArray) {
    Bone bone;

    bone.localTransform = nMaxTransform::GetLocalTM(node, 0);
    bone.parentID       = parentID;
    bone.id             = this->skeletonsArray[skeleton].Size();
    bone.name           = nMaxUtil::CorrectName(node->GetName());
    bone.node           = node;

    // add only known bone.
    if (boneNodeArray.Find(node)) {
        this->skeletonsArray[skeleton].Append(bone);
        if (this->boneToSkel.find(bone.node) == this->boneToSkel.end()) {
            this->boneToSkel[bone.node] = skeleton;
        }
    }

    for (int i=0; i<node->NumberOfChildren(); i++)
    {
        this->ReconstructBoneHierarchy(bone.id, skeleton, node->GetChildNode(i), boneNodeArray);
    }
}


//-----------------------------------------------------------------------------
/**
*/
void nMaxBoneManager::AddBoneToNode(INode* inode, INode* bone) {
    n_assert(inode);
    n_assert(bone);
    if (this->nodeToBone.find(inode) == this->nodeToBone.end()) {
        this->nodeToBone[inode] = bone;
    }
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed a bug that it cannot find correct nskinanimator 
                      when there are more than one root bones. (The case that
                      two or more nskinanimator is created)
                      When multiple nskinanimator are created, already added 
                      bone has -1 so it always fails to find correct skinanimator 
                      because nskinshapenode always find the first skinanimaotr0 
                      when it try to find nskinanimator.
                      Thanks Kim, Seung Hoon for this fix.
*/
void nMaxBoneManager::ExtractPhysiqueBones(INode* node, Modifier* phyMod, ObjectState* os, 
                                     nArray<INode*> &boneNodeArray)
{
    if (phyMod == NULL) 
        return;

    // create a Physique Export Interface for the given Physique Modifier       
    IPhysiqueExport *phyExport = (IPhysiqueExport *)phyMod->GetInterface(I_PHYINTERFACE);

    if (phyExport)
    {
        // create a ModContext Export Interface for the specific node of the Physique Modifier
        IPhyContextExport *mcExport = (IPhyContextExport *)phyExport->GetContextInterface(node);

        if (mcExport)
        {
            // we convert all vertices to Rigid in this example
            mcExport->ConvertToRigid(TRUE);
            mcExport->AllowBlending(TRUE);

            // compute the transformed Point3 at time t
            for (int i=0; i<os->obj->NumPoints(); i++)
            {
                IPhyVertexExport *vtxExport = mcExport->GetVertexInterface(i);
                if (vtxExport)
                {
                    //need to check if vertex has blending
                    if (vtxExport->GetVertexType() & BLENDED_TYPE)
                    {
                        IPhyBlendedRigidVertex *vtxBlend = (IPhyBlendedRigidVertex *)vtxExport;

                        for (int n=0; n<vtxBlend->GetNumberNodes(); n++)
                        {
                            INode* bone	= vtxBlend->GetNode(n);

                            // add found bone to the bones array.
                            if (!boneNodeArray.Find(bone))
                            {
                                nString nodeName = node->GetName();
                                nString boneName = bone->GetName();
                                n_maxlog(High, "      node '%s' -> bone '%s' ", nodeName.Get(), boneName.Get());
                                boneNodeArray.Append(bone);
                            }

                            this->AddBoneToNode(node, bone);
                        }

                        mcExport->ReleaseVertexInterface(vtxExport);
                        vtxExport = NULL;	
                    }
                    else 
                    {
                        IPhyRigidVertex *rigidVertex = (IPhyRigidVertex *)vtxExport;
                        INode* bone = rigidVertex->GetNode();

                        // add found bone to the bones array.
                        if (!boneNodeArray.Find(bone))
                        {
                            nString nodeName = node->GetName();
                            nString boneName = bone->GetName();
                            n_maxlog(High, "      node '%s' -> bone '%s' ", nodeName.Get(), boneName.Get());
                            boneNodeArray.Append(bone);
                        }

                        this->AddBoneToNode(node, bone);

                        mcExport->ReleaseVertexInterface(vtxExport);
                        vtxExport = NULL;
                    }
                }
            }

            phyExport->ReleaseContextInterface(mcExport);
        }

        phyMod->ReleaseInterface(I_PHYINTERFACE, phyExport);
    }
}

//-----------------------------------------------------------------------------
/**
    -03-Jun-06  kims  Fixed a bug that it cannot find correct nskinanimator 
                      when there are more than one root bones. (The case that
                      two or more nskinanimator is created)
                      When multiple nskinanimator are created, already added 
                      bone has -1 so it always fails to find correct skinanimator 
                      because nskinshapenode always find the first skinanimaotr0 
                      when it try to find nskinanimator.
                      Thanks Kim, Seung Hoon for this fix.

*/
void nMaxBoneManager::ExtractSkinBones(INode* node, Modifier* skinMod, 
                                       nArray<INode*> &boneNodeArray)
{
    // get the skin interface
    ISkin* skin = (ISkin*)skinMod->GetInterface(I_SKIN);
    if (skin == NULL)
        return;

    // get the skin context
    ISkinContextData* context = skin->GetContextInterface( node );

    if (context != NULL)
    {
        // for all skinned points
        const int numPoints = context->GetNumPoints();
        for (int i=0; i<numPoints; i++)
        {
            const int numBones = context->GetNumAssignedBones( i );
            for (int b=0; b<numBones; b++)
            {
                // check if it is a valid bone number
                int	boneNr	= context->GetAssignedBone(i, b);
                if (boneNr < 0)
                {
                    boneNr = 0;
                    continue;
                }

                INode* bone	= skin->GetBone( context->GetAssignedBone(i, b) );

                // add found bone to the bones array
                if (!boneNodeArray.Find(bone))
                {
                    nString nodeName = node->GetName();
                    nString boneName = bone->GetName();
                    n_maxlog(High, "      node '%s' -> bone '%s' ", nodeName.Get(), boneName.Get());
                    boneNodeArray.Append(bone);
                }
                this->AddBoneToNode(node, bone);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxBoneManager::IsGeomObject(INode *node)
{
    ObjectState os = node->EvalWorldState(0); 

    // only add geometry nodes
    if (os.obj)
    {
        if (os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
        {
            if (os.obj->ClassID() != Class_ID(TARGET_CLASS_ID, 0))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Check the given node is bone or not.

    @note
    This method does not check 'FOOTPRINT_CLASS_ID' type.
    see nMaxBone::IsFootStep()

    @param inode pointer to INode
    @return true, if the given INode is bone.
*/
bool nMaxBoneManager::IsBone(INode *inode)
{
    if (NULL == inode)
        return false;

    if (inode->IsRootNode())
        return false;

    ObjectState os;
    os = inode->EvalWorldState(0);
    if (os.obj && os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0x00))
        return true;

#if MAX_RELEASE >= 4000
    if (os.obj && os.obj->ClassID() == Class_ID(BONE_OBJ_CLASSID))
        return true;
#endif
    if (os.obj && os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0x00))
        return false;

    // check for biped node
    Control *control;
    control = inode->GetTMController();

    if ((control->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
        (control->ClassID() == BIPBODY_CONTROL_CLASS_ID))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
/**
    Check the given node is Biped's foot step or not.
*/
bool nMaxBoneManager::IsFootStep(INode* inode)
{
    if (NULL == inode)
        return false;

    if (inode->IsRootNode())
        return false;

    // check for biped node
    Control *control;
    control = inode->GetTMController();

    if ((control->ClassID() == FOOTPRINT_CLASS_ID))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxBoneManager::IsDummy(INode* inode)
{
    if (NULL == inode)
        return false;

    ObjectState os;
    os = inode->EvalWorldState(0);

    if (os.obj && os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0x00))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxBoneManager::FindBoneIDByNode(INode* inode)
{
    for (int s=0; s<this->skeletonsArray.Size(); s++) {
        for (int i=0; i<this->skeletonsArray[s].Size(); i++)
    {
            if (skeletonsArray[s][i].node == inode)
        {
                return skeletonsArray[s][i].id;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
/**
    @param name
    @return array index for bone array.
*/
int nMaxBoneManager::FindBoneIDByName(const nString &name)
{
    for (int s=0; s<this->skeletonsArray.Size(); s++) {
        for (int i=0; i<this->skeletonsArray[s].Size(); i++)
    {
            if (this->skeletonsArray[s][i].name == name)
        {
                return this->skeletonsArray[s][i].id;
            }
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
    @param nodeID
    @return array index for bone array.
*/
int nMaxBoneManager::FindBoneIndexByNodeId(int nodeID)
{
    for (int i=0;i<this->skeletonsArray[0].Size(); i++)
    {
        if (skeletonsArray[0][i].id == nodeID)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
/**
*/
INode* nMaxBoneManager::FindBoneNodeByIndex(int index)
{
    for (int i=0; i<this->skeletonsArray[0].Size(); i++)
    {
        if (skeletonsArray[0][i].id == index)
        {
            return skeletonsArray[0][i].node;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Build skeletal animations and saves it to the disc.

    @param animFileName .nanim2(or .nax2) file name
*/
bool nMaxBoneManager::Export(int skelIndex, const char* animFileName) {
    n_assert(animFileName);
    n_assert(skelIndex >= 0);

    nAnimBuilder animBuilder;

    // retrieves sampled keys.
    int sampleRate = nMaxOptions::Instance()->GetSampleRate();
    float keyDuration = (float)sampleRate / GetFrameRate();
    int sceneFirstKey = 0;

    int numBones = this->GetNumBones(skelIndex);

    typedef nArray<nMaxSampleKey> Keys;
    Keys keys;
    keys.SetFixedSize(numBones);

    nArray<Keys> keysArray;
    keysArray.SetFixedSize(numBones+1);

    for (int boneIndex=0; boneIndex<numBones; boneIndex++)
    {
        const nMaxBoneManager::Bone &bone = this->GetBone(skelIndex, boneIndex);
        INode* boneNode = bone.node;

        nMaxControl::GetSampledKey(boneNode, keysArray[boneIndex], sampleRate, nMaxTM);
    }

    // builds skeletal animations.
    nMaxNoteTrack& noteTrack = this->GetNoteTrack(skelIndex);
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

        // determine animations loop type. 
        // the value is specified inside of nMaxNoteTrack::GetAnimState(). 
        // default is 'REPEAT'.
        switch(animState.loopType) 
        {
        case nAnimBuilder::Group::REPEAT:
            animGroup.SetLoopType(nAnimBuilder::Group::REPEAT);
        	break;
        case nAnimBuilder::Group::CLAMP:
            animGroup.SetLoopType(nAnimBuilder::Group::CLAMP);
            break;
        default:
            break; // unknown loop type.
        }
        
        animGroup.SetKeyTime(keyDuration);
        animGroup.SetNumKeys(numClipKeys);

        for (int clip=0; clip<numClips; clip++)
        {
            int numBones = this->GetNumBones(skelIndex);

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
                    key_idx = n_iclamp(key_idx, 0, tmpSampleArray.Size() - 1);

                    nMaxSampleKey& skey = tmpSampleArray[key_idx];

                    skey.tm.NoScale();

                    AffineParts ap;
                    decomp_affine(skey.tm, &ap );

                    keyTrans.Set(vector4(-ap.t.x, ap.t.z, ap.t.y, 0.0f));
                    animCurveTrans.SetKey(clipKey, keyTrans);

                    keyRot.Set(vector4(-ap.q.x, ap.q.z, ap.q.y, -ap.q.w));
                    animCurveRot.SetKey(clipKey, keyRot);

                    keyScale.Set(vector4(ap.k.x, ap.k.z, ap.k.y, 0.0f));
                    animCurveScale.SetKey(clipKey, keyScale);
                }

                animGroup.AddCurve(animCurveTrans);
                animGroup.AddCurve(animCurveRot);
                animGroup.AddCurve(animCurveScale);
            }
        }

        animBuilder.AddGroup(animGroup);
    }

    n_maxlog(Medium, "Optimizing animation curves...");
    int numOptimizedCurves = animBuilder.Optimize();
    n_maxlog(Medium, "Number of optimized curves : %d", numOptimizedCurves);

    animBuilder.FixKeyOffsets();

    if (animBuilder.Save(nKernelServer::Instance()->GetFileServer(), animFileName))
    {
        n_maxlog(Low, "'%s' animation file was saved.", animFileName);
    }
    else
    {
        n_maxlog(Error, "Failed to save '%s' animation file.", animFileName);
        return false;
    }

    return true;
}
