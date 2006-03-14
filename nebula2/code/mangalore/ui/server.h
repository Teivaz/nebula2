#ifndef UI_SERVER_H
#define UI_SERVER_H
//------------------------------------------------------------------------------
/**
    @class UI::Server

    Server for the Mangalore 3D-GUI subsystem.
    
    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "util/narray.h"
#include "graphics/lightentity.h"

//------------------------------------------------------------------------------
namespace Message
{
    class Port;
}

namespace UI
{
class UIEntity;
class Canvas;
class EventHandler;
class FactoryManager;
class Window;

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
    /// return the active window (can return 0)
    Window* GetActiveWindow() const;

    /// *** OBSOLETE *** create and display a user interface
    void DisplayGui(const nString& resName, Message::Port* eventHandler);
    /// *** OBSOLETE *** get pointer to currently displayed canvas, can be 0!
    Canvas* GetToplevelCanvas() const;
    /// *** OBSOLETE *** hide the current user interface
    void HideGui();

    /// send a gui event to the UI
    void PutEvent(const nString& eventName);    
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
    /// show a tooltip
    void ShowToolTip(const nString& tooltip);
    /// get pointer to UI server's graphics light entity
    Graphics::LightEntity* GetLightEntity() const;

    /// get mouse position
    const vector2& GetMousePosition() const;
    /// is mouse over a UI element
    bool IsMouseOverGui() const;

private:
    friend class Window;

    /// attach a window to the UI
    void AttachWindow(Window* window);
    /// remove a window from the UI
    void RemoveWindow(Window* window);

    static Server* Singleton;
    bool isOpen;
    nTime time;
    nTime frameTime;
    vector2 mousePos;

    Ptr<FactoryManager> factoryManager;
    Ptr<Graphics::LightEntity> lightEntity;
    nArray<Ptr<Window> > windows;

    // *** OBSOLETE ***
    Ptr<Canvas> curCanvas;              // pointer to current root GUI element
    Ptr<Message::Port> curEventHandler;  // pointer to current event handler
    bool inTrigger;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Window*
Server::GetActiveWindow() const
{
    // FIXME!
    if (this->windows.Size() > 0)
    {
        return this->windows[0];
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
Graphics::LightEntity*
Server::GetLightEntity() const
{
    return this->lightEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline
Canvas*
Server::GetToplevelCanvas() const
{
    return this->curCanvas.get_unsafe();
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

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetFrameTime(nTime f)
{
    this->frameTime = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
Server::GetMousePosition() const
{
    return this->mousePos;
}

}; // namespace UI
//------------------------------------------------------------------------------
#endif
