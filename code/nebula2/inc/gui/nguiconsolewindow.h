#ifndef N_GUICONSOLEWINDOW_H
#define N_GUICONSOLEWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiConsoleWindow
    @ingroup Gui
    @brief A complete Nebula command console window.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiCmdEntry;

//------------------------------------------------------------------------------
class nGuiConsoleWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiConsoleWindow();
    /// destructor
    virtual ~nGuiConsoleWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();

protected:
    nRef<nGuiCmdEntry> refCmdEntry;
};
//------------------------------------------------------------------------------
#endif
