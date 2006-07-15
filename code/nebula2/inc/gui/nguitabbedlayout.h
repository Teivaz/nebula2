#ifndef N_GUITABBEDLAYOUT_H
#define N_GUITABBEDLAYOUT_H
//------------------------------------------------------------------------------
/**
    @class nGuiTabbedLayout
    @ingroup Gui

    @brief A tabbed layout is a form layout which embeds several child form layouts
    which can be flipped through a row of tab buttons at the top.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"

class nGuiCheckButton;

//------------------------------------------------------------------------------
class nGuiTabbedLayout : public nGuiFormLayout
{
public:
    /// constructor
    nGuiTabbedLayout();
    /// destructor
    virtual ~nGuiTabbedLayout();
    /// set number of tabs
    virtual void SetNumTabs(int n);
    /// get number of tabs
    int GetNumTabs() const;
    /// set the currently active tab
    void SetActiveTab(int i);
    /// get the currently active tab
    int GetActiveTab() const;
    /// get pointer to tab button at tab index
    nGuiCheckButton* GetTabButtonAt(int tabIndex) const;
    /// get pointer to child form layout at tab index
    nGuiFormLayout* GetChildLayoutAt(int tabIndex) const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

protected:
    struct Tab
    {
        nRef<nGuiCheckButton> refTabButton;
        nRef<nGuiFormLayout> refChildLayout;
    };

    /// make active tab visible, inactive tabs invisible
    void UpdateActivePanel();

    nArray<Tab> tabs;

private:
    /// release existing tab objects
    void ClearTabs();

    int activeTab;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiTabbedLayout::GetNumTabs() const
{
    return this->tabs.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTabbedLayout::SetActiveTab(int i)
{
    this->activeTab = i;
    this->UpdateActivePanel();
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiTabbedLayout::GetActiveTab() const
{
    return this->activeTab;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCheckButton*
nGuiTabbedLayout::GetTabButtonAt(int tabIndex) const
{
    return this->tabs[tabIndex].refTabButton.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiFormLayout*
nGuiTabbedLayout::GetChildLayoutAt(int tabIndex) const
{
    return this->tabs[tabIndex].refChildLayout.get();
}

//------------------------------------------------------------------------------
#endif

