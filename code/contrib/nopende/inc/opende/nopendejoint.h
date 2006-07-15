#ifndef N_OPENDEJOINT_H
#define N_OPENDEJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

class nOpendeWorld;
class nOpendeBody;
//----------------------------------------------------------------------------
class nOpendeJoint : public nRoot
{
    public:
        /// constructor
        nOpendeJoint();
        /// destructor
        virtual ~nOpendeJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void AttachTo( dBodyID body1, dBodyID body2 );
        void AttachTo( const char* body1, const char* body2 );
        void SetData( void* data );
        void* GetData();
        int GetJointType();
        const char* GetJointTypeName();
        dBodyID GetBody( int );
        const char* GetBodyName( int );
        // the raw feedback interface
        void SetFeedback( dJointFeedback* );
        dJointFeedback* GetFeedback();
        bool IsFeedbackEnabled();
        void GetFeedback( vector3& force1, vector3& torque1,
                          vector3& force2, vector3& torque2 );
        /// set a joint parameter
        virtual void SetParam( int param, float value );
        /// get a joint parameter
        virtual float GetParam( int param );
        /// set a joint parameter
        void SetParam2( const char* param, int axis, float value );
        /// get a joint parameter
        float GetParam2( const char* param, int axis );
        
        /// the actual ODE object (do NOT destroy!)
        dJointID id;
        
    protected:
        nDynAutoRef<nOpendeWorld> ref_World;
    
        static const int NUM_JOINT_PARAMS;
        static const char* JOINT_PARAM_NAMES[];
        
    private:
        nString firstBodyName;
        nString secondBodyName;
};

//----------------------------------------------------------------------------
#endif // N_OPENDEJOINT_H
