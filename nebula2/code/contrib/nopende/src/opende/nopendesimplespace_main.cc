//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendesimplespace.h"

nNebulaClass(nOpendeSimpleSpace, "nopendespace");

//----------------------------------------------------------------------------
/**
*/
nOpendeSimpleSpace::nOpendeSimpleSpace()
{
    // empty
}

//----------------------------------------------------------------------------
/**
    @param parent The NOH path of the parent space, use "none" to indicate 
                  this space doesn't have a parent.
*/
void nOpendeSimpleSpace::Create( const char* parent )
{
    nOpendeGeom::Create( parent );
    this->geomId = (dGeomID)nOpende::SimpleSpaceCreate( this->getSpace() );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeSimpleSpace::~nOpendeSimpleSpace()
{
    // empty
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
