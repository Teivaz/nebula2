#ifndef N_GUIDOCKWINDOW_H
#define N_GUIDOCKWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiDockWindow
    @ingroup Gui
    @brief The global dock window which replaces the old scripting console as 
    Nebula's default ingame console.
    
    FIXME:
    Offered tools are currently hardcoded. Some sort of scriptable
    plugin mechanism should be implemented to add additional 
    tools to the dock.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "kernel/nscriptserver.h"

//------------------------------------------------------------------------------
class nGuiDockWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiDockWindow();
    /// destructor
    virtual ~nGuiDockWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

protected:
    nAutoRef<nScriptServer> refScriptServer;
    nRef<nGuiButton> refConsoleButton;
    nRef<nGuiButton> refTexBrowserButton;
    nRef<nGuiButton> refGfxBrowserButton;
    nRef<nGuiButton> refSceneControlButton;
    nRef<nGuiButton> refWatcherButton;
    nRef<nGuiButton> refSysInfoButton;
    nRef<nGuiButton> refAdjustButton;
    nRef<nGuiButton> refHideButton;
    nRef<nGuiButton> refQuitButton;
    nRef<nGuiButton> refHardpointButton;
    nRef<nGuiButton> refSettingsManagementButton;
    nRef<nGuiButton> refCharacterControlButton;
};

//------------------------------------------------------------------------------
#endif
