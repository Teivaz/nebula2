#ifndef N_GUIADJUSTDISPLAYWINDOW_H
#define N_GUIADJUSTDISPLAYWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiAdjustDisplayWindow
    @ingroup Gui

    Controls for adjusting saturation, color balance, etc...
    Required a nMRTSceneServer scene server.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiHoriSliderGroup;
class nGuiTextButton;

//------------------------------------------------------------------------------
class nGuiAdjustDisplayWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiAdjustDisplayWindow();
    /// destructor
    virtual ~nGuiAdjustDisplayWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

private:
    /// update slider from scene server
    void UpdateSlidersFromSceneServer();
    /// update scene server from sliders
    void UpdateSceneServerFromSliders();
    /// reset to defaults
    void ResetValues();

    nRef<nGuiHoriSliderGroup> refSaturate;
    nRef<nGuiHoriSliderGroup> refBalanceRed;
    nRef<nGuiHoriSliderGroup> refBalanceGreen;
    nRef<nGuiHoriSliderGroup> refBalanceBlue;
    nRef<nGuiHoriSliderGroup> refLuminanceRed;
    nRef<nGuiHoriSliderGroup> refLuminanceGreen;
    nRef<nGuiHoriSliderGroup> refLuminanceBlue;
    nRef<nGuiTextButton> refResetButton;
};
//------------------------------------------------------------------------------
#endif

