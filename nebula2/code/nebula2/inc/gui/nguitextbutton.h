#ifndef N_GUITEXTBUTTON_H
#define N_GUITEXTBUTTON_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextButton
    @ingroup Gui
    @brief A gui button which contains a label text.
    
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
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);
    /// called per frame when parent widget is visible
    virtual void OnFrame();

protected:
    bool focus;
    bool pressed;
};

//------------------------------------------------------------------------------
#endif

