#ifndef CEUI_SERVER_H
#define CEUI_SERVER_H

#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "cegui/CEGUISystem.h"

namespace CEUI
{

class Renderer;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// return instance pointer
    static Server* Instance();
    /// open the ui subsystem
    bool Open();
    /// close the ui subsystem
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// set current frame time
    void SetFrameTime(nTime f);
    /// get current frame time
    nTime GetFrameTime() const;
    /// trigger the server
    void Trigger();
    /// render the user interface
    void Render();

private:
    static Server* Singleton;
    CEUI::Renderer* renderer;
    CEGUI::System* ceGuiSystem;
    bool isOpen;
    nTime time;
    nTime frameTime;
};

RegisterFactory(Server);


//------------------------------------------------------------------------------
/**
*/
inline
Server* Server::Instance() {
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool Server::IsOpen() const {
    return isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void Server::SetTime(nTime t) {
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime Server::GetTime() const {
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void Server::SetFrameTime(nTime f) {
    this->frameTime = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime Server::GetFrameTime() const {
    return this->frameTime;
}

}

#endif