//------------------------------------------------------------------------------
//  nguitabbedlayout_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitabbedlayout.h"
#include "gui/nguicheckbutton.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTabbedLayout, "gui::nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiTabbedLayout::nGuiTabbedLayout() :
    tabs(0, 0),
    activeTab(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTabbedLayout::~nGuiTabbedLayout()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Releases all objects associated with the current tabs.
*/
void
nGuiTabbedLayout::ClearTabs()
{
    int i;
    for (i = 0; i < this->tabs.Size(); i++)
    {
        Tab& tab = this->tabs[i];
        if (tab.refTabButton.isvalid())
        {
            tab.refTabButton->Release();
            n_assert(!tab.refTabButton.isvalid());
        }
        if (tab.refChildLayout.isvalid())
        {
            tab.refChildLayout->Release();
            n_assert(!tab.refChildLayout.isvalid());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Defines the number of tabs. This will immediately create the required
    child form layouts and child buttons.
*/
void
nGuiTabbedLayout::SetNumTabs(int numTabs)
{
    n_assert(numTabs > 0);

    this->ClearTabs();

    kernelServer->PushCwd(this);
    this->tabs.SetFixedSize(numTabs);
    int i;
    for (i = 0; i < numTabs; i++)
    {
        Tab& tab = this->tabs[i];
        nString buttonName = "TabButton";
        buttonName.AppendInt(i);
        nString formName = "ChildForm";
        formName.AppendInt(i);
        tab.refTabButton = (nGuiCheckButton*) kernelServer->New("nguicheckbutton", buttonName.Get());
        tab.refChildLayout = (nGuiFormLayout*) kernelServer->New("nguiformlayout", formName.Get());
    }
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTabbedLayout::OnShow()
{
    n_assert(this->GetNumTabs() > 0);

    nGuiFormLayout::OnShow();

    // compute size of tab buttons
    vector2 tabButtonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("tabbutton_n");
    vector2 layoutMinSize(this->GetNumTabs() *  tabButtonSize.x, tabButtonSize.y);

    // initialize the tab buttons
    nGuiCheckButton* prevTabButton = 0;
    int tabIndex;
    for (tabIndex = 0; tabIndex < this->GetNumTabs(); tabIndex++)
    {
        const Tab& tab = this->tabs[tabIndex];

        // initialize tab button
        nGuiCheckButton* button = tab.refTabButton.get();
        button->SetDefaultBrush("tabbutton_n");
        button->SetHighlightBrush("tabbutton_h");
        button->SetPressedBrush("tabbutton_p");
        button->SetMinSize(tabButtonSize);
        button->SetMaxSize(tabButtonSize);

        this->AttachForm(button, nGuiFormLayout::Top, 0.0f);
        if (prevTabButton)
        {
            this->AttachWidget(button, nGuiFormLayout::Left, prevTabButton, 0.0f);
        }
        else
        {
            this->AttachForm(button, nGuiFormLayout::Left, 0.0f);
        }
        prevTabButton = button;

        // initialize child form layout
        nGuiFormLayout* layout = tab.refChildLayout.get();
        layout->SetMinSize(layoutMinSize);
        this->AttachForm(layout, nGuiFormLayout::Left, 0.0f);
        this->AttachForm(layout, nGuiFormLayout::Right, 0.0f);
        this->AttachForm(layout, nGuiFormLayout::Bottom, 0.0f);
        this->AttachWidget(layout, nGuiFormLayout::Top, button, 0.0f);
    }

    // update active/inactive panel status
    this->UpdateActivePanel();

    // register as event listener on GUI server
    nGuiServer::Instance()->RegisterEventListener(this);

    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTabbedLayout::OnHide()
{
    // unregister as event listener
    nGuiServer::Instance()->UnregisterEventListener(this);

    // clear layout rules
    this->ClearAttachRules();

    // up to parent class
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTabbedLayout::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::Action)
    {
        // check if tab buttons have been pressed
        int tabIndex = 0;
        for (tabIndex = 0; tabIndex < this->GetNumTabs(); tabIndex++)
        {
            if (this->tabs[tabIndex].refTabButton.get() == event.GetWidget())
            {
                this->SetActiveTab(tabIndex);
                this->UpdateActivePanel();
            }
        }
    }

    // call parent class
    nGuiFormLayout::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTabbedLayout::UpdateActivePanel()
{
    // hide everything
    int tabIndex;
    for (tabIndex = 0; tabIndex < this->GetNumTabs(); tabIndex++)
    {
        this->tabs[tabIndex].refTabButton->SetState(false);
        this->tabs[tabIndex].refChildLayout->Hide();
    }

    // activate active tab
    this->tabs[this->activeTab].refTabButton->SetState(true);
    this->tabs[this->activeTab].refChildLayout->Show();

    // update tabbed layout
    this->UpdateLayout(this->GetRect());
}
