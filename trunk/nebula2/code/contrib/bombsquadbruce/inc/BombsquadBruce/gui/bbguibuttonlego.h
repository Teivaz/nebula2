#ifndef N_GUIBUTTONLEGO_H
#define N_GUIBUTTONLEGO_H
//-----------------------------------------------------------------------------
/**
    @class BBGuiButtonLego
    @ingroup CrazyChipmunkGuiSystem
    @brief A groupable GUI button class.
    
    The only difference between this and an nGuiButton is that the "inside"
    of an BBGuiButtonLego (used for e.g. determining when the mouse is over 
    the widget) is defined to be the entire inside of the buttonLego's NOH
    parent. The idea is that a "virtual button" can be created, composed of 
    several nGuiButtonLegos, and when the mouse moves over any part of the
    "virtual button", all the BBGuiButtonLegos will be highlighted.

    e.g. define brushes for the left edge, middle, and right edge of a menu
    item button.  Using only these brushes, you can then make "virtual buttons"
    of varying width by composing a left edge, n middle pieces, and a right edge.
    If these brushes are used in nGuiButtonLegos, and all are placed on any
    nGuiWidget (sized to fit all the legos), the result will be indistinguishable
    from one big nGuiButton.

    (C) 2004 Rafael Van Daele-Hunt
*/
#include "gui/nguibutton.h"

//-----------------------------------------------------------------------------
class BBGuiButtonLego : public nGuiButton
{
public:
    /// constructor
    BBGuiButtonLego();
    /// destructor
    virtual ~BBGuiButtonLego();
    /// is position inside?
    virtual bool Inside(const vector2& p);
    /// currently enabled?
    bool IsEnabled() const;
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
};
//-----------------------------------------------------------------------------
#endif

