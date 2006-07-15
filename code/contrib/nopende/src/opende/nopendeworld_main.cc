//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendeworld.h"
#include "kernel/nkernelserver.h"

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
/**
+*/      
void nOpendeWorld::QuickStep( float stepsize )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldQuickStep( this->id, stepsize );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetQuickStepNumIterations( int num )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetQuickStepNumIterations( this->id, num );
}

//----------------------------------------------------------------------------
/**
*/      
int nOpendeWorld::GetQuickStepNumIterations()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetQuickStepNumIterations( this->id );
}
        
//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetAutoDisableFlag( int do_auto_disable )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetAutoDisableFlag( this->id, do_auto_disable );
}

//----------------------------------------------------------------------------
/**
*/      
int nOpendeWorld::GetAutoDisableFlag()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetAutoDisableFlag( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetAutoDisableLinearThreshold( float threshold )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetAutoDisableLinearThreshold( this->id, threshold );
}

//----------------------------------------------------------------------------
/**
*/      
float nOpendeWorld::GetAutoDisableLinearThreshold()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetAutoDisableLinearThreshold( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetAutoDisableAngularThreshold( float threshold )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetAutoDisableAngularThreshold( this->id, threshold );
}

//----------------------------------------------------------------------------
/**
*/      
float nOpendeWorld::GetAutoDisableAngularThreshold()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetAutoDisableAngularThreshold( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetAutoDisableSteps( int steps )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetAutoDisableSteps( this->id, steps );
}

//----------------------------------------------------------------------------
/**
*/      
int nOpendeWorld::GetAutoDisableSteps()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetAutoDisableSteps( this->id );
}

//----------------------------------------------------------------------------
/**
*/      
void nOpendeWorld::SetAutoDisableTime( float time )
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    nOpende::WorldSetAutoDisableTime( this->id, time );
}

//----------------------------------------------------------------------------
/**
*/      
float nOpendeWorld::GetAutoDisableTime()
{
    n_assert( this->id && "nOpendeWorld::id not valid!" );
    return nOpende::WorldGetAutoDisableTime( this->id );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
