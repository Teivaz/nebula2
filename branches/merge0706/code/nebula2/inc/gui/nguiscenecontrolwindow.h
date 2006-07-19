#ifndef N_GUISCENECONTROLWINDOW_H
#define N_GUISCENECONTROLWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiSceneControlWindow
    @ingroup Gui

    @brief Scene Controls for adjusting stdlight, postion & color...
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiHoriSliderGroup;
class nGuiColorSliderGroup;
class nTransformNode;
class nLightNode;
//class nGuiSkyEditor;

//------------------------------------------------------------------------------
class nGuiSceneControlWindow : public nGuiClientWindow
{
    // friend defined for access for reading and assigning sliders on load and safe throug nGuiSettingsManagment
    friend class nGuiSettingsManagementWindow;

public:
    /// constructor
    nGuiSceneControlWindow();
    /// destructor
    virtual ~nGuiSceneControlWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event); 
    /// set the path to working light
    void SetLightPath(const nString& path);
    /// get the path from working light
    const nString& GetLightPath() const;
    
    /// color type of light 
    enum ColorType
    {
        Diffuse,
        Specular,
        Ambient
    };

private:
    /// update Light Position
    void UpdateLightPosition();

    // Gui elements
    nRef<nGuiHoriSliderGroup> refLightDirection;
    nRef<nGuiHoriSliderGroup> refLightHeight;
    nRef<nGuiColorSliderGroup> refDiffuseSlider;
    nRef<nGuiColorSliderGroup> refSpecularSlider;
    nRef<nGuiColorSliderGroup> refAmbientSlider;
//  nRef<nGuiSkyEditor> refSkyEditor;

    nDynAutoRef<nTransformNode> refLightTransform;
    nDynAutoRef<nLightNode> refLight;

    vector4 diffuseColor;
    vector4 specularColor;
    vector4 ambientColor;
    nString lightPath;
    nString lightTransformPath;
    vector3 lightAngles;
};

//------------------------------------------------------------------------------
/**
    Set the path to working light
*/
inline
void
nGuiSceneControlWindow::SetLightPath(const nString& path)
{
    this->lightPath = path;
    this->lightPath.StripTrailingSlash();
    this->lightTransformPath = this->lightPath.ExtractToLastSlash();
}

//------------------------------------------------------------------------------
/**
    Get the path from working light
*/
inline
const nString&
nGuiSceneControlWindow::GetLightPath() const
{
    return this->lightPath;
}

#endif
