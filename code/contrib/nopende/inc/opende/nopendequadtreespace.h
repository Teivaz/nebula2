#ifndef N_OPENDEQUADTREESPACE_H
#define N_OPENDEQUADTREESPACE_H
//----------------------------------------------------------------------------
/**
    @class nOpendeQuadTreeSpace
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dSpace functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendespace.h"

//----------------------------------------------------------------------------
class nOpendeQuadTreeSpace : public nOpendeSpace
{
    public:
        /// constructor
        nOpendeQuadTreeSpace();
        /// create the underlying ODE object
        void Create( const char* parent, const vector3& center, 
                     const vector3& extents, int depth );
        /// create the underlying ODE object
        void Create( const char* space = "none" );
        /// destructor
        virtual ~nOpendeQuadTreeSpace();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEQUADTREESPACE_H
