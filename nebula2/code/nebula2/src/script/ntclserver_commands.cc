//------------------------------------------------------------------------------
//  ntclserver_commands.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "script/ntclserver.h"
#include "signals/nsignalserver.h"

//------------------------------------------------------------------------------
/**
    Put an "unknown object" error message.
*/
static void tclUnknownObject(Tcl_Interp* interp, const char *objName)
{
    n_assert(objName);
    char buf[N_MAXPATH];
    snprintf(buf, sizeof(buf), "Object '%s' does not exist.", objName);
    Tcl_SetResult(interp, buf, TCL_VOLATILE);
}

//------------------------------------------------------------------------------
/**
    Execute a replaced command with the argument list
    of another command.
*/
static int tclPipeCommand(Tcl_Interp *interp, const char *cmd, int objc, Tcl_Obj *CONST objv[])
{
    const int maxArgs = 64;

    n_assert(objc < maxArgs);
    int i;
    Tcl_Obj *newObjv[maxArgs];
    int res = TCL_OK;

    // increment refcount of original args
    for (i=1; i<objc; i++) 
    {
        newObjv[i] = objv[i];
        Tcl_IncrRefCount(newObjv[i]);
    }
    newObjv[0] = Tcl_NewStringObj(cmd, strlen(cmd));
    Tcl_IncrRefCount(newObjv[0]);
    res = Tcl_EvalObjv(interp, objc, newObjv, TCL_EVAL_DIRECT);

    // decrement refcount of newly created command, this will effectively release it
    Tcl_DecrRefCount(newObjv[0]);

    // decrement refcount of args
    for (i = 1; i < objc; i++) 
    {
        newObjv[i] = objv[i];
        Tcl_DecrRefCount(newObjv[i]);
    }
    return res;
}

