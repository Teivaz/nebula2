#ifndef N_OPENDESIMPLESPACE_H
#define N_OPENDESIMPLESPACE_H
//----------------------------------------------------------------------------
/**
    @class nOpendeSimpleSpace
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dSpace functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendespace.h"

//----------------------------------------------------------------------------
class nOpendeSimpleSpace : public nOpendeSpace
{
    public:
        /// constructor
        nOpendeSimpleSpace();
        /// destructor
        virtual ~nOpendeSimpleSpace();
        /// create the underlying ODE object
        virtual void Create( const char* space = "none" );
};

//----------------------------------------------------------------------------
#endif // N_OPENDESIMPLESPACE_H
