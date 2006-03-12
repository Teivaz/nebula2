//------------------------------------------------------------------------------
//  ntclserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "script/ntclserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaClass(nTclServer, "nscriptserver");

// new tcl commands and tcl command replacements
extern Tcl_ObjCmdProc tclcmd_New;
extern Tcl_ObjCmdProc tclcmd_Delete;
extern Tcl_ObjCmdProc tclcmd_Sel;
extern Tcl_ObjCmdProc tclcmd_Psel;
extern Tcl_ObjCmdProc tclcmd_Get;
extern Tcl_ObjCmdProc tclcmd_Exit;
extern Tcl_ObjCmdProc tclcmd_Unknown;
extern Tcl_ObjCmdProc tclcmd_Puts;
extern Tcl_ObjCmdProc tclcmd_Dir;
extern Tcl_ObjCmdProc tclcmd_Exists;
extern Tcl_ObjCmdProc tclcmd_Emit;
extern Tcl_ObjCmdProc tclcmd_Post;

//------------------------------------------------------------------------------
/**
*/
nTclServer::nTclServer() :
    interp(0),
    redirectUnknown(false),
    indentLevel(0),
    isStandAloneTcl(true),
    printError(false)
{
#ifndef __MICROTCL__
    // initialize data internal to TCL to make encodings work.
    char buf[N_MAXPATH];
    kernelServer->GetFileServer()->ManglePath("home:bin/tcl/tcl8.4",buf,sizeof(buf));
    #ifdef __WIN32__
        // under Windows let's be nice with backslashes
        char *tmp;
        while ((tmp = strchr(buf,'/'))) *tmp='\\';
    #endif
    Tcl_FindExecutable(buf);
#endif

    // create interpreter
    this->interp = Tcl_CreateInterp();
    n_assert(this->interp);

    this->LinkToInterp(this->interp, true);
}

//------------------------------------------------------------------------------
/**
*/
nTclServer::~nTclServer()
{
    n_assert(this->interp);
    this->UnlinkFromInterp(this->interp, true);
    if (this->isStandAloneTcl)
    {
        Tcl_DeleteInterp(this->interp);
    }
}

