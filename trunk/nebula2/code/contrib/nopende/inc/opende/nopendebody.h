#ifndef N_OPENDEBODY_H
#define N_OPENDEBODY_H
//----------------------------------------------------------------------------
/**
    @class nOpendeBody
    @ingroup NOpenDEContribModule
    @brief Encapsulates dBody functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

class nOpendeWorld;
//----------------------------------------------------------------------------
class nOpendeBody : public nRoot
{
    public:
        /// constructor
        nOpendeBody();
        /// destructor
        virtual ~nOpendeBody();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetPosition( const vector3& );
        void SetRotation( const matrix33& );
        void SetQuaternion( const quaternion& );
        void SetLinearVel( const vector3& );
        void SetAngularVel( const vector3& );
        vector3 GetPosition();
        void GetRotation( matrix33& );
        quaternion GetQuaternion();
        vector3 GetLinearVel();
        vector3 GetAngularVel();
        void SetMass( const dMass* );
        void GetMass( dMass* );
        void AddForce( const vector3& );
        void AddTorque( const vector3& );
        void AddRelForce( const vector3& );
        void AddRelTorque( const vector3& );
        void AddForceAtPos( const vector3& f, const vector3& p );
        void AddForceAtRelPos( const vector3& f, const vector3& p );
        void AddRelForceAtPos( const vector3& f, const vector3& p );
        void AddRelForceAtRelPos( const vector3& f, const vector3& p );
        vector3 GetForce();
        vector3 GetTorque();
        void SetForce( const vector3& );
        void SetTorque( const vector3& );
        void Enable();
        void Disable();
        bool IsEnabled();
        void SetFiniteRotationMode( int );
        int GetFiniteRotationMode() ;
        void SetFiniteRotationAxis( const vector3& );
        void GetFiniteRotationAxis( vector3& );
        int GetNumJoints();
        dJointID GetJoint( int );
        void SetGravityMode( bool );
        bool GetGravityMode();
        void GetRelPointPos( const vector3& p, vector3& result );
        void GetRelPointVel( const vector3& p, vector3& result );
        void GetPointVel( const vector3& p, vector3& result );
        void GetPosRelPoint( const vector3& p, vector3& result );
        void VectorToWorld( const vector3& p, vector3& result );
        void VectorFromWorld( const vector3& p, vector3& result );
        bool IsConnectedTo( dBodyID body2 );
        bool IsConnectedToExcluding( dBodyID body2, int joint_type );
        // mass functions
        void ResetMass();
        void SetMassParams( float theMass, const vector3& cg,
                            float I11, float I22, float I33,
                            float I12, float I13, float I23 );
        void SetSphereMass( float density, float radius );
        void SetSphereMassTotal( float totalMass, float radius );
        void SetCapsuleMass( float density, int direction,
                             float radius, float length );
        void SetCapsuleMassTotal( float totalMass, int direction,
                                  float radius, float length );
        void SetCylinderMass( float density, int direction,
                              float radius, float length );
        void SetCylinderMassTotal( float totalMass, int direction,
                                   float radius, float length );
        void SetBoxMass( float density, float lx, float ly, float lz );
        void SetBoxMassTotal( float totalMass, float lx, float ly, float lz );
        void AdjustMass( float newMass );
        void TranslateMass( const vector3& );
        void RotateMass( const matrix33& );
        void AddMassOf( const char* otherBody );
    
        /// the actual ODE object (do NOT destroy!)
        dBodyID id;
        
    private:
        nDynAutoRef<nOpendeWorld> ref_World;
};

//----------------------------------------------------------------------------
#endif // N_OPENDEBODY_H
