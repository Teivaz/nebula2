#ifndef N_OPENDEFIXEDJOINT_H
#define N_OPENDEFIXEDJOINT_H
//----------------------------------------------------------------------------
/**
    @class nOpendeFixedJoint
    @ingroup NOpenDEContribModule
    @brief Encapsulates some fixed joint related dJoint functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeFixedJoint : public nOpendeJoint
{
    public:
        /// constructor
        nOpendeFixedJoint();
        /// destructor
        virtual ~nOpendeFixedJoint();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
    
        /// create the corresponding ODE object
        virtual void Create( const char* world );
                
        /*
            Convenience methods.
        */
        
        void Fix();
};

//----------------------------------------------------------------------------
#endif // N_OPENDEFIXEDJOINT_H
