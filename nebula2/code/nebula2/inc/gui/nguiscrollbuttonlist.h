#ifndef N_GUISCROLLBUTTONLIST_H
#define N_GUISCROLLBUTTONLIST_H

//------------------------------------------------------------------------------
/**
    List of checkbuttons which can be scrolled.

    (C) 2006 Radon Labs GmbH
*/

#include "gui/nguicheckbuttongroup2.h"
#include "gui/nguibutton.h"

//------------------------------------------------------------------------------
class nGuiScrollButtonList : public nGuiCheckButtonGroup2
{
public:
    /// constructor
    nGuiScrollButtonList();
    /// destructor
    virtual ~nGuiScrollButtonList();
    /// On show
    virtual void OnShow();
    /// On hide
    virtual void OnHide();
    /// on event
    virtual void OnEvent(const nGuiEvent& event);

    /// set alignment
    void SetHorizontalAlignment(bool b);

    /// set forward button default brush
    void SetForwardButtonDefaultBrush(const nString& s);
    /// set forward button pressed brush
    void SetForwardButtonPressedBrush(const nString& s);
    /// set forward button hightlight rush
    void SetForwardButtonHighlightBrush(const nString& s);
    /// set tooltip
    void SetForwardButtonTooltip(const nString& s);

    /// set forward button default brush
    void SetBackwardButtonDefaultBrush(const nString& s);
    /// set Backward button pressed brush
    void SetBackwardButtonPressedBrush(const nString& s);
    /// set Backward button hightlight rush
    void SetBackwardButtonHighlightBrush(const nString& s);
    /// set tooltip
    void SetBackwardButtonTooltip(const nString& s);

    /// set empty button brush
    void SetEmptyButtonBrush(const nString& s);

    /// set number of visible buttons
    void SetNumVisibleBtns(int i);

    /// set foward and backward button directly
    void SetScrollBtns(nGuiButton* fwdBtn, nGuiButton* bwdBtn);

    /// set number of btns which should be scrolled, when pushed a scroll btn
    void SetScrollSize(int i);

    /// get selected check button
    nGuiCheckButton* GetSelectedListButton() const;

    /// disable scroll buttons
    void SetScrollButtonsEnabled(bool b);

    /// set btn offsets
    void SetButtonOffsets(const nArray<float>& offsets);

    /// set btn texts
    void SetButtonTexts(const nArray<nString>& texts);

    /// set btn scale
    void SetButtonScale(const vector2&  scale);

    ///
    virtual void SetSelection(int index);

    ///
    const nArray<nString>& GetButtonTexts() const;


    // deselet all buttons
    void DeselectAll();

    ///
    virtual void UpdateCheckButtons();


private:
    /// update list buttons
    void UpdateVisibleListButtons();

    bool horizontal;
    int visibleButtons;
    int btnIndex;
    int scrollSize;
    bool scrollButtonsEnabled;
    bool allButtonsDeselected;
    vector2 buttonScale;
    nString fwdButtonDefaultBrush;
    nString fwdButtonHighlightBrush;
    nString fwdButtonPressedBrush;
    nString fwdButtonTooltip;

    nString bwdButtonDefaultBrush;
    nString bwdButtonHighlightBrush;
    nString bwdButtonPressedBrush;
    nString bwdButtonTooltip;

    nString emptyButtonBrush;

    nRef<nGuiButton> forwardbutton;
    nRef<nGuiButton> backwardbutton;
    nArray<nRef<nGuiButton>> emptyBtns;
    nArray<float> btnOffsets;
    nArray<nString> btnTexts;
};


//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetScrollBtns(nGuiButton* fwdBtn, nGuiButton* bwdBtn)
{
    n_assert(fwdBtn != 0 &&  bwdBtn != 0);
    this->forwardbutton = fwdBtn;
    this->backwardbutton = bwdBtn;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetHorizontalAlignment(bool b)
{
    this->horizontal = b;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetEmptyButtonBrush(const nString& s)
{
    this->emptyButtonBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetForwardButtonDefaultBrush(const nString& s)
{
    this->fwdButtonDefaultBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetForwardButtonPressedBrush(const nString& s)
{
    this->fwdButtonPressedBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetForwardButtonHighlightBrush(const nString& s)
{
    this->fwdButtonHighlightBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetForwardButtonTooltip(const nString& s)
{
    this->fwdButtonTooltip = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetBackwardButtonDefaultBrush(const nString& s)
{
    this->bwdButtonDefaultBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetBackwardButtonPressedBrush(const nString& s)
{
    this->bwdButtonPressedBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetBackwardButtonHighlightBrush(const nString& s)
{
    this->bwdButtonHighlightBrush = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetBackwardButtonTooltip(const nString& s)
{
    this->bwdButtonTooltip = s;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetNumVisibleBtns(int i)
{
    this->visibleButtons = i;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetScrollSize(int i)
{
    this->scrollSize = i;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetScrollButtonsEnabled(bool b)
{
    this->scrollButtonsEnabled = b;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetButtonOffsets(const nArray<float>& offsets)
{
    this->btnOffsets = offsets;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetButtonTexts(const nArray<nString>& texts)
{
    this->btnTexts = texts;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::SetButtonScale(const vector2& scale)
{
    this->buttonScale = scale;
}

//------------------------------------------------------------------------------
inline
void
nGuiScrollButtonList::DeselectAll()
{
    this->allButtonsDeselected = true;
    UpdateCheckButtons();
}

//------------------------------------------------------------------------------
#endif
