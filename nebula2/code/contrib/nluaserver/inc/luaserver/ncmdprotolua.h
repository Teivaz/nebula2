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

#include "kernel/ncmdproto.h"

//--------------------------------------------------------------------
class nCmdProtoLua : public nCmdProto 
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

