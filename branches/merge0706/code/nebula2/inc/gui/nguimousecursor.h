#ifndef N_GUIMOUSECURSOR_H
#define N_GUIMOUSECURSOR_H
//------------------------------------------------------------------------------
/**
    @class nGuiMouseCursor
    @ingroup Gui

    (C) 2005 RadonLabs GmbH
*/
#include "gui/nguilabel.h"

class nInputServer;


//------------------------------------------------------------------------------
class nGuiMouseCursor : public nGuiLabel
{
public:
    /// constructor
    nGuiMouseCursor();
    /// destructor
    virtual ~nGuiMouseCursor();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// render the widget
    virtual bool Render();
    /// set the brush
    virtual void SetBrush(const nString& brush);
    /// is position inside? -> always return false!
    virtual bool Inside(const vector2& p);
    /// set fadein time
    void SetFadeInTime(nTime t);
    /// set brush size
    void SetBrushSize(const vector2& size);
    /// get brush size
    vector2 GetBrushSize() const;
    /// lock brush size
    void LockBrushSize(bool lock);
    /// set hotspot
    void SetHotSpot(const vector2& hotSpot);
    /// get hotspot
    vector2 GetHotSpot() const;

private:
    /// compute current rect
    void UpdateRect();
    /// compute current color
    void UpdateColor();

    bool openFirstFrame;
    bool fadeinRequested;
    bool brushSizeIsDirty;
    bool lockBrushSize;
    vector2 hotSpot;
    vector2 brushSize;
    vector4 windowColor;
    nTime fadeinRequestTime;
    nTime fadeInTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMouseCursor::SetFadeInTime(nTime t)
{
    this->fadeInTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2
nGuiMouseCursor::GetBrushSize() const
{
    return this->brushSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMouseCursor::SetBrushSize(const vector2& size)
{
    this->brushSize = size;
    this->UpdateRect();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMouseCursor::LockBrushSize(bool lock)
{
    this->lockBrushSize = lock;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2
nGuiMouseCursor::GetHotSpot() const
{
    return this->hotSpot;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMouseCursor::SetHotSpot(const vector2& hotSpot)
{
    this->hotSpot = hotSpot;
    this->UpdateRect();
}




//------------------------------------------------------------------------------
#endif

    
