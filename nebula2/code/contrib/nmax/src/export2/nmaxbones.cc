//---------------------------------------------------------------------------
//  nmaxbones.cc
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#include "export2/nmax.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxtransform.h"

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
            // it's the first node in bone node hierarcy.
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

        //HACK: if the dummy node is just like a group node or something not 
        //      actually bone?
        bool isDummy = this->IsDummy(node);

        if (isBone || isDummy)
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

*/
//void nMaxBoneManager::BuildBoneList(int parentID, INode* node)
void nMaxBoneManager::BuildBoneList(INode* node)
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
    }
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
                    //MCore::LOG("Help! BoneNr is < 0 (%d)", boneNr);
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
