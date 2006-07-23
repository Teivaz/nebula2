//------------------------------------------------------------------------------
//  nguigraphicsbrowserwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguigraphicsbrowserwindow.h"
#include "kernel/ntimeserver.h"
#include "tools/nnodelist.h"

nNebulaClass(nGuiGraphicsBrowserWindow, "gui::nguiclientwindow");

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

    // create addScene button
    nGuiTextButton* addButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "addButton");
    n_assert(addButton);
    addButton->SetText("Append");
    addButton->SetFont("GuiSmall");
    addButton->SetAlignment(nGuiTextButton::Center);
    addButton->SetDefaultBrush("button_n");
    addButton->SetPressedBrush("button_p");
    addButton->SetHighlightBrush("button_h");
    addButton->SetMinSize(buttonSize);
    addButton->SetMaxSize(buttonSize);
    addButton->SetColor(skin->GetButtonTextColor());
    layout->AttachWidget(addButton, nGuiFormLayout::Right, loadButton, 0.005f);
    layout->AttachForm(addButton, nGuiFormLayout::Bottom, 0.005f);
    this->refAddButton = addButton;

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
    layout->AttachWidget(timeButton, nGuiFormLayout::Right, addButton, 0.005f);
    layout->AttachForm(timeButton, nGuiFormLayout::Bottom, 0.005f);
    this->refTimeButton = timeButton;

    // create category browser
    nGuiCategoryBrowser* catBrowser = (nGuiCategoryBrowser*) kernelServer->New("nguicategorybrowser", "CatBrowser");
    n_assert(catBrowser);
    catBrowser->SetDirectory("proj:export/gfxlib");
    catBrowser->SetLookUpEnabled(true);
    layout->AttachForm(catBrowser, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(catBrowser, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(catBrowser, nGuiFormLayout::Right, 0.005f);
    layout->AttachWidget(catBrowser, nGuiFormLayout::Bottom, loadButton, 0.005f);
    layout->AttachWidget(catBrowser, nGuiFormLayout::Bottom, addButton, 0.005f);
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
    else if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refAddButton))
    {
        // append button pressed?
        nString objPath = this->refCatBrowser->GetSelectedPath();
        if (!objPath.IsEmpty())
        {
            this->AddObject(objPath);
        }
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refTimeButton))
    {
        // reset time
        nTimeServer::Instance()->ResetTime();
    }
    else if ((event.GetType() == nGuiEvent::SelectionDblClicked) && (event.GetWidget() == this->refCatBrowser))
    {
        // load object on doubleclick on selectiontext
        nString objPath = this->refCatBrowser->GetSelectedPath();
        if (!objPath.IsEmpty())
        {
            this->LoadObject(objPath);
        }
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

    nNodeList* nodeList = nNodeList::Instance();
    n_assert( nodeList != 0 );

    nodeList->Clear();
    nodeList->AddDefaultEntry();
    nodeList->LoadObject(objPath);

    // reset time
    nTimeServer::Instance()->ResetTime();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiGraphicsBrowserWindow::AddObject(const nString& objPath)
{
    n_assert(!objPath.IsEmpty());

    n_assert( nNodeList::Instance() != 0 );

    // Load Object in /usr/scene
    nNodeList::Instance()->LoadObject(objPath);

    // reset time
    nTimeServer::Instance()->ResetTime();
}
