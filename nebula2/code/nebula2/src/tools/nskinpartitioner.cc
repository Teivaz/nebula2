//------------------------------------------------------------------------------
//  nskinpartitioner.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nskinpartitioner.h"

//------------------------------------------------------------------------------
/**
*/
nSkinPartitioner::nSkinPartitioner() :
    partitionArray(64,64)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinPartitioner::~nSkinPartitioner()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Does the actual partitioning. For each triangle:

    - add it to an existing partition object if it has enough space in
      its joint palette
    - otherwise add it to a new empty partition
    - build a new mesh containing one triangle group for each partition
    
    The number or ordering of vertices will not be changed. The number
    of triangles will not change, the ordering will change so that
    triangles belonging to one group are packed together. An internal 
    group mapping array will contain for each new group index the index
    of the original mesh group.

    @param  srcMesh                 the source mesh
    @param  dstMesh                 the destination mesh
    @param  maxJointPaletteSize     max number of joints in a joint palette
*/
bool
nSkinPartitioner::PartitionMesh(nMeshBuilder& srcMesh, 
                                nMeshBuilder& dstMesh, 
                                int maxJointPaletteSize)
{
    this->partitionArray.Clear();

    // pack group triangles together
    srcMesh.PackTrianglesByGroup();

    // for each group in source mesh...
    int numGroups = srcMesh.GetNumGroups();
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const nMeshBuilder::Group group = srcMesh.GetGroupAt(groupIndex);
        int firstTriIndex = group.GetFirstTriangle();
        int lastTriIndex  = firstTriIndex + group.GetNumTriangles();
        int triIndex;
        for (triIndex = firstTriIndex; triIndex < lastTriIndex; triIndex++)
        {
            // try to add the triangle to an existing partition
            bool triangleAdded = false;
            int numPartitions = this->partitionArray.Size();
            int partitionIndex;
            for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
            {
                if (this->partitionArray[partitionIndex].GetGroupIndex() == groupIndex)
                {
                    if (this->partitionArray[partitionIndex].CheckAddTriangle(triIndex))
                    {
                        triangleAdded = true;
                        break;
                    }
                }
            }
            if (!triangleAdded)
            {
                // triangle didn't fit into any existing partition, create a new partition
                Partition newPartition(&srcMesh, maxJointPaletteSize, groupIndex);
                triangleAdded = newPartition.CheckAddTriangle(triIndex);
                n_assert(triangleAdded);
                this->partitionArray.PushBack(newPartition);
            }
        }
    }

    // build a new mesh with one group per partition
    this->BuildDestMesh(srcMesh, dstMesh);

    return true;
}

//------------------------------------------------------------------------------
/**
    Private helper method which builds the actual destination mesh after
    partitioning has happened.
*/
void
nSkinPartitioner::BuildDestMesh(nMeshBuilder& srcMesh, nMeshBuilder& dstMesh)
{
    dstMesh.vertexArray.Clear();
    dstMesh.triangleArray.Clear();
    dstMesh.groupArray.Clear();

    int partitionIndex;
    int numPartitions = this->GetNumPartitions();
    for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
    {
        const Partition& partition = this->partitionArray[partitionIndex];
        
        // add a new group for the partition
        char groupName[N_MAXPATH];
        nMeshBuilder::Group newGroup;
        sprintf(groupName, "g%d", partitionIndex);
        newGroup.SetId(partitionIndex);
        newGroup.SetFirstTriangle(dstMesh.GetNumTriangles());
        newGroup.SetNumTriangles(partition.GetTriangleIndices().Size());
        dstMesh.AddGroup(newGroup);

        // record the original group id in the groupMapArray
        this->groupMappingArray.PushBack(partition.GetGroupIndex());

        // transfer the partition vertices and triangles
        int i;
        const nArray<int>& triArray = partition.GetTriangleIndices();
        for (i = 0; i < triArray.Size(); i++)
        {
            nMeshBuilder::Triangle tri = srcMesh.GetTriangleAt(triArray[i]);

            // transfer the 3 vertices of the triangle
            int origVertexIndex[3];
            tri.GetVertexIndices(origVertexIndex[0], origVertexIndex[1], origVertexIndex[2]);
            int triPoint;
            int newVertexIndex[3];
            for (triPoint = 0; triPoint < 3; triPoint++)
            {
                const nMeshBuilder::Vertex& v = srcMesh.GetVertexAt(origVertexIndex[triPoint]);

                // convert the global joint indices to partition-local joint indices,
                // and multiply them by 3 to get direct vertex shader constant indices
                // (not matrix indices)
                const vector4& globalJointIndices = v.GetJointIndices();
                vector4 localJointIndices;
                localJointIndices.x = 3 * float(partition.GlobalToLocalJointIndex(int(globalJointIndices.x)));
                localJointIndices.y = 3 * float(partition.GlobalToLocalJointIndex(int(globalJointIndices.y)));
                localJointIndices.z = 3 * float(partition.GlobalToLocalJointIndex(int(globalJointIndices.z)));
                localJointIndices.w = 3 * float(partition.GlobalToLocalJointIndex(int(globalJointIndices.w)));

                // add the vertex to the destination mesh and correct the joint indices
                newVertexIndex[triPoint] = dstMesh.GetNumVertices();
                dstMesh.AddVertex(v);
                dstMesh.GetVertexAt(newVertexIndex[triPoint]).SetJointIndices(localJointIndices);
            }

            // update the triangle and add to the dest mesh's triangle array
            tri.SetGroupId(partitionIndex);
            tri.SetVertexIndices(newVertexIndex[0], newVertexIndex[1], newVertexIndex[2]);
            dstMesh.AddTriangle(tri);
        }
    }

    // finally, do a cleanup on the dest mesh to remove any redundant vertices
    dstMesh.Cleanup(0);
}
