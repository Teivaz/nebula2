//------------------------------------------------------------------------------
//  nguigraphicsbrowserwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguigraphicsbrowserwindow.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nGuiGraphicsBrowserWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiGraphicsBrowserWindow::nGuiGraphicsBrowserWindow() :
    refScriptServer("/sys/servers/script"),
    refUsrScene("/usr/scene")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiGraphicsBrowserWindow::~nGuiGraphicsBrowserWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiGraphicsBrowserWindow::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    this->SetTitle("Graphics Object Browser");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // create load button
    nGuiTextButton* loadButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "LoadButton");
    n_assert(loadButton);
    loadButton->SetText("Load");
    loadButton->SetFont("GuiSmall");
    loadButton->SetAlignment(nGuiTextButton::Center);
    loadButton->SetDefaultBrush("button_n");
    loadButton->SetPressedBrush("button_p");
    loadButton->SetHighlightBrush("button_h");
    loadButton->SetMinSize(buttonSize);
    loadButton->SetMaxSize(buttonSize);
    loadButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(loadButton, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(loadButton, nGuiFormLayout::Bottom, 0.005f);
    this->refLoadButton = loadButton;

    // create reset time button
    nGuiTextButton* timeButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "TimeButton");
    n_assert(timeButton);
    timeButton->SetText("Reset Time");
    timeButton->SetFont("GuiSmall");
    timeButton->SetAlignment(nGuiTextButton::Center);
    timeButton->SetDefaultBrush("button_n");
    timeButton->SetPressedBrush("button_p");
    timeButton->SetHighlightBrush("button_h");
    timeButton->SetMinSize(buttonSize);
    timeButton->SetMaxSize(buttonSize);
    timeButton->SetColor(skin->GetButtonTextColor());
    layout->AttachWidget(timeButton, nGuiFormLayout::Right, loadButton, 0.005f);
    layout->AttachForm(timeButton, nGuiFormLayout::Bottom, 0.005f);
    this->refTimeButton = timeButton;

    // create category browser
    nGuiCategoryBrowser* catBrowser = (nGuiCategoryBrowser*) kernelServer->New("nguicategorybrowser", "CatBrowser");
    n_assert(catBrowser);
    catBrowser->SetDirectory("proj:export/gfxlib");
    layout->AttachForm(catBrowser, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(catBrowser, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(catBrowser, nGuiFormLayout::Right, 0.005f);
    layout->AttachWidget(catBrowser, nGuiFormLayout::Bottom, loadButton, 0.005f);
    this->refCatBrowser = catBrowser;

    // need to invoke on show manually on layout
    layout->OnShow();

    kernelServer->PopCwd();

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiGraphicsBrowserWindow::OnHide()
{
    this->refLoadButton->Release();
    this->refTimeButton->Release();
    this->refCatBrowser->Release();
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiGraphicsBrowserWindow::OnEvent(const nGuiEvent& event)
{
    if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refLoadButton))
    {
        // load button pressed?
        nString objPath = this->refCatBrowser->GetSelectedPath();
        if (!objPath.IsEmpty())
        {
            this->LoadObject(objPath);
        }
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refTimeButton))
    {
        // reset time
        nTimeServer::Instance()->ResetTime();
    }

    // hand to parent class
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiGraphicsBrowserWindow::LoadObject(const nString& objPath)
{
    n_assert(!objPath.IsEmpty());

    // clear usr/scene
    this->refUsrScene->Release();
    kernelServer->New("ntransformnode", "/usr/scene");

    // load new object
    kernelServer->PushCwd(this->refUsrScene);
    // source the light stage...
    kernelServer->Load("home:export/gfxlib/stdlight.n2");
    kernelServer->Load(objPath.Get());
    kernelServer->PopCwd();

    // Set the new window title
    nString title = objPath;
    title.Append(" - Nebula2 Viewer");
    nGfxServer2::Instance()->SetWindowTitle(title.Get());

    // reset time
    nTimeServer::Instance()->ResetTime();
}
