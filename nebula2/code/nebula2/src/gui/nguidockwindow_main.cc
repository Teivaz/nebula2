//------------------------------------------------------------------------------
//  nguidockwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidockwindow.h"
#include "gui/nguibutton.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiDockWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiDockWindow::nGuiDockWindow() :
    refScriptServer("/sys/servers/script")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiDockWindow::~nGuiDockWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDockWindow::OnShow()
{
    // update window flags
    this->SetMovable(true);
    this->SetResizable(false);
    this->SetCloseButton(false);

    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    this->SetTitle("Nebula Dock");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // horizontal size of 1 button
    float btnSize = 1.0f / 6;

    // command console button
    nGuiButton* btn;
    btn = (nGuiButton*) kernelServer->New("nguibutton", "ConsoleButton");
    n_assert(btn);
    btn->SetDefaultBrush("terminal_n");
    btn->SetPressedBrush("terminal_p");
    btn->SetHighlightBrush("terminal_h");
    btn->SetTooltip("Command Console");
    layout->AttachForm(btn, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Right, 1 * btnSize);
    btn->OnShow();
    this->refConsoleButton = btn;

    // texture browser button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "TexBrowserButton");
    n_assert(btn);
    btn->SetDefaultBrush("settings_n");
    btn->SetPressedBrush("settings_p");
    btn->SetHighlightBrush("settings_h");
    btn->SetTooltip("Texture Browser");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Left, 1 * btnSize);
    layout->AttachPos(btn, nGuiFormLayout::Right, 2 * btnSize);
    btn->OnShow();
    this->refTexBrowserButton = btn;

    // watcher window button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "WatcherButton");
    n_assert(btn);
    btn->SetDefaultBrush("computer_n");
    btn->SetPressedBrush("computer_p");
    btn->SetHighlightBrush("computer_h");
    btn->SetTooltip("Debug Watchers");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Left, 2 * btnSize);
    layout->AttachPos(btn, nGuiFormLayout::Right, 3 * btnSize);
    btn->OnShow();
    this->refWatcherButton = btn;

    // system info button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SysInfoButton");
    n_assert(btn);
    btn->SetDefaultBrush("info_n");
    btn->SetPressedBrush("info_p");
    btn->SetHighlightBrush("info_h");
    btn->SetTooltip("System Info");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Left, 3 * btnSize);
    layout->AttachPos(btn, nGuiFormLayout::Right, 4 * btnSize);
    btn->OnShow();
    this->refSysInfoButton = btn;

    // hide button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "HideButton");
    n_assert(btn);
    btn->SetDefaultBrush("eject_n");
    btn->SetPressedBrush("eject_p");
    btn->SetHighlightBrush("eject_h");
    btn->SetTooltip("Hide System GUI");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Left, 4 * btnSize);
    layout->AttachPos(btn, nGuiFormLayout::Right, 5 * btnSize);
    btn->OnShow();
    this->refHideButton = btn;

    // quit button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "QuitButton");
    n_assert(btn);
    btn->SetDefaultBrush("quit_n");
    btn->SetPressedBrush("quit_p");
    btn->SetHighlightBrush("quit_h");
    btn->SetTooltip("Quit Application");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachPos(btn, nGuiFormLayout::Left, 5 * btnSize);
    layout->AttachForm(btn, nGuiFormLayout::Right, 0.0f);
    btn->OnShow();
    this->refQuitButton = btn;

    kernelServer->PopCwd();

    // set window position and size
    rectangle rect;
    const float width  = 0.4f;     // width is 40% of screen size
    const float height = 0.1f;     // height is 10% of screen size
    rect.v0.set(0.5f - (width * 0.5f), 1.0f - height);
    rect.v1.set(0.5f + (width * 0.5f), 1.0f);
    this->SetRect(rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDockWindow::OnHide()
{
    if (this->refConsoleButton.isvalid())
    {
        this->refConsoleButton->Release();
    }
    if (this->refTexBrowserButton.isvalid())
    {
        this->refTexBrowserButton->Release();
    }
    if (this->refWatcherButton.isvalid())
    {
        this->refWatcherButton->Release();
    }
    if (this->refSysInfoButton.isvalid())
    {
        this->refSysInfoButton->Release();
    }
    if (this->refHideButton.isvalid())
    {
        this->refHideButton->Release();
    }
    if (this->refQuitButton.isvalid())
    {
        this->refQuitButton->Release();
    }

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiDockWindow::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (event.GetWidget() == this->refConsoleButton.get())
        {
            // open a new console window
            this->refGuiServer->NewWindow("nguiconsolewindow", true);
        }
        else if (event.GetWidget() == this->refTexBrowserButton.get())
        {
            // open a texture browser window
            this->refGuiServer->NewWindow("nguitexbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refWatcherButton.get())
        {
            // open debug watcher window
            this->refGuiServer->NewWindow("nguiwatcherwindow", true);
        }
        else if (event.GetWidget() == this->refSysInfoButton.get())
        {
            // open system info window
            this->refGuiServer->NewWindow("nguisysteminfowindow", true);
        }
        else if (event.GetWidget() == this->refHideButton.get())
        {
            // hide the system gui
            this->refGuiServer->ToggleSystemGui();
        }
        else if (event.GetWidget() == this->refQuitButton.get())
        {
            // quit application
            this->refScriptServer->SetQuitRequested(true);
        }
    }

    nGuiClientWindow::OnEvent(event);
}
