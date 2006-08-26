#ifndef N_GUISKYEDITOR_H
#define N_GUISKYEDITOR_H
//------------------------------------------------------------------------------
/**
    @class nGuiSkyEditor
    @ingroup Gui

    @brief To edit nSkyNode on the fly...

    (C) 2005 RadonLabs GmbH
*/
#include "scene/nskynode.h"

class nGuiHoriSliderGroup;
class nGuiColorSliderGroup;
class nGuiTextButton;
class nGuiTextLabel;
class nSkyNode;
class nAbstractShaderNode;

//------------------------------------------------------------------------------
class nGuiSkyEditor : public nGuiFormLayout
{
public:
    /// constructor
    nGuiSkyEditor();
    /// destructor
    virtual ~nGuiSkyEditor();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set the path to nSkyNode
    void SetSkyPath(const char* path);
    /// get the path from nSkyNode
    const char* GetSkyPath() const;
    /// nskynode found?
    bool SkyLoad();

private:
    ///
    void FindSkyNode(nRoot* node);
    ///
    void ShowSky();
    ///
    void HideSky();
    ///
    void EventSky(const nGuiEvent& event);
    ///
    void ShowCloud();
    ///
    void CreateCloud();
    ///
    void HideCloud();
    ///
    void ReleaseCloud();
    ///
    void EventCloud(const nGuiEvent& event);
    ///
    void ShowSkycolor();
    ///
    void CreateSkycolor();
    ///
    void HideSkycolor();
    ///
    void ReleaseSkycolor();
    ///
    void EventSkycolor(const nGuiEvent& event);
    ///
    void ShowSun();
    ///
    void CreateSun();
    ///
    void HideSun();
    ///
    void ReleaseSun();
    ///
    void EventSun(const nGuiEvent& event);
    ///
    void ShowSunlight();
    ///
    void CreateSunlight();
    ///
    void HideSunlight();
    ///
    void ReleaseSunlight();
    ///
    void EventSunlight(const nGuiEvent& event);
    ///
    void UpdateSliderFromElement(nGuiHoriSliderGroup* slider,nShaderState::Param param, float min, float max, int vectornr = 0);
    ///
    void UpdateColorSliderFromElement(nGuiColorSliderGroup* slider,nShaderState::Param param, float max);


    // Gui elements
    nRef<nGuiTextButton> refRefreshButton;
    nRef<nGuiTextButton> refSaveButton;
    /*nRef<nGuiTextButton> refDeleteStateButton;      //FIXME: Not implemented
    nRef<nGuiTextButton> refDeleteElementButton;
    nRef<nGuiTextButton> refNewStateButton;
    nRef<nGuiTextButton> refNewElementButton;*/
    nRef<nGuiHoriSliderGroup> refSkyTimeSlider;
    nRef<nGuiHoriSliderGroup> refTimeFactorSlider;
    nRef<nGuiHoriSliderGroup> refElementSlider;
    nRef<nGuiHoriSliderGroup> refStateSlider;
    nRef<nGuiHoriSliderGroup> refStateTimeSlider;
    nRef<nGuiTextLabel> refElementLabel;

    //Cloud
    nRef<nGuiHoriSliderGroup> refCloudAddSlider;
    nRef<nGuiHoriSliderGroup> refCloudMulSlider;
    nRef<nGuiHoriSliderGroup> refCloudGlowSlider;
    nRef<nGuiHoriSliderGroup> refCloudRangeSlider;
    nRef<nGuiHoriSliderGroup> refCloudDensSlider;
    nRef<nGuiHoriSliderGroup> refCloudMapResSlider;
    nRef<nGuiHoriSliderGroup> refCloudStrucResSlider;
    nRef<nGuiColorSliderGroup> refCloudColSlider;
    nRef<nGuiHoriSliderGroup> refCloudMapDirSlider;
    nRef<nGuiHoriSliderGroup> refCloudMapSpeedSlider;
    nRef<nGuiHoriSliderGroup> refCloudStrucSpeedSlider;
    nRef<nGuiHoriSliderGroup> refCloudWeightSlider;

    //Skycolor
    nRef<nGuiColorSliderGroup> refSkyTopColSlider;
    nRef<nGuiColorSliderGroup> refSkyBotColSlider;
    nRef<nGuiHoriSliderGroup> refSkyBottomSlider;
    nRef<nGuiHoriSliderGroup> refSkyBrightSlider;
    nRef<nGuiHoriSliderGroup> refSkySatSlider;
    nRef<nGuiColorSliderGroup> refSkySunColSlider;
    nRef<nGuiHoriSliderGroup> refSkySunRangeSlider;
    nRef<nGuiHoriSliderGroup> refSkySunIntensSlider;
    nRef<nGuiHoriSliderGroup> refSkySunFlatSlider;

    //Sun
    nRef<nGuiColorSliderGroup> refSunColSlider;
    nRef<nGuiHoriSliderGroup> refSunScaleSlider;

    //Sunlight
    nRef<nGuiColorSliderGroup> refSunLightDiffuseColSlider;
    nRef<nGuiColorSliderGroup> refSunLightDiffuse1ColSlider;
    nRef<nGuiColorSliderGroup> refSunLightAmbientColSlider;
    nRef<nGuiColorSliderGroup> refSunLightSpecularColSlider;

    //Stars
    // FIXME: Not implemented


    nDynAutoRef<nSkyNode> refSky;
    nRef<nAbstractShaderNode> refElement;
    int oldElement;
    int activeElement;
    int activeState;
    bool elementReady;
    bool stateReady;
    bool layoutChanged;
    bool refresh;
    bool saveSky;
    bool updateSkyTime;
    nSkyNode::ElementType activeType;

    nString skyPath;

    bool sliderChanged;    // ???
};

//------------------------------------------------------------------------------
/**
    Set the path to nSkyNode
*/
inline
void
nGuiSkyEditor::SetSkyPath(const char* path)
{
    this->skyPath.Set(path);
    this->skyPath.StripTrailingSlash();
}

//------------------------------------------------------------------------------
/**
    Get the path from working light
*/
inline
const char*
nGuiSkyEditor::GetSkyPath() const
{
   return this->skyPath.Get();
}

//------------------------------------------------------------------------------
/**

*/
inline
bool
nGuiSkyEditor::SkyLoad()
{
    return this->refSky.isvalid();
}

#endif
