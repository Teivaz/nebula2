//--------------------------------------------------------------------
//  nluamds.cc --  tha veggies - Lua bindings to static Neb items
//
//  Derived from npythoncmds.cc by Jason Asbahr
//  Derived from ntclcmds.cc by Andre Weissflog
//  (c) 2003  Matthew T. Welker & Vadim Macagon
//
//  nLuaServer is licensed under the terms of the Nebula License
//--------------------------------------------------------------------

#include "luaserver/nluaserver.h"
#include "kernel/nfileserver2.h"
#include "luaserver/ncmdprotolua.h"
#include "signals/nsignalserver.h"

#ifdef __VC__
    // VC6 has the most ignorant warnings...
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4800)
#endif

//--------------------------------------------------------------------
//  _luaDispatch() - by nCmdProto
//--------------------------------------------------------------------    
/**
    @brief Static function that will handle Lua to nCmd translations
    and dispatch to the provided nObject* pointer.
*/
int _luaDispatch(lua_State* L, nObject* obj, nCmdProto* cmd_proto, bool print)
{
    n_assert(cmd_proto); // -- unfriendly isn't it?
    nCmd* cmd = cmd_proto->NewCmd();
    n_assert(cmd);
    
    // Need to get the proper args in...
    int numargs = cmd->GetNumInArgs();
    if ((lua_gettop(L) - 1) != numargs)
    {
        n_message("Wrong number of arguments for command: %s\n", 
                    cmd_proto->GetProtoDef());
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    if (!nLuaServer::StackToInArgs(L, cmd))
    {
        n_message("Incorrect arguments for: %s\n",
                    cmd_proto->GetProtoDef());
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    lua_settop(L, 0);

    if (!obj->Dispatch(cmd))
    {
        n_message("Could not dispatch the command: %s\n",
                    cmd_proto->GetProtoDef());
        lua_pushnil(L);
        return 1;
    }

    nLuaServer::OutArgsToStack(L, cmd, print);

    int retnum = cmd->GetNumOutArgs();
    cmd_proto->RelCmd(cmd);

    return retnum;
}

//--------------------------------------------------------------------
//  luacmd_Error()
//--------------------------------------------------------------------
int luacmd_Error(lua_State* L)
{
    if (nLuaServer::Instance->GetFailOnError())
        nLuaServer::Instance->SetQuitRequested(true);
    // The line buffer is being ignorant
    // without the \n (nDefaultLogHandler)
    n_message("%s\n", lua_tostring(L, -1));
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_StackDump()
//--------------------------------------------------------------------
int luacmd_StackDump(lua_State* L)
{
    if (nLuaServer::Instance->GetFailOnError())
        nLuaServer::Instance->SetQuitRequested(true);
    nLuaServer::Instance->GenerateStackTrace();
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_Panic()
//--------------------------------------------------------------------
int luacmd_Panic(lua_State* L)
{  
    n_error("Lua paniced!");
    nScriptServer* ss = (nScriptServer*)lua_touserdata(L, lua_upvalueindex(1));
    ss->SetQuitRequested(1);
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_New()
//--------------------------------------------------------------------
int luacmd_New(lua_State* L)
{
    // takes 2 strings as arguments
    // returns true on success or nil on failure
    const char* class_name;
    const char* object_name;
    if ((2 != lua_gettop(L)) || !lua_isstring(L, -1) || !lua_isstring(L, -2))
    {
        n_message("Usage is new('class', 'name')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    class_name = lua_tostring(L, -2);
    object_name = lua_tostring(L, -1);
    lua_settop(L, 0);
    nRoot* o = nLuaServer::kernelServer->NewNoFail(class_name, object_name);
    if (!o)
    {
        n_message("Could not create object '%s' of class '%s'\n", 
                    object_name, class_name);
        lua_pushnil(L);
    }
    else
    {
        lua_pushboolean(L, true);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_NewThunk()
//
//  Just like luacmd_New() except that it also creates and returns
//  a thunk for the new object (the thunk is also stored in the
//  _nebthunks table).
//--------------------------------------------------------------------
int luacmd_NewThunk(lua_State* L)
{
    // takes 2 strings as arguments
    // returns 1 thunk on success or nil on failure
    if ((2 != lua_gettop(L)) || !lua_isstring(L, -1) || !lua_isstring(L, -2))
    {
        n_message("Usage is newthunk('class', 'name')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    
    const char* className = lua_tostring(L, -2);
    const char* objectName = lua_tostring(L, -1);
    
    lua_settop(L, 0);
    
    nRoot* obj = nLuaServer::kernelServer->NewNoFail(className, objectName);
    if (!obj)
    {
        n_message("Could not create object '%s' of class '%s'\n", 
                    objectName, className);
        lua_pushnil(L);
    }
    else
    {
        // create thunk and leave it on stack
        nLuaServer::Instance->ThunkNebObject(L, obj);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Delete()
//
//  If a string is passed in it is assumed to be a NOH path, the
//  corresponding nRoot instance will be deleted and if there's a
//  thunk associated with that instance it will be removed from
//  the _nebthunks table. If a thunk is passed in pretty much the
//  same thing happens.
//--------------------------------------------------------------------
int luacmd_Delete(lua_State* L)
{
    // takes 1 string or thunk as an argument
    // returns nil on failure and true on success
    nRoot* o;
    const char* object_name;
    
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1) && !lua_istable(L, -1))
    {
        n_message("Usage is delete('object name') or delete(thunk)");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, -1))
    {
        object_name = lua_tostring(L, -1);
        o = nLuaServer::kernelServer->Lookup(object_name);
    }
    else
    {
        object_name = "_thunk";
        o = nLuaServer::UnpackThunkRoot(L, 1);
    }
    lua_settop(L, 0);
    if (o)
    {
        // remove the corresponding thunk (if there is one) from _nebthunks
        nLuaServer::RemoveThunk(L, (void*)o);
        o->Release();
        lua_settop(L, 0);
        lua_pushboolean(L, true);
    }
    else
    {
        n_message("Could not find %s", object_name);
        lua_pushnil(L);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_UnpinThunk()
//
//  Removes the thunk passed in from the _nebthunks table, but
//  does not delete the corresponding nRoot instance.
//
//  If a string is passed in it is assumed to be a NOH path,
//  otherwise it a thunk is expected.
//--------------------------------------------------------------------
int luacmd_UnpinThunk(lua_State* L)
{
    // takes 1 string or thunk as an argument
    // returns nil on failure and true on success
    nRoot* o;
    const char* object_name;
    
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1) && !lua_istable(L, -1))
    {
        n_message("Usage is unpin('object name') or unpin(thunk)");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, -1))
    {
        object_name = lua_tostring(L, -1);
        o = nLuaServer::kernelServer->Lookup(object_name);
    }
    else
    {
        object_name = "_thunk";
        o = nLuaServer::UnpackThunkRoot(L, 1);
    }
    lua_settop(L, 0);
    if (o)
    {
        // remove the corresponding thunk (if there is one) from _nebthunks
        nLuaServer::RemoveThunk(L, (void*)o);
        lua_settop(L, 0);
        lua_pushboolean(L, true);
    }
    else
    {
        n_message("Could not find %s", object_name);
        lua_pushnil(L);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_PinThunk()
//
//  Pins a thunk, which means that a reference to it is stored in
//  _nebthunks. The thunk will then be reused whenever possible
//  instead of thunking an object all over again. If you want to
//  define script side commands for an object you'll have
//  to pin the corresponding thunk.
//
//  If you have multiple thunks that correspond to the same object
//  and you pin them all then only the last pin will actually "work".
//--------------------------------------------------------------------
int luacmd_PinThunk(lua_State* L)
{
    // takes 1 thunk as an argument
    // returns nil on failure and true on success
      
    if ((1 != lua_gettop(L)) || !lua_istable(L, -1))
    {
        n_message("Usage is pin(thunk)");
        lua_settop(L, 0);
        lua_pushnil(L);
    }
    else
    {
        nRoot* root = nLuaServer::UnpackThunkRoot(L, 1);
        nLuaServer::AddThunk(L, (void*)root);
        lua_settop(L, 0);
        lua_pushboolean(L, true);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Sel()
//--------------------------------------------------------------------
int luacmd_Sel(lua_State* L)
{
    // takes 1 string as an argument
    // returns false on incorrect usage, nil if there is no such object, 
    // or true on success
    nRoot* o;

    if ((1 != lua_gettop(L)) || (!lua_isstring(L, -1) && !lua_istable(L, -1)))
    {
        n_message("Usage is sel('path') or sel(obj ref)\n");
        lua_settop(L, 0);
        lua_pushboolean(L, false);
        return 1;
    }

    if (lua_isstring(L, -1))
    {
        const char* path = lua_tostring(L, -1);
        o = nLuaServer::kernelServer->Lookup(path);
    }
    else
    {
        o = nLuaServer::UnpackThunkRoot(L, 1);
    }
    lua_settop(L, 0);

    if (!o)
    {
        n_printf("Could not select object\n");
        lua_pushnil(L);
    }
    else 
    {
        nLuaServer::kernelServer->SetCwd(o);
        lua_pushboolean(L, true);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Psel()
//
//  Returns the cwd of the NOH as a thunk.
//--------------------------------------------------------------------
int luacmd_Psel(lua_State* L)
{
    // takes no arguments
    // returns 1 thunk
    nRoot* o = nLuaServer::kernelServer->GetCwd();
    if (o)
    {
        // check if a thunk for this object already exists in _nebthunks
        nLuaServer::FindThunk(L, (void*)o);
        if (1 == lua_isnil(L, -1)) // no thunk found? create it
        {
            lua_settop(L, 0);
            nLuaServer::Instance->ThunkNebObject(L, o);
        }   
    }
    else
    {
        n_message("Could not find current working directory");
        lua_settop(L, 0);
        lua_pushnil(L);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Get()
//
//  FIXME: Split this up into Get/GetThunk? Get() won't create a
//         thunk, GetThunk() will do what Get() does now.
//--------------------------------------------------------------------
int luacmd_Get(lua_State* L)
{
    // takes 1 string (filename)
    // returns 1 thunk or nil
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is get('filename')");
        lua_settop(L, 0);
        lua_pushboolean(L, false);
        return 1;
    }
    nObject* o = nLuaServer::kernelServer->Load(lua_tostring(L, -1));
    bool isRoot = o->IsA("nroot");
    n_assert(o && isRoot);
    if (!o)
    {
        n_message("Could not load object '%s'\n", lua_tostring(L, -1));
        lua_settop(L, 0); // clear stack
    }
    else if (!isRoot)
    {
        n_message("Could not load object '%s', not an nRoot", lua_tostring(L, -1));
        lua_settop(L, 0); // clear stack
    }
    else
    {
        lua_settop(L, 0); // clear stack
        nLuaServer::Instance->ThunkNebObject(L, (nRoot *)o);
    }
    return 1;   
}

//--------------------------------------------------------------------
//  luacmd_Exit()
//--------------------------------------------------------------------
int luacmd_Exit(lua_State* L)
{
    //Takes no arguments
    //Returns nothing
    nScriptServer* ss = (nScriptServer*)lua_touserdata(L, lua_upvalueindex(1));
    ss->SetQuitRequested(1);
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_Puts()
//--------------------------------------------------------------------
int luacmd_Puts(lua_State* L)
{
    // takes 1 string as an argument
    // returns nothing
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is puts('some text here')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    n_printf(lua_tostring(L, -1));
    lua_settop(L, 0); // clear the stack
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_Dir()
//--------------------------------------------------------------------
int luacmd_Dir(lua_State* L)
{
    // This should return a table - for output this will
    // have to be translated back to a string.
    lua_settop(L, 0);
    nRoot* cwd = nLuaServer::kernelServer->GetCwd();
    if (!cwd)
    {
        n_message("Could not acquire the current working directory.\n");
        lua_pushnil(L);
        return 1;
    }
    // Construct a table to return
    lua_newtable(L);
    int i = 0;
    nRoot* o = 0;
    for (o=cwd->GetHead(); o; o=o->GetSucc())
    {
        lua_pushnumber(L, i++);
        lua_pushstring(L, o->GetName());
        lua_settable(L, -3);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_CmdDispatch()
//--------------------------------------------------------------------
int luacmd_CmdDispatch(lua_State* L)
{
    // Thunks *only* come here
    // Get the nRoot* out of the table self ref first
    // The parent table is only guarenteed to be first
    // if the : syntax is used - otherwise this fails
    if (!lua_istable(L, 1))
    {
        n_message("On calling member functions make sure to use the ':' operator to access methods\n");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    nRoot* root = nLuaServer::UnpackThunkRoot(L, 1);
    // haul out the particular call - in the upvalue
    nCmdProto* cmdproto = (nCmdProto*)lua_touserdata(L, lua_upvalueindex(1));
    
    return _luaDispatch(L, root, cmdproto, false);
}

//--------------------------------------------------------------------
//  luacmd_Call()
//  executes a func on the current working directory
//--------------------------------------------------------------------
int luacmd_Call(lua_State* L)
{
    // Anonymous call version
    // This requires no thunk and has
    // to do the lookup from here.
    int num = lua_gettop(L);
    if (!num || !lua_isstring(L, 1))
    {
        n_message("Usage is call('func', ...)\n");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    nObject* obj = nScriptServer::GetCurrentTargetObject();
    if (!obj)
        obj = nKernelServer::Instance()->GetCwd();
    const char* cmdname = lua_tostring(L, 1);
    nClass* cl = obj->GetClass();
    nCmdProto* cmd_proto = (nCmdProto*) cl->FindCmdByName(cmdname);
    if (!cmd_proto)
    {
        n_message("Could not find the command '%s'\n", cmdname);
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    return _luaDispatch(L, obj, cmd_proto, false);
}

//--------------------------------------------------------------------
//  luacmd_ConCall()
//  executes a func on the current working directory and prints 
//  the result to the console
//--------------------------------------------------------------------
int luacmd_ConCall(lua_State* L)
{
    //Anonymous call version
    //This requires no thunk and has
    //to do the look up from here.
    int num = lua_gettop(L);
    if (!num || !lua_isstring(L, 1))
    {
        n_message("Usage is concall('func', ...)\n");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    nRoot* root = nLuaServer::kernelServer->GetCwd();
    const char* cmdname = lua_tostring(L, 1);
    nClass* cl = root->GetClass();
    nCmdProto* cmd_proto = (nCmdProto*) cl->FindCmdByName(cmdname);
    if (!cmd_proto)
    {
        n_message("Could not find the command '%s'\n", cmdname);
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    return _luaDispatch(L, root, cmd_proto, true);
}

//--------------------------------------------------------------------
//  luacmd_Lookup()
//
//  Return the thunk that corresponds to specified NOH path. If the
//  thunk hasn't been created yet it will be created, otherwise the
//  existing thunk is returned.
//--------------------------------------------------------------------
int luacmd_Lookup(lua_State* L)
{
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is lookup('fullpathname')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    nRoot* root = (nRoot*)nLuaServer::kernelServer->Lookup(lua_tostring(L, -1));
    lua_settop(L, 0);
    if (!root)
    {
        n_message("Could not find object: %s", lua_tostring(L, -1));
        lua_pushnil(L);
    }
    else
    {
        nLuaServer::FindThunk(L, (void*)root);
        if (1 == lua_isnil(L, -1)) // no thunk? create it
        {
            nLuaServer::ThunkNebObject(L, root);
        }
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Mangle()
//--------------------------------------------------------------------
int luacmd_Mangle(lua_State* L)
{
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is mangle('path')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    
    nString path = nFileServer2::Instance()->ManglePath(lua_tostring(L, -1));
    if (path.IsEmpty())
    {
        n_message("Failed to mangle %s", path.Get());
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    lua_settop(L, 0);
    lua_pushstring(L, path.Get());
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_Exists()
//--------------------------------------------------------------------
int luacmd_Exists(lua_State* L)
{
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is exists('name')");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }
    nRoot* root = (nRoot*)nLuaServer::kernelServer->Lookup(lua_tostring(L, -1));
    lua_settop(L, 0);
    if (root)
      lua_pushboolean(L, 1);
    else
      lua_pushboolean(L, 0);
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_BeginCmds()
//--------------------------------------------------------------------
int luacmd_BeginCmds(lua_State* L)
{
    // takes two arguments - the class name and the number of cmds.
    // returns nothing.
    if ((2 != lua_gettop(L)) || !(lua_isstring(L, -2) && lua_isnumber(L, -1)))
    {
        n_message("Usage is begincmds('classname', iNumCmds)");
        lua_settop(L, 0);
        return 0;
    }

    const char* className = lua_tostring(L, -2);
    nClass* clazz = nLuaServer::kernelServer->FindClass(className);
    if (clazz)
        clazz->BeginScriptCmds(int(lua_tonumber(L, -1)));
    else
        n_error("Failed to open class %s!\n", className);
    
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_AddCmd()
//--------------------------------------------------------------------
int luacmd_AddCmd(lua_State* L)
{
    // takes in 2 strings, the name of the class and the name of the cmd.
    // returns nothing.
    if ((2 != lua_gettop(L)) || !(lua_isstring(L, -1) && lua_isstring(L, -2)))
    {
        n_message("Usage is addcmd('classname', 'cmd def')");
        lua_settop(L, 0);
        return 0;
    }
    
    nCmdProtoLua* cmdProto = new nCmdProtoLua(lua_tostring(L, -1));
    const char* className = lua_tostring(L, -2);
    nClass* clazz = nLuaServer::kernelServer->FindClass(className);
    if (clazz)
        clazz->AddScriptCmd((nCmdProto*)cmdProto);
    else
        n_error("Failed to find class %s!\n", className);
        
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_EndCmds()
//--------------------------------------------------------------------
int luacmd_EndCmds(lua_State* L)
{
    // Takes one argument - the class name.
    // Returns nothing.
    if ((1 != lua_gettop(L)) || !lua_isstring(L, -1))
    {
        n_message("Usage is endcmds('classname')");
        lua_settop(L, 0);
        return 0;
    }
    
    const char* className = lua_tostring(L, -1);
    nClass* clazz = nLuaServer::kernelServer->FindClass(className);
    if (clazz)
        clazz->EndScriptCmds();
    else
        n_error("Failed to find class %s!\n", className);
        
    lua_settop(L, 0);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_DeleteNRef()
//
//  Deletes an nRef previously created during thunking.
//  This method will be automatically called when a thunk gets
//  garbage collected.
//--------------------------------------------------------------------
int luacmd_DeleteNRef(lua_State* L)
{
    // Takes in one argument - the nref userdata
    n_assert((1 == lua_gettop(L)) && lua_isuserdata(L, -1));
    nRef<nRoot>* ref = (nRef<nRoot>*)lua_unboxpointer(L, -1);
    if (ref)
        n_delete(ref);
    return 0;
}

//--------------------------------------------------------------------
//  luacmd_IsZombieThunk()
//
//  Checks if a thunk is a zombie (has an nRef that points to a
//  non-existent nRoot).
//--------------------------------------------------------------------
int luacmd_IsZombieThunk(lua_State* L)
{
    // Takes in one argument - the thunk
    // Returns true if thunk is a zombie, false if it isn't, nil on error
    if ((1 != lua_gettop(L)) || !lua_istable(L, -1))
    {
      n_printf("Usage is IsZombieThunk(thunk)\n");
      lua_settop(L, 0);
      lua_pushnil(L);
      return 1;
    }
    lua_pushstring(L, "_");
    lua_rawget(L, -2);
    n_assert2(lua_isuserdata(L, -1), "nRef userdata not found in thunk!");
    nRef<nRoot>* ref = (nRef<nRoot>*)lua_unboxpointer(L, -1);
    lua_settop(L, 0);
    if (ref)
        lua_pushboolean(L, !ref->isvalid());
    else
    {
        n_error("Degenerate thunk detected!");
        lua_pushnil(L);
    }
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_PushCwd()
//
//  Just exposes nKernelServer::PushCwd()
//--------------------------------------------------------------------
int luacmd_PushCwd(lua_State* L)
{
    // Takes in one argument - either a path string or a thunk.
    // Returns true on success, false on failure.
    if ((1 != lua_gettop(L)) || !(lua_istable(L, -1) || lua_isstring(L, -1)))
    {
        n_printf("Usage is pushcwd('path') or pushcwd(thunk)\n");
        lua_settop(L, 0);
        lua_pushboolean(L, false);
        return 1;
    }
    
    if (lua_istable(L, -1)) // thunk case
    {
        lua_pushstring(L, "_");
        lua_rawget(L, -2);
        n_assert2(lua_isuserdata(L, -1), "nRef userdata not found in thunk!");
        nRef<nRoot>* ref = (nRef<nRoot>*)lua_unboxpointer(L, -1);
        if (ref)
        {
            nLuaServer::kernelServer->PushCwd(ref->get());
            lua_settop(L, 0);
            lua_pushboolean(L, true);
        }
        else
            n_error("Degenerate thunk detected!");
    }
    else // string path case
    {
        nRoot* node = nLuaServer::kernelServer->Lookup(lua_tostring(L, -1));
        if (node)
        {
            nLuaServer::kernelServer->PushCwd(node);
            lua_settop(L, 0);
            lua_pushboolean(L, true);
        }
        else
        {
            n_printf("pushdir: %s not found!", lua_tostring(L, -1));
            lua_settop(L, 0);
            lua_pushboolean(L, false);
        }
    }
        
    return 1;
}

//--------------------------------------------------------------------
//  luacmd_PopCwd()
//
//  Just exposes nKernelServer::PopCwd()
//--------------------------------------------------------------------
int luacmd_PopCwd(lua_State* L)
{
    // Takes in no arguments.
    // Returns nothing.
    if (0 != lua_gettop(L))
    {
      n_printf("Usage is popcwd()\n");
      return 0;
    }
    nLuaServer::kernelServer->PopCwd();
    return 0;
}

//--------------------------------------------------------------------
//  luaobject_Emit()
//--------------------------------------------------------------------
int luaobject_Emit( lua_State *L )
{
    if (!lua_istable(L, 1))
    {
        n_message("On calling member functions make sure to use the ':' operator to access methods\n");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    // find the object
    nObject * obj = nLuaServer::UnpackThunkRoot(L, 1);
    if( !obj )
    {
        n_printf( "Object not found in object:emit( ... )\n");
        lua_settop(L, 0);
        return 0;
    }

    // check the parameters
    if( lua_gettop(L)<2 || !lua_isstring(L, 2) )
    {
        n_printf( "Usage is object:emit( 'signal', ... )\n");
        lua_settop(L, 0);
        return 0;
    }

    nString signal( lua_tostring(L, 2) );

    // find the command
    nCmdProto *cmd_proto = static_cast<nCmdProto *>( obj->GetClass()->FindSignalByName( signal.Get() ) );
    if( !cmd_proto )
    {
        n_printf( "Signal not found in object:emit( '%s', ... )\n", signal.Get() );
        lua_settop(L, 0);
        return 0;
    }

    // quit object from stack
    lua_remove( L, 1 );

    return _luaDispatch( L, obj, cmd_proto, false );
}

//--------------------------------------------------------------------
//  luaobject_Post()
//--------------------------------------------------------------------
int luaobject_Post( lua_State *L )
{
    if (!lua_istable(L, 1))
    {
        n_message("On calling member functions make sure to use the ':' operator to access methods\n");
        lua_settop(L, 0);
        lua_pushnil(L);
        return 1;
    }

    // find the object
    nObject * obj = nLuaServer::UnpackThunkRoot(L, 1);
    if( !obj )
    {
        n_printf( "Object not found in object:emit( ... )\n");
        lua_settop(L, 0);
        return 0;
    }

    // check the parameters
    if( lua_gettop(L)<3 || !lua_isnumber(L,2) || !lua_isstring(L, 3) )
    {
        n_printf( "Usage is object:post( time, 'signal', ... )\n");
        lua_settop(L, 0);
        return 0;
    }

    double timeValue = lua_tonumber( L, 2 );
    nString signal( lua_tostring(L, 3) );

    // find the command in signal list or in regular command list
    nCmdProto *cmd_proto = static_cast<nCmdProto *>( obj->GetClass()->FindSignalByName( signal.Get() ) );
    if( !cmd_proto )
    {
        cmd_proto = static_cast<nCmdProto*>( obj->GetClass()->FindCmdByName( signal.Get() ) );
        if( !cmd_proto )
        {
            n_printf( "Signal not found in object:emit( '%s', ... )\n", signal.Get() );
            lua_settop(L, 0);
            return 0;
        }
    }

    // quit object from stack
    lua_remove( L, 1 );

    nCmd* cmd = cmd_proto->NewCmd();
    n_assert(cmd);

    if (!nLuaServer::StackToInArgs(L, cmd))
    {
        n_printf("Incorrect arguments for: %s\n", cmd_proto->GetProtoDef());
        lua_settop(L, 0);
        return 0;
    }

    lua_settop(L, 0);

    // let signal server object handle the command
    nSignalServer * signalServer = nSignalServer::Instance();
    n_assert( signalServer );
    if( signalServer )
    {
        if( ! signalServer->PostCmd( timeValue, obj, cmd) )
        {
            n_printf( "Post error, in object of class '%s', with signal '%s'", 
                obj->GetClass()->GetName(), signal.Get() );
            cmd_proto->RelCmd( cmd );
        }
    }
    else
    {
        cmd_proto->RelCmd(cmd);
    }

    return 0;
}

#ifdef __VC__
    // VC6
    #pragma warning(pop)
#endif

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
 
