//--------------------------------------------------------------------
//  nluarun.cc -- Command processing
//
//  Derived from npythonrun.cc by Jason Asbahr
//  Derived from ntclrun.cc by Andre Weissflog
//  (c) 2003  Matthew T. Welker
//
//  nLuaServer is licensed under the terms of the Nebula License
//--------------------------------------------------------------------

#include "luaserver/nluaserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#ifdef __VC__
    // VC6 warning ignorance
    #pragma warning(push)
    #pragma warning(disable:4800)
#endif

//--------------------------------------------------------------------
//  _lua_tostring(lua_State* L)
//--------------------------------------------------------------------
/**
    @brief _lua_tostring empties the Lua stack and returns a string
    represntation for the contents.
    
    @todo The list handling ought to recurse, but doing so crashes  
    the console if the returned value is a thunk.  rawget doesn't 
    seem to help this any either and it seems that Lua is still
    calling the __index metatable values.
*/
const char* nLuaServer::_lua_tostring( lua_State* L, int bottom )
{
    nString* buf = nLuaServer::Instance->output;
    buf->Set("");
    while (bottom < lua_gettop(L))
    {
        switch (lua_type(L,-1))
        {
            case LUA_TBOOLEAN:
            {
                if(lua_toboolean(L,-1)) buf->Append("true");
                else buf->Append("false");
                break;  
            }
            case LUA_TNUMBER:
            case LUA_TSTRING:
            {
                buf->Append(lua_tostring(L,-1));
                break;
            }
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
            {
                buf->Append("<obect>");
                break;
            }
            case LUA_TNIL:
            {
                buf->Append("<nil>");
                break;
            }
            case LUA_TTABLE:
            {
                buf->Append("{ ");
                lua_pushnil(L);
                lua_gettable(L, -2);

                while (lua_next(L, -2) != 0) //pops a key and pushes key and value
                {
                    if(lua_isstring(L,-1))
                    {
                        buf->Append(lua_tostring(L,-1));
                    }
                    lua_pop(L,1);
                    buf->Append(" ");
                }
                lua_pop(L,1);
                buf->Append(" }");
                break;
            }
            default:
                buf->Append("???");
                break;  
        }
        lua_pop(L,-1);
    }
    return buf->Get();
}

//--------------------------------------------------------------------
// Prompt()
//--------------------------------------------------------------------
nString nLuaServer::Prompt()
{
    char buffer[N_MAXPATH];
    nString prompt = kernelServer->GetCwd()->GetFullName(buffer, sizeof(buffer));       
    prompt.Append("> ");
    return prompt;
}


//--------------------------------------------------------------------
//  Run(const char *cmdStr, const char *& result)
//--------------------------------------------------------------------
bool nLuaServer::Run(const char *cmdStr, const char*& result)
{
    n_assert(cmdStr);

    bool retval = lua_dostring(this->L, cmdStr);

    result = _lua_tostring(this->L, 0);
    return !retval;
}

//--------------------------------------------------------------------
//  RunScript()
//  This function will allow explicit return statements from the
//  file - and requires it for output.
//--------------------------------------------------------------------
bool nLuaServer::RunScript(const char *filename, const char*& result)
{
    n_assert(filename);
    char buf[N_MAXPATH];
    int filesize;
    char *cmdbuf;
    bool retval;
    
    nFileServer2* fileServer = this->ref_FileServer.get();
    nFile* nfile = fileServer->NewFileObject();
    fileServer->ManglePath(filename, buf, N_MAXPATH);
    if (!nfile->Open(buf, "r"))
    {
        result = 0;
        nfile->Release();
        return false;
    }
    
    nfile->Seek(0, nFile::END);
    filesize = nfile->Tell();
    nfile->Seek(0, nFile::START);
    
    cmdbuf = (char*)n_malloc(filesize+1);
    nfile->Read(cmdbuf, filesize+1);
    cmdbuf[filesize] = 0;
    
    nfile->Close();
    nfile->Release();
    
    retval = this->Run(cmdbuf, result);
    n_free(cmdbuf);
    return retval;
}

//--------------------------------------------------------------------
/**
    @brief Invoke a Lua function.
*/
bool nLuaServer::RunFunction(const char *funcName, const char*& result )
{
    nString cmdStr = funcName;
    cmdStr.Append("()");
    return this->Run(cmdStr.Get(), result);
}

//--------------------------------------------------------------------
//  Trigger()
//--------------------------------------------------------------------
bool nLuaServer::Trigger(void)
{
    // The Trigger, she does nothing...
    return nScriptServer::Trigger();
}

#ifdef __VC__
    // VC6 thing
    #pragma warning(pop)
#endif

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

