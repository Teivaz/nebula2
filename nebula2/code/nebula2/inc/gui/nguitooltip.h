#ifndef N_GUITOOLTIP_H
#define N_GUITOOLTIP_H
//------------------------------------------------------------------------------
/**
    @class nGuiToolTip
    @ingroup Gui
    @brief A tooltip widget with text which always positions itself
    relative to the mouse.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

class nInputServer;

//------------------------------------------------------------------------------
class nGuiToolTip : public nGuiTextLabel
{
public:
    /// constructor
    nGuiToolTip();
    /// destructor
    virtual ~nGuiToolTip();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// render the widget
    virtual bool Render();
    /// set the text as string
    virtual void SetText(const char* text);
    /// set the font to use (see nGuiServer::AddFont())
    virtual void SetFont(const char* fontName);

private:
    /// compute current rect
    void UpdateRect();
    /// compute current color
    void UpdateColor();

    bool openFirstFrame;
    bool fadeinRequested;
    nTime fadeinRequestTime;
    vector4 windowColor;
    vector2 textSize;
    bool textSizeIsDirty;
};

//------------------------------------------------------------------------------
#endif

