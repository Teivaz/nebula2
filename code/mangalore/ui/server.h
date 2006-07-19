#ifndef UI_SERVER_H
#define UI_SERVER_H
//------------------------------------------------------------------------------
/**
    @class UI::Server

    Server for the Mangalore 3D-GUI subsystem.

    (C) 2005 Radon Labs GmbH
*/
#include "message/port.h"
#include "foundation/ptr.h"
#include "util/narray.h"
#include "graphics/lightentity.h"

//------------------------------------------------------------------------------
namespace Message
{
    class Port;
}

namespace Input
{
    class Mapping;
}

namespace UI
{
class EventHandler;
class FactoryManager;
class Window;
class Event;

class Server : public Message::Port
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

    /// listen to mouse inputs
    virtual bool Accepts(Message::Msg* msg);
    /// handle mouse inputs
    virtual void HandleMessage(Message::Msg* msg);

    /// send a gui event to the UI
    void PutEvent(Event* guiEvent);    
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
    /// convert a 2D screen space position into a position in view space
    vector3 ScreenSpacePosToViewSpace(const vector2& screenSpacePos);
    /// return the projection matrix used by gui elements
    const matrix44& GetGuiProjectionMatrix() const;
    /// return the inverse of the gui projection matrix
    const matrix44& GetInvGuiProjectionMatrix() const;

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

    Ptr<Message::Port> curEventHandler;  // pointer to current event handler
    bool inTrigger;
    matrix44 guiProjMatrix;
    matrix44 invGuiProjMatrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Server::GetGuiProjectionMatrix() const
{
    return this->guiProjMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Server::GetInvGuiProjectionMatrix() const
{
    return this->invGuiProjMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
Window*
Server::GetActiveWindow() const
{
    // FIXME!
    // find the first valid window
    int i;
    for (i = 0; i < this->windows.Size(); i++)
    {
        if (this->windows[i].isvalid())
        {
            return this->windows[i];
        }
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

} // namespace UI
//------------------------------------------------------------------------------
#endif
