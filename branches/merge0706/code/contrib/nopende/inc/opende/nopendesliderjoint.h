#ifndef N_OPENDESLIDERJOINT_H
#define N_OPENDESLIDERJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeSliderJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some slider joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeSliderJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeSliderJoint();
        /// destructor
        virtual ~nOpendeSliderJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetAxis( const vector3& );
        void GetAxis( vector3& );
        float GetPosition();
        float GetPositionRate();
            
        virtual void SetParam( int param, float value );
        virtual float GetParam( int param );
};

//----------------------------------------------------------------------------
#endif // N_OPENDESLIDERJOINT_H
