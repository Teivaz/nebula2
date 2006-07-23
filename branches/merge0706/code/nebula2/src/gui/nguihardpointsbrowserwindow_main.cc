//------------------------------------------------------------------------------
//  nguihardpointsbrowserwindow_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihardpointsbrowserwindow.h"
#include "kernel/ntimeserver.h"
#include "tools/nnodelist.h"

nNebulaClass(nGuiHardpointsBrowserWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiHardpointsBrowserWindow::nGuiHardpointsBrowserWindow() :
    refScriptServer("/sys/servers/script"),
    refUsrScene("/usr/scene")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiHardpointsBrowserWindow::~nGuiHardpointsBrowserWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHardpointsBrowserWindow::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    this->SetTitle("Attach Object to Hardpoint - Browser");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // create Attach button
    nGuiTextButton* addButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "addButton");
    n_assert(addButton);
    addButton->SetText("Attach");
    addButton->SetFont("GuiSmall");
    addButton->SetAlignment(nGuiTextButton::Center);
    addButton->SetDefaultBrush("button_n");
    addButton->SetPressedBrush("button_p");
    addButton->SetHighlightBrush("button_h");
    addButton->SetMinSize(buttonSize);
    addButton->SetMaxSize(buttonSize);
    addButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(addButton, nGuiFormLayout::Right, 0.005f);
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
    layout->AttachForm(catBrowser, nGuiFormLayout::Left, 0.150f);
    layout->AttachForm(catBrowser, nGuiFormLayout::Right, 0.005f);
    layout->AttachWidget(catBrowser, nGuiFormLayout::Bottom, addButton, 0.005f);
    this->refCatBrowser = catBrowser;

    // create hardpoints browser
    nGuiHardpointsLister* hardpointsLister = (nGuiHardpointsLister*) kernelServer->New("nguihardpointslister", "HardpointsLister");
    n_assert(hardpointsLister);
    hardpointsLister->SetDefaultBrush("list_background");
    hardpointsLister->SetHighlightBrush("list_selection");
    hardpointsLister->SetFont("GuiSmall");
    hardpointsLister->SetSelectionEnabled(true);
    hardpointsLister->SetLookUpEnabled(true);
    layout->AttachForm(hardpointsLister, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(hardpointsLister, nGuiFormLayout::Left, 0.005f);
    layout->AttachWidget(hardpointsLister, nGuiFormLayout::Right, catBrowser, 0.005f);
    layout->AttachWidget(hardpointsLister, nGuiFormLayout::Bottom, addButton, 0.005f);
    hardpointsLister->OnShow();
    this->refHardpointsLister = hardpointsLister;

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
nGuiHardpointsBrowserWindow::OnHide()
{
    this->refTimeButton->Release();
    this->refCatBrowser->Release();
    if (this->refHardpointsLister.isvalid())
    {
        this->refHardpointsLister->Release();
        n_assert(!this->refHardpointsLister.isvalid());
    }
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHardpointsBrowserWindow::OnEvent(const nGuiEvent& event)
{
    nSkinAnimator*  animator;
    int             jointIndex;
    bool            validJointSelected = false;

    if(this->refHardpointsLister.isvalid())
    {
        if(this->refHardpointsLister->GetSelectedJoint(animator,jointIndex))
            validJointSelected = true;
    };

    if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refAddButton))
    {
        // append button pressed?
        nString objPath = this->refCatBrowser->GetSelectedPath();
        if ((!objPath.IsEmpty()) && (validJointSelected))
        {
            this->AddObject(objPath,jointIndex);
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
        if ((!objPath.IsEmpty()) && (validJointSelected))
        {
            this->AddObject(objPath,jointIndex);
        }
	}

    // hand to parent class
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHardpointsBrowserWindow::AddObject(const nString& objPath,int jointIndex)
{
    n_assert(!objPath.IsEmpty());

    n_assert( nNodeList::Instance() != 0 );

    // Load Object in /usr/scene
    nNodeList::Instance()->LoadObjectAndAttachToHardpoint(objPath,jointIndex);

    // reset time
    nTimeServer::Instance()->ResetTime();
}
