#ifndef N_GUIHARDPOINTSBROWSERWINDOW_H
#define N_GUIHARDPOINTSBROWSERWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiHardpointsBrowserWindow
    @ingroup Gui

    A client window which browses the proj:export/gfxlib directory, and loads 
    graphics objects under /usr/scene. The objects are attached to a selected hard-
    point
    
    (C) 2005 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "gui/nguicategorybrowser.h"
#include "gui/nguihardpointslister.h"
#include "gui/nguitextbutton.h"
#include "kernel/nscriptserver.h"

//------------------------------------------------------------------------------
class nGuiHardpointsBrowserWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiHardpointsBrowserWindow();
    /// destructor
    virtual ~nGuiHardpointsBrowserWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

protected:
    /// append new gfx object under /usr/scene and attach it to the jointindex
    /// (assuming there is only 1 model with skeleton)
    void AddObject(const nString& objPath,int jointIndex);

    nAutoRef<nScriptServer> refScriptServer;
    nRef<nGuiCategoryBrowser> refCatBrowser;
    nRef<nGuiTextButton> refAddButton;
    nRef<nGuiTextButton> refTimeButton;
    nRef<nGuiHardpointsLister>  refHardpointsLister;
    nAutoRef<nRoot> refUsrScene;
};

//------------------------------------------------------------------------------
#endif
