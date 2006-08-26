#ifndef N_GUILABEL_H
#define N_GUILABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiLabel
    @ingroup Gui
    @brief A widget which is a graphical label.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiLabel : public nGuiWidget
{
public:
    /// constructor
    nGuiLabel();
    /// destructor
    virtual ~nGuiLabel();
    /// render the widget
    virtual bool Render();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);

private:
    bool mouseOver;
};
//------------------------------------------------------------------------------
#endif
