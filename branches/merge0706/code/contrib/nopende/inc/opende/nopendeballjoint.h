#ifndef N_OPENDEBALLJOINT_H
#define N_OPENDEBALLJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeBallJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some ball joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeBallJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeBallJoint();
        /// destructor
        virtual ~nOpendeBallJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void SetAnchor( const vector3& );
        void GetAnchor( vector3& );
        void GetAnchor2( vector3& );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEBALLJOINT_H
