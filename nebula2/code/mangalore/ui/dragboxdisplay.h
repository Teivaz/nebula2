#ifndef UI_DRAGBOXDISPLAY_H
#define UI_DRAGBOXDISPLAY_H
//------------------------------------------------------------------------------
/**
    @class UI::DragBoxDisplay


    (C) 2006 RadonLabs GmbH
*/
#include "ui/window.h"
#include "mathlib/rectangle.h"

//------------------------------------------------------------------------------
namespace UI
{

class DragBoxDisplay : public Window
{
    DeclareRtti;

public:

    /// constructor
    //DragBoxDisplay();
    /// destructor
    //virtual ~DragBoxDisplay();

    /// update the DragBoxDisplay with a new rect
    virtual bool Update( const rectangle& r ) = 0;

private:

    //rectangle dragBox2D;

};

} // namespace UI
//------------------------------------------------------------------------------
#endif