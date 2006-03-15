#ifndef SCRIPT_SERVER_H
#define SCRIPT_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Script::Server

    The server of the Scripting subsystem is the central point
    where all commands will be register and deregister.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "script/ntclserver.h"

//------------------------------------------------------------------------------
namespace Script
{
class Command;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    /// open the script server
    bool Open();
    /// close the script server
    void Close();
    /// register a command on the server
    void RegisterCommand(Command *command);
    /// deregister a command from the server
    void DeRegisterCommand(const nString& name);

    /// Callback for the TCL-Interpreter-Object
    //static int Tcl_CallMethod(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv);

private:

    static Server* Singleton;
    bool isOpen;
    Tcl_Interp* interp;
    nArray<Ptr<Command> > commandArray;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

}; // namespace Message
//------------------------------------------------------------------------------
#endif