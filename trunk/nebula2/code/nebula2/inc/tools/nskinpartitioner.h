#ifndef N_SKINPARTITIONER_H
#define N_SKINPARTITIONER_H
//------------------------------------------------------------------------------
/**
    @class nSkinPartitioner
    @ingroup NebulaGraphicsSystemTools

    Partitions a skin for a given joint palette size. The initial skin is
    represented by a nMeshBuilder object.

    @note
    Do not partition a skinned mesh which is used for shadow.
    If you do, it does not work correctly. See @ref Shadow2 section for
    more details about how to the shadow system of Nebula2 works.

    (C) 2003 RadonLabs GmbH
*/
#include "tools/nmeshbuilder.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nSkinPartitioner
{
public:
    /// constructor
    nSkinPartitioner();
    /// destructor
    ~nSkinPartitioner();
    /// do the actual partitioning
    bool PartitionMesh(nMeshBuilder& srcMesh, nMeshBuilder& dstMesh, int maxJointPaletteSize);
    /// get number of partitions
    int GetNumPartitions() const;
    /// get a joint palette of a partition
    const nArray<int>& GetJointPalette(int partitionIndex) const;
    /// get triangle indices of a partition
    const nArray<int>& GetTriangleIndices(int partitionIndex) const;
    /// get the group mapping array which maps new group indices to original group indices
    const nArray<int>& GetGroupMappingArray() const;

private:
    /// build destination mesh from partition info
    void BuildDestMesh(nMeshBuilder& srcMesh, nMeshBuilder& dstMesh);

    /// a partition object
    class Partition
    {
    public:
        /// default constructor
        Partition();
        /// constructor
        Partition(nMeshBuilder* mBuilder, int maxJoints, int groupId);
        /// check if a triangle can be added, and if yes, add the triangle
        bool CheckAddTriangle(int triangleIndex);
        /// get joint palette of the partition
        const nArray<int>& GetJointPalette() const;
        /// get the triangle indices of the partition
        const nArray<int>& GetTriangleIndices() const;
        /// get the original group index the partition belongs to
        int GetGroupId() const;
        /// convert a global joint index into a partition-local joint index
        int GlobalToLocalJointIndex(int globalJointIndex) const;

    private:
        /// add a unique index to an int array
        void AddUniqueJointIndex(nArray<int>& intArray, int index);
        /// get all joint indices of a triangle
        void GetTriangleJoints(const nMeshBuilder::Triangle& tri, nArray<int>& triJoints);
        /// get all triangle joint indices not currently in joint palette
        void GetJointIndexDifferenceSet(const nArray<int>& triJoints, nArray<int>& diffSet);

        enum
        {
            MaxJointsInSkeleton = 1024,
        };

        nMeshBuilder* sourceMesh;
        int maxJointPaletteSize;
        int groupId;
        bool jointMask[MaxJointsInSkeleton];
        nArray<int> jointPalette;
        nArray<int> triangleArray;
    };

    nArray<Partition> partitionArray;
    nArray<int> groupMappingArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSkinPartitioner::Partition::Partition() :
    sourceMesh(0),
    maxJointPaletteSize(0),
    groupId(0),
    jointPalette(32, 32),
    triangleArray(2048, 2048)
{
    memset(this->jointMask, 0, sizeof(this->jointMask));
}

//------------------------------------------------------------------------------
/**
*/
inline
nSkinPartitioner::Partition::Partition(nMeshBuilder* mBuilder, int maxJoints, int gId) :
    sourceMesh(mBuilder),
    maxJointPaletteSize(maxJoints),
    groupId(gId),
    jointPalette(32, 32),
    triangleArray(2048, 2048)
{
    n_assert(maxJoints <= MaxJointsInSkeleton);
    memset(this->jointMask, 0, sizeof(this->jointMask));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSkinPartitioner::Partition::GetGroupId() const
{
    return this->groupId;
}

//------------------------------------------------------------------------------
/**
    Add a unique index to an int array.
*/
inline
void
nSkinPartitioner::Partition::AddUniqueJointIndex(nArray<int>& intArray, int index)
{
    int num = intArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (intArray[i] == index)
        {
            return;
        }
    }
    // fallthrough: new index
    intArray.Append(index);
}

//------------------------------------------------------------------------------
/**
    Fills the provided int array with the unique joint indices in the
    triangle.
*/
inline
void
nSkinPartitioner::Partition::GetTriangleJoints(const nMeshBuilder::Triangle& tri, nArray<int>& triJoints)
{
    n_assert(this->sourceMesh);

    int index[3];
    tri.GetVertexIndices(index[0], index[1], index[2]);
    int i;
    for (i = 0; i < 3; i++)
    {
        nMeshBuilder::Vertex& v = this->sourceMesh->GetVertexAt(index[i]);
        const vector4& weights = v.GetWeights();
        const vector4& indices = v.GetJointIndices();
        if (weights.x > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.x));
        }
        if (weights.y > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.y));
        }
        if (weights.z > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.z));
        }
        if (weights.w > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.w));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Fills the provided diffSet int array with all joint indices in the triJoints
    array which are not currently in the joint palette.
