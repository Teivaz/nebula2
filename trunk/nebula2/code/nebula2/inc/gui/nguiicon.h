#ifndef N_GUIICON_H
#define N_GUIICON_H
//------------------------------------------------------------------------------
/**
    @class nGuiIcon
    @ingroup Gui
    @brief A drag and drop icon widget. (OBSOLETE?)

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguilabel.h"

class nGuiIconCanvas;

//------------------------------------------------------------------------------
class nGuiIcon : public nGuiLabel
{
public:
    /// constructor
    nGuiIcon();
    /// destructor
    virtual ~nGuiIcon();
    /// get pointer to canvas this icon is embedded in
    nGuiIconCanvas* GetCanvas() const;
    /// called when dragging this icon starts, return true if dragging is valid
    virtual bool OnDragStart(const vector2& mousePos);
    /// called while this icon is dragging
    virtual void OnDragging(const vector2& mousePos);
    /// called when this icon is dropped
    virtual void OnDropped(const vector2& mousePos);
    /// called when drag action has been cancelled
    virtual void OnDragCancelled(const vector2& mousePos);
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle right button down
    virtual bool OnRButtonDown(const vector2& mousePos);
    /// return true if the icon thinks that this is a valid drop position
    virtual bool IsValidDropPos(const vector2& mousePos);

private:
    bool dragging;
    vector2 dragStartMousePos;
    rectangle dragStartRect;
    nClass* canvasClass;
};

//------------------------------------------------------------------------------
#endif

