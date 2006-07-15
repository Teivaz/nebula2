#ifndef N_OPENDEHINGE2JOINT_H
#define N_OPENDEHINGE2JOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeHinge2Joint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some hinge2 joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeHinge2Joint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeHinge2Joint();
        /// destructor
        virtual ~nOpendeHinge2Joint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetAnchor( const vector3& );
        void SetAxis1( const vector3& );
        void SetAxis2( const vector3& );
        void GetAnchor( vector3& );
        void GetAnchor2( vector3& );
        void GetAxis1( vector3& );
        void GetAxis2( vector3& );
        float GetAngle1();
        float GetAngle1Rate();
        float GetAngle2Rate();
    
        virtual void SetParam( int param, float value );
        virtual float GetParam( int param );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEHINGE2JOINT_H
