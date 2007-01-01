#ifndef N_HARDREFSERVER_H
#define N_HARDREFSERVER_H
//------------------------------------------------------------------------------
/**
    @class nHardRefServer
    @ingroup NebulaSmartPointers

    @brief Validates nHardRef objects as soon as target object is created.

    (C) 2002 RadonLabs GmbH
*/
#include "util/nstrlist.h"

//------------------------------------------------------------------------------
template<class TYPE> class nHardRef;
class nRoot;
class nKernelServer;

class nHardRefServer
{
public:
    /// default constructor.
    nHardRefServer();
    /// default destructor.
    ~nHardRefServer();

    /// adds a nHardRef to internal list
    void RegisterHardRef(nKernelServer* kernelServer, nHardRef<nRoot>* hardRef);
    /// registers a potential target object
    void RegisterTargetObject(nRoot& targetObject);

private:
    nStrList strList;
};
//------------------------------------------------------------------------------
#endif
