#ifndef N_GUITEXTBUTTON_H
#define N_GUITEXTBUTTON_H
//------------------------------------------------------------------------------
/**
	@class nGuiTextButton
    @ingroup NebulaGuiSystem

    A gui button which contains a label text.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiTextButton : public nGuiTextLabel
{
public:
    /// constructor
    nGuiTextButton();
    /// destructor
    virtual ~nGuiTextButton();
    /// Rendering.
    virtual bool Render();
    /// handle mouse moved event
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);

protected:
    bool focus;
    bool pressed;
};

//------------------------------------------------------------------------------
#endif

