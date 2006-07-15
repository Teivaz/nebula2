//------------------------------------------------------------------------------
//  nocclusionquery.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nocclusionquery.h"

//------------------------------------------------------------------------------
/**
*/
nOcclusionQuery::nOcclusionQuery()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nOcclusionQuery::~nOcclusionQuery()
{
    // query
}

//------------------------------------------------------------------------------
/**
*/
void
nOcclusionQuery::Clear()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nOcclusionQuery::Begin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nOcclusionQuery::AddShapeQuery(nGfxServer2::ShapeType /*type*/, const matrix44& /*modelMatrix*/, const void* /*userData*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nOcclusionQuery::End()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nOcclusionQuery::GetNumQueries() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
const void*
nOcclusionQuery::GetUserData(int /*queryIndex*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nOcclusionQuery::GetOcclusionStatus(int /*queryIndex*/)
{
    return false;
}