*/
inline
void
nSkinPartitioner::Partition::GetJointIndexDifferenceSet(const nArray<int>& triJoints, nArray<int>& diffSet)
{
    int num = triJoints.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (!this->jointMask[triJoints[i]])
        {
            diffSet.Append(triJoints[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSkinPartitioner::Partition::CheckAddTriangle(int triangleIndex)
{
    n_assert(this->sourceMesh);
    const nMeshBuilder::Triangle& tri = this->sourceMesh->GetTriangleAt(triangleIndex);

    // get the unique joints in the triangle
    nArray<int> triJoints;
    this->GetTriangleJoints(tri, triJoints);

    // get the difference set between the triangle joint indices and the indices in the joint palette
    nArray<int> diffSet;
    this->GetJointIndexDifferenceSet(triJoints, diffSet);

    // if the resulting number of joints would be within the max joint palette size,
    // add the triangle to the partition and update the joint palette
    if ((this->jointPalette.Size() + diffSet.Size()) <= this->maxJointPaletteSize)
    {
        this->triangleArray.Append(triangleIndex);
        int i;
        for (i = 0; i < diffSet.Size(); i++)
        {
            this->jointPalette.Append(diffSet[i]);
            this->jointMask[diffSet[i]] = true;
        }
        return true;
    }

    // fallthrough: could not add the triangle to this partition, because there
    // the partition is or would be full
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<int>&
nSkinPartitioner::Partition::GetJointPalette() const
{
    return this->jointPalette;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<int>&
nSkinPartitioner::Partition::GetTriangleIndices() const
{
    return this->triangleArray;
}

//------------------------------------------------------------------------------
/**
    Converts a global joint palette index into a partition-local joint index.
    If the global joint index cannot be found in the local index, a 0 index
    will be returned.
*/
inline
int
nSkinPartitioner::Partition::GlobalToLocalJointIndex(int globalJointIndex) const
{
    int num = this->jointPalette.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (globalJointIndex == this->jointPalette[i])
        {
            return i;
        }
    }
    // fallthrough: globalJointIndex not in partition
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSkinPartitioner::GetNumPartitions() const
{
    return this->partitionArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<int>&
nSkinPartitioner::GetJointPalette(int partitionIndex) const
{
    return this->partitionArray[partitionIndex].GetJointPalette();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<int>&
nSkinPartitioner::GetTriangleIndices(int partitionIndex) const
{
    return this->partitionArray[partitionIndex].GetTriangleIndices();
}

//------------------------------------------------------------------------------
/**
*/
inline
const
nArray<int>&
nSkinPartitioner::GetGroupMappingArray() const
{
    return this->groupMappingArray;
}

//------------------------------------------------------------------------------
#endif
