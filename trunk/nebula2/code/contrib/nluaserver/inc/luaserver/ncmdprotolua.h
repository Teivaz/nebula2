#ifndef N_CMDPROTOLUA_H
#define N_CMDPROTOLUA_H
//------------------------------------------------------------------------------
/**
  @class nCmdProtoLua
  @ingroup ScriptServices
  @brief A factory for nCmd objects that correspond to LUA implemented
         script commands.

  (c) 2003 Vadim Macagon
  
  nCmdProtoLua is licensed under the terms of the Nebula License
*/

#ifndef N_CMDPROTO_H
#include "kernel/ncmdproto.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCmdProtoLua
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class N_PUBLIC nCmdProtoLua : public nCmdProto 
{
  private:

  public:
    /// Class constructor
    nCmdProtoLua(const char* protoDef);
    /// Class constructor
    nCmdProtoLua(const nCmdProtoLua& rhs);
    
    bool Dispatch(void *, nCmd *);    
};
//--------------------------------------------------------------------
#endif    

