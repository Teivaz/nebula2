#ifndef INPUT_SERVER_H
#define INPUT_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Input::Server

    The Input::Server object is the central object of the input
    subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nref.h"
#include "foundation/server.h"
#include "foundation/refcounted.h"

class nInputServer;

//------------------------------------------------------------------------------
namespace Input
{
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
    /// initialize the input subsystem
    virtual bool Open();
    /// close the input subsystem
    virtual void Close();
    /// set the current time
    virtual void SetTime(nTime t);
    /// per-frame trigger method, generates input events
    virtual void Trigger();
    /// get a slider value
    virtual float GetSlider(const char* name) const;
    /// get a button value
    virtual bool GetButton(const char* name) const;
    /// get current mouse position
    virtual const vector2& GetMousePos() const;

protected:
    static Server* Singleton;

    bool isOpen;
    nRef<nInputServer> refInputServer;
    nTime time;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Server::Singleton);
    return Server::Singleton;
}

}; // namespace Input
//------------------------------------------------------------------------------
#endif
