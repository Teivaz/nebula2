#ifndef E_UIDSERVER_H
#define E_UIDSERVER_H
//------------------------------------------------------------------------------
/**
    @class eUIDServer

    @brief A server designed to doll out unique resource IDs

    The eUIDserver just hands out unique IDs from an internal pool.  Any deleted IDs are returned
    to the pool, and all new uIDs recieved from a single server are guaranteed to be unique. Of
    course, uIDs from two separately-created servers could be identical, but that should be obvious.

    Be careful if you request more than 2^31 unique IDs - the server is using signed integers internally
    so as to play nice with itoa() conversions being used as script object names, and the wrap-around
    you'll get past 2^31 may or may not cause problems in your own case.  Probably won't be an issue in
    the vast majority of cases, and you could change it to use unsigned's if you cared to.

    (C)	2003	Megan Fox
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class eUIDServer : public nRoot
{
public:
    /// constructor
    eUIDServer();
    /// destructor
    virtual ~eUIDServer();
    
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Get a new unique ID
    int GetNewID();
    /// Release an unused unique ID
    void ReleaseID(int OldID);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

    int nextUniqueID;
    nList releasedUIDList;
};
//------------------------------------------------------------------------------
#endif

