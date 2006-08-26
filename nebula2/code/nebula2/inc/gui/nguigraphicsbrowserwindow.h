#ifndef N_GUIGRAPHICSBROWSERWINDOW_H
#define N_GUIGRAPHICSBROWSERWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiGraphicsBrowserWindow
    @ingroup Gui

    @brief A client window which browses the proj:export/gfxlib directory, and loads
    graphics objects under /usr/scene.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "gui/nguicategorybrowser.h"
#include "gui/nguitextbutton.h"
#include "kernel/nscriptserver.h"

//------------------------------------------------------------------------------
class nGuiGraphicsBrowserWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiGraphicsBrowserWindow();
    /// destructor
    virtual ~nGuiGraphicsBrowserWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

protected:
    /// load new gfx object under /usr/scene
    void LoadObject(const nString& objPath);
    /// append new gfx object under /usr/scene
    void AddObject(const nString& objPath);

    nAutoRef<nScriptServer> refScriptServer;
    nRef<nGuiCategoryBrowser> refCatBrowser;
    nRef<nGuiTextButton> refLoadButton;
    nRef<nGuiTextButton> refAddButton;
    nRef<nGuiTextButton> refTimeButton;
    nAutoRef<nRoot> refUsrScene;
};

//------------------------------------------------------------------------------
#endif

