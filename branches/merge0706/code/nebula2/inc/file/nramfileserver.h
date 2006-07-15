#ifndef N_RAMFILESERVER_H
#define N_RAMFILESERVER_H
//------------------------------------------------------------------------------
/**
    @class nRamFileServer
    @ingroup RAMFile

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nRamFileServer : public nFileServer2
{
public:
    /// constructor
    nRamFileServer();
    /// destructor
    virtual ~nRamFileServer();

    /// create new nRamFile object
    virtual nFile* NewFileObject() const;

private:
    // Data
};

//------------------------------------------------------------------------------
#endif
