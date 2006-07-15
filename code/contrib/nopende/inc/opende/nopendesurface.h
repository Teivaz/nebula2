#ifndef N_OPENDESURFACE_H
#define N_OPENDESURFACE_H
//------------------------------------------------------------------------------
/**
    @class nOpendeSurface
    @ingroup NOpenDEContribModule
    @brief Encapsulates a dSurfaceParameters structure.

    (C) 2004  Vadim Macagon
    
    nOpendeSurface is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------

#include "kernel/ntypes.h"
#include "opende/nopendelayer.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nOpendeSurface
{
    public:
        nOpendeSurface();
        ~nOpendeSurface();
  
        void SetName( const char* );
        const char* GetName();
  
        void SetMode( int flags );
        int GetMode();
        void ToggleFlag( int flag );
        void EnableFlag( int flag );
    
        void SetMU( float mu );
        float GetMU();
    
        void SetMU2( float mu2 );
        float GetMU2();
    
        void SetBounce( float value );
        float GetBounce();
  
        void SetBounceVel( float value );
        float GetBounceVel();
    
        void SetSoftERP( float value );
        float GetSoftERP();
    
        void SetSoftCFM( float value );
        float GetSoftCFM();
    
        void SetMotion1( float value );
        float GetMotion1();
        void SetMotion2( float value );
        float GetMotion2();
    
        void SetSlip1( float value );
        float GetSlip1();
        void SetSlip2( float value );
        float GetSlip2();
    
    private:
        dSurfaceParameters surface;
        nString name;
};

//------------------------------------------------------------------------------
inline
nOpendeSurface::nOpendeSurface() : name( "" )
{
    this->surface.mode = 0;
}

//------------------------------------------------------------------------------
inline
nOpendeSurface::~nOpendeSurface()
{
    // empty
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetName( const char* n )
{
    this->name = n;
}

//------------------------------------------------------------------------------
inline    
const char* nOpendeSurface::GetName()
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetMode( int flags )
{
    this->surface.mode = flags;
}

//------------------------------------------------------------------------------
inline
int nOpendeSurface::GetMode()
{
    return this->surface.mode;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::ToggleFlag( int flag )
{
    if ( this->surface.mode & flag )  // if on, turn it off
        this->surface.mode &= ~flag;
    else // turn it on
        this->surface.mode |= flag;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::EnableFlag( int flag )
{
    this->surface.mode |= flag;
}

//------------------------------------------------------------------------------
/**
  @brief 
  
  A value less than zero is interpreted as infinity.
*/
inline
void nOpendeSurface::SetMU( float mu )
{
    if ( mu < 0.0f )
        this->surface.mu = dInfinity;
    else
        this->surface.mu = mu;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetMU()
{
    return this->surface.mu;
}

//------------------------------------------------------------------------------
/**
  @brief 
  
  A value less than zero is interpreted as infinity.
*/
inline
void nOpendeSurface::SetMU2( float mu2 )
{
    if ( mu2 < 0.0f )
        this->surface.mu2 = dInfinity;
    else
        this->surface.mu2 = mu2;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetMU2()
{
    return this->surface.mu2;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetBounce( float value )
{
    this->surface.bounce = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetBounce()
{
    return this->surface.bounce;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetBounceVel( float value )
{
    this->surface.bounce_vel = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetBounceVel()
{
    return this->surface.bounce_vel;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetSoftERP( float value )
{
    this->surface.soft_erp = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetSoftERP()
{
    return this->surface.soft_erp;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetSoftCFM( float value )
{
    this->surface.soft_cfm = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetSoftCFM()
{
    return this->surface.soft_cfm;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetMotion1( float value )
{
    this->surface.motion1 = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetMotion1()
{
    return this->surface.motion1;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetMotion2( float value )
{
    this->surface.motion2 = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetMotion2()
{
    return this->surface.motion2;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetSlip1( float value )
{
    this->surface.slip1 = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetSlip1()
{
    return this->surface.slip1;
}

//------------------------------------------------------------------------------
inline
void nOpendeSurface::SetSlip2( float value )
{
    this->surface.slip2 = value;
}

//------------------------------------------------------------------------------
inline
float nOpendeSurface::GetSlip2()
{
    return this->surface.slip2;
}

//------------------------------------------------------------------------------
#endif // N_OPENDESURFACE_H
