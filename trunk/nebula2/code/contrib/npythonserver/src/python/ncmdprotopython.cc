//------------------------------------------------------------------------------
//   ncmdprotopython.cc
//
//   (C)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "python/ncmdprotopython.h"
#include "python/npythonserver.h"

//------------------------------------------------------------------------------
/**
    Constructor.
*/
nCmdProtoPython::nCmdProtoPython(const char* protoDef)
    : nCmdProto(protoDef, 0)
{
}

//------------------------------------------------------------------------------
/**
    Copy-constructor.
*/
nCmdProtoPython::nCmdProtoPython(const nCmdProtoPython& rhs)
    : nCmdProto(rhs)
{
}

//------------------------------------------------------------------------------
/**
    Dispatch specified command.

    @param obj pointer to nebula object (Must be derived from nObject)
    @param cmd pointer to nCmd which contains in-args and out-args.
*/
bool
nCmdProtoPython::Dispatch(void* obj, nCmd* cmd)
{
    nString result;
    const char* funcName = cmd->GetProto()->GetName();

    return nPythonServer::Instance->RunFunction(funcName, result);
}
