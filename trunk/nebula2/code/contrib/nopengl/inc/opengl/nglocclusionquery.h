#ifndef N_GLOCCLUSIONQUERY_H
#define N_GLOCCLUSIONQUERY_H
//------------------------------------------------------------------------------
/**
    @class nGLOcclusionQuery
    @ingroup OpenGL

    OpenGL version of nOcclusionQuery.

    (C) 2006 Oleg Khryptul (Haron)
*/
#include "gfx2/nocclusionquery.h"
#include "util/narray.h"
#include "kernel/nref.h"
#include "opengl/nglserver2.h"

class nShader2;

//------------------------------------------------------------------------------
class nGLOcclusionQuery : public nOcclusionQuery
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

private:
    //friend class nD3D9Server;
    friend class nGLServer2;

    /// constructor
    nGLOcclusionQuery();
    /// destructor
    virtual ~nGLOcclusionQuery();

    struct Query
    {
        uint        queryID;
        const void* userData;
    };

    nArray<Query>  queryArray;
    nRef<nShader2> refShader;
    bool inBegin;
};
//------------------------------------------------------------------------------
#endif