//-----------------------------------------------------------------------------
//  nmaxmesh_skin.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxutil.h"
#include "util/nstring.h"
#include "export2/nmaxoptions.h"
#include "tools/nmeshbuilder.h"

#include "export2/nmaxmesh.h"
#include "scene/nskinshapenode.h"
#include "tools/nskinpartitioner.h"

//-----------------------------------------------------------------------------
/**
*/
bool nMaxMesh::IsSkinned()
{
    return isSkinned;
}
//-----------------------------------------------------------------------------
/**
*/
bool nMaxMesh::IsPhysique()
{
    return isPhysique;
}
//-----------------------------------------------------------------------------
/**
*/
IPhyVertexExport* nMaxMesh::GetVertexInterface(int vertexId)
{
    return (IPhyVertexExport*)phyContextExport->GetVertexInterface(vertexId);
}
//-----------------------------------------------------------------------------
/**
*/
void nMaxMesh::ReleaseVertexInterface(IPhyVertexExport* phyVertexExport)
{
    //IPhyVertexExport* vertexExport = static_cast<IPhyVertexExport*>(phyContextExport);
    phyContextExport->ReleaseVertexInterface(phyVertexExport);
}
//-----------------------------------------------------------------------------
/**
*/
ISkinContextData* nMaxMesh::GetSkinContextData()
{
    return skinContext;
}
//-----------------------------------------------------------------------------
/**
*/
ISkin* nMaxMesh::GetSkin()
{
    return skin;
}
//-----------------------------------------------------------------------------
/**
    Determine if the given mesh has physique or skin modifier.

    @note
    DO NOT FORGET TO CALL EndSkin()

    @param node 3dsmax's node.
    @return true, if the given mesh has physique or skin modifier
*/
bool nMaxMesh::BeginSkin(INode* node)
{
    n_assert(node);
    n_assert(beginSkin == false);

    beginSkin = true;   

    // Get object from given node.
    Object *obj = node->GetObjectRef();
    if (!obj)
    {
        return false;
    }

    // find physique first
    modifier = nMaxUtil::FindModifier(obj, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
    if (modifier)
    {
        // create a physique export interface for the given physique modifier.
        phyExport = (IPhysiqueExport*)modifier->GetInterface(I_PHYINTERFACE);
        if (NULL == phyExport)
        {
            return false;
        }

        //phyExport->SetInitialPose(true);

        // create a ModContext Export Interface for the given node of the physique modifier.
        phyContextExport = (IPhyContextExport*)phyExport->GetContextInterface(node);
        if (NULL == phyContextExport)
        {
            return false;
        }

        // we convert all vertices to rigid
        phyContextExport->ConvertToRigid(TRUE);
        phyContextExport->AllowBlending(TRUE);

        Matrix3 initTM;
        int result = phyExport->GetInitNodeTM(node, initTM);
        if (result != MATRIX_RETURNED)
        {
            initTM = node->GetObjectTM(0);
        }

        isPhysique = true;
        isSkinned = false;

        return true;
    }
    else
    {
        // no physique, so we try to find skin modifier.
        modifier = nMaxUtil::FindModifier(obj, SKIN_CLASSID);
        if (!modifier)
        {
            return false;
        }

        skin = (ISkin*)modifier->GetInterface(I_SKIN);
        if (NULL == skin)
        {
            return false;
        }

        skinContext = skin->GetContextInterface(node);

        isPhysique = false;
        isSkinned = true;

        return true;
    }

    object = obj;

    // not physique and skin
    return false;
}

//-----------------------------------------------------------------------------
/**
    Uninitialize things for any physique or skin modifier.
*/
void nMaxMesh::EndSkin()
{
    n_assert(beginSkin == true);

    if (isPhysique)
    {
        phyExport->ReleaseContextInterface(phyContextExport);
        modifier->ReleaseInterface(I_PHYINTERFACE, phyExport);
    }

    isPhysique = false;
    isSkinned = false;
    beginSkin = false;  
    modifier = 0;
    phyExport = 0;
    phyContextExport = 0;
}

//-----------------------------------------------------------------------------
/**
    @note
    Call this function only when use individual mesh file.
*/
void nMaxMesh::PartitionMesh()
{
    nArray<nMaxMeshFragment> meshFragmentArray;

    nMeshBuilder tmpMeshBuilder;
    nSkinPartitioner skinPartitioner;

    int maxJointPaletteSize = nMaxOptions::Instance()->GetMaxJointPaletteSize();

    // do skin partitioning.
    if (skinPartitioner.PartitionMesh(this->localMeshBuilder, tmpMeshBuilder, maxJointPaletteSize))
    {
        //n_maxlog(Midium, "Number of partitions: %d", skinPartitioner.GetNumPartitions());

        for (int i=0; i<this->GetNumGroupMeshes(); i++)
        {
            const nMaxGroupMesh& groupMesh = this->GetGroupMesh(i);

            nSkinShapeNode* node = groupMesh.GetNode(); 
            int groupIndex = groupMesh.GetGroupIndex();

            // create per shape.
            nMaxMeshFragment meshFragment;
            meshFragment.node = node;

            if (groupIndex >= 0)
            {
                const nArray<int>& groupMapArray = skinPartitioner.GetGroupMappingArray();

                for ( int j=0; j<groupMapArray.Size(); j++ )
                {
                    if ( groupMapArray[j] == groupIndex )
                    {                          
                        nArray<int> bonePaletteArray = skinPartitioner.GetJointPalette(j);

                        if (bonePaletteArray.Size() > 0)
                        {

                            // fragment group index : j
                            // fragment bone palette array : bonePaletteArray;
                            // create per shape fragment
                            nMaxMeshFragment::Fragment frg;

                            frg.groupMapIndex    = j;
                            frg.bonePaletteArray = bonePaletteArray;

                            meshFragment.fragmentArray.Append(frg);
                        }
                    }
                }
            }

            meshFragmentArray.Append(meshFragment);
        }
    }

    this->localMeshBuilder = tmpMeshBuilder;

    // build skin shape node's fragments.
    BulldMeshFragments(meshFragmentArray);
}

//-----------------------------------------------------------------------------
/**
    Build nskinshapenode's fragments and joint palettes.

    @param meshFragmentArray mesh fragments array.
*/
void nMaxMesh::BulldMeshFragments(nArray<nMaxMeshFragment>& meshFragmentArray)
{
    // build skin shape node's fragments.
    for (int i=0; i<meshFragmentArray.Size(); i++)
    {
        nMaxMeshFragment& meshFragment = meshFragmentArray[i];

        int numFragments = meshFragment.fragmentArray.Size();
        if (numFragments > 0)
        {
            nSkinShapeNode* node = meshFragment.node;

            node->BeginFragments(numFragments);

            for (int j=0; j<numFragments; j++)
            {
                nMaxMeshFragment::Fragment& frag = meshFragment.fragmentArray[j];

                node->SetFragGroupIndex(j, frag.groupMapIndex);

                int numJointPaletteSize = frag.bonePaletteArray.Size();
                node->BeginJointPalette(j, numJointPaletteSize);

                for (int k=0; k<numJointPaletteSize; k++)
                {
                    node->SetJointIndex(j, k, frag.bonePaletteArray[k]);
                }

                node->EndJointPalette(j);
            }

            node->EndFragments();
        }// end of if
    }//end of for
}
