#ifndef N_OCCLUSIONQUERY_H
#define N_OCCLUSIONQUERY_H
//------------------------------------------------------------------------------
/**
    @class nOcclusionQuery
    @ingroup Gfx2

    Runs a sequence of occlusion queries. Will render bounding boxes
    and check how many pixels pass the z test. If none
    of the pixels pass the test, the rectangle is occluded. Try to issue
    as many queries as possible before getting the query results, this 
    maximizes GPU/CPU parallelization.

    This class should be subclassed by an graphics API-specific implementation.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nrefcounted.h"
#include "mathlib/bbox.h"
#include "mathlib/matrix.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nOcclusionQuery : public nRefCounted
{
public:
    /// begin issuing occlusion queries
    virtual void Begin();
    /// add a bounding box query
    virtual void AddShapeQuery(nGfxServer2::ShapeType type, const matrix44& modelMatrix, const void* userData);
    /// finish issuing queries
    virtual void End();
    /// get number of issued queries
    virtual int GetNumQueries() const;
    /// get user data associated with a query
    virtual const void* GetUserData(int queryIndex);
    /// get occlusion status for a issued query (true is occluded)
    virtual bool GetOcclusionStatus(int queryIndex);
    /// clear all queries
    virtual void Clear();

protected:
    /// constructor
    nOcclusionQuery();
    /// destructor
    virtual ~nOcclusionQuery();
};
//------------------------------------------------------------------------------
#endif
