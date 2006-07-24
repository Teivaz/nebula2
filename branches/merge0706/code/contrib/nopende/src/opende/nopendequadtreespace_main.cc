//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendequadtreespace.h"

nNebulaScriptClass(nOpendeQuadTreeSpace, "nopendespace");

//----------------------------------------------------------------------------
/**
*/
nOpendeQuadTreeSpace::nOpendeQuadTreeSpace()
{
    // empty
}

//----------------------------------------------------------------------------
/**
    @param parent The NOH path of the parent space, use "none" to indicate
                  this space doesn't have a parent.
*/
void nOpendeQuadTreeSpace::Create( const char* parent, const vector3& center,
                                   const vector3& extents, int depth )
{
    nOpendeGeom::Create( parent );
    this->geomId = (dGeomID)nOpende::QuadTreeSpaceCreate( this->getSpace(),
                                                          center, extents,
                                                          depth );
}

//----------------------------------------------------------------------------
/**
    @brief This method is not implemented for the quad tree space, use
           the other Create().
*/
void nOpendeQuadTreeSpace::Create( const char* space )
{
    n_error( "Not implemented!" );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeQuadTreeSpace::~nOpendeQuadTreeSpace()
{
    // empty
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
