#ifndef N_GUICHECKBUTTON_H
#define N_GUICHECKBUTTON_H
//------------------------------------------------------------------------------
/**
    An on/off check button widget.

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

private:
    /// get the mouse over flag
    bool GetMouseOver() const;

    friend class nGuiCheckButtonGroup;

    bool mouseOver;
    bool pressed;
};
//------------------------------------------------------------------------------
#endif



