//------------------------------------------------------------------------------
//  nguicharactercontrolwindow_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicharactercontrolwindow.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguitextlabel.h"
#include "gui/nguicolorslidergroup.h"
#include "gui/nguitextlabel.h"
#include "gui/nguitextview.h"
#include "gui/nguibutton.h"
#include "gui/nguicheckbutton.h"
#include "gui/nguidirlister.h"
#include "scene/ntransformnode.h"
#include "scene/nlightnode.h"
#include "scene/nskinanimator.h"
#include "scene/ncharacter3skinanimator.h"
#include "anim2/nanimstate.h"
#include "variable/nvariable.h"
#include "variable/nvariableserver.h"
#include "tools/nnodelist.h"

nNebulaClass(nGuiCharacterControlWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiCharacterControlWindow::nGuiCharacterControlWindow():
    characterRenderContext(0)
{
    this->skinAnimatorClass = nKernelServer::Instance()->FindClass("nskinanimator");
    n_assert(0 != this->skinAnimatorClass);
    this->character3SkinAnimatorClass = nKernelServer::Instance()->FindClass("ncharacter3skinanimator");
    n_assert(0 != this->character3SkinAnimatorClass);
}


//------------------------------------------------------------------------------
/**
*/
nGuiCharacterControlWindow::~nGuiCharacterControlWindow()
{
    // make sure everything gets cleared
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiCharacterControlWindow::HasCharacter2() const
{
    return this->characterRenderContext && this->refSkinAnimator.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiCharacterControlWindow::HasCharacter3() const
{
    return this->HasCharacter2() && this->refSkinAnimator->IsA(this->character3SkinAnimatorClass);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiCharacterControlWindow::ExistsCharacter2() const
{
    // find skin animator
    nRoot* startNode = this->kernelServer->Lookup("/usr/scene");
    n_assert(0 != startNode);
    nSkinAnimator* firstFoundNode = (nSkinAnimator*) this->FindFirstInstance(startNode, this->skinAnimatorClass);
    return (0 != firstFoundNode);
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiCharacterControlWindow::ExistsCharacter3() const
{
    // find character 3 skin animator
    nRoot* startNode = this->kernelServer->Lookup("/usr/scene");
    n_assert(0 != startNode);
    nSkinAnimator* firstFoundNode = (nSkinAnimator*) this->FindFirstInstance(startNode, this->character3SkinAnimatorClass);
    return (0 != firstFoundNode);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // look for character objects
    if (this->ExistsCharacter3())
    {
        this->FindCharacter3();
    }
    else if (this->ExistsCharacter2())
    {
        this->FindCharacter2();
    }

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout;
    kernelServer->PushCwd(layout);

    // sliders and color labels ...
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360;
    const float minHeight = -90;
    const float maxHeight = 90;
    const float border = 0.005f;
    const float knobSize = 45.0f;

    // create text view field for skeletons
    nGuiTextLabel* titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CharactersLabel");
    n_assert(titleLabel);
    titleLabel->SetText("Characters");
    titleLabel->SetFont("GuiSmall");
    titleLabel->SetAlignment(nGuiTextLabel::Left);
    vector2 textSize = titleLabel->GetTextExtent();
    titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    titleLabel->SetMinSize(vector2(0.0f, textSize.y * 1.25f));
    titleLabel->SetMaxSize(vector2(0.4f, textSize.y * 1.25f));
    layout->AttachForm(titleLabel, nGuiFormLayout::Top, border);
    layout->AttachForm(titleLabel, nGuiFormLayout::Left, border);
    layout->AttachPos(titleLabel, nGuiFormLayout::Right, 0.25f);
    titleLabel->OnShow();
    this->refCharacter3SkeletonsLabel = titleLabel;

    // create the file lister widget
    nGuiDirLister* fileLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "FileLister");
    n_assert(fileLister);
    fileLister->SetDefaultBrush("list_background");
    fileLister->SetHighlightBrush("list_selection");
    fileLister->SetFont("GuiSmall");
    fileLister->SetIgnoreSubDirs(false);
    fileLister->SetIgnoreFiles(false);
    fileLister->SetSelectionEnabled(true);
    fileLister->SetLookUpEnabled(true);
    fileLister->SetMaxSize(vector2(0.4f, 0.3f));
    layout->AttachWidget(fileLister, nGuiFormLayout::Top, this->refCharacter3SkeletonsLabel, border);
    layout->AttachForm(fileLister, nGuiFormLayout::Left, border);
    layout->AttachPos(fileLister, nGuiFormLayout::Right, 0.25f);
    fileLister->SetDirectory("proj:export/gfxlib/characters");
    fileLister->OnShow();
    this->refFileLister = fileLister;

    // Create Character 3 Controls, if nCharacter3Node was found
    if (this->HasCharacter3())
    {
        // create text view field for skins
        nGuiTextLabel* titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CharacterSkinsLabel");
        n_assert(titleLabel);
        titleLabel->SetText("Skins");
        titleLabel->SetFont("GuiSmall");
        titleLabel->SetAlignment(nGuiTextLabel::Left);
        vector2 textSize = titleLabel->GetTextExtent();
        titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        titleLabel->SetMinSize(vector2(0.0f, textSize.y * 1.25f));
        titleLabel->SetMaxSize(vector2(0.25f, textSize.y * 1.25f));
        layout->AttachForm(titleLabel, nGuiFormLayout::Top, border);
        layout->AttachWidget(titleLabel, nGuiFormLayout::Left, this->refCharacter3SkeletonsLabel, border);
        layout->AttachPos(titleLabel, nGuiFormLayout::Right, 0.5f);
        titleLabel->OnShow();
        this->refCharacter3SkinsLabel = titleLabel;

        nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "CharacterSkins");
        n_assert(textView);
        textView->SetSelectionEnabled(true);
        textView->SetHighlightBrush("textentry_h");
        textView->SetDefaultBrush("list_background");
        textView->SetHighlightBrush("list_selection");
        textView->SetMaxSize(vector2(0.25f, 0.3f));
        layout->AttachWidget(textView, nGuiFormLayout::Top, this->refCharacter3SkinsLabel, border);
        layout->AttachWidget(textView, nGuiFormLayout::Left, this->refFileLister, border);
        layout->AttachPos(textView, nGuiFormLayout::Right, 0.5f);
        textView->OnShow();
        this->refCharacter3Skins = textView;

        // create text view field for variations
        titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CharacterVariationsLabel");
        n_assert(titleLabel);
        titleLabel->SetText("Variations");
        titleLabel->SetFont("GuiSmall");
        titleLabel->SetAlignment(nGuiTextLabel::Left);
        textSize = titleLabel->GetTextExtent();
        titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        titleLabel->SetMinSize(vector2(0.0f, textSize.y * 1.25f));
        titleLabel->SetMaxSize(vector2(0.25f, textSize.y * 1.25f));
        layout->AttachForm(titleLabel, nGuiFormLayout::Top, border);
        layout->AttachWidget(titleLabel, nGuiFormLayout::Left, this->refCharacter3SkinsLabel, border);
        layout->AttachPos(titleLabel, nGuiFormLayout::Right, 0.75f);
        titleLabel->OnShow();
        this->refCharacter3VariationsLabel = titleLabel;

        textView = (nGuiTextView*) kernelServer->New("nguitextview", "CharacterVariations");
        n_assert(textView);
        textView->SetSelectionEnabled(true);
        textView->SetHighlightBrush("textentry_h");
        textView->SetDefaultBrush("list_background");
        textView->SetHighlightBrush("list_selection");
        textView->SetMaxSize(vector2(0.25f, 0.3f));
        layout->AttachWidget(textView, nGuiFormLayout::Top, this->refCharacter3VariationsLabel, border);
        layout->AttachWidget(textView, nGuiFormLayout::Left, this->refCharacter3Skins, border);
        layout->AttachPos(textView, nGuiFormLayout::Right, 0.75f);
        textView->OnShow();
        this->refCharacter3Variations = textView;

        // create text view field for animations
        titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CharacterAnimationsLabel");
        n_assert(titleLabel);
        titleLabel->SetText("Animations");
        titleLabel->SetFont("GuiSmall");
        titleLabel->SetAlignment(nGuiTextLabel::Left);
        textSize = titleLabel->GetTextExtent();
        titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        titleLabel->SetMinSize(vector2(0.0f, textSize.y * 1.25f));
        titleLabel->SetMaxSize(vector2(0.25f, textSize.y * 1.25f));
        layout->AttachForm(titleLabel, nGuiFormLayout::Top, border);
        layout->AttachWidget(titleLabel, nGuiFormLayout::Left, this->refCharacter3VariationsLabel, border);
        layout->AttachForm(titleLabel, nGuiFormLayout::Right, border);
        titleLabel->OnShow();
        this->refCharacterAnimationsLabel = titleLabel;

        textView = (nGuiTextView*) kernelServer->New("nguitextview", "CharacterAnimations");
        n_assert(textView);
        textView->SetSelectionEnabled(true);
        textView->SetHighlightBrush("textentry_h");
        textView->SetDefaultBrush("list_background");
        textView->SetHighlightBrush("list_selection");
        textView->SetMaxSize(vector2(0.25f, 0.3f));
        layout->AttachWidget(textView, nGuiFormLayout::Top, this->refCharacterAnimationsLabel, border);
        layout->AttachWidget(textView, nGuiFormLayout::Left, this->refCharacter3Variations, border);
        layout->AttachForm(textView, nGuiFormLayout::Right, border);
        textView->OnShow();
        this->refCharacterAnimations = textView;
    }
    else  if (this->HasCharacter2())
    {
        // create text view field for animations
        nGuiTextLabel* titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "CharacterAnimationsLabel");
        n_assert(titleLabel);
        titleLabel->SetText("Animations");
        titleLabel->SetFont("GuiSmall");
        titleLabel->SetAlignment(nGuiTextLabel::Left);
        vector2 textSize = titleLabel->GetTextExtent();
        titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        titleLabel->SetMinSize(vector2(0.0f, textSize.y * 1.25f));
        titleLabel->SetMaxSize(vector2(0.75f, textSize.y * 1.25f));
        layout->AttachForm(titleLabel, nGuiFormLayout::Top,border);
        layout->AttachWidget(titleLabel, nGuiFormLayout::Left, this->refCharacter3SkeletonsLabel, border);
        layout->AttachForm(titleLabel, nGuiFormLayout::Right, border);
        titleLabel->OnShow();
        this->refCharacterAnimationsLabel = titleLabel;

        nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "CharacterAnimations");
        n_assert(textView);
        textView->SetSelectionEnabled(true);
        textView->SetHighlightBrush("textentry_h");
        textView->SetDefaultBrush("list_background");
        textView->SetHighlightBrush("list_selection");
        textView->SetMaxSize(vector2(0.75f, 0.3f));
        layout->AttachWidget(textView, nGuiFormLayout::Top, this->refCharacterAnimationsLabel, border);
        layout->AttachWidget(textView, nGuiFormLayout::Left, this->refFileLister, border);
        layout->AttachForm(textView, nGuiFormLayout::Right, border);
        textView->OnShow();
        this->refCharacterAnimations = textView;
    }

    // Create Animation Controls, if nSkinAnimator was found
    if (this->HasCharacter2())
    {
        // create text label
        nGuiTextLabel* titleLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "ClipsLabel");
        n_assert(titleLabel);
        titleLabel->SetText("Active clips");
        titleLabel->SetFont("GuiSmall");
        titleLabel->SetAlignment(nGuiTextLabel::Left);
        vector2 textSize = titleLabel->GetTextExtent();
        vector2 textMinSize(0.0f, textSize.y * 1.25f);
        vector2 textMaxSize(1.0f, textSize.y * 1.25f);
        titleLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        titleLabel->SetMinSize(textMinSize);
        titleLabel->SetMaxSize(textMaxSize);
        layout->AttachWidget(titleLabel, nGuiFormLayout::Top, this->HasCharacter3() ? this->refCharacterAnimations : this->refCharacterAnimations, border);
        layout->AttachForm(titleLabel, nGuiFormLayout::Left, border);
        layout->AttachPos(titleLabel, nGuiFormLayout::Right, 0.45f);
        titleLabel->OnShow();
        this->refClipWeightsLabel = titleLabel;

        nCharacter2Set* characterSet = this->GetCharacterSet();
        if (0 != characterSet)
        {
            const int numClips = characterSet->GetNumClips();
            float offset = 0.01f;
            int i;
            for (i = 0; i < numClips; i++)
            {
                const nString& clipName = characterSet->GetClipNameAt(i);
                nString sliderName = "Slider_";
                sliderName.Append(clipName);
                nRef<nGuiHoriSliderGroup> slider = (nGuiHoriSliderGroup*) this->kernelServer->New("nguihorislidergroup", sliderName.Get());
                slider->SetMinValue(0);
                slider->SetMaxValue(100);
                slider->SetValue(100);
                nString rightText;
                rightText.SetFloat(((float)slider->GetValue()/100.0f));
                rightText.TerminateAtIndex(4);
                slider->SetLeftText(clipName.Get());
                slider->SetRightText(rightText.Get());
                slider->SetKnobSize(knobSize);
                slider->SetLeftWidth(leftWidth - 0.1f);
                slider->SetRightWidth(rightWidth + 0.3f);
                slider->SetDisabledBrush("sliderbg");
                layout->AttachWidget(slider, nGuiFormLayout::Top, this->refClipWeightsLabel, offset);
                layout->AttachForm(slider, nGuiFormLayout::Bottom, border);
                layout->AttachForm(slider, nGuiFormLayout::Left, border);
                layout->AttachForm(slider, nGuiFormLayout::Right, border);
                slider->OnShow();
                this->refClipWeights.Append(slider);
                offset += 0.05f;
            }
        }
    }

    this->kernelServer->PopCwd();

    // set new window rect
    this->SetTitle("Character Control");
    rectangle windowRect(vector2(0.0f, 0.0f), vector2(0.8f, 0.6f));
    this->SetRect(windowRect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::OnHide()
{
    if (this->refCharacter3SkeletonsLabel.isvalid())
    {
        this->refCharacter3SkeletonsLabel->Release();
    }
    if (this->refFileLister.isvalid())
    {
        this->refFileLister->Release();
    }
    if (this->refCharacter3SkinsLabel.isvalid())
    {
        this->refCharacter3SkinsLabel->Release();
    }
    if (this->refCharacter3Skins.isvalid())
    {
        this->refCharacter3Skins->Release();
    }
    if (this->refCharacter3VariationsLabel.isvalid())
    {
        this->refCharacter3VariationsLabel->Release();
    }
    if (this->refCharacter3Variations.isvalid())
    {
        this->refCharacter3Variations->Release();
    }
    if (this->refCharacterAnimationsLabel.isvalid())
    {
        this->refCharacterAnimationsLabel->Release();
    }
    if (this->refCharacterAnimations.isvalid())
    {
        this->refCharacterAnimations->Release();
    }
    if (this->refClipWeightsLabel.isvalid())
    {
        this->refClipWeightsLabel->Release();
    }
    int i;
    for (i = 0; i < refClipWeights.Size(); i++)
    {
        if (this->refClipWeights.At(i))
        {
            this->refClipWeights.At(i)->Release();
            this->refClipWeights.At(i) = 0;
        }
    }

    if (this->refSkinAnimator.isvalid())
    {
        this->refSkinAnimator.invalidate();
    }
    this->characterRenderContext = 0;

    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::SelectionChanged)
    {
        if (event.GetWidget() == this->refFileLister.get())
        {
            this->LoadSelectedCharacter();
            //this->Reopen();
        }
    }

    // character 3 menu
    if (this->HasCharacter3())
    {
        nCharacter3Set* character3Set = (nCharacter3Set*) this->GetCharacterSet();
        if (0 != character3Set)
        {
            if (this->refCharacter3Skins.isvalid() && event.GetType() == nGuiEvent::SelectionChanged && event.GetWidget() == this->refCharacter3Skins)
            {
                // switch state of selections
                int selection = this->refCharacter3Skins->GetSelectionIndex();
                if (selection >= 0 && selection < character3Set->GetNumAvailableSkins())
                {
                    character3Set->SetSkinVisibleAtIndex(selection, !character3Set->IsSkinVisibleAtIndex(selection));
                }
            }

            if (this->refCharacter3Variations.isvalid() && event.GetType() == nGuiEvent::SelectionChanged && event.GetWidget() == this->refCharacter3Variations)
            {
                // switch variation
                int selection = this->refCharacter3Variations->GetSelectionIndex();
                if (selection >= 0 && selection <= character3Set->GetNumAvailableVariations())
                {
                    selection--;
                    character3Set->SetCurrentVariationIndexed(selection);
                }
            }

            if (this->refCharacterAnimations.isvalid() && event.GetType() == nGuiEvent::SelectionChanged && event.GetWidget() == this->refCharacterAnimations)
            {
                // switch animations
                int selection = this->refCharacterAnimations->GetSelectionIndex();
                if (selection >= 0 && selection < this->refSkinAnimator->GetNumClips())
                {
                    const nString& clipName = this->refCharacterAnimations->GetLineAt(selection);
                    int clipIndex = character3Set->GetClipIndexByName(clipName);
                    if (-1 != clipIndex)
                    {
                        character3Set->RemoveClip(clipName);
                    }
                    else
                    {
                        character3Set->AddClip(clipName, 1.0f);
                    }
                    this->Reopen();
                }
            }
        }
    }
    else if (this->HasCharacter2())
    {
        nCharacter2Set* characterSet = this->GetCharacterSet();
        if (0 != characterSet)
        {
            if (this->refCharacterAnimations.isvalid() && event.GetType() == nGuiEvent::SelectionChanged && event.GetWidget() == this->refCharacterAnimations)
            {
                // switch animations
                int selection = this->refCharacterAnimations->GetSelectionIndex();
                if (selection >= 0 && selection < this->refSkinAnimator->GetNumClips())
                {
                    const nString& clipName = this->refCharacterAnimations->GetLineAt(selection);
                    int clipIndex = characterSet->GetClipIndexByName(clipName);
                    if (-1 != clipIndex)
                    {
                        characterSet->RemoveClip(clipName);
                    }
                    else
                    {
                        characterSet->AddClip(clipName, 1.0f);
                    }
                    this->Reopen();
                }
            }
        }
    }

    // animation state menu
    if (this->HasCharacter2())
    {
        nCharacter2Set* characterSet = this->GetCharacterSet();
        if (0 != characterSet)
        {
            // update weight value
            if (event.GetType() == nGuiEvent::SliderChanged)
            {
                int i;
                for (i = 0; i< this->refClipWeights.Size(); i++)
                {
                    nGuiHoriSliderGroup* slider = this->refClipWeights.At(i);
                    n_assert(0 != slider);
                    if (event.GetWidget() == slider)
                    {
                        // update text
                        nString rightText;
                        rightText.SetFloat((((float)slider->GetValue())/100.0f));
                        rightText.TerminateAtIndex(4);
                        slider->SetRightText(rightText.Get());

                        nString curClipName = slider->GetLeftText();
                        int j;
                        for (j = 0; j < characterSet->GetNumClips(); j++)
                        {
                            if (characterSet->GetClipNameAt(j) == curClipName)
                            {
                                characterSet->SetClipWeightAt(j, slider->GetValue()/100.0f);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    called per frame when parent widget is visible
*/
void
nGuiCharacterControlWindow::OnFrame()
{
    // update character 3 controls
    if (this->HasCharacter3())
    {
        nCharacter3Set* character3Set = (nCharacter3Set*) this->GetCharacterSet();
        if (0 != character3Set)
        {
            nArray<nString> names = character3Set->GetNamesOfLoadedSkins();
            this->refCharacter3Skins->BeginAppend();
            int i;
            for( i = 0; i < names.Size(); i++)
            {
                if (character3Set->IsSkinVisibleAtIndex(i))
                {
                    this->refCharacter3Skins->AppendColoredLine(names[i].Get(),vector4(0,0.5,0,1));
                }
                else
                {
                    this->refCharacter3Skins->AppendLine(names[i].Get());
                }
            }
            this->refCharacter3Skins->EndAppend();

            names = character3Set->GetNamesOfLoadedVariations();
            this->refCharacter3Variations->BeginAppend();
            this->refCharacter3Variations->AppendLine("none");
            for( i = 0; i < names.Size(); i++)
            {
                this->refCharacter3Variations->AppendLine(names[i].Get());
            }
            this->refCharacter3Variations->EndAppend();
        }
    }

    // update character 2 controls
    if (this->HasCharacter2())
    {
        nCharacter2Set* characterSet = this->GetCharacterSet();
        if (0 != characterSet)
        {
            n_assert(this->refSkinAnimator.isvalid());

            this->refCharacterAnimations->BeginAppend();
            int numClips = this->refSkinAnimator->GetNumClips();
            int i;
            for (i = 0; i < numClips; i++)
            {
                const nString& name = this->refSkinAnimator->GetClipAt(i).GetClipName();
                if (characterSet->GetClipIndexByName(name) != -1)
                {
                    this->refCharacterAnimations->AppendColoredLine(name.Get(), vector4(0,0.5,0,1));
                }
                else
                {
                    this->refCharacterAnimations->AppendLine(name.Get());
                }
            }
            this->refCharacterAnimations->EndAppend();
        }
    }

    nGuiClientWindow::OnFrame();

    // if nskinanimator is released because of loading a new non-animation scene,
    // open a new scenecontrolwindow and close the old one
    if (!this->HasCharacter2())
    {
        if (this->ExistsCharacter3() || this->ExistsCharacter2())
        {
            this->Reopen();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Recursively Find instance of nClass, used to search for lights, skin
    animators, etc...
*/
nRoot*
nGuiCharacterControlWindow::FindFirstInstance(nRoot* node, nClass* classType) const
{
    nRoot* resultNode = NULL;
    if (node == NULL)
    {
        resultNode = NULL;
    }
    else
    {
        if (node->IsInstanceOf(classType))
        {
            resultNode = node;
        }
        else
        {
            resultNode = FindFirstInstance(node->GetSucc(), classType);
            if (resultNode == NULL)
            {
                resultNode = FindFirstInstance(node->GetHead(), classType);
            }
        }
    }
    return resultNode;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::FindCharacter2()
{
    n_assert(!this->refSkinAnimator.isvalid());

    // find skin animator
    nRoot* startNode = this->kernelServer->Lookup("/usr/scene");
    n_assert(0 != startNode);
    nSkinAnimator* firstFoundNode = (nSkinAnimator*) this->FindFirstInstance(startNode, this->skinAnimatorClass);
    if (firstFoundNode)
    {
        this->refSkinAnimator = firstFoundNode;

        // lookup parent of parent of the skin animator registered in the nodelist
        // to acquire the render context
        nRoot* parentOfSkinAnimator = this->refSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);
        parentOfSkinAnimator = parentOfSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);
        parentOfSkinAnimator = parentOfSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);

        int numObjects = nNodeList::Instance()->GetCount();
        int i;
        for ( i = 0; i < numObjects; i++)
        {
            nTransformNode* node = nNodeList::Instance()->GetNodeAt(i);
            if (node == parentOfSkinAnimator)
            {
                this->characterRenderContext = nNodeList::Instance()->GetRenderContextAt(i);
                break;
            }
        }
        n_assert(0 != this->characterRenderContext);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::FindCharacter3()
{
    n_assert(!this->refSkinAnimator.isvalid());

    // find skin animator
    nRoot* startNode = this->kernelServer->Lookup("/usr/scene");
    n_assert(0 != startNode);
    nSkinAnimator* firstFoundNode = (nSkinAnimator*) this->FindFirstInstance(startNode, this->character3SkinAnimatorClass);
    if (firstFoundNode)
    {
        this->refSkinAnimator = firstFoundNode;

        // lookup parent of parent of the skin animator registered in the nodelist
        // to acquire the render context
        nRoot* parentOfSkinAnimator = this->refSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);
        parentOfSkinAnimator = parentOfSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);
        parentOfSkinAnimator = parentOfSkinAnimator->GetParent();
        n_assert(parentOfSkinAnimator);

        int numObjects = nNodeList::Instance()->GetCount();
        int i;
        for ( i = 0; i < numObjects; i++)
        {
            nTransformNode* node = nNodeList::Instance()->GetNodeAt(i);
            if (node == parentOfSkinAnimator)
            {
                this->characterRenderContext = nNodeList::Instance()->GetRenderContextAt(i);
                break;
            }
        }
        n_assert(0 != this->characterRenderContext);
    }
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2Set*
nGuiCharacterControlWindow::GetCharacterSet()
{
    n_assert(0 != this->characterRenderContext);

    nVariable::Handle characterSetHandle = nVariableServer::Instance()->GetVariableHandleByName("charSetPointer");
    nVariable* var = this->characterRenderContext->FindLocalVar(characterSetHandle);
    n_assert(0 != var);
    nCharacter2Set* characterSet = (nCharacter2Set*) var->GetObj();
    n_assert(characterSet);
    return characterSet;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::Reopen()
{
    if (!this->IsCloseRequested())
    {
        nGuiServer::Instance()->NewWindow("nguicharactercontrolwindow", true);
        this->SetCloseRequested(true);
    }
}


//------------------------------------------------------------------------------
/**
*/
void
nGuiCharacterControlWindow::LoadSelectedCharacter() const
{
    n_assert(this->refFileLister.isvalid());

    nString path;

    const char* rootDir = this->refFileLister->GetDirectory();
    const char* selection = this->refFileLister->GetSelection();
    if (rootDir && selection)
    {
        path = rootDir;
        path.Append("/");
        path.Append(selection);

        if (nFileServer2::Instance()->DirectoryExists(path))
        {
            path.Append("/skeleton.n2");
        }

        nNodeList* nodeList = nNodeList::Instance();
        n_assert( nodeList != 0 );

        nodeList->Clear();
        nodeList->AddDefaultEntry();
        nodeList->LoadObject(path);

        // reset time
        nTimeServer::Instance()->ResetTime();
    }
}
