#ifndef N_OPENDEUNIVERSALJOINT_H
#define N_OPENDEUNIVERSALJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeUniversalJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some universal joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeUniversalJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeUniversalJoint();
        /// destructor
        virtual ~nOpendeUniversalJoint();
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
            
        virtual void SetParam( int param, float value );
        virtual float GetParam( int param );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEUNIVERSALJOINT_H
