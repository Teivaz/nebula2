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
    this->SetMovable(false);
    this->SetResizable(false);
    this->SetCloseButton(false);
    this->SetTitleBar(false);

    // call parent class
    nGuiClientWindow::OnShow();

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // horizontal size of 1 button
    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("console_n");

    // command console button
    nGuiButton* btn;
    btn = (nGuiButton*) kernelServer->New("nguibutton", "ConsoleButton");
    n_assert(btn);
    btn->SetDefaultBrush("console_n");
    btn->SetPressedBrush("console_p");
    btn->SetHighlightBrush("console_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Command Console");
    layout->AttachForm(btn, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    btn->OnShow();
    this->refConsoleButton = btn;

    // texture browser button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "TexBrowserButton");
    n_assert(btn);
    btn->SetDefaultBrush("texbrowser_n");
    btn->SetPressedBrush("texbrowser_p");
    btn->SetHighlightBrush("texbrowser_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Texture Browser");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refConsoleButton, 0.0f);
    btn->OnShow();
    this->refTexBrowserButton = btn;

    // gfx browser button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "GfxBrowserButton");
    n_assert(btn);
    btn->SetDefaultBrush("gfxbrowser_n");
    btn->SetPressedBrush("gfxbrowser_p");
    btn->SetHighlightBrush("gfxbrowser_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Graphics Browser");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refTexBrowserButton, 0.0f);
    btn->OnShow();
    this->refGfxBrowserButton = btn;

    // Hardpoint Object Attach button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "HardpointButton");
    n_assert(btn);
    btn->SetDefaultBrush("hardpoint_n");
    btn->SetPressedBrush("hardpoint_p");
    btn->SetHighlightBrush("hardpoint_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Attach objects to Hardpoints");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refGfxBrowserButton, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Right, 0.0f);
    btn->OnShow();
    this->refHardpointButton = btn;

    // scene control button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SceneControlButton");
    n_assert(btn);
    btn->SetDefaultBrush("contrwindow_n");
    btn->SetPressedBrush("contrwindow_p");
    btn->SetHighlightBrush("contrwindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Scene Control");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refHardpointButton, 0.0f);
    btn->OnShow();
    this->refSceneControlButton = btn;

    // watcher window button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "WatcherButton");
    n_assert(btn);
    btn->SetDefaultBrush("dbgwindow_n");
    btn->SetPressedBrush("dbgwindow_p");
    btn->SetHighlightBrush("dbgwindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Debug Watchers");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSceneControlButton, 0.0f);
    btn->OnShow();
    this->refWatcherButton = btn;

    // system info button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SysInfoButton");
    n_assert(btn);
    btn->SetDefaultBrush("syswindow_n");
    btn->SetPressedBrush("syswindow_p");
    btn->SetHighlightBrush("syswindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("System Info");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refWatcherButton, 0.0f);
    btn->OnShow();
    this->refSysInfoButton = btn;

    // adjust display button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "AdjustDisplayButton");
    n_assert(btn);
    btn->SetDefaultBrush("disp_n");
    btn->SetPressedBrush("disp_p");
    btn->SetHighlightBrush("disp_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Adjust Display");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSysInfoButton, 0.0f);
    btn->OnShow();
    this->refAdjustButton = btn;

    // hide button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "HideButton");
    n_assert(btn);
    btn->SetDefaultBrush("hidegui_n");
    btn->SetPressedBrush("hidegui_p");
    btn->SetHighlightBrush("hidegui_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Hide System GUI");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refAdjustButton, 0.0f);
    btn->OnShow();
    this->refHideButton = btn;

    // character control button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "CharacterControlButton");
    n_assert(btn);
    btn->SetDefaultBrush("charwindow_n");
    btn->SetPressedBrush("charwindow_p");
    btn->SetHighlightBrush("charwindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Control Characters");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refHideButton, 0.0f);
    btn->OnShow();
    this->refCharacterControlButton = btn;

    // save and load button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SaveLoadButton");
    n_assert(btn);
    btn->SetDefaultBrush("savewindow_n");
    btn->SetPressedBrush("savewindow_p");
    btn->SetHighlightBrush("savewindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Load and save settings");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refCharacterControlButton, 0.0f);
    btn->OnShow();
    this->refSettingsManagementButton = btn;

    // quit button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "QuitButton");
    n_assert(btn);
    btn->SetDefaultBrush("quit_n");
    btn->SetPressedBrush("quit_p");
    btn->SetHighlightBrush("quit_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Quit Application");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSettingsManagementButton, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Right, 0.0f);
    btn->OnShow();
    this->refQuitButton = btn;

    kernelServer->PopCwd();

    // set window position and size
    rectangle rect;
    const float width  = 12 * btnSize.x;
    const float height = btnSize.y;
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
    this->refConsoleButton->Release();
    this->refTexBrowserButton->Release();
    this->refGfxBrowserButton->Release();
    this->refHardpointButton->Release();  
    this->refSceneControlButton->Release();
    this->refWatcherButton->Release();
    this->refSysInfoButton->Release();
    this->refAdjustButton->Release();
    this->refHideButton->Release();
    this->refQuitButton->Release();
    this->refCharacterControlButton->Release();
    this->refSettingsManagementButton->Release();

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
        if (event.GetWidget() == this->refConsoleButton)
        {
            // open a new console window
            nGuiServer::Instance()->NewWindow("nguiconsolewindow", true);
        }
        else if (event.GetWidget() == this->refTexBrowserButton)
        {
            // open a texture browser window
            nGuiServer::Instance()->NewWindow("nguitexbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refGfxBrowserButton)
        {
            // open a graphics browser window
            nGuiServer::Instance()->NewWindow("nguigraphicsbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refHardpointButton)
        {
            // open a hardpoints browser window
            nGuiServer::Instance()->NewWindow("nguihardpointsbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refSceneControlButton)
        {
            // open a graphics browser window - nur mal als Test.
            nGuiServer::Instance()->NewWindow("nguiscenecontrolwindow", true);
        }
        else if (event.GetWidget() == this->refWatcherButton)
        {
            // open debug watcher window
            nGuiServer::Instance()->NewWindow("nguiwatcherwindow", true);
        }
        else if (event.GetWidget() == this->refSysInfoButton)
        {
            // open system info window
            nGuiServer::Instance()->NewWindow("nguisysteminfowindow", true);
        }
        else if (event.GetWidget() == this->refAdjustButton)
        {
            // open system info window
            nGuiServer::Instance()->NewWindow("nguiadjustdisplaywindow", true);
        }
        else if (event.GetWidget() == this->refCharacterControlButton)
        {
            // open character controls window
            nGuiServer::Instance()->NewWindow("nguicharactercontrolwindow", true);
        }
        else if (event.GetWidget() == this->refSettingsManagementButton)
        {
            // open settings management window for loading and saving values
            nGuiServer::Instance()->NewWindow("nguisettingsmanagementwindow", true);
        }
        else if (event.GetWidget() == this->refHideButton)
        {
            // hide the system gui
            nGuiServer::Instance()->ToggleSystemGui();
        }
        else if (event.GetWidget() == this->refQuitButton)
        {
            // quit application
            this->refScriptServer->SetQuitRequested(true);
        }
    }

    nGuiClientWindow::OnEvent(event);
}
