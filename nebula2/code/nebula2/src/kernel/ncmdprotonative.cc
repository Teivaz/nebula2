//--------------------------------------------------------------------
#include "kernel/ncmdprotonative.h"

//--------------------------------------------------------------------
/**
  @brief Constructor.
  @param _proto_def [in] Blue print string.
  @param _id        [in] 4cc code
  @param _cmd_proc  [in] Pointer to C style command handler.
*/
nCmdProtoNative::nCmdProtoNative(const char *_proto_def, nFourCC _id, 
                                 void (*_cmd_proc)(void *, nCmd *))
  : nCmdProto(_proto_def, _id)
{
    this->cmd_proc = _cmd_proc; // can be NULL if legacy cmd handling used
}

//--------------------------------------------------------------------
/**
*/
nCmdProtoNative::nCmdProtoNative(const nCmdProtoNative& rhs)
  : nCmdProto(rhs)
{
    this->cmd_proc = rhs.cmd_proc;
}

//--------------------------------------------------------------------
/**
*/
bool nCmdProtoNative::Dispatch(void* obj, nCmd* cmd)
{
    n_assert(this->cmd_proc);
    this->cmd_proc(obj, cmd);
    return true;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
