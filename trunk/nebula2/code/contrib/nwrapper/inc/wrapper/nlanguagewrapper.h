#ifndef N_LANGUAGEWRAPPER_H
#define N_LANGUAGEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nLanguageWrapper

    The generic backend for the nwrapper executable. This demonstrates
    how to extract most of the data that you'll need from the Nebula
    Object System and outputs it in a generic manner.

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nLanguageWrapper : public nRoot 
{
public:
    /// constructor
    nLanguageWrapper();
    /// destructor
    virtual ~nLanguageWrapper();

    virtual void OpenClass(nClass *);
    virtual void CloseClass(nClass *);
    virtual void WriteCommand(nClass *, nCmdProto *);

private:
};

//------------------------------------------------------------------------------
#endif
