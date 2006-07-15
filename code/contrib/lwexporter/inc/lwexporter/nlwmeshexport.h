#ifndef N_LW_MESH_EXPORT_H
#define N_LW_MESH_EXPORT_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h> // for XCALL_
#include <lwsdk/lwsurf.h>
#include <lwsdk/lwmeshes.h> // for LWPolID
}

#include "util/nstring.h"
#include "tools/nmeshbuilder.h"
#include "util/narray.h"
#include "mathlib/bbox.h"
#include "lwexporter/nlwbonejointmap.h"

class nLWObjectExportSettings;
class nLWShaderExportSettings;
class nLWLayoutMonitor;
class nLWMeshInfo;
class nHashMap;

//----------------------------------------------------------------------------
/**
    @class nLWMeshExport
    @brief Handles exporting of mesh data from Lightwave objects.

    Handles exporting of static meshes, and weighted meshes for bone-animated 
    characters.
*/
class nLWMeshExport
{
public:
    nLWMeshExport(const nString& meshName, nLWObjectExportSettings*, 
                  nLWLayoutMonitor*);
    ~nLWMeshExport();

    bool Run();

    int GetNumGroups() const;
    bbox3 GetGroupBBox(int groupIdx) const;
    nLWShaderExportSettings* GetShaderForGroup(int groupIdx) const;
    const nString& GetMeshFileName() const;
    bbox3 GetBBox() const;

    // fragment methods can only be used for partitioned (skinned) meshes
    const nArray<int>& GetGroupFragments(int groupIdx);
    const nArray<int>& GetFragmentJoints(int fragIdx);

private:
    struct MeshPolygon
    {
        LWPolID id;
        ushort surfaceKey; // key into surfaceHashMap
    };

    struct MeshSurface
    {
        LWSurfaceID id;
        nString name; // name of the surface
        bool used; // do any polygons belong to this surface?
    };

    XCALL_(static int) PolygonScan(void* boxedObject, LWPolID polId);

    bool NeedUV() const;
    bool NeedNormals() const;
    bool NeedJointWeights() const;

    void ExtractAllMeshSurfaces(nArray<MeshSurface>&);
    void ExtractMeshTriangles();
    void CollectUsedSurfaces(nArray<MeshSurface>&);
    void CollectTriangles();
    void GroupTrianglesBySurface(nArray<MeshSurface>&);
    void CollectUV();
    void CollectWeightMaps(LWItemID boneId, nArray<nString>&, LWItemInfo*, LWBoneInfo*);
    void MapWeightMapsToJointIndices(LWItemID boneId, const nArray<nString>&, 
                                     nKeyArray<int>&, const nLWBoneJointMap&,
                                     LWItemInfo* itemInfo, LWBoneInfo* boneInfo);
    bool MapWeightMapsToPointers(const nArray<nString>&, nKeyArray<void*>&);
    void CollectJointWeights();
    void ExtractGeometry();
    void ConstructMeshFileName();
    void PartitionMesh();
    bool Save();

    nString meshName;
    nLWObjectExportSettings* objSettings;
    nLWLayoutMonitor* monitor;
    nMeshBuilder meshBuilder;
    nLWMeshInfo* curMeshInfo;
    nHashMap* surfaceHashMap;
    nArray<MeshPolygon> meshPolygons;
    bool polyScanFoundNonTriPoly;
    int numMeshGroups;
    nArray<LWSurfaceID> usedSurfaceArray;
    nString meshFileName;
    
    nMeshBuilder* skinMeshBuilder;
    // maps pre-partitioning mesh groups to fragment arrays
    nArray<nArray<int>*> groupFragmentsArray;
    // maps fragments to joint indices
    nArray<nArray<int>*> fragmentJointsArray;
};

//----------------------------------------------------------------------------
/**
    @brief Get the number of pre-partitioning mesh groups.
*/
inline
int 
nLWMeshExport::GetNumGroups() const
{
    return this->numMeshGroups;
}

//----------------------------------------------------------------------------
/**
    @brief Get the bounding box for the given pre-partitioning mesh group.
    @param groupIdx Index of a pre-partitioning mesh-group.
*/
inline
bbox3 
nLWMeshExport::GetGroupBBox(int groupIdx) const
{
    return this->meshBuilder.GetGroupBBox(groupIdx);
}

//----------------------------------------------------------------------------
/**
    @brief Get the full file name under which the mesh was saved.
*/
inline
const nString& 
nLWMeshExport::GetMeshFileName() const
{
    return this->meshFileName;
}

//----------------------------------------------------------------------------
/**
    @brief Get the bounding box for the entire mesh.
*/
inline
bbox3 
nLWMeshExport::GetBBox() const
{
    return this->meshBuilder.GetBBox();
}

//----------------------------------------------------------------------------
/**
    @brief Get a list of fragment indices that correspond to the given 
           pre-partitioning mesh group index.

    When the mesh is partitioned (this happens when the mesh has joint weights)
    the pre-partitioning mesh groups are split into fragments (which is just 
    another name for post-partitioning mesh groups). Each pre-partitiong mesh 
    group is split into one or more fragments.
*/
inline
const nArray<int>& 
nLWMeshExport::GetGroupFragments(int groupIdx)
{
    return *(this->groupFragmentsArray[groupIdx]);
}

//----------------------------------------------------------------------------
/**
    @brief Get the joint palette for the given fragment index.

    After the mesh is partitioned into fragments, each fragment will have a 
    joint palette.
*/
inline
const nArray<int>& 
nLWMeshExport::GetFragmentJoints(int fragIdx)
{
    return *(this->fragmentJointsArray[fragIdx]);
}

//----------------------------------------------------------------------------
#endif // N_LW_MESH_EXPORT_H
