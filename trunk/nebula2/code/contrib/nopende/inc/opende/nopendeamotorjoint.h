#ifndef N_OPENDEAMOTORJOINT_H
#define N_OPENDEAMOTORJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeAMotorJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some AMotor joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeAMotorJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeAMotorJoint();
        /// destructor
        virtual ~nOpendeAMotorJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetMode( int );
        void SetMode( const char* );
        int GetMode();
        const char* GetModeName();
        void SetNumAxes( int );
        int GetNumAxes();
        void SetAxis( int anum, int rel, const vector3& axis );
        void GetAxis( int anum, vector3& result );
        int GetAxisRel( int anum );
        void SetAngle( int anum, float angle );
        float GetAngle( int anum );
        float GetAngleRate( int anum );
    
        virtual void SetParam( int param, float value );
        virtual float GetParam( int param );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEAMOTORJOINT_H
