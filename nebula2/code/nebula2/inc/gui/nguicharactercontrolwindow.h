#ifndef N_GUICHARACTERCONTROLWINDOW_H
#define N_GUICHARACTERCONTROLWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiCharacterControlWindow
    @ingroup Gui

    @brief Character Controls

    (C) 2006 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "scene/ncharacter3node.h"
#include "character/ncharacter3set.h"

class nGuiDirLister;
class nGuiHoriSliderGroup;
class nGuiTextButton;
class nTransformNode;
class nLightNode;
class nSkinAnimator;
class nGuiTextLabel;
class nGuiTextView;
class nGuiCheckButton;

//------------------------------------------------------------------------------
class nGuiCharacterControlWindow : public nGuiClientWindow
{
    // friend defined for access for reading and assigning sliders on load and safe throug nGuiSettingsManagment
    friend class nGuiSettingsManagementWindow;

public:
    /// constructor
    nGuiCharacterControlWindow();
    /// destructor
    virtual ~nGuiCharacterControlWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

private:
    /// initiate reopening window
    void Reopen();
    /// find Instance of nClass
    nRoot* FindFirstInstance(nRoot* node, nClass* classType) const;
    /// find character 2
    void FindCharacter2();
    /// find character 3
    void FindCharacter3();
    /// has character 2
    bool HasCharacter2() const;
    /// has character 3
    bool HasCharacter3() const;
    /// exists character 2
    bool ExistsCharacter2() const;
    /// exists character 3
    bool ExistsCharacter3() const;

    /// get character set
    nCharacter2Set* GetCharacterSet();
    /// loads the character from the currently selected path
    void LoadSelectedCharacter() const;

    // Gui elements
    nRef<nGuiTextLabel> refCharacter3SkeletonsLabel;
    nRef<nGuiDirLister> refFileLister;
    nRef<nGuiTextLabel> refCharacter3SkinsLabel;
    nRef<nGuiTextView> refCharacter3Skins;
    nRef<nGuiTextLabel> refCharacter3VariationsLabel;
    nRef<nGuiTextView> refCharacter3Variations;
    nRef<nGuiTextLabel> refCharacterAnimationsLabel;
    nRef<nGuiTextView> refCharacterAnimations;
    nRef<nGuiTextLabel> refClipWeightsLabel;
    nArray<nRef<nGuiHoriSliderGroup> > refClipWeights;

    nRef<nSkinAnimator> refSkinAnimator;
    nRenderContext* characterRenderContext;

    nClass* skinAnimatorClass;
    nClass* character3SkinAnimatorClass;
};

#endif