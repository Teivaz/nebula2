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
#include "scene/ncharacter3node.h"
#include "character/ncharacter3set.h"

class nGuiHoriSliderGroup;
class nGuiColorSliderGroup;
class nGuiTextButton;
class nTransformNode;
class nLightNode;
class nSkinAnimator;
class nGuiTextLabel;
class nGuiTextView;
class nGuiSkyEditor;

//------------------------------------------------------------------------------
class nGuiSceneControlWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiSceneControlWindow();
    /// destructor
    virtual ~nGuiSceneControlWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set the path to working light
    void SetLightPath(const char* path);
    /// get the path from working light
    const char* GetLightPath() const;

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
    /// update channel slider
    void UpdateChnSlider();
    /// find Instance of nClass
    nRoot* FindFirstInstance(nRoot* node, nClass* classType);

    // Gui elements
    nRef<nGuiHoriSliderGroup> refLightDirection;
    nRef<nGuiHoriSliderGroup> refLightHeight;
    nRef<nGuiColorSliderGroup> refDiffuseSlider;
    nRef<nGuiColorSliderGroup> refSpecularSlider;
    nRef<nGuiColorSliderGroup> refAmbientSlider;
    nArray<nGuiHoriSliderGroup*> refWeightChnListSlider;
    nRef<nGuiTextLabel> refStatesLabel;
    nRef<nGuiTextLabel> refChnLabel;
    nRef<nGuiTextView> refAnimStates;
    nRef<nGuiTextView> refCharacter3Skins;
    nRef<nGuiTextView> refCharacter3Animations;
    nRef<nGuiTextView> refCharacter3Variations;
    nDynAutoRef<nTransformNode> refLightTransform;
    nDynAutoRef<nLightNode> refLight;

//    nRef<nSkinAnimator> refSkinAnimator;
    nRef<nSkinAnimator> refSkinAnimator;
    nRef<nCharacter3Node> refCharacter3Node;
    nRenderContext* character3RCPtr;
    nCharacter3Set* character3SetPtr;

    vector4 diffuseColor;
    vector4 specularColor;
    vector4 ambientColor;
    nString lightPath;
    nString lightTransformPath;
    vector3 lightAngles;
    int numAnimStates;
    bool skinAnimatorLoaded;
    bool character3NodeLoaded;
    bool sliderChanged;
    nArray<nVariable::Handle> chnHandles;

    nRef<nGuiSkyEditor> refSkyEditor;
};

//------------------------------------------------------------------------------
/**
    Set the path to working light
*/
inline
void
nGuiSceneControlWindow::SetLightPath(const char* path)
{
    this->lightPath.Set(path);
    this->lightPath.StripTrailingSlash();
    this->lightTransformPath.Set(this->lightPath.ExtractToLastSlash().Get());
}

//------------------------------------------------------------------------------
/**
    Get the path from working light
*/
inline
const char*
nGuiSceneControlWindow::GetLightPath() const
{
    return this->lightPath.Get();
}

#endif
