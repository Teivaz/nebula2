#ifndef N_GUIEVENTHANDLER_H
#define N_GUIEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nGuiEventHandler
    @ingroup Gui

    A simple gui event handler class which you derive from and may attach to
    a widget (usually a window).

    NOTE: the event handler object belongs to the application code, not
    the widget, thus the application code is responsible to delete it, the
    widget will just leave it alone.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/ntypes.h"

class nGuiEvent;

//------------------------------------------------------------------------------
class nGuiEventHandler
{
public:
    /// constructor
    nGuiEventHandler();
    /// destructor
    virtual ~nGuiEventHandler();
    /// handle a gui event, override this method in a subclass
    virtual void HandleEvent(const nGuiEvent& e);
};
//------------------------------------------------------------------------------
#endif
