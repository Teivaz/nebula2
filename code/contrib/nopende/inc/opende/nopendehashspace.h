#ifndef N_OPENDEHASHSPACE_H
#define N_OPENDEHASHSPACE_H
//----------------------------------------------------------------------------
/**
    @class nOpendeHashSpace
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dSpace functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendespace.h"

//----------------------------------------------------------------------------
class nOpendeHashSpace : public nOpendeSpace
{
    public:
        /// constructor
        nOpendeHashSpace();
        /// destructor
        virtual ~nOpendeHashSpace();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        /// create the underlying ODE object
        virtual void Create( const char* space = "none" );
        
        /*
            Convenience methods.
        */
            
        /// set the smallest and largest cell sizes 
        void SetLevels( int minLevel, int maxLevel );
        /// get the smallest and largest cell sizes 
        void GetLevels( int* minLevel, int* maxLevel );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEHASHSPACE_H
