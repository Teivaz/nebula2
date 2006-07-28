//------------------------------------------------------------------------------
//  ceui/server.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_SERVER_H
#define CEUI_SERVER_H

#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "cegui/CEGUISystem.h"
#include "cegui/CEGUIWindow.h"

namespace CEUI
{

class Renderer;
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
    /// destroy GUI font
    void DestroyFont(const nString& fontName);
    /// destroy all GUI fonts
    void DestroyAllFonts();
    /// load GUI scheme
    void LoadScheme(const nString& schemeName);
    /// unload GUI scheme
    void UnloadScheme(const nString& schemeName);
    /// unload all GUI scheme
    void UnloadAllSchemes();

#ifdef CreateWindow
#undef CreateWindow
#endif
    /// create window
    CEGUI::Window* CreateWindow(const nString& type, const nString& winName);
    /// find window by name
    CEGUI::Window* GetWindow(const nString& winName);
    /// destroy window
    void DestroyWindow(CEGUI::Window* window);
    /// destroy all windows
    void DestroyAllWindows();
    /// load window layout
    void LoadWindowLayout(const nString& resName);
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
    set current time
*/
inline
void Server::SetTime(nTime t) {
    time = t;
}

//------------------------------------------------------------------------------
/**
    get current time
*/
inline
nTime Server::GetTime() const {
    return time;
}

//------------------------------------------------------------------------------
/**
    set current frame time
*/
inline
void Server::SetFrameTime(nTime f) {
    frameTime = f;
}

//------------------------------------------------------------------------------
/**
    get current frame time
*/
inline
nTime Server::GetFrameTime() const {
    return frameTime;
}

//------------------------------------------------------------------------------
/**
    display GUI
*/
inline
void Server::DisplayGui() {
    if (ceGuiSystem->getGUISheet() != 0) {
        ceGuiSystem->getGUISheet()->setVisible(true);
    }
}

//------------------------------------------------------------------------------
/**
    hide GUI
*/
inline
void Server::HideGui() {
    if (ceGuiSystem->getGUISheet() != 0) {
        ceGuiSystem->getGUISheet()->setVisible(false);
    }
}

//------------------------------------------------------------------------------
/**
    check if current GUI visible
*/
inline
bool Server::IsGuiVisible() const {
    if (ceGuiSystem->getGUISheet() != 0) {
        return ceGuiSystem->getGUISheet()->isVisible();
    }
    return false;
}

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
