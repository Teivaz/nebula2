#ifndef N_OPENDEHINGEJOINT_H
#define N_OPENDEHINGEJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeHingeJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some hinge joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeHingeJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeHingeJoint();
        /// destructor
        virtual ~nOpendeHingeJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetAnchor( const vector3& );
        void SetAxis( const vector3& );
        void GetAnchor( vector3& );
        void GetAnchor2( vector3& );
        void GetAxis( vector3& );
        float GetAngle();
        float GetAngleRate();
    
        virtual void SetParam( int param, float value );
        virtual float GetParam( int param );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEHINGEJOINT_H