//------------------------------------------------------------------------------
/**
    Link to a tcl interpreter.
*/
void
nTclServer::LinkToInterp(Tcl_Interp* interp, bool /*isStandAlone*/)
{
    n_assert(interp);

#ifndef __MICROTCL__
    if (isStandAlone) 
    {
        // initialize tcl_library variable
        char buf[N_MAXPATH];
        kernelServer->GetFileServer()->ManglePath("home:bin/tcl/tcl8.4", buf, sizeof(buf));
        #ifdef __WIN32__
            // under Windows let's be nice with backslashes
            char *tmp;
            while ((tmp = strchr(buf,'/'))) *tmp='\\';
        #endif

        // FIXME: strangeness... if the following 'puts' block does not
        // come before setting the tcl library, and Nebula has been started 
        // from nlaunch Tcl_Init() won't find the init.tcl script... nsh works ok.
        // also, puts is only overriden in standalone mode, since 
        // Tcl debuggers (at least the TclPro debugger) don't like it when puts
        // is modified
        Tcl_EvalEx(this->interp,"rename puts tcl_puts",-1, TCL_EVAL_DIRECT);
        Tcl_CreateObjCommand(this->interp, "puts", tclcmd_Puts, (ClientData) this, 0);

        n_printf("Setting tcl_library to '%s'\n",buf);
        Tcl_SetVar(this->interp,"tcl_library",buf,0);
        Tcl_Init(this->interp);    
    }

    Tcl_EvalEx(this->interp,"rename unknown tcl_unknown",-1,TCL_EVAL_DIRECT);
    Tcl_CreateObjCommand(this->interp, "unknown", tclcmd_Unknown, (ClientData)this, 0);
#endif /* __MICROTCL__ */

#ifdef __MICROTCL__
    Tcl_CreateObjCommand(this->interp, "puts",    tclcmd_Puts,    (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "unknown", tclcmd_Unknown, (ClientData)this, 0);
#endif

    Tcl_CreateObjCommand(this->interp, "new",           tclcmd_New,         (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "delete",        tclcmd_Delete,      (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "sel",           tclcmd_Sel,         (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "psel",          tclcmd_Psel,        (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "get",           tclcmd_Get,         (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "exit",          tclcmd_Exit,        (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "dir",           tclcmd_Dir,         (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "exists",        tclcmd_Exists,      (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "emit",          tclcmd_Emit,        (ClientData)this, 0);
    Tcl_CreateObjCommand(this->interp, "post",          tclcmd_Post,        (ClientData)this, 0);
}

//------------------------------------------------------------------------------
/**
    Unlink from tcl interpreter if running as extension.
*/
void 
nTclServer::UnlinkFromInterp(Tcl_Interp *interp, bool /*isStandAlone*/)
{
    n_assert(interp);

    Tcl_DeleteCommand(this->interp, "exists");
    Tcl_DeleteCommand(this->interp, "dir");
    Tcl_DeleteCommand(this->interp, "unknown");
    Tcl_DeleteCommand(this->interp, "server");
    Tcl_DeleteCommand(this->interp, "get");
    Tcl_DeleteCommand(this->interp, "psel");
    Tcl_DeleteCommand(this->interp, "sel");
    Tcl_DeleteCommand(this->interp, "delete");
    Tcl_DeleteCommand(this->interp, "new");

#ifdef __MICROTCL__
    Tcl_DeleteCommand(this->interp, "puts");
    Tcl_DeleteCommand(this->interp, "file");
#endif

#ifndef __MICROTCL__
    if (isStandAlone)
    {
        Tcl_DeleteCommand(this->interp,"puts");
        Tcl_EvalEx(this->interp, "rename tcl_puts puts", -1, TCL_EVAL_DIRECT);
    }    
    Tcl_EvalEx(this->interp, "rename tcl_unknown unknown", -1, TCL_EVAL_DIRECT);
#endif
}

//------------------------------------------------------------------------------
/**
    Migrate from standalone mode to tcl extension mode. Our "owned"
    Tcl intrepreter created in the nTclServer constructor will
    be killed, and Nebula will be linked to the provided
    Tcl interpreter.
*/
#ifndef __MICROTCL__
void 
nTclServer::InitAsExtension(Tcl_Interp *extInterp)
{
    n_assert(extInterp);
    n_assert(this->interp);

    // unlink from previous interpreter
    this->UnlinkFromInterp(this->interp, this->isStandAloneTcl);

    // if we created the previous interpreter, kill it
    if (this->isStandAloneTcl) 
    {
        Tcl_DeleteInterp(this->interp);
        this->interp = NULL;
    }

    // link to the external interpreter
    this->interp = extInterp;
    this->isStandAloneTcl = false;
    this->LinkToInterp(this->interp, this->isStandAloneTcl);
}
#endif

//------------------------------------------------------------------------------
/**
    Begin writing a persistent object.
    
     - 27-Feb-04   cubejk  check for already existing file and delete before
                           creating the new
*/
nFile* 
nTclServer::BeginWrite(const char* filename, nObject* obj)
{
    n_assert(filename);
    n_assert(obj);

    this->indentLevel = 0;

    //check if the file already exist
    if (kernelServer->GetFileServer()->FileExists(filename))
    {
        //delete the old file before writing the new
        kernelServer->GetFileServer()->DeleteFile(filename);
    }

    nFile* file = kernelServer->GetFileServer()->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char buf[N_MAXPATH];
        snprintf(buf, sizeof(buf), "# $parser:ntclserver$ $class:%s$\n", obj->GetClass()->GetName());

        file->PutS("# ---\n");
        file->PutS(buf);
        file->PutS("# ---\n");
        return file;
    }
    else
    {
        n_printf("nTclServer::WriteBegin(): failed to open file '%s' for writing!\n", filename);
        file->Release();
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Finish writing a persistent object.
*/
bool 
nTclServer::EndWrite(nFile* file)
{
    n_assert(file);
 
    file->PutS("# ---\n");
    file->PutS("# Eof\n");
    
    file->Close();
    file->Release();
    return (this->indentLevel == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
nTclServer::Indent(int i, char* buf)
{
    int j;
    for (j = 0; j < i; j++)
    {
        buf[j] = '\t';
    }
    buf[j] = 0;
}

//------------------------------------------------------------------------------
/**
    Write the statement to select an object after its creation
    statement.
*/
void
nTclServer::WriteSelectStatement(nFile* file, nRoot* o, nRoot* owner)
{
    char indentBuf[MAXINDENT];

    switch (this->GetSelectMethod()) 
    {
        case SELCOMMAND:
            {
                // get relative path from owner to o and write select statement
                this->Indent(++this->indentLevel, indentBuf);
                nString relPath = owner->GetRelPath(o);

                file->PutS(indentBuf);
                file->PutS("sel ");
                file->PutS(relPath.Get());
                file->PutS("\n");
            }
            break;

        case NOSELCOMMAND:
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Write start of persistent object with default constructor.
*/
bool 
nTclServer::WriteBeginNewObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    char indentBuf[MAXINDENT];

    // write generic 'new' statement
    const char *objName  = o->GetName();
    const char *objClass = o->GetClass()->GetName();
    this->Indent(this->indentLevel, indentBuf);

    file->PutS(indentBuf);
    file->PutS("new ");
    file->PutS(objClass);
    file->PutS(" ");
    file->PutS(objName);
    file->PutS("\n");

    // write select object statement
    this->WriteSelectStatement(file, o, owner);
    return true;
}

//------------------------------------------------------------------------------
/**
    Write start of persistent object with custom constructor
    defined by command.
*/
bool 
nTclServer::WriteBeginNewObjectCmd(nFile* file, nRoot *o, nRoot *owner, nCmd *cmd)
{
    n_assert(file);
    n_assert(o);
    n_assert(cmd);

    // write constructor statement
    this->WriteCmd(file, cmd);

    // write select object statement
    this->WriteSelectStatement(file, o, owner);
    return true;
}

//------------------------------------------------------------------------------
/**
    Write start of persisting object without constructor, only
    write the select statement.
*/
bool 
nTclServer::WriteBeginSelObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // write select object statement
    this->WriteSelectStatement(file, o, owner);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nTclServer::WriteEndObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    char indentBuf[MAXINDENT];

    // get relative path from owner to o and write select statement
    this->Indent(--this->indentLevel, indentBuf);
    nString relPath = o->GetRelPath(owner);

    file->PutS(indentBuf);
    file->PutS("sel ");
    file->PutS(relPath.Get());
    file->PutS("\n");

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool nTclServer::WriteCmd(nFile* file, nCmd *cmd)
{
    n_assert(file);
    n_assert(cmd);
    const char *name = cmd->GetProto()->GetName();
    n_assert(name);
    nArg *arg;

    char indentBuf[MAXINDENT];
    this->Indent(this->indentLevel, indentBuf);

    // write the command name
    file->PutS(indentBuf);
    file->PutS(".");
    file->PutS(name);

    // write command arguments
    cmd->Rewind();
    int numArgs = cmd->GetNumInArgs();
    int i;
    for (i = 0; i < numArgs; i++) 
    {
        nString str(" ");
        arg = cmd->In();

        switch(arg->GetType()) {

            case nArg::Int:
                str.AppendInt(arg->GetI());
                break;

            case nArg::Float:
                str.AppendFloat(arg->GetF());
                break;

            case nArg::String:
                str.Append("\"");
                str.Append(arg->GetS());
                str.Append("\"");
                break;

            case nArg::Bool:
                if (arg->GetB())
                {
                    str.Append("true");
                }
                else
                {
                    str.Append("false");
                }
                break;

            case nArg::Object:
                {
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) 
                    {
                        str.Append(o->GetFullName());
                    } 
                    else 
                    {
                        str.Append("null");
                    }
                }
                break;

            default:
                str.Append("???");
                break;
        }

        file->PutS(str.Get());
    }
    return file->PutS("\n");
}

//------------------------------------------------------------------------------
/**
    Generate a prompt string for interactive mode.
*/
nString
nTclServer::Prompt()
{
    nString prompt = kernelServer->GetCwd()->GetFullName();
    prompt.Append("> ");
    return prompt;
}

//------------------------------------------------------------------------------
/**
    Evaluate a Tcl statement.
*/
bool 
nTclServer::Run(const char *cmdStr, nString& result)
{
    result.Clear();
    int errCode = Tcl_EvalEx(this->interp, (char *) cmdStr, -1, TCL_EVAL_DIRECT);

    result = "NO RESULT";
    Tcl_Obj *res = Tcl_GetObjResult(interp);
    result = Tcl_GetString(res);

    if (errCode == TCL_ERROR)
    {
        if (this->GetFailOnError())
        {
            n_error("*** Tcl error '%s'\n", Tcl_GetString(res));
        }
        else
        {
            n_printf("*** Tcl error '%s'\n", Tcl_GetString(res));
        }
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Invoke a TCL procedure
*/
bool
nTclServer::RunFunction(const char *functionName, nString& result)
{
    return this->Run(functionName, result);
}


//------------------------------------------------------------------------------
/**
    Evaluate a Tcl script.
*/
bool 
nTclServer::RunScript(const char *filename, nString& result)
{
    result.Clear();

#ifdef __MICROTCL__
    // the microtcl Tcl_EvalFile() implementation accepts Nebula paths
    this->printError = true;
    int errCode = Tcl_EvalFile(this->interp, (char*) filename);
    this->printError = false;

#else
    // standard tcl implementations need a mangled path
    char buf[N_MAXPATH];
    kernelServer->GetFileServer()->ManglePath(filename,buf,sizeof(buf));
    
    this->printError = true;
    int errCode = Tcl_EvalFile(this->interp, buf);
    this->printError = false;
#endif

    Tcl_Obj *res = Tcl_GetObjResult(interp);
    result = Tcl_GetString(res);
    if (errCode == TCL_ERROR) 
    {
        if (this->GetFailOnError())
        {
            n_error("*** Tcl error '%s' in file %s line %d.\n", result.Get(), filename, this->interp->errorLine);
        }
        else
        {
            n_printf("*** Tcl error '%s' in file %s line %d.\n", result.Get(), filename, this->interp->errorLine);
        }
        return false;
    }             
    return true;
}

//------------------------------------------------------------------------------
/**
    Handle Tcl events. 
    
    NOTE:
    The MicroTcl implementation on the xbox doesnt implement the event system.
*/
bool 
nTclServer::Trigger(void)
{
#ifndef __MICROTCL__
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT));
#endif

    return nScriptServer::Trigger();
}

//------------------------------------------------------------------------------
/**
    Get the pointer of the tcl-interpreter.
*/
Tcl_Interp* 
nTclServer::GetInterp()
{
    return this->interp;
}


//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------

