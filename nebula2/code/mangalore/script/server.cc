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
    commandArray(128, 128)
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->interp = 0x0L;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(this->commandArray.Empty());
    n_assert(0 != Singleton);
    Singleton = 0;
    this->interp = 0x0L;
}

//------------------------------------------------------------------------------
/**
    Open the message subsystem for use.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);
    if( ((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp() )
    {
        this->interp = ((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp();
        this->isOpen = true;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Close the message subsystem after use.
*/
void
Server::Close()
{
    int index;

    n_assert(this->isOpen);

    if( this->isOpen )
    {
        // deregister the command-objects and remove the array
        for( index = 0; index < this->commandArray.Size(); index++ )
        {
            Tcl_DeleteCommand(this->interp, (char *)this->commandArray[index].get()->GetName().Get());
        }

        this->commandArray.Clear();
    }

    n_assert(this->commandArray.Empty());
    this->interp = 0x0L;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Register a Command with the server. Only registered Commands
    will be callable from the Nebula-Console.

    @param  Command    pointer to a Command object
*/
void
Server::RegisterCommand(Command *command)
{
    int index;
    bool exists = false;


    n_assert(((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp());
    n_assert(this->isOpen);

    if( this->isOpen )
    {
        // check if the command exists in the array
        for( index = 0; index < this->commandArray.Size(); index++ )
        {
            // if exists, than set exists to true
            if( command->GetName() == this->commandArray[index].get()->GetName() )
            {
                exists = true;
                break;
            }
        }

        n_assert( !exists );

        // if not exists, than register the command-object and add to the array
        if( !exists )
        {
            Tcl_CreateObjCommand(this->interp, (char *)(command->GetName()).Get(), Tcl_CallMethod, (ClientData)command, 0);
            this->commandArray.Append( command );
        }
    }
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
    int index;
    bool exists = false;

    n_assert(((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp());
    n_assert(this->isOpen);

    if( this->isOpen )
    {
        // check if the command exists in the array
        for( index = 0; index < this->commandArray.Size(); index++ )
        {
            // if exists,  than deregister the command-object and remove it from the array
            if( this->commandArray[index].get()->GetName() == name )
            {
                Tcl_DeleteCommand(this->interp, (char *)name.Get());
                this->commandArray.Erase(index);
                exists = true;
                break;
            }
        }

        n_assert( !exists );
    }
}



//------------------------------------------------------------------------------
/**
    Callback for the TCL-Interpreter-Object

    @param  cdata    pointer to the userdata
    @param  interp   pointer to the TCL-Interpreter-Object
    @param  objc     count of the commandline-parameters
    @param  objv[]   array with the commandline-parameters

*/
int 
Tcl_CallMethod(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    nString argsData;
    int idx;
    
    if( 0 < objc )
    {
        for( idx = 1; idx < objc; idx++)
        {
            argsData.Append( (const char *)Tcl_GetString(objv[idx]) );
            argsData.Append( " " );
        }

        Util::CmdLineArgs args( argsData );

        return ((Command *)cdata)->Execute( args );
    }

    return TCL_ERROR;
}




} // namespace Script