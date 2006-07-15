//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendehashspace.h"

nNebulaScriptClass(nOpendeHashSpace, "nopendespace");

//----------------------------------------------------------------------------
/**
*/
nOpendeHashSpace::nOpendeHashSpace()
{
    // empty
}

//----------------------------------------------------------------------------
/**
    @param parent The NOH path of the parent space, use "none" to indicate 
                  this space doesn't have a parent.
*/
void nOpendeHashSpace::Create( const char* parent )
{
    nOpendeGeom::Create( parent );
    this->geomId = (dGeomID)nOpende::HashSpaceCreate( this->getSpace() );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeHashSpace::~nOpendeHashSpace()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Set the smallest and largest cell sizes to be used in the
           hash space.
*/
void nOpendeHashSpace::SetLevels( int minLevel, int maxLevel )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::HashSpaceSetLevels( this->id(), minLevel, maxLevel );
}

//------------------------------------------------------------------------------
/**
    @brief Get the smallest and largest cell sizes to be used in the
           hash space.
*/
void nOpendeHashSpace::GetLevels( int* minLevel, int* maxLevel )
{
    n_assert( this->id() && "nOpendeSpace::id not valid!" );
    nOpende::HashSpaceGetLevels( this->id(), minLevel, maxLevel );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
