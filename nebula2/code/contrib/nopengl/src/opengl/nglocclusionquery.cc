//------------------------------------------------------------------------------
//  nglocclusionquery.cc
//  Cloned from nd3d9occlusionquery.cc
//  (C) 2006 Oleg Khryptul (Haron)
//------------------------------------------------------------------------------
#include "opengl/nglocclusionquery.h"
#include "opengl/nglextensionserver.h"

#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
/**
*/
nGLOcclusionQuery::nGLOcclusionQuery() :
    queryArray(256, 256),
    inBegin(false)
{
    this->queryArray.SetFlags(nArray<GLuint>::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
nGLOcclusionQuery::~nGLOcclusionQuery()
{
    this->Clear();

    // release shader
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    This clears all queries regardless of whether they'd been finished or not.
*/
void
nGLOcclusionQuery::Clear()
{
    int i;
    int num = this->queryArray.Size();

    for (i = 0; i < num; i++)
    {
        Query& query = this->queryArray[i];
        if (GL_TRUE == glIsQueryARB(query.queryID))
        {
            glDeleteQueriesARB(1, &query.queryID);
            n_gltrace("nGLOcclusionQuery::Clear(): glDeleteQueriesARB failed!");
        }
        query.queryID = 0;
        query.userData = 0;
    }
    this->queryArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Begin occlusion queries. This initializes a the shader and calls
    BeginShapes() on the gfx server.
*/
void
nGLOcclusionQuery::Begin()
{
    n_assert(!this->inBegin);
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // clear current mesh (LEAVE THIS IN! IT'S IMPORTANT SO THAT
    // Nebula2's REDUDANCY DETECTOR WON'T BE FOOLED!)
    gfxServer->SetMesh(0, 0);

    // initialize shader
    if (!this->refShader.isvalid())
    {
        nShader2* shd = gfxServer->NewShader("shaders:occlusionquery.fx");
        shd->SetFilename("shaders:occlusionquery.fx");
        bool occlusionShaderLoaded = shd->Load();
        n_assert(occlusionShaderLoaded);
        this->refShader = shd;
    }

    // clear any existing queries
    this->Clear();

    // activate shader
    gfxServer->SetShader(this->refShader.get());
    int numPasses = this->refShader->Begin(true);
    n_assert(1 == numPasses);
    this->refShader->BeginPass(0);

    this->inBegin = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLOcclusionQuery::AddShapeQuery(nGfxServer2::ShapeType type, const matrix44& modelMatrix, const void* userData)
{
    n_assert(this->inBegin);

    GLuint queryID;
    glGenQueriesARB(1, &queryID);

    if (queryID > 0) // TODO: check this
    {
        // store query so we can check its status later
        Query newQuery;
        newQuery.queryID  = queryID;
        newQuery.userData = userData;
        this->queryArray.Append(newQuery);

        // start the query
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queryID);
        n_gltrace("nGLOcclusionQuery::AddShapeQuery(): glBeginQueryARB failed!");

        // render the shape to check for
        nGfxServer2::Instance()->DrawShapeNS(type, modelMatrix);

        // tell the query that we're done, note that this is an asynchronous
        // query, so we'll get the result later in GetOcclusionStatus()
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        n_gltrace("nGLOcclusionQuery::AddShapeQuery(): glEndQueryARB failed!");
    }
    else
    {
        // hmm, maybe no occlusion queries on this device...
        // just store a null pointer, we'll just simulate a failed query later on
        Query newQuery;
        newQuery.queryID = 0;
        newQuery.userData = userData;
        this->queryArray.Append(newQuery);

        n_message("nGLOcclusionQuery::AddShapeQuery(): CreateQuery failed!");
    }
    //else
    //{
    //    // an error...
    //    n_error("nGLOcclusionQuery::AddShapeQuery(): CreateQuery failed!");
    //}
}

//------------------------------------------------------------------------------
/**
*/
void
nGLOcclusionQuery::End()
{
    n_assert(this->inBegin);
    this->refShader->EndPass();
    this->refShader->End();
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
int
nGLOcclusionQuery::GetNumQueries() const
{
    n_assert(!this->inBegin);
    return this->queryArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get user data associated with a query.
*/
const void*
nGLOcclusionQuery::GetUserData(int queryIndex)
{
    return this->queryArray[queryIndex].userData;
}

//------------------------------------------------------------------------------
/**
    Returns the occlusion status of a query defined by index. This method
    may wait for the query to finish, so it's wise to queue as many
    queries as possible before checking their status.
*/
bool
nGLOcclusionQuery::GetOcclusionStatus(int queryIndex)
{
    n_assert(!this->inBegin);
    Query& query = this->queryArray[queryIndex];
    GLuint queryID = query.queryID;

    if (0 == queryID)
    {
        // special case: no occlusion query available on this device,
        // always return that we're not occluded
        return false;
    }
    // an occlusion query returns the number of pixels which have passed
    // the z test in a GLuint
    GLuint numVisiblePixels = 0;

    do
    {
        glGetQueryObjectuivARB(queryID, GL_QUERY_RESULT_AVAILABLE_ARB, &numVisiblePixels);
        n_gltrace("nGLOcclusionQuery::GetOcclusionStatus(): glGetQueryObjectuivARB(GL_QUERY_RESULT_AVAILABLE_ARB) failed!");
    }
    while (numVisiblePixels == GL_FALSE);

    glGetQueryObjectuivARB(queryID, GL_QUERY_RESULT_ARB, &numVisiblePixels);
    n_gltrace("nGLOcclusionQuery::GetOcclusionStatus(): glGetQueryObjectuivARB(GL_QUERY_RESULT_ARB) failed!");

    // return true if we're fully occluded
    return (numVisiblePixels == 0);
}
