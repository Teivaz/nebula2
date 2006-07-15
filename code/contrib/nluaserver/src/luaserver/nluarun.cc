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
/**
    @brief Executes the chunk at the top of the Lua stack.
*/
bool nLuaServer::ExecuteLuaChunk(nString& result, int errfunc)
{
    n_assert2(errfunc > 0, "Error function stack index must be absolute!");
    
    // call chunk main
    int status = lua_pcall(this->L, 
                           0 /* no args */, 
                           LUA_MULTRET, 
                           errfunc /* stack index of error handler */);
    if (0 != status) // error occured
    {
        result = this->outputStr; // contains the error info
        n_message(result.Get());
        lua_settop(this->L, 0); // clear stack
    }
    else
    {
        result.Clear();
        this->StackToString(this->L, 0, result);
    }
    
    return (0 == status);
}

//--------------------------------------------------------------------
/**
    @brief Empties the Lua stack and returns a string representation 
           of the contents.
           
    @param L Pointer to the Lua state.
    @param bottom Absolute index of the bottom stack item.
    
    Only items between the bottom index and the stack top
    (excluding the bottom index) will be crammed into the 
    string, passing 0 for the bottom will dump all the 
    items in the stack.
*/
void nLuaServer::StackToString(lua_State* L, int bottom, nString& result)
{
    while (bottom < lua_gettop(L))
    {
        switch (lua_type(L, -1))
        {
            case LUA_TBOOLEAN:
            {
                if (lua_toboolean(L, -1)) 
                    result.Append("true");
                else 
                    result.Append("false");
                break;  
            }
            case LUA_TNUMBER:
            case LUA_TSTRING:
            {
                result.Append(lua_tostring(L, -1));
                break;
            }
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
            {
                result.Append("<userdata>");
                break;
            }
            case LUA_TNIL:
            {
                result.Append("<nil>");
                break;
            }
            case LUA_TTABLE:
            {
                // check if it's a thunk
                lua_pushstring(L, "_");
                lua_rawget(L, -2);
                if (lua_isuserdata(L, -1))
                {
                    // assume it's a thunk
                    result.Append("<thunk>");
                }
                else
                {
                    result.Append("{ ");
                    lua_pushnil(L);
                    lua_gettable(L, -2);
                    bool firstItem = true;
                    while (lua_next(L, -2) != 0)
                    {
                        if (!firstItem)
                            result.Append(", ");
                        else
                            firstItem = false;
                        nLuaServer::StackToString(L, lua_gettop(L) - 1, result);
                    }
                    lua_pop(L, 1);
                    result.Append(" }");
                }
                break;
            }
            default:
                //result.Append("???");
                break;  
        }
        lua_pop(L, -1);
    }
}

//--------------------------------------------------------------------
// Prompt()
//--------------------------------------------------------------------
nString nLuaServer::Prompt()
{
    nString prompt = kernelServer->GetCwd()->GetFullName();       
    prompt.Append("> ");
    return prompt;
}


//--------------------------------------------------------------------
/**
    @brief Execute a chunk of Lua code and provide a string 
           representation of the result.
    @param cmdStr A null-terminated string of Lua code.
    @param result The result (if any) of the execution of the Lua code.
    @return true if Lua code ran without any errors, false otherwise.
*/
bool nLuaServer::Run(const char *cmdStr, nString& result)
{
    n_assert(cmdStr);
    // push the error handler on stack
    lua_pushstring(this->L, "_LUASERVER_STACKDUMP");
    lua_gettable(this->L, LUA_GLOBALSINDEX);
    n_assert2(lua_isfunction(this->L, -1), "Error handler not registered!");
    int errfunc = lua_gettop(this->L);
    // load chunk
    int status = luaL_loadbuffer(this->L, cmdStr, strlen(cmdStr), cmdStr);
    if (0 == status) // parse OK?
    {
        return this->ExecuteLuaChunk(result, errfunc);
    }
    else
    {
        // pop error message from the stack
        result.Clear();
        this->StackToString(this->L, lua_gettop(this->L) - 1, result);
    }
    return false;
}

//--------------------------------------------------------------------
/**
    @brief Read in Lua code from a file and execute it.
    @param filename The name of the file to be read and executed.
    @param result Result (if any) of executing the Lua code.
    @return true if Lua code ran without any errors, false otherwise.

    This function will allow explicit return statements from the
    file - and requires it for output.
*/
bool nLuaServer::RunScript(const char *filename, nString& result)
{
    n_assert(filename);
    
    int filesize;
    char *cmdbuf;
    bool retval;
    
    nFile* nfile = nFileServer2::Instance()->NewFileObject();
    nString path = nFileServer2::Instance()->ManglePath(filename);
    if (!nfile->Open(path.Get(), "r"))
    {
        result.Clear();
        nfile->Release();
        return false;
    }
    
    nfile->Seek(0, nFile::END);
    filesize = nfile->Tell();
    nfile->Seek(0, nFile::START);
       
    cmdbuf = (char*)n_malloc(filesize + 1);
    n_assert2(cmdbuf, "Failed to allocate command buffer!");
    nfile->Read(cmdbuf, filesize + 1);
    cmdbuf[filesize] = 0;
    
    nfile->Close();
    nfile->Release();
    
    retval = this->Run(cmdbuf, result);
    if (!retval)
    {
        if (!result.IsEmpty())
        {
            n_message("nLuaServer::RunScript failed:\nfile: %s\nmessage:\n%s\n", 
                      path.Get(), result.Get());
        }
        else
        {
            n_message("nLuaServer::RunScript failed:\nfile: %s\n", path.Get());
        }
    }
    n_free(cmdbuf);
    return retval;
}

//--------------------------------------------------------------------
/**
    @brief Invoke a Lua function.
*/
bool nLuaServer::RunFunction(const char *funcName, nString& result)
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

