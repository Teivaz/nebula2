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
bool nLuaServer::ExecuteLuaChunk( const char*& result, int errfunc )
{
    n_assert( (errfunc > 0) && "Error function stack index must be absolute!" );
    
    // call chunk main
    int status = lua_pcall( this->L, 
                            0 /* no args */, 
                            LUA_MULTRET, 
                            errfunc /* stack index of error handler */ );
    if ( 0 != status ) // error occured
    {
        result = this->outputStr.Get(); // contains the error info
        n_message( result );
    }
    else
    {
        this->outputStr.Set( "" );
        result = this->StackToString( this->L, 0 );
        if ( 0 == result[0] ) // empty string?
            result = 0; // don't want a blank to be printed in the console
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
const char* nLuaServer::StackToString( lua_State* L, int bottom )
{
    nString* buf = &nLuaServer::Instance->outputStr;
    while (bottom < lua_gettop(L))
    {
        switch (lua_type(L,-1))
        {
            case LUA_TBOOLEAN:
            {
                if (lua_toboolean(L,-1)) 
                    buf->Append("true");
                else 
                    buf->Append("false");
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
                buf->Append("<userdata>");
                break;
            }
            case LUA_TNIL:
            {
                buf->Append("<nil>");
                break;
            }
            case LUA_TTABLE:
            {
                // check if it's a thunk
                lua_pushstring(L, "_");
                lua_rawget(L,-2);
                if ( lua_isuserdata(L,-1) )
                {
                    // assume it's a thunk
                    buf->Append("<thunk>");
                }
                else
                {
                    buf->Append("{ ");
                    lua_pushnil(L);
                    lua_gettable(L,-2);
                    bool firstItem = true;
                    while (lua_next(L,-2) != 0)
                    {
                        if ( !firstItem )
                            buf->Append(", ");
                        else
                            firstItem = false;
                        nLuaServer::StackToString(L,lua_gettop(L)-1);
                    }
                    lua_pop(L,1);
                    buf->Append(" }");
                }
                break;
            }
            default:
                //buf->Append("???");
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
    nString prompt = kernelServer->GetCwd()->GetFullName();       
    prompt.Append("> ");
    return prompt;
}


//--------------------------------------------------------------------
//  Run(const char *cmdStr, const char *& result)
//--------------------------------------------------------------------
bool nLuaServer::Run(const char *cmdStr, const char*& result)
{
    n_assert(cmdStr);
    // push the error handler on stack
    lua_pushstring( this->L, "_LUASERVER_STACKDUMP" );
    lua_gettable( this->L, LUA_GLOBALSINDEX );
    n_assert( lua_isfunction( this->L, -1 ) && "Error handler not registered!" );
    int errfunc = lua_gettop( this->L );
    // load chunk
    int status = luaL_loadbuffer( this->L, cmdStr, strlen(cmdStr), cmdStr );
    if ( 0 == status ) // parse OK?
        return this->ExecuteLuaChunk( result, errfunc );
    else
        result = 0;
    return false;
}

//--------------------------------------------------------------------
//  RunScript()
//  This function will allow explicit return statements from the
//  file - and requires it for output.
//--------------------------------------------------------------------
bool nLuaServer::RunScript(const char *filename, const char*& result)
{
    n_assert(filename);
    
    int filesize;
    char *cmdbuf;
    bool retval;
    
    nFile* nfile = nFileServer2::Instance()->NewFileObject();
    nString path = nFileServer2::Instance()->ManglePath(filename);
    if (!nfile->Open(path.Get(), "r"))
    {
        result = 0;
        nfile->Release();
        return false;
    }
    
    nfile->Seek(0, nFile::END);
    filesize = nfile->Tell();
    nfile->Seek(0, nFile::START);
       
    cmdbuf = (char*)n_malloc(filesize + 1);
    n_assert(cmdbuf && "Failed to allocate command buffer!");
    nfile->Read(cmdbuf, filesize + 1);
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

