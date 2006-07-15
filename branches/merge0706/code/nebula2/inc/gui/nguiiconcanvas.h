#ifndef N_GUIICONCANVAS_H
#define N_GUIICONCANVAS_H
//------------------------------------------------------------------------------
/**
    @class nGuiIconCanvas
    @ingroup Gui
    @brief A canvas for drag and drop icons (OBSOLETE?).
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

class nGuiIcon;

//------------------------------------------------------------------------------
class nGuiIconCanvas : public nGuiWidget
{
public:
    /// constructor
    nGuiIconCanvas();
    /// destructor
    virtual ~nGuiIconCanvas();
    /// called on button down over canvas
    virtual bool OnButtonDown(const vector2& mousePos);
    /// a drag action has started within this canvas, return true if drag is valid
    virtual bool OnDragStart(const vector2& mousePos, nGuiIcon* icon);
    /// an icons is currently dragged over this canvas
    virtual void OnDragging(const vector2& mousePos, nGuiIcon* icon);
    /// an icon has been dropped on this canvas
    virtual void OnDropped(const vector2& mousePos, nGuiIcon* icon);
    /// a drag action of an icon belonging to this canvas has been cancelled
    virtual void OnDragCancelled(const vector2& mousePos, nGuiIcon* icon);
    /// return true if mouse position is over a valid drag position
    virtual bool IsValidDropPos(const vector2& mousePos, nGuiIcon* icon);
    /// render the canvas background
    virtual bool Render();
};
//------------------------------------------------------------------------------
#endif

