//------------------------------------------------------------------------------
//  ceui/server.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#include "ceui/server.h"
#include "ceui/renderer.h"
#include "ceui/logger.h"
#include "input/ninputserver.h"
#include "cegui/CEGUILogger.h"
#include "cegui/CEGUIFontManager.h"
#include "cegui/CEGUISchemeManager.h"
#include "cegui/CEGUIWindowManager.h"

namespace CEUI
{
ImplementRtti(CEUI::Server, Foundation::RefCounted);
ImplementFactory(CEUI::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() {
    n_assert(0 == Singleton);
    Singleton = this;
    renderer = 0;
    ceGuiSystem = 0;
    isOpen = false;
    time = 0.0;
    frameTime = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server() {
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool Server::Open() {
    n_new(CEUI::Logger);
    renderer = n_new(CEUI::Renderer);
    ceGuiSystem = n_new(CEGUI::System(renderer));
    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Standard);
    nGfxServer2::Instance()->SetCursorVisibility(nGfxServer2::Custom);
    isOpen = true;
    return isOpen;
}

//------------------------------------------------------------------------------
/**
*/
void Server::Close() {
    n_delete(ceGuiSystem);
    n_delete(renderer);
    isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
static CEGUI::MouseButton Button2CeGuiButton(int button) {
    switch(button) {
    case 0: return CEGUI::LeftButton;
    case 1: return CEGUI::RightButton;
    case 2: return CEGUI::MiddleButton;
    default: return CEGUI::NoButton;
    }
}

//------------------------------------------------------------------------------
/**
*/
static CEGUI::uint Key2CeGuiKey(nKey key) {
    switch(key) {
    case N_KEY_BACK: return CEGUI::Key::Backspace;
    case N_KEY_TAB: return CEGUI::Key::Tab;
    case N_KEY_RETURN: return CEGUI::Key::Return;
    case N_KEY_SHIFT: return CEGUI::Key::LeftShift;
    case N_KEY_CONTROL: return CEGUI::Key::LeftControl;
    case N_KEY_PAUSE: return CEGUI::Key::Pause;
    case N_KEY_CAPITAL: return CEGUI::Key::Capital;
    case N_KEY_ESCAPE: return CEGUI::Key::Escape;
    case N_KEY_SPACE: return CEGUI::Key::Space;
    case N_KEY_END: return CEGUI::Key::End;
    case N_KEY_HOME: return CEGUI::Key::Home;
    case N_KEY_LEFT: return CEGUI::Key::ArrowLeft;
    case N_KEY_UP: return CEGUI::Key::ArrowUp;
    case N_KEY_RIGHT: return CEGUI::Key::ArrowRight;
    case N_KEY_DOWN: return CEGUI::Key::ArrowDown;
    case N_KEY_INSERT: return CEGUI::Key::Insert;
    case N_KEY_DELETE: return CEGUI::Key::Delete;
    case N_KEY_1: return CEGUI::Key::One;
    case N_KEY_2: return CEGUI::Key::Two;
    case N_KEY_3: return CEGUI::Key::Three;
    case N_KEY_4: return CEGUI::Key::Four;
    case N_KEY_5: return CEGUI::Key::Five;
    case N_KEY_6: return CEGUI::Key::Six;
    case N_KEY_7: return CEGUI::Key::Seven;
    case N_KEY_8: return CEGUI::Key::Eight;
    case N_KEY_9: return CEGUI::Key::Nine;
    case N_KEY_0: return CEGUI::Key::Zero;
    case N_KEY_A: return CEGUI::Key::A;
    case N_KEY_B: return CEGUI::Key::B;
    case N_KEY_C: return CEGUI::Key::C;
    case N_KEY_D: return CEGUI::Key::D;
    case N_KEY_E: return CEGUI::Key::E;
    case N_KEY_F: return CEGUI::Key::F;
    case N_KEY_G: return CEGUI::Key::G;
    case N_KEY_H: return CEGUI::Key::H;
    case N_KEY_I: return CEGUI::Key::I;
    case N_KEY_J: return CEGUI::Key::J;
    case N_KEY_K: return CEGUI::Key::K;
    case N_KEY_L: return CEGUI::Key::L;
    case N_KEY_M: return CEGUI::Key::M;
    case N_KEY_N: return CEGUI::Key::N;
    case N_KEY_O: return CEGUI::Key::O;
    case N_KEY_P: return CEGUI::Key::P;
    case N_KEY_Q: return CEGUI::Key::Q;
    case N_KEY_R: return CEGUI::Key::R;
    case N_KEY_S: return CEGUI::Key::S;
    case N_KEY_T: return CEGUI::Key::T;
    case N_KEY_U: return CEGUI::Key::U;
    case N_KEY_V: return CEGUI::Key::V;
    case N_KEY_W: return CEGUI::Key::W;
    case N_KEY_X: return CEGUI::Key::X;
    case N_KEY_Y: return CEGUI::Key::Y;
    case N_KEY_Z: return CEGUI::Key::Z;
    case N_KEY_LWIN: return CEGUI::Key::LeftWindows;
    case N_KEY_RWIN: return CEGUI::Key::RightWindows;
    case N_KEY_APPS: return CEGUI::Key::AppMenu;
    case N_KEY_NUMPAD0: return CEGUI::Key::Numpad0;
    case N_KEY_NUMPAD1: return CEGUI::Key::Numpad1;
    case N_KEY_NUMPAD2: return CEGUI::Key::Numpad2;
    case N_KEY_NUMPAD3: return CEGUI::Key::Numpad3;
    case N_KEY_NUMPAD4: return CEGUI::Key::Numpad4;
    case N_KEY_NUMPAD5: return CEGUI::Key::Numpad5;
    case N_KEY_NUMPAD6: return CEGUI::Key::Numpad6;
    case N_KEY_NUMPAD7: return CEGUI::Key::Numpad7;
    case N_KEY_NUMPAD8: return CEGUI::Key::Numpad8;
    case N_KEY_NUMPAD9: return CEGUI::Key::Numpad9;
    case N_KEY_MULTIPLY: return CEGUI::Key::Multiply;
    case N_KEY_ADD: return CEGUI::Key::Add;
    case N_KEY_SUBTRACT: return CEGUI::Key::Subtract;
    case N_KEY_DECIMAL: return CEGUI::Key::Decimal;
    case N_KEY_DIVIDE: return CEGUI::Key::Divide;
    case N_KEY_F1: return CEGUI::Key::F1;
    case N_KEY_F2: return CEGUI::Key::F2;
    case N_KEY_F3: return CEGUI::Key::F3;
    case N_KEY_F4: return CEGUI::Key::F4;
    case N_KEY_F5: return CEGUI::Key::F5;
    case N_KEY_F6: return CEGUI::Key::F6;
    case N_KEY_F7: return CEGUI::Key::F7;
    case N_KEY_F8: return CEGUI::Key::F8;
    case N_KEY_F9: return CEGUI::Key::F9;
    case N_KEY_F10: return CEGUI::Key::F10;
    case N_KEY_F11: return CEGUI::Key::F11;
    case N_KEY_F12: return CEGUI::Key::F12;
    case N_KEY_F13: return CEGUI::Key::F13;
    case N_KEY_F14: return CEGUI::Key::F14;
    case N_KEY_F15: return CEGUI::Key::F15;
    case N_KEY_NUMLOCK: return CEGUI::Key::NumLock;
    case N_KEY_SCROLL: return CEGUI::Key::ScrollLock;
    case N_KEY_SEMICOLON: return CEGUI::Key::Semicolon;
    case N_KEY_SLASH: return CEGUI::Key::Slash;
    case N_KEY_TILDE: return CEGUI::Key::Grave;
    case N_KEY_LEFTBRACKET: return CEGUI::Key::LeftBracket;
    case N_KEY_BACKSLASH: return CEGUI::Key::Backslash;
    case N_KEY_RIGHTBRACKET: return CEGUI::Key::RightBracket;
    case N_KEY_QUOTE: return CEGUI::Key::Apostrophe;
    case N_KEY_COMMA: return CEGUI::Key::Comma;
    case N_KEY_UNDERBAR: return CEGUI::Key::Underline;
    case N_KEY_PERIOD: return CEGUI::Key::Period;
    case N_KEY_EQUALITY: return CEGUI::Key::Equals;
    default: return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the CEUI server. This distributes input to the current canvas
*/
void Server::Trigger() {
    // inject time pulse
    ceGuiSystem->injectTimePulse((float)frameTime);
    // inject input
    for(nInputEvent* inputEvent = nInputServer::Instance()->FirstEvent(); inputEvent != NULL; inputEvent = nInputServer::Instance()->NextEvent(inputEvent)) {
        switch(inputEvent->GetDeviceId()) {
        case N_INPUT_MOUSE(0):
            switch(inputEvent->GetType()) {
            case N_INPUT_MOUSE_MOVE:
                ceGuiSystem->injectMousePosition((float)inputEvent->GetAbsXPos(), (float)inputEvent->GetAbsYPos());
                break;
            case N_INPUT_BUTTON_DOWN:
                ceGuiSystem->injectMouseButtonDown(Button2CeGuiButton(inputEvent->GetButton()));
                break;
            case N_INPUT_BUTTON_UP:
                ceGuiSystem->injectMouseButtonUp(Button2CeGuiButton(inputEvent->GetButton()));
                break;
            }
            break;
        case N_INPUT_KEYBOARD(0):
            switch(inputEvent->GetType()) {
            case N_INPUT_KEY_DOWN:
                ceGuiSystem->injectKeyDown(Key2CeGuiKey(inputEvent->GetKey()));
                break;
            case N_INPUT_KEY_UP:
                ceGuiSystem->injectKeyUp(Key2CeGuiKey(inputEvent->GetKey()));
                break;
            case N_INPUT_KEY_CHAR:
                ceGuiSystem->injectChar(inputEvent->GetKey());
                break;
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void Server::Render() {
    nGfxServer2::Instance()->Clear(nGfxServer2::DepthBuffer, 0, 0, 0, 0, 1, 0);
    ceGuiSystem->renderGUI();
}

//------------------------------------------------------------------------------
/**
    create GUI font
*/
void Server::CreateFont(const nString& fontName) {
    CEGUI::FontManager::getSingleton().createFont(fontName.Get());
}

//------------------------------------------------------------------------------
/**
    destroy GUI font
*/
void Server::DestroyFont(const nString& fontName) {
    CEGUI::FontManager::getSingleton().destroyFont(fontName.Get());
}

//------------------------------------------------------------------------------
/**
    destroy all GUI fonts
*/
void Server::DestroyAllFonts() {
    CEGUI::FontManager::getSingleton().destroyAllFonts();
}

//------------------------------------------------------------------------------
/**
    load GUI scheme
*/
void Server::LoadScheme(const nString& schemeName) {
    CEGUI::SchemeManager::getSingleton().loadScheme(schemeName.Get());
}

//------------------------------------------------------------------------------
/**
    unload GUI scheme
*/
void Server::UnloadScheme(const nString& schemeName) {
    CEGUI::SchemeManager::getSingleton().unloadScheme(schemeName.Get());
}

//------------------------------------------------------------------------------
/**
    unload all GUI schemes
*/
void Server::UnloadAllSchemes() {
    CEGUI::SchemeManager::getSingleton().unloadAllSchemes();
}

//------------------------------------------------------------------------------
/**
    create window
*/
CEGUI::Window* Server::CreateWindow(const nString& type, const nString& winName) {
    return CEGUI::WindowManager::getSingleton().createWindow(type.Get(), winName.Get());
}

//------------------------------------------------------------------------------
/**
    find window by name
*/
CEGUI::Window* Server::GetWindow(const nString& winName) {
    return CEGUI::WindowManager::getSingleton().getWindow(winName.Get());
}

//------------------------------------------------------------------------------
/**
    destroy window
*/
void Server::DestroyWindow(CEGUI::Window* window) {
    CEGUI::WindowManager::getSingleton().destroyWindow(window);
}

//------------------------------------------------------------------------------
/**
    destroy all windows
*/
void Server::DestroyAllWindows() {
    CEGUI::WindowManager::getSingleton().destroyAllWindows();
}

//------------------------------------------------------------------------------
/**
    load window layout from XML and display GUI
*/
void Server::LoadWindowLayout(const nString& resName) {
    CEGUI::WindowManager::getSingleton().loadWindowLayout(resName.Get());
}

//------------------------------------------------------------------------------
/**
    set default mouse cursor
*/
void Server::SetDefaultMouseCursor(const nString& schemeName, const nString& cursorName) {
    ceGuiSystem->setDefaultMouseCursor(schemeName.Get(), cursorName.Get());
}

//-----------------------------------------------------------------------------
/**
    check if the mouse is currently over an GUI element.
*/
bool Server::IsMouseOverGui() const {
    return ceGuiSystem->getWindowContainingMouse() != ceGuiSystem->getGUISheet();
}

} // namespace CEUI
