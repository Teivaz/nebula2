#ifndef CEUI_SERVER_H
#define CEUI_SERVER_H

#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "cegui/CEGUISystem.h"
#include "cegui/CEGUIWindow.h"

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
    /// create GUI font
    void CreateFont(const nString& fontName);
    /// load GUI scheme
    void LoadScheme(const nString& schemeName);
    /// load window layout
    void LoadWindowLayout(const nString& resName);
    /// create empty layout
    void CreateEmptyLayout();
    /// get root window
    CEGUI::Window* GetRootWindow() const;
    /// display GUI
    void DisplayGui();
    /// hide GUI
    void HideGui();
    /// check if current GUI visible
    bool IsGuiVisible() const;
    /// set default mouse cursor
    void SetDefaultMouseCursor(const nString& schemeName, const nString& cursorName);
    /// check if mouse over an UI element
    bool IsMouseOverGui() const;

private:
    static Server* Singleton;
    CEUI::Renderer* renderer;
    CEGUI::System* ceGuiSystem;
    CEGUI::Window* rootWindow;
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

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::Window* Server::GetRootWindow() const {
    return this->rootWindow;
}

//------------------------------------------------------------------------------
/**
*/
inline
void Server::DisplayGui() {
    if (this->rootWindow != 0 && this->ceGuiSystem->getGUISheet() == 0) {
        this->ceGuiSystem->setGUISheet(this->rootWindow);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void Server::HideGui() {
    if (this->ceGuiSystem->getGUISheet() != 0) {
        this->ceGuiSystem->setGUISheet(NULL);
    }
}

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
