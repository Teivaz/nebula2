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
    - update the triangle group id in the mesh to account for the
      additional partitions
    - a group id mapping array will be created which maps new
      group ids to old group ids

    @param  srcMesh                 the source mesh to partition
    @param  dstMesh                 the destination mesh
    @param  maxJointPaletteSize     max number of joints in a joint palette
*/
bool
nSkinPartitioner::PartitionMesh(nMeshBuilder& srcMesh,
                                nMeshBuilder& dstMesh,
                                int maxJointPaletteSize)
{
    this->partitionArray.Clear();

    // for each triangle...
    int triIndex;
    int numTriangles = srcMesh.GetNumTriangles();
    for (triIndex = 0; triIndex < numTriangles; triIndex++)
    {
        const nMeshBuilder::Triangle& triangle = srcMesh.GetTriangleAt(triIndex);

        // try to add the triangle to an existing partition
        bool triangleAdded = false;
        int numPartitions = this->partitionArray.Size();
        int partitionIndex;
        for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
        {
            if (this->partitionArray[partitionIndex].GetGroupId() == triangle.GetGroupId())
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
            Partition newPartition(&srcMesh, maxJointPaletteSize, triangle.GetGroupId());
            triangleAdded = newPartition.CheckAddTriangle(triIndex);
            n_assert(triangleAdded);
            this->partitionArray.Append(newPartition);
        }
    }

    // update the triangle group ids
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

    int partitionIndex;
    int numPartitions = this->GetNumPartitions();
    for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
    {
        const Partition& partition = this->partitionArray[partitionIndex];

        // record the original group id in the groupMapArray
        this->groupMappingArray.Append(partition.GetGroupId());

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

                // convert the global joint indices to partition-local joint indices
                const vector4& globalJointIndices = v.GetJointIndices();
                vector4 localJointIndices;
                localJointIndices.x = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.x)));
                localJointIndices.y = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.y)));
                localJointIndices.z = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.z)));
                localJointIndices.w = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.w)));

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
