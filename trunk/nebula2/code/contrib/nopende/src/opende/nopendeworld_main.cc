//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeworld.h"

nNebulaScriptClass(nOpendeWorld, "nroot");

//----------------------------------------------------------------------------
/**
*/
nOpendeWorld::nOpendeWorld()
{
    this->id = nOpende::WorldCreate();
}

//----------------------------------------------------------------------------
/**
*/
nOpendeWorld::~nOpendeWorld()
{
    if ( this->id )
    {
        nOpende::WorldDestroy( this->id );
        this->id = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeWorld::SetGravity( const vector3& g )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetGravity( this->id, g );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeWorld::GetGravity( vector3& g )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldGetGravity( this->id, g );
}
        
//----------------------------------------------------------------------------
/**
*/
void nOpendeWorld::SetERP( float erp )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetERP( this->id, erp );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeWorld::GetERP()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetERP( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetCFM( float cfm )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetCFM( this->id, cfm );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeWorld::GetCFM()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetCFM( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::Step( float stepsize )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldStep( this->id, stepsize );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::ImpulseToForce( float stepsize, const vector3& impulse, 
                                   vector3& force )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldImpulseToForce( this->id, stepsize, impulse, force );
}
        
//----------------------------------------------------------------------------
/**
*/
void nOpendeWorld::StepFast1( float stepsize, int maxiterations )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldStepFast1( this->id, stepsize, maxiterations );
}
        
//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
