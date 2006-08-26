#ifndef N_GUICHECKBUTTON_H
#define N_GUICHECKBUTTON_H
//------------------------------------------------------------------------------
/**
    @class nGuiCheckButton
    @ingroup Gui
    @brief An on/off check button widget.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiCheckButton : public nGuiTextLabel
{
public:
    /// constructor
    nGuiCheckButton();
    /// destructor
    virtual ~nGuiCheckButton();
    /// Rendering.
    virtual bool Render();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// set button state
    void SetState(bool b);
    /// get button state
    bool GetState() const;
    /// Is the checkbutton part of a checkbuttongroup?
    bool IsPartOfGroup() const;
    /// Set if the checkbutton is part of a checkbuttongroup
    void SetIsPartOfGroup(bool b);

private:
    /// get the mouse over flag
    bool GetMouseOver() const;
    /// set the mouse over flag
    void SetMouseOver(bool b);

    friend class nGuiCheckButtonGroup;
    friend class nGuiCheckButtonGroup2;

    bool mouseOver;
    bool pressed;
    bool partOfGroup;
};

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButton::SetIsPartOfGroup(bool b)
{
    this->partOfGroup = b;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiCheckButton::IsPartOfGroup() const
{
    return this->partOfGroup;
}


//------------------------------------------------------------------------------
#endif



