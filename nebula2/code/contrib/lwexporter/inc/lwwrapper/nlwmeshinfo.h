#ifndef N_LW_MESH_INFO_H
#define N_LW_MESH_INFO_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwmeshes.h>
}

#include "kernel/ntypes.h"

//----------------------------------------------------------------------------
/**
    @class nLWMeshInfo
    @brief Encapsulates Lightwave's LWMeshInfo structure.
*/
class nLWMeshInfo
{
public:
    nLWMeshInfo(LWMeshInfo*);
    ~nLWMeshInfo();
    bool IsValid() const;

    int GetNumPoints();
    int GetNumPolygons();
    int ScanPoints(LWPntScanFunc*, void* userData);
    int ScanPolygons(LWPolScanFunc*, void* userData);
    void GetPointBasePos(LWPntID, LWFVector);
    void GetPointOtherPos(LWPntID, LWFVector);
    void* pntVLookup(LWID vmapType, const char* vmapName);
    int pntVSelect(void*);
    int pntVGet(LWPntID, float* vec);
    LWID GetPolygonType(LWPolID);
    int GetPolygonSize(LWPolID);
    LWPntID GetPolygonVertex(LWPolID, int vertexIndex);
    const char* GetPolygonTag(LWPolID, LWID tagId);
    const char* GetSurfaceName(LWPolID);
    int pntVPGet(LWPntID, LWPolID, float* vec);
    unsigned int GetPolygonFlags(LWPolID);
    int pntVIDGet(LWPntID, float* vec, void* vmap);
    int pntVPIDGet(LWPntID, LWPolID, float* vec, void* vmap);

private:
    LWMeshInfo* meshInfo;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWMeshInfo::nLWMeshInfo(LWMeshInfo* meshInfo) :
meshInfo(meshInfo)
{
    n_assert(meshInfo);
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWMeshInfo::~nLWMeshInfo()
{
    n_assert(this->meshInfo);
    if (this->meshInfo)
    {
        if (this->meshInfo->destroy)
            this->meshInfo->destroy(this->meshInfo);
        this->meshInfo = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWMeshInfo::IsValid() const
{
    return (this->meshInfo != 0);    
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::GetNumPoints()
{
    return this->meshInfo->numPoints(this->meshInfo);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::GetNumPolygons()
{
    return this->meshInfo->numPolygons(this->meshInfo);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::ScanPoints(LWPntScanFunc* callback, void* userData)
{
    return this->meshInfo->scanPoints(this->meshInfo, callback, userData);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::ScanPolygons(LWPolScanFunc* callback, void* userData)
{
    return this->meshInfo->scanPolys(this->meshInfo, callback, userData);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWMeshInfo::GetPointBasePos(LWPntID pntId, LWFVector vec)
{
    this->meshInfo->pntBasePos(this->meshInfo, pntId, vec);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWMeshInfo::GetPointOtherPos(LWPntID pntId, LWFVector vec)
{
    this->meshInfo->pntOtherPos(this->meshInfo, pntId, vec);
}

//----------------------------------------------------------------------------
/**
*/
inline
void* 
nLWMeshInfo::pntVLookup(LWID vmapType, const char* vmapName)
{
    return this->meshInfo->pntVLookup(this->meshInfo, vmapType, vmapName);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::pntVSelect(void* vmap)
{
    return this->meshInfo->pntVSelect(this->meshInfo, vmap);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::pntVGet(LWPntID pntId, float* vec)
{
    return this->meshInfo->pntVGet(this->meshInfo, pntId, vec);
}

//----------------------------------------------------------------------------
/**
*/
inline
LWID 
nLWMeshInfo::GetPolygonType(LWPolID polId)
{
    return this->meshInfo->polType(this->meshInfo, polId);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::GetPolygonSize(LWPolID polId)
{
    return this->meshInfo->polSize(this->meshInfo, polId);
}

//----------------------------------------------------------------------------
/**
*/
inline
LWPntID 
nLWMeshInfo::GetPolygonVertex(LWPolID polId, int vertexIndex)
{
    return this->meshInfo->polVertex(this->meshInfo, polId, vertexIndex);
}

//----------------------------------------------------------------------------
/**
*/
inline
const char* 
nLWMeshInfo::GetPolygonTag(LWPolID polId, LWID tagId)
{
    return this->meshInfo->polTag(this->meshInfo, polId, tagId);
}

//----------------------------------------------------------------------------
/**
*/
inline
const char* 
nLWMeshInfo::GetSurfaceName(LWPolID polId)
{
    return this->GetPolygonTag(polId, LWPTAG_SURF);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::pntVPGet(LWPntID pntId, LWPolID polId, float* vec)
{
    return this->meshInfo->pntVPGet(this->meshInfo, pntId, polId, vec);
}

//----------------------------------------------------------------------------
/**
*/
inline
unsigned int 
nLWMeshInfo::GetPolygonFlags(LWPolID polId)
{
    return this->meshInfo->polFlags(this->meshInfo, polId);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::pntVIDGet(LWPntID pntId, float* vec, void* vmap)
{
    return this->meshInfo->pntVIDGet(this->meshInfo, pntId, vec, vmap);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWMeshInfo::pntVPIDGet(LWPntID pntId, LWPolID polId, float* vec, void* vmap)
{
    return this->meshInfo->pntVPIDGet(this->meshInfo, pntId, polId, vec, vmap);
}

//----------------------------------------------------------------------------
#endif // N_LW_MESH_INFO_H
