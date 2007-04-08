//------------------------------------------------------------------------------
//  ceui/server.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_SERVER_H
#define CEUI_SERVER_H

#include "foundation/refcounted.h"
#include "CEGUISystem.h"
#include "CEGUIWindow.h"

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
    /// load GUI scheme
    void LoadScheme(const nString& schemeName);
    /// unload GUI scheme
    void UnloadScheme(const nString& schemeName);
    /// unload all GUI scheme
    void UnloadAllSchemes();
    /// create GUI image set
    void CreateImageSet(const nString& imagesetName, const nString& fileName);
    /// destroy GUI image set
    void DestroyImageSet(const nString& imagesetName);
    /// destroy all GUI image sets
    void DestroyAllImageSets();
    /// create GUI font
    CEGUI::Font* CreateFont(const nString& fontName);
    /// destroy GUI font
    void DestroyFont(const nString& fontName);
    /// destroy all GUI fonts
    void DestroyAllFonts();

#ifdef CreateWindow
#undef CreateWindow
#endif
    /// create window
    CEGUI::Window* CreateWindow(const nString& type, const nString& winName);
    /// find window by name
    CEGUI::Window* GetWindow(const nString& winName);
    /// destroy window by pointer
    void DestroyWindow(CEGUI::Window* window);
    /// destroy window by name
    void DestroyWindow(const nString& winName);
    /// destroy all windows
    void DestroyAllWindows();
    /// load window layout
    CEGUI::Window* LoadWindowLayout(const nString& resName, const nString& prefix = "");
    /// set root window
    void SetRootWindow(CEGUI::Window* window);
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

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
