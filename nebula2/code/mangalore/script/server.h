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
    /// return true if server is open
    bool IsOpen() const;
    /// register a command on the server
    void RegisterCommand(Command *cmd);
    /// deregister a command from the server
    void DeRegisterCommand(const nString& cmdName);
    /// return true if a command exists
    bool CommandExists(const nString& cmdName) const;

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

//------------------------------------------------------------------------------
/**
*/
inline
bool
Server::IsOpen() const
{
    return this->isOpen;
}

} // namespace Message
//------------------------------------------------------------------------------
#endif
