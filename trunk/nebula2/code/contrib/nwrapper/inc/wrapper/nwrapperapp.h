#ifndef N_WRAPPERAPP_H
#define N_WRAPPERAPP_H
//------------------------------------------------------------------------------
/**
    @class nWrapperApp
    @ingroup NWrapperContribModule
    @brief The generic code for wrapper-generating executables.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"
#include "util/nstring.h"

class nClass;
class nKernelServer;
class nLanguageWrapper;

class nWrapperApp
{
public:
    /// constructor
    nWrapperApp();
    /// destructor
    ~nWrapperApp();

    void Run();

    void SetBackendClass(const char *);
    const char * GetBackendClass() const;

protected:
    nString backendClass;

    void FindSubClasses(nKernelServer*, nClass*, nArray<nClass *>*);
    void DumpClassTree(nKernelServer*, nLanguageWrapper*, nClass*);
    void DumpClass(nKernelServer*, nLanguageWrapper*, nClass*);
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nWrapperApp::SetBackendClass(const char* backendClass)
{
    this->backendClass = backendClass;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nWrapperApp::GetBackendClass() const
{
    return this->backendClass.Get();
}
//------------------------------------------------------------------------------
#endif
