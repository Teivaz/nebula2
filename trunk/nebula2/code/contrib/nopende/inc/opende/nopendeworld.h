#ifndef N_OPENDEWORLD_H
#define N_OPENDEWORLD_H
//----------------------------------------------------------------------------
/**
    @class nOpendeWorld
    @ingroup NOpenDEContribModule
    @brief Encapsulates dWorld functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "opende/nopendelayer.h"

//----------------------------------------------------------------------------
class nOpendeWorld : public nRoot
{
    public:
        /// constructor
        nOpendeWorld();
        /// destructor
        virtual ~nOpendeWorld();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
                
        /*
            Convenience methods.
        */
        
        void SetGravity( const vector3& );
        void GetGravity( vector3& );
        
        void SetERP( float );
        float GetERP();
        
        void SetCFM( float );
        float GetCFM();
        
        void Step( float );
        
        void ImpulseToForce( float, const vector3&, vector3& );
        
        void StepFast1( float, int );
        
        
        /// pointer to nKernelServer
        static nKernelServer* kernelServer;
    
        /// the actual ODE object (do NOT destroy!)
        dWorldID id;
};

//----------------------------------------------------------------------------
#endif // N_OPENDEWORLD_H