//------------------------------------------------------------------------------
/**
    Implement 'new' command.
*/
int tclcmd_New(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    if (objc != 3) 
    {
        Tcl_SetResult(interp, "Syntax is 'name = new class name'", TCL_STATIC);
    }
    else 
    {
        char *className = Tcl_GetString(objv[1]);
        char *objName   = Tcl_GetString(objv[2]);
        nRoot *o = nTclServer::kernelServer->NewNoFail(className, objName);
        if (o) 
        {
            Tcl_SetResult(interp, (char*) o->GetFullName().Get(), TCL_VOLATILE);
            retval = TCL_OK;
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement 'delete' command.
*/
int 
tclcmd_Delete(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    if (objc != 2) 
    {
        Tcl_SetResult(interp, "Syntax is 'delete name'", TCL_STATIC);
    }
    else
    {
        nRoot *o = nTclServer::kernelServer->Lookup(Tcl_GetString(objv[1]));
        if (o) 
        {
            o->Release();
            retval = TCL_OK;
        } 
        else 
        {
            tclUnknownObject(interp, Tcl_GetString(objv[1]));
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement 'sel' command.
*/
int 
tclcmd_Sel(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    if (objc != 2) 
    {
        Tcl_SetResult(interp, "Syntax is 'name = sel name'", TCL_STATIC);
    }
    else
    {
        char *objName = Tcl_GetString(objv[1]);
        nRoot* obj = nTclServer::kernelServer->Lookup(objName);
        if (obj)
        {
            nTclServer::kernelServer->SetCwd(obj);
            Tcl_SetResult(interp, (char*) obj->GetFullName().Get(), TCL_VOLATILE);
            retval = TCL_OK;

        } 
        else 
        {
            tclUnknownObject(interp, Tcl_GetString(objv[1]));
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement 'psel' command.
*/
int 
tclcmd_Psel(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST /*objv*/[])
{
    int retval = TCL_ERROR;
    if (objc != 1) 
    {
        Tcl_SetResult(interp, "Syntax is 'name = psel'", TCL_STATIC);
    }
    else
    {
        nRoot *obj = nTclServer::kernelServer->GetCwd();
        n_assert(obj);
        Tcl_SetResult(interp, (char*) obj->GetFullName().Get(), TCL_VOLATILE);
        retval = TCL_OK;
    }    
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement 'dir' command.
*/
int 
tclcmd_Dir(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST /*objv*/[])
{
    int retval = TCL_ERROR;
    Tcl_ResetResult(interp);
    if (objc != 1) 
    {
        Tcl_SetResult(interp, "Syntax is 'dir'", TCL_STATIC);
    }
    else 
    {
        nRoot *cwd = nTclServer::kernelServer->GetCwd();
        n_assert(cwd);
        nRoot *obj;
        Tcl_Obj *res = Tcl_GetObjResult(interp);
        for (obj = cwd->GetHead(); obj; obj = obj->GetSucc()) 
        {
            const char *n = obj->GetName();
            Tcl_AppendToObj(res, (char *)n, strlen(n));
            Tcl_AppendToObj(res, " ", 1);
        }
        retval = TCL_OK;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement 'get' command.
*/
int 
tclcmd_Get(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    if (objc != 2)
    {
        Tcl_SetResult(interp, "Syntax is 'name = get filename'", TCL_STATIC);
    }
    else 
    {
        char *objName = Tcl_GetString(objv[1]);
        nObject *obj = nTclServer::kernelServer->Load(objName);
        if (obj) 
        {
            bool isRoot = obj->IsA("nroot");
            n_assert(isRoot);
            if (isRoot)
            {
                Tcl_SetResult(interp, (char*) ((nRoot *)obj)->GetFullName().Get(), TCL_VOLATILE);
                retval = TCL_OK;
            }
            else
            {
                char buf[N_MAXPATH];
                sprintf(buf, "Could not load object '%s', not an nRoot", objName); 
                Tcl_SetResult(interp, buf, TCL_VOLATILE);
            }
        } 
        else 
        {   
            char buf[N_MAXPATH];
            sprintf(buf, "Could not load object '%s'", objName); 
            Tcl_SetResult(interp, buf, TCL_VOLATILE);
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Convert Tcl args into into nCmd object.
*/
static 
bool 
_getInArgs(Tcl_Interp *interp, nCmd *cmd, int objc, Tcl_Obj *CONST objv[])
{
    long num_args;
        
    num_args = cmd->GetNumInArgs();
    if (num_args == objc) 
    {
        // read out arguments
        int i;
        nArg *arg;
        cmd->Rewind();
        for (i=0; i<num_args; i++) 
        {
            bool arg_ok = false;
            arg = cmd->In();
            switch(arg->GetType()) 
            {
                case nArg::Int:
                {
                    int n;
                    if (Tcl_GetIntFromObj(interp,objv[i],&n)==TCL_OK) 
                    {
                        arg->SetI(n);                                
                        arg_ok = true;
                    }
                } 
                break;

                case nArg::Float:
                {
                    double d;
                    if (Tcl_GetDoubleFromObj(interp,objv[i],&d)==TCL_OK) 
                    {
                        float f = (float) d;
                        arg->SetF(f);
                        arg_ok = true;
                    }
                }
                break;


                case nArg::String:
                {
                    const char *str = Tcl_GetString(objv[i]);
                    if (strcmp(":null:",str)==0) 
                    {
                        str = NULL;
                    }
                    arg->SetS(str);
                    arg_ok = true;
                }
                break;    

                case nArg::Bool:
                {
                    int bi;
                    if (Tcl_GetBooleanFromObj(interp,objv[i],&bi)==TCL_OK) 
                    {
                        bool b = bi ? true : false;
                        arg->SetB(b);
                        arg_ok = true;
                    }
                }
                break;

                case nArg::Object:
                    {
                        nRoot *o;
                        char *o_name = Tcl_GetString(objv[i]);
                        if (strcmp("null",o_name)==0) 
                        {
                            o = NULL;
                            arg_ok = true;
                        } 
                        else 
                        {
                            o = nTclServer::kernelServer->Lookup(o_name);
                            if (o) arg_ok = true;
                            else   
                            {
                                n_printf("could not lookup '%s' as object!\n",o_name);
                            }
                        }
                        arg->SetO(o);
                    }
                    break;
                    
                case nArg::Void:
                    break;

                case nArg::List:
                    n_printf("List values aren't acceptable in arguments.");
                    arg_ok = false;
                    break;

                default:
                    n_error("nTclServer::_getInArg(): unsupported data type!");
                    break;
            }
            if (!arg_ok) return false;
        }
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
/**
    utility function for nArg of type List
*/
static Tcl_Obj* _putOutListArg(Tcl_Interp *interp, nArg *listArg)
{
    n_assert(nArg::List == listArg->GetType());
    
    nArg* args;
    int num_args = listArg->GetL(args);

    Tcl_Obj* res = Tcl_NewListObj(0,0);

    nArg* arg = args;
    for (int i=0; i<num_args; i++)
    {
        switch (arg->GetType())
        {
            case nArg::Int:
            {
                Tcl_Obj *io = Tcl_NewIntObj(arg->GetI());
                Tcl_ListObjAppendElement(interp,res,io);
            }
            break;

            case nArg::Float:
            {
                Tcl_Obj *fo = Tcl_NewDoubleObj((double)arg->GetF());
                Tcl_ListObjAppendElement(interp,res,fo);
            }
            break;

            case nArg::String:
            {
                const char *s = arg->GetS();
                if (!s)
                {
                    s = ":null:";
                }
                Tcl_Obj *so = Tcl_NewStringObj((char *)s, strlen(s));
                Tcl_ListObjAppendElement(interp, res, so);
            }
            break;

            case nArg::Bool:
            {
                const char *s = arg->GetB() ? "true" : "false";
                Tcl_Obj *so = Tcl_NewStringObj((char *)s, strlen(s));
                Tcl_ListObjAppendElement(interp, res, so);
            }
            break;

            case nArg::Object:
            {
                nString str;
                nRoot *o = (nRoot *) arg->GetO();
                if (o)
                {
                    str = o->GetFullName();
                }
                else
                {
                    str = "null";
                }
                Tcl_Obj *so = Tcl_NewStringObj(str.Get(), str.Length());
                Tcl_ListObjAppendElement(interp,res,so);
            }
            break;

            case nArg::List:
                Tcl_ListObjAppendElement(interp,res,_putOutListArg(interp,arg));
                break;

            case nArg::Void:
                break;

            default:
                n_error("nTclServer _putOutListArg(): unsupported argument type!");
                break;
        }
        arg++;
    }

    return res;
}

//------------------------------------------------------------------------------
/**
    Convert nCmd into Tcl args.
    
    *** TODO ***
    BOOL Objekte auch als Tcl-Bool-Objekte behandeln, nicht als
    String-Objekte (dafuer muessen aber die existieren
    Tcl-Objekte angepasst werden!)
*/
static 
void 
_putOutArgs(Tcl_Interp *interp, nCmd *cmd)
{
    nArg *arg;
    int num_args = cmd->GetNumOutArgs();
    cmd->Rewind();

    Tcl_ResetResult(interp);
    Tcl_Obj *res = Tcl_GetObjResult(interp);

    // handle single return args (no need to create list)
    if (1 == num_args) 
    {
        arg = cmd->Out();
        switch (arg->GetType()) 
        {
            case nArg::Int:
                Tcl_SetIntObj(res,arg->GetI());
                break;

            case nArg::Float:
                Tcl_SetDoubleObj(res,arg->GetF());
                break;

            case nArg::String:
                {
                    const char *s = arg->GetS();
                    if (!s) 
                    {
                        s = ":null:";
                    }
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::Bool:
                {
                    const char *s = arg->GetB() ? "true" : "false";
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::Object:
                {
                    nString str;
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) 
                    {
                        str = o->GetFullName();
                    } 
                    else 
                    {
                        str = "null";
                    }
                    Tcl_SetStringObj(res, (char *) str.Get(), str.Length());
                }
                break;

            case nArg::Void:
                break;

            case nArg::List:
                Tcl_ListObjAppendList(interp,res,_putOutListArg(interp,arg));
                break;

            default:
                n_error("nTclServer::_putOutArgs(): unsupported data type!");
                break;
        }
    }
    else
    {
        // more then one output arg, create a list
        int i;
        for (i=0; i<num_args; i++) 
        {
            arg = cmd->Out();
            switch (arg->GetType()) 
            {
                case nArg::Int:
                    {
                        Tcl_Obj *io = Tcl_NewIntObj(arg->GetI());
                        Tcl_ListObjAppendElement(interp,res,io);
                    }
                    break;

                case nArg::Float:
                    {
                        Tcl_Obj *fo = Tcl_NewDoubleObj((double)arg->GetF());
                        Tcl_ListObjAppendElement(interp,res,fo);
                    }
                    break;

                case nArg::String:
                    {
                        const char *s = arg->GetS();
                        if (!s) 
                        {
                            s = ":null:";
                        }
                        Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::Bool:
                    {
                        const char *s = arg->GetB() ? "true" : "false";
                        Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::Object:
                    {
                        nString str;
                        nRoot *o = (nRoot *) arg->GetO();
                        if (o) 
                        {
                            str = o->GetFullName();
                        } 
                        else 
                        {
                            str = "null";
                        }
                        Tcl_Obj *so = Tcl_NewStringObj(str.Get(), str.Length());
                        Tcl_ListObjAppendElement(interp, res, so);
                    }
                    break;

                case nArg::Void:
                    break;

                case nArg::List:
                    Tcl_ListObjAppendElement(interp,res,_putOutListArg(interp,arg));
                    break;

                default:
                    n_error("nTclServer::_putOutArgs(): unsupported data type!");
                    break;
            }
        }
    }
}         

//------------------------------------------------------------------------------
/**
    Print a error message involving an object and a command.
*/
static 
void 
tcl_objcmderror(Tcl_Interp *interp,
                nTclServer *tcl,
                const char *msg,    // message, must contain 2 '%s'
                nObject *o,         // name is 1st '%s'
                char *cmd_name)     // 2nd '%s'
{
    char errorBuf[1024];
    if (o->IsA("nroot"))
        snprintf(errorBuf, sizeof(errorBuf), msg, ((nRoot *)o)->GetFullName().Get(), cmd_name);
    else
        snprintf(errorBuf, sizeof(errorBuf), msg, "nObject", cmd_name);
    Tcl_SetResult(interp,  errorBuf, TCL_VOLATILE);
    n_printf("*** %s\n", errorBuf);
}

//------------------------------------------------------------------------------
/**
    Wird vom Tcl-Interpreter aufgerufen, wenn ein unbekanntes
    Kommando empfangen wird.
    Die Routine versucht den String so zu interpretieren:
    obj_name.command
    Also, auf "obj_name" wird das durch Punkt getrennte Kommando
    angewendet.
    Fehlt der obj_name, oder faengt das Kommando mit einem
    Punkt an, wird "command" auf das momentan ausgewaehlte
    Objekt angewendet.
    Wenn was schiefgeht, wird das Kommando an das originale
    Unknown-Kommando weitergereicht.
*/
int 
tclcmd_Unknown(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;

    char *obj_name;
    char *cmd_name;
    char *dot;
    char cmd[N_MAXPATH];
    nObject *o;
    bool has_dot = false;

    // extract object name and cmd name
    n_strncpy2(cmd, Tcl_GetString(objv[1]), sizeof(cmd));
    dot = strchr(cmd,'.');

    // special case handle path components
    while (dot && ((dot[1] == '.')||(dot[1] == '/'))) dot=strchr(++dot,'.'); 
    if (dot) 
    {
        has_dot = true;
        *dot = 0;
        obj_name = cmd;
        if (obj_name == dot) 
        {
            obj_name = 0;
        }
        cmd_name = dot+1;
    } 
    else 
    {
        obj_name = 0;
        cmd_name = cmd;
    }
    if (*cmd_name == 0) 
    {
        cmd_name = NULL;
    }

    // find object to invoke command on
    if (obj_name) 
    {
        o = nTclServer::kernelServer->Lookup(obj_name);
    }
    else          
    {
        o = nScriptServer::GetCurrentTargetObject(); // use the nObject if one is set
        if (!o)
            o = nTclServer::kernelServer->GetCwd(); // otherwise use the current nRoot
    }
    if (!o) 
    {
        tclUnknownObject(interp, obj_name);
        return TCL_ERROR;
    }
    if (!cmd_name) {
        Tcl_SetResult(interp, "No command.", TCL_STATIC);
        return TCL_ERROR;
    }

    // invoke command
    nClass *cl = o->GetClass();
    nCmdProto *cmd_proto = (nCmdProto *) cl->FindCmdByName(cmd_name);
    if (cmd_proto) 
    {     
        nCmd *cmd = cmd_proto->NewCmd();
        n_assert(cmd);

        // retrieve input args (skip the 'unknown' and cmd statement)            
        if (!_getInArgs(interp,cmd,objc-2,objv+2)) 
        {
            tcl_objcmderror(interp,tcl,"Broken input args, object '%s', command '%s'",o,cmd_name);
            cmd_proto->RelCmd(cmd);
            return TCL_ERROR;
        }

        // let object handle the command
        if (o->Dispatch(cmd))
        {
            retval = TCL_OK;
            _putOutArgs(interp,cmd);
        }
        else 
        {
            tcl_objcmderror(interp,tcl,"Dispatch error, object '%s', command '%s'",o,cmd_name);
        }
        cmd_proto->RelCmd(cmd);
        
    } 
    else 
    {
        // exception, the object doesn't know about the command!
        // if there was a dot in the command name, we know that
        // it was supposed to be a Nebula command statement,
        // so we will just barf.
        if (has_dot) 
        {
            // NOTE: use SetFailOnError(false) before loading script
            // files if you don't want the program to abort
            tcl_objcmderror(interp,tcl,"Object '%s' doesn't accept command '%s'",o,cmd_name);
            retval = TCL_ERROR;
        }
        else
        {
            #ifdef __MICROTCL__
                // NOTE: the microTcl implementation has no unknown command handler!
                char buf[N_MAXPATH];
                sprintf(buf, "unknown command '%s'", Tcl_GetString(objv[1]));
                Tcl_SetResult(interp, buf, TCL_VOLATILE);
            #else
                // otherwise, let's see if the original unknown command
                // knows what to do...
                retval = tclPipeCommand(interp, "tcl_unknown", objc, objv);
            #endif
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement the 'exit' command.
*/
int 
tclcmd_Exit(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST /*objv*/[])
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;
    if (objc != 1) 
    {
        Tcl_SetResult(interp, "Syntax is 'exit'", TCL_STATIC);
    }
    else 
    {
        // turn off the interactive mode and set the quit requested flag,
        // if the Tcl server is in standalone mode (thus a Nebula application
        // wraps has loaded Tcl), this is the signal for the application
        // that the script server requested to quit the application. 
        // In extension mode however, Nebula has been loaded
        // into an existing Tcl interpreter. In this case, we simply call
        // Tcl_Exit() which does the basic cleanup stuff, and then exits
        // the process, Nebula then gets unloaded in the detach handler
        // of the extension dll.
        tcl->SetQuitRequested(true);

#ifndef __MICROTCL__
        if (!tcl->isStandAloneTcl) 
        {
            Tcl_Exit(0);
        }
#endif

        retval = TCL_OK;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement the 'puts' command.

    NOTE: on the Xbox' MicroTcl implementation, channels are not
    supported. All output will simply be rerouted to the Nebula
    kernel.
*/
int 
tclcmd_Puts(ClientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval     = TCL_OK;
    bool is_stdout = true;

    // emulate puts behaviour but reroute output into Nebula kernel
    if (objc > 1) 
    {
        bool newline = true;
        char *str = Tcl_GetString(objv[objc-1]);
        int i;
        for (i = 1; i < (objc - 1); i++) 
        {
            char *s = Tcl_GetString(objv[i]);
            if (strcmp(s, "-nonewline") == 0) 
            {
                newline = false;
            }
#ifndef __MICROTCL__
            else if (strcmp(s,"stdout")!=0) 
            {
                is_stdout = false;
            }
#endif
        }
        if (is_stdout) 
        {
            nTclServer::kernelServer->Print(str);
            Tcl_SetResult(interp, str, TCL_VOLATILE);
            if (newline) 
            {
                nTclServer::kernelServer->Print("\n");
            }
        }
    }

#ifndef __MICROTCL__
    // hand control to original puts command
    if (!is_stdout) 
    {
        retval = tclPipeCommand(interp, "tcl_puts", objc, objv);
    }
#endif

    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement the 'exists' command.
*/
int 
tclcmd_Exists(ClientData /*cdata*/, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    if (objc != 2) 
    {
        Tcl_SetResult(interp, "Syntax is 'exists name'", TCL_STATIC);
    }
    else 
    {
        nRoot *o = nTclServer::kernelServer->Lookup(Tcl_GetString(objv[1]));
        if (o) Tcl_SetResult(interp, "1", TCL_STATIC);
        else   Tcl_SetResult(interp, "0", TCL_STATIC);
        retval = TCL_OK;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Implement signal emission (emit command). Examples:
    emit .signal arg1 arg2 arg3  (for object currently selected)
    emit object.signal arg1 arg2 arg3
*/
int 
tclcmd_Emit(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;

    char *obj_name;
    char *cmd_name;
    char *dot;
    char cmd[N_MAXPATH];
    nObject *o;
    bool has_dot = false;

    // extract object name and cmd name
    n_strncpy2(cmd, Tcl_GetString(objv[1]), sizeof(cmd));
    dot = strchr(cmd,'.');

    // special case handle path components
    while (dot && ((dot[1] == '.')||(dot[1] == '/'))) dot=strchr(++dot,'.'); 
    if (dot) 
    {
        has_dot = true;
        *dot = 0;
        obj_name = cmd;
        if (obj_name == dot) 
        {
            obj_name = 0;
        }
        cmd_name = dot+1;
    } 
    else 
    {
        obj_name = 0;
        cmd_name = cmd;
    }
    if (*cmd_name == 0) 
    {
        cmd_name = NULL;
    }

    // find object to invoke command on
    if (obj_name) 
    {
        o = nTclServer::kernelServer->Lookup(obj_name);
    }
    else          
    {
        o = nScriptServer::GetCurrentTargetObject(); // use the nObject if one is set
        if (!o)
            o = nTclServer::kernelServer->GetCwd(); // otherwise use the current nRoot
    }
    if (!o) 
    {
        tclUnknownObject(interp, obj_name);
        return TCL_ERROR;
    }
    if (!cmd_name) {
        Tcl_SetResult(interp, "No command.", TCL_STATIC);
        return TCL_ERROR;
    }

    // invoke command
    nClass *cl = o->GetClass();
    nCmdProto *cmd_proto = (nCmdProto *) cl->FindSignalByName(cmd_name);
    if (cmd_proto) 
    {     
        nCmd *cmd = cmd_proto->NewCmd();
        n_assert(cmd);

        // retrieve input args (skip the 'unknown' and cmd statement)            
        if (!_getInArgs(interp,cmd,objc-2,objv+2)) 
        {
            tcl_objcmderror(interp,tcl,"Broken input args, object '%s', signal '%s'",o,cmd_name);
            cmd_proto->RelCmd(cmd);
            return TCL_ERROR;
        }

        // let object handle the command
        if (o->Dispatch(cmd))
        {
            retval = TCL_OK;
            _putOutArgs(interp,cmd);
        }
        else 
        {
            tcl_objcmderror(interp,tcl,"Dispatch error, object '%s', signal '%s'",o,cmd_name);
        }
        cmd_proto->RelCmd(cmd);
        
    } 
    else 
    {
        tcl_objcmderror(interp,tcl,"Object '%s' doesn't accept signal '%s'",o,cmd_name);
        retval = TCL_ERROR;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Post signal and commands (post command). Examples:
    post time .signal arg1 arg2 arg3  (for object currently selected)
    post time object.signal arg1 arg2 arg3 (for any object)

    - time is always relative time in seconds from the present time
        - time 0 will be the next invocation of the signal server
        - time 1.5 will be one and a half seconds from now (aprox.)
    - signal can be either be a signal name or command name
*/
int 
tclcmd_Post(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;

    char *obj_name;
    char *cmd_name;
    char *dot;
    char cmd[N_MAXPATH];
    nObject *o;
    bool has_dot = false;

    // extract time and convert time
    n_strncpy2(cmd, Tcl_GetString(objv[1]), sizeof(cmd));
    nTime relT = atof(cmd);

    // extract object name and cmd name
    n_strncpy2(cmd, Tcl_GetString(objv[2]), sizeof(cmd));
    dot = strchr(cmd,'.');

    // special case handle path components
    while (dot && ((dot[1] == '.')||(dot[1] == '/'))) dot=strchr(++dot,'.'); 
    if (dot) 
    {
        has_dot = true;
        *dot = 0;
        obj_name = cmd;
        if (obj_name == dot) 
        {
            obj_name = 0;
        }
        cmd_name = dot+1;
    } 
    else 
    {
        obj_name = 0;
        cmd_name = cmd;
    }
    if (*cmd_name == 0) 
    {
        cmd_name = NULL;
    }

    // find object to invoke command on
    if (obj_name) 
    {
        o = nTclServer::kernelServer->Lookup(obj_name);
    }
    else          
    {
        o = nScriptServer::GetCurrentTargetObject(); // use the nObject if one is set
        if (!o)
            o = nTclServer::kernelServer->GetCwd(); // otherwise use the current nRoot
    }
    if (!o) 
    {
        tclUnknownObject(interp, obj_name);
        return TCL_ERROR;
    }
    if (!cmd_name) {
        Tcl_SetResult(interp, "No command.", TCL_STATIC);
        return TCL_ERROR;
    }

    // invoke command
    nClass *cl = o->GetClass();
    nCmdProto *cmd_proto = (nCmdProto *) cl->FindSignalByName(cmd_name);
    if (0 == cmd_proto)
    {
        cmd_proto = (nCmdProto *) cl->FindCmdByName(cmd_name);
    }
    if (cmd_proto) 
    {
        nCmd *cmd = cmd_proto->NewCmd();
        n_assert(cmd);

        // retrieve input args (skip the 'unknown' and cmd statement)            
        if (!_getInArgs(interp,cmd,objc-3,objv+3)) 
        {
            tcl_objcmderror(interp,tcl,"Broken input args, object '%s', command/signal '%s'",o,cmd_name);
            cmd_proto->RelCmd(cmd);
            return TCL_ERROR;
        }

        // let object handle the command
        nSignalServer * signalServer = nSignalServer::Instance();
        if (0 == signalServer)
        {
            tcl_objcmderror(interp,tcl,"Signal server not available, object '%s', command/signal '%s'",o,cmd_name);
            cmd_proto->RelCmd(cmd);
            return TCL_ERROR;            
        }

        if (signalServer->PostCmd(relT, o, cmd))
        {
            retval = TCL_OK;
        }
        else
        {
            cmd_proto->RelCmd(cmd);
            tcl_objcmderror(interp,tcl,"Post error, object '%s', command/signal '%s'",o,cmd_name);
        }
    } 
    else 
    {
        tcl_objcmderror(interp,tcl,"Object '%s' doesn't accept signal '%s'",o,cmd_name);
        retval = TCL_ERROR;
    }
    return retval;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
