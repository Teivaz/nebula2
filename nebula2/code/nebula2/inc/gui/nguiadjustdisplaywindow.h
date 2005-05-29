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
class nGuiColorSliderGroup;
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
    /// initialize variables
    void InitVariables();
    /// update slider from values
    void UpdateSlidersFromValues();
    /// update values from sliders
    void UpdateValuesFromSliders();
    /// reset to defaults
    void ResetValues();

    nRef<nGuiHoriSliderGroup>  refSaturateSlider;
    nRef<nGuiColorSliderGroup> refBalanceSlider;
    nRef<nGuiHoriSliderGroup>  refHdrBloomIntensitySlider;
    nRef<nGuiHoriSliderGroup>  refHdrBrightPassThresholdSlider;
    nRef<nGuiHoriSliderGroup>  refHdrBrightPassOffsetSlider;
    nRef<nGuiTextButton>       refResetButton;

    nVariable::Handle saturationHandle;
    nVariable::Handle balanceHandle;
    nVariable::Handle hdrBloomIntensityHandle;
    nVariable::Handle hdrBrightPassThresholdHandle;
    nVariable::Handle hdrBrightPassOffsetHandle;

    float resetSaturation;
    vector4 resetBalance;
    float resetBloomIntensity;
    float resetBrightPassThreshold;
    float resetBrightPassOffset;
};
//------------------------------------------------------------------------------
#endif
