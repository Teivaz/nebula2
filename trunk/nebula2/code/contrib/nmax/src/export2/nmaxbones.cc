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
//---------------------------------------------------------------------------
/**
*/
void nMaxBoneManager::GetRootBones(INode* sceneRoot, const nArray<INode*> &boneNodeArray, 
                             nArray<INode*> &rootBoneArray)
{
    for(int i=0; i<boneNodeArray.Size(); i++)
    {
        INode* node = boneNodeArray[i];

        if (node->GetParentNode() == sceneRoot)
        {
            // it's the first node in bone node hierarchy.
            rootBoneArray.Append(node);
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
    The way collect bone type node is a bit tricky.
    3dsmax support any type of object to be bone, so it is not enough
    to check only object type to know the given node is bone or not.
    We should a object has any physique or skin modifier and if the
    object has it, retrieve bone from it refer via physique(or skin)
    interface.

    It uses two way to collect bones from the given scene.
    One is that by node class ID and the other is that by check the modifier
    of the node. Functions for each of the way are GetBoneByModifier() and
    GetBoneByClassID().

*/
void nMaxBoneManager::Build(INode* node)
{
    INode* sceneRoot = node;

    // array for scene nodes.
    nArray<INode*> nodeArray;

    // retrieves all scene nodes.
    this->GetNodes(node, nodeArray);

    // array for bone nodes.
    nArray<INode*> boneNodeArray;

    // retrieves only bones from scene nodes.
    this->GetBoneByModifier(nodeArray, boneNodeArray);

    n_maxlog(Midium, "Actually used number of bones : %d", boneNodeArray.Size());

    this->GetBoneByClassID(nodeArray, boneNodeArray);

    // if there is no bones in the scene, just return.
    if (boneNodeArray.Size() <= 0)
    {
        n_maxlog(Midium, "The scene has no bones.");
        return;
    }

    nArray<INode*> rootBoneArray;
    this->GetRootBones(sceneRoot, boneNodeArray, rootBoneArray);

    // recursively builds bones.
    this->BuildBones(-1, rootBoneArray.At(0));

    for (int i=0; i<this->boneArray.Size(); i++)
    {
        Bone bone = this->boneArray[i];
        n_maxlog(High, "%s ID:%d", bone.name.Get(), bone.id);

        // build animation states.
        INode* boneNode = bone.node;

        noteTrack.GetAnimState(boneNode);
    }

    n_maxlog(Midium, "Found %d bones", this->GetNumBones());
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxBoneManager::BuildBones(int parentID, INode* node)
{
    Bone bone;

    bone.localTransform = nMaxTransform::GetLocalTM(node, 0);
    bone.parentID       = parentID;
    bone.id             = this->boneArray.Size();
    bone.name           = nMaxUtil::CorrectName(node->GetName());
    bone.node           = node;

    this->boneArray.Append(bone);

    for (int i=0; i<node->NumberOfChildren(); i++)
    {
        BuildBones(bone.id, node->GetChildNode(i));
    }
}

//-----------------------------------------------------------------------------
/**
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
                                boneNodeArray.Append(bone);
                            }
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
                            boneNodeArray.Append(bone);
                        }

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
*/
void nMaxBoneManager::ExtractSkinBones(INode* node, Modifier* skinMod, nArray<INode*> &boneNodeArray)
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
                    boneNodeArray.Append(bone);
                }
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
    for (int i=0;i<this->boneArray.Size(); i++)
    {
        if (boneArray[i].node == inode)
        {
            return boneArray[i].id;
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
    for (int i=0;i<this->boneArray.Size(); i++)
    {
        if (this->boneArray[i].name == name)
        {
            return this->boneArray[i].id;
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
    for (int i=0;i<this->boneArray.Size(); i++)
    {
        if (boneArray[i].id == nodeID)
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
    for (int i=0; i<this->boneArray.Size(); i++)
    {
        if (boneArray[i].id == index)
        {
            return boneArray[i].node;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Build skeletal animations and saves it to the disc.

    @param animFileName .nanim2(or .nax2) file name
*/
bool nMaxBoneManager::Export(const char* animFileName)
{
    n_assert(animFileName);

    nAnimBuilder animBuilder;

    // retrieves smapled keys.
    int sampleRate = nMaxOptions::Instance()->GetSampleRate();
    float keyDuration = (float)sampleRate / GetFrameRate();
    int sceneFirstKey = 0;

    int numBones = this->GetNumBones();

    typedef nArray<nMaxSampleKey> Keys;
    Keys keys;
    keys.SetFixedSize(numBones);

    nArray<Keys> keysArray;
    keysArray.SetFixedSize(numBones+1);

    for (int boneIndex=0; boneIndex<numBones; boneIndex++)
    {
        const nMaxBoneManager::Bone &bone = this->GetBone(boneIndex);
        INode* boneNode = bone.node;

        nMaxControl::GetSampledKey(boneNode, keysArray[boneIndex], sampleRate, nMaxTM);
    }

    // builds skeletal animations.
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
            int numBones = this->GetNumBones();

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

    if (animBuilder.Save(nKernelServer::Instance()->GetFileServer(), animFileName))
    {
        n_maxlog(Low, "'%s' animation file saved.", animFileName);
    }
    else
    {
        n_maxlog(Error, "Failed to save '%s' animation file.", animFileName);
        return false;
    }

    return true;
}