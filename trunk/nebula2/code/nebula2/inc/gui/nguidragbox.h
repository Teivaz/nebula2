#ifndef N_GUIDRAGBOX_H
#define N_GUIDRAGBOX_H
//------------------------------------------------------------------------------
/**
    @class nGuiDragBox
    @ingroup Gui

    @brief Implements a drag box as 2d screen space rectangle.

    While the dragbox widget is enabled, left-clicking and moving the mouse
    will open the drag box. The following events will be generated:

     - @c DragBoxStarted   - a drag box operation has started
     - @c DragBoxFinished  - a drag box operation has been successfully finished
     - @c DragBoxCancelled - drag box operation has been cancelled
     - @c DragBoxUpdated   - the drag box has been updated

    If the drag box is smaller then the size threshold, no drag box will
    be rendered.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiDragBox : public nGuiWidget
{
public:
    /// constructor
    nGuiDragBox();
    /// destructor
    virtual ~nGuiDragBox();
    /// set the drag size threshold in screen space units
    void SetDragThreshold(float s);
    /// get the drag size threshold in screen space units
    float GetDragThreshold() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle right button down
    virtual bool OnRButtonDown(const vector2& mousePos);
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// rendering
    virtual bool Render();
    /// return true if currently dragging
    bool IsDragging() const;
    /// return true if the drag box data is valid
    bool IsDragBoxValid() const;
    /// return the current drag box in screen space
    const rectangle& GetDragBox2D() const;
    /// return true if world space position is inside current drag box
    bool IsWorldCoordInDragBox(const vector3& pos) const;
    /// manually cancel the current dragging operation
    void CancelDragging();

private:
    /// update 2d and 3d drag box data
    void UpdateDragBox(const vector2& mousePos);

    float dragThreshold;
    bool dragging;
    bool dragBoxValid;
    bool dragBoxStartedSent;
    vector2 dragStartMousePos;
    rectangle dragBox2D;
};

//------------------------------------------------------------------------------
/**
    Set the mouse-move distance from the start point until a drag operation
    is valid in screen space. To set the threshold in pixels, compute
    numPixels / displayResolution.
*/
inline
void
nGuiDragBox::SetDragThreshold(float s)
{
    n_assert(s > 0.0f);
    this->dragThreshold = s;
}

//------------------------------------------------------------------------------
/**
    Returns the drag threshold distance.
*/
inline
float
nGuiDragBox::GetDragThreshold() const
{
    return this->dragThreshold;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDragBox::IsDragging() const
{
    return this->dragging;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDragBox::IsDragBoxValid() const
{
    return this->dragBoxValid;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
nGuiDragBox::GetDragBox2D() const
{
    n_assert(this->dragBoxValid);
    return this->dragBox2D;
}

//------------------------------------------------------------------------------
#endif

