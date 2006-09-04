//------------------------------------------------------------------------------
//  script/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "script/server.h"
#include "script/command.h"
#include "util/cmdlineargs.h"

namespace Script
{
ImplementRtti(Script::Server, Foundation::RefCounted);
ImplementFactory(Script::Server);

Server* Server::Singleton = 0;

Tcl_ObjCmdProc Tcl_CallMethod;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    commandArray(128, 128),
    interp(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->IsOpen());
    n_assert(this->commandArray.Empty());
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the message subsystem for use.
*/
bool
Server::Open()
{
    n_assert(!this->IsOpen());

    // get Tcl interpreter pointer from Nebula2 script server
    this->interp = ((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp();
    n_assert(0 != this->interp);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the message subsystem after use.
*/
void
Server::Close()
{
    n_assert(this->IsOpen());

    // unregister all commands
    while (this->commandArray.Size() > 0)
    {
        this->DeRegisterCommand(this->commandArray[0]->GetClassName());
    }
    this->interp = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Return true if a command exists.
*/
bool
Server::CommandExists(const nString& cmdName) const
{
    for (int i = 0; i < this->commandArray.Size(); i++)
    {
        if (this->commandArray[i]->GetClassName() == cmdName)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Register a Command with the server. Only registered Commands
    will be callable from the Nebula-Console.

    @param  Command    pointer to a Command object
*/
void
Server::RegisterCommand(Command *cmd)
{
    n_assert(this->IsOpen());
    n_assert(cmd);
    n_assert(!this->CommandExists(cmd->GetClassName()));
    Tcl_CreateObjCommand(this->interp, (char *)cmd->GetClassName().Get(), Tcl_CallMethod, (ClientData)cmd, 0);
    this->commandArray.Append(cmd);
}

//------------------------------------------------------------------------------
/**
    DeRegister a Command from the server. The Command will no longer
    be callable from the Nebula-Console.

    @param  Command    pointer to a Command object
*/
void
Server::DeRegisterCommand(const nString& name)
{
    n_assert(this->IsOpen());
    n_assert(this->CommandExists(name));

    for (int i = 0; i < this->commandArray.Size(); i++)
    {
        if (this->commandArray[i]->GetClassName() == name)
        {
            Tcl_DeleteCommand(this->interp, (char *)name.Get());
            this->commandArray.Erase(i);
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Command callback for the TCL-Interpreter-Object

    @param  cdata    pointer to the user data
    @param  interp   pointer to the TCL-Interpreter-Object
    @param  objc     count of the command line parameters
    @param  objv[]   array with the command line parameters

*/
int
Tcl_CallMethod(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    n_assert(objc > 0);
    nString argsData;
    for (int i = 0; i < objc; i++)
    {
        argsData.Append((const char *)Tcl_GetString(objv[i]));
        argsData.Append(" ");
    }
    Util::CmdLineArgs args(argsData);
    return ((Command *)cdata)->Execute(args);
}

} // namespace Script
