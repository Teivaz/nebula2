//------------------------------------------------------------------------------
//   ncmdprotopython.h
//
//   (C)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#ifndef N_CMDPROTOPYTHON_H
#define N_CMDPROTOPYTHON_H
//------------------------------------------------------------------------------
/**
    @class nCmdProtoPython
    @ingroup ScriptServices
    @ingroup PythonScriptServices
    @brief A factory for nCmd objects that correspond to Python
           implemented script commands.

    (c) 2004 Kim, Hyoun Woo

    nCmdProtoPython is licensed under the terms of the Nebula License
*/

#include "kernel/ncmdproto.h"

//------------------------------------------------------------------------------
class nCmdProtoPython : public nCmdProto
{
public:
    nCmdProtoPython(const char* protodef);
    nCmdProtoPython(const nCmdProtoPython& rhs);

    bool Dispatch(void *, nCmd *);
};
//------------------------------------------------------------------------------
#endif
