#ifndef N_GUIBUTTON_H
#define N_GUIBUTTON_H
//-----------------------------------------------------------------------------
/**
    @class nGuiButton
    @ingroup NebulaGuiSystem
    @brief A GUI button class.

    (C) 2001 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//-----------------------------------------------------------------------------
class nGuiButton : public nGuiWidget
{
public:
    /// constructor
    nGuiButton();
    /// destructor
    virtual ~nGuiButton();
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
//-----------------------------------------------------------------------------
#endif

