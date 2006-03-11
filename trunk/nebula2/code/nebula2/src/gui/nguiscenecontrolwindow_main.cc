//------------------------------------------------------------------------------
//  nguiscenecontrolwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiscenecontrolwindow.h"
#include "gui/nguihorislidergroup.h"
#include "scene/ntransformnode.h"
#include "scene/nlightnode.h"
#include "gui/nguitextlabel.h"
#include "gui/nguicolorslidergroup.h"
#include "scene/nskinanimator.h"
#include "gui/nguitextlabel.h"
#include "gui/nguitextview.h"
#include "anim2/nanimstate.h"
#include "variable/nvariable.h"
#include "variable/nvariableserver.h"

nNebulaClass(nGuiSceneControlWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiSceneControlWindow::nGuiSceneControlWindow():
    diffuseColor(1.0f,1.0f,1.0f,1.0f),
    specularColor(1.0f,1.0f,1.0f,1.0f),
    ambientColor(1.0f,1.0f,1.0f,1.0f),
    lightPath("/usr/scene/default/stdlight/l"),
    lightTransformPath("/usr/scene/default/stdlight"),
    numAnimStates(0),
    sliderChanged(false),
    skinAnimatorLoaded(false),
    refLightTransform("/usr/scene/default/stdlight"),
    refLight("/usr/scene/default/stdlight/l")
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiSceneControlWindow::~nGuiSceneControlWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    this->SetTitle("Scene Control");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout;
    kernelServer->PushCwd(layout);

    // read current light values
    if (this->refLight.isvalid())
    {
        // look up the current Light Colors and update Brightness Sliders
        this->diffuseColor  = this->refLight->GetVector(nShaderState::LightDiffuse);
        this->specularColor = this->refLight->GetVector(nShaderState::LightSpecular);
        this->ambientColor  = this->refLight->GetVector(nShaderState::LightAmbient);
    }
    if (this->refLightTransform.isvalid())
    {
        this->lightAngles = this->refLightTransform->GetEuler();
    }

    if (!this->refSkinAnimator.isvalid())
    {
        // Find nSkinAnimator Class
        nClass* skinAnimatorClass = this->kernelServer->FindClass("nskinanimator");
        nRoot* startNode = this->kernelServer->Lookup("/usr/scene");
        this->refSkinAnimator = (nSkinAnimator*)this->FindFirstInstance(startNode, skinAnimatorClass);        
    }
    
    rectangle windowRect(vector2(0.0f, 0.0f), vector2(0.4f, 0.3f));

    // sliders and color labels ...
    const float leftWidth = 0.3f;
    const float rightWidth = 0.15f;
    const float maxAngle = 360;
    const float minHeight = -90;
    const float maxHeight = 90;
    const float border = 0.005f;
    
    nGuiHoriSliderGroup* slider;
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LightHori");
    slider->SetLeftText("Light Hori");
    slider->SetRightText("%d");
    slider->SetMinValue(0.0f);
    slider->SetMaxValue(maxAngle);
    slider->SetValue(n_rad2deg(this->lightAngles.y));
    slider->SetKnobSize(36);
    slider->SetIncrement(1.0f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLightDirection = slider;         
    
    slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", "LightVert");
    slider->SetLeftText("Light Vert");
    slider->SetRightText("%d");
    slider->SetMinValue(minHeight);
    slider->SetMaxValue(maxHeight);
    slider->SetValue(n_rad2deg(this->lightAngles.x));
    slider->SetKnobSize(20);
    slider->SetIncrement(1.0f);
    slider->SetLeftWidth(leftWidth);
    slider->SetRightWidth(rightWidth);
    layout->AttachWidget(slider, nGuiFormLayout::Top, this->refLightDirection, border);
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);
    slider->OnShow();
    this->refLightHeight = slider;

    nGuiColorSliderGroup* colorSlider;
    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Diffuse");
    colorSlider->SetLabelText("Diffuse");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->diffuseColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refLightHeight, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refDiffuseSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Specular");
    colorSlider->SetLabelText("Specular");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->specularColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refDiffuseSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refSpecularSlider = colorSlider;

    colorSlider = (nGuiColorSliderGroup*) kernelServer->New("nguicolorslidergroup", "Ambient");
    colorSlider->SetLabelText("Ambient");
    colorSlider->SetMaxIntensity(10.0f);
    colorSlider->SetTextLabelWidth(leftWidth);
    colorSlider->SetColor(this->ambientColor);
    layout->AttachWidget(colorSlider, nGuiFormLayout::Top, this->refSpecularSlider, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Left, border);
    layout->AttachForm(colorSlider, nGuiFormLayout::Right, border);
    colorSlider->OnShow();
    this->refAmbientSlider = colorSlider;

    // Create SkyEditor
/*
    nGuiSkyEditor* skyEditor = (nGuiSkyEditor*) kernelServer->New("nguiskyeditor","SkyEditor");
    layout->AttachWidget(skyEditor, nGuiFormLayout::Top, this->refAmbientSlider, 2*border);
    layout->AttachForm(skyEditor, nGuiFormLayout::Left, border);
    layout->AttachForm(skyEditor, nGuiFormLayout::Right, border);
    skyEditor->OnShow();
    this->refSkyEditor = skyEditor;
    if (this->refSkyEditor->SkyLoad())
    {
        windowRect = rectangle(vector2(0.0f, 0.0f), vector2(0.4f, 0.8f));
    }
*/

    // Create Animation Controls, if nSkinAnimator was found
    if (this->refSkinAnimator.isvalid())
    {
        this->skinAnimatorLoaded = true;
        // create text label
        nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "StatesLabel");
        n_assert(textLabel);
        textLabel->SetText("AnimStates:");
        textLabel->SetFont("GuiSmall");
        textLabel->SetAlignment(nGuiTextLabel::Left);
        vector2 textSize = textLabel->GetTextExtent();
        vector2 textMinSize(0.0f, textSize.y * 1.25f);
        vector2 textMaxSize(1.0f, textSize.y * 1.25f);
        textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        textLabel->SetMinSize(textMinSize);
        textLabel->SetMaxSize(textMaxSize);
        layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refAmbientSlider, 0.025f);
        layout->AttachForm(textLabel, nGuiFormLayout::Left, border); 
        layout->AttachPos(textLabel, nGuiFormLayout::Right, 0.45f);        
        textLabel->OnShow();
        this->refStatesLabel = textLabel;     

        // create text view field for nAnimStates
        nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "AnimStates");
        n_assert(textView);        
        textView->SetSelectionEnabled(true);
        textView->SetHighlightBrush("textentry_h");
        textView->SetDefaultBrush("list_background");
        textView->SetHighlightBrush("list_selection");
        layout->AttachWidget(textView, nGuiFormLayout::Top, this->refStatesLabel, border);
        layout->AttachForm(textView, nGuiFormLayout::Left, border);
        layout->AttachPos(textView, nGuiFormLayout::Right, 0.4f);
        layout->AttachForm(textView, nGuiFormLayout::Bottom, border);
        textView->OnShow();
        this->refAnimStates = textView;

        textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "ChnLabel");
        n_assert(textLabel);
        textLabel->SetText("WeightChannels:");
        textLabel->SetFont("GuiSmall");
        textLabel->SetAlignment(nGuiTextLabel::Left);        
        textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        textLabel->SetMinSize(textMinSize);
        textLabel->SetMaxSize(textMaxSize);
        layout->AttachWidget(textLabel, nGuiFormLayout::Left, this->refStatesLabel, border);
        layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refAmbientSlider, 0.025f);
        layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
        textLabel->OnShow();
        this->refChnLabel = textLabel;
        // Create Channel Sliders
        this->UpdateChnSlider();   
        windowRect.set(vector2(0.0f, 0.0f), vector2(0.4f, 0.5f));
    }

    kernelServer->PopCwd();

    // set new window rect   
    this->SetRect(windowRect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnHide()
{
    this->refLightDirection->Release();
    this->refLightHeight->Release();
    this->refDiffuseSlider->Release();
    this->refSpecularSlider->Release();
    this->refAmbientSlider->Release();
    if (this->refSkinAnimator.isvalid())
    {
        this->refSkinAnimator.invalidate();
    }
    if (this->refStatesLabel.isvalid())
    {
        this->refStatesLabel->Release();
    }
    if (this->refChnLabel.isvalid())
    {
        this->refChnLabel->Release();
    }
    if (this->refAnimStates.isvalid())
    {
        this->refAnimStates->Release();
    }

    for (int countChn = 0; countChn < this->refWeightChnListSlider.Size(); countChn++)
    {
        if (this->refWeightChnListSlider.At(countChn))
        {
            this->refWeightChnListSlider.At(countChn)->Release();
        }
    }
/*
    if (this->refSkyEditor.isvalid())
    {
        this->refSkyEditor->Release();
    }
*/
    
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSceneControlWindow::OnEvent(const nGuiEvent& event)
{   
    if (event.GetType() == nGuiEvent::SliderChanged)
    {
        if (this->refLightDirection.isvalid() && 
            this->refLightHeight.isvalid() &&
            this->refDiffuseSlider.isvalid() && 
            this->refSpecularSlider.isvalid() &&
            this->refAmbientSlider.isvalid())
        {                
            if ((event.GetWidget() == this->refLightDirection) ||  (event.GetWidget() == this->refLightHeight))
            {
                this->UpdateLightPosition();               
            }    
            if (event.GetWidget() == this->refDiffuseSlider)
            {
                this->diffuseColor = this->refDiffuseSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightDiffuse, this->diffuseColor);
                }
            }
            if (event.GetWidget() == this->refSpecularSlider)
            {
                this->specularColor = this->refSpecularSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightSpecular, this->specularColor);
                }
            }
            if (event.GetWidget() == this->refAmbientSlider)
            {
                this->ambientColor = this->refAmbientSlider->GetColor();
                if (this->refLight.isvalid())
                {
                    this->refLight->SetVector(nShaderState::LightAmbient, this->ambientColor);
                }
            }
        }
        if (this->refSkinAnimator.isvalid())
        {
            // Update animation channels from sliders
            for (int countChnSlider = 0; countChnSlider< this->refWeightChnListSlider.Size(); countChnSlider++)
            {
                nGuiHoriSliderGroup* slider  = this->refWeightChnListSlider.At(countChnSlider);
                if (slider != NULL)
                {
                    if (event.GetWidget() == slider)
                    {
                        nString rightText;
                        rightText.SetFloat(((float)slider->GetValue())/100.0f);
                        rightText.TerminateAtIndex(4);
                        slider->SetRightText(rightText.Get()); 
                        nVariableServer::Instance()->SetFloatVariable(this->chnHandles.At(countChnSlider),(float)slider->GetValue()/100.0f);
                    }
                }
            }
        }
    }
    
    // handle animation state change
    if (event.GetType() == nGuiEvent::SelectionChanged)
    {
        if (this->refAnimStates.isvalid() && 
            this->refSkinAnimator.isvalid() && 
            (event.GetWidget() == this->refAnimStates))
        {
            nVariable::Handle varHandle;
            varHandle = nVariableServer::Instance()->GetVariableHandleByName("chnCharState");
            nVariableServer::Instance()->SetIntVariable(varHandle, this->refAnimStates->GetSelectionIndex());
            this->UpdateChnSlider();
        }
    }

/*
    if (this->refSkyEditor.isvalid())
    {
        this->refSkyEditor->OnEvent(event);
    }
*/
    
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    called per frame when parent widget is visible   
*/
void
nGuiSceneControlWindow::OnFrame()
{
    // Build up textview for animstate selection
    if (this->refSkinAnimator.isvalid())
    {
        this->refAnimStates->BeginAppend();         
        this->numAnimStates = this->refSkinAnimator->GetNumStates();
        for (int countAnimStates = 0; countAnimStates < this->numAnimStates; countAnimStates++)
        {
            const nAnimState* state = &this->refSkinAnimator->GetStateAt(countAnimStates);
            this->refAnimStates->AppendLine(state->GetName().Get());
        }
        this->refAnimStates->EndAppend();
        // Set the correct selection from current nAnimState
        nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName("chnCharState");
        const nVariableContext variableContext = nVariableServer::Instance()->GetGlobalVariableContext();
        nVariable* var = variableContext.GetVariable(varHandle);
        if(var)
        {
            int curState = nVariableServer::Instance()->GetIntVariable(varHandle);
            this->refAnimStates->SetSelectionIndex(curState);
        }        
     }

    nGuiClientWindow::OnFrame();   

    // if nskinanimator is released because of loading a new non-animation scene, 
    // open a new sceneconrtrolwindow and close the old one
    if ((!this->refSkinAnimator.isvalid()) && (this->skinAnimatorLoaded))
    {
        nGuiServer::Instance()->NewWindow("nguiscenecontrolwindow", true);        
        this->SetCloseRequested(true);
        this->skinAnimatorLoaded = false;
    }
}

//------------------------------------------------------------------------------
/**
    Update Light Rotation and Height.
*/
void
nGuiSceneControlWindow::UpdateLightPosition()
{
    if (this->refLightTransform.isvalid())
    {
        this->lightAngles = vector3(n_deg2rad(this->refLightHeight->GetValue()), n_deg2rad(this->refLightDirection->GetValue()), 0.0f);
        this->refLightTransform->SetEuler(this->lightAngles);
    }
}
//------------------------------------------------------------------------------
/**
    Recursively Find instance of nClass, used to search for lights, skin 
    animators, etc...
*/
nRoot*
nGuiSceneControlWindow::FindFirstInstance(nRoot* node, nClass* classType)
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
    Update Channel Slider
*/
void
nGuiSceneControlWindow::UpdateChnSlider()
{       
    // Release old sliders before creating new one from the scratch
    for (int chnCount = 0; chnCount < this->refWeightChnListSlider.Size(); chnCount++)
    {
        this->refWeightChnListSlider.At(chnCount)->OnHide();
    }
    this->chnHandles.Clear();
    refWeightChnListSlider.Clear();

    nGuiFormLayout* layout = this->refFormLayout;
    kernelServer->PushCwd(layout);

    const float leftWidth = 0.25f;
    const float rightWidth = 0.25f;
    const int knobSize = 60;
    const float border = 0.005f;
    const vector2 colorLabelSize(0.05f,0.02f);
            
    nAnimState animState;               
    animState = this->refSkinAnimator->GetStateAt(this->refAnimStates->GetSelectionIndex());
    int numAnimClips = animState.GetNumClips();
    // Get all weight channels of current AnimState
    for (int animClipCount = 0; animClipCount<numAnimClips; animClipCount++)
    {
        nAnimClip& animClip = animState.GetClipAt(animClipCount);
        this->chnHandles.Append(animClip.GetWeightChannelHandle());
    }        

    // create all sliders
    nGuiHoriSliderGroup* prevSlider;
    for (int chnCount = 0; chnCount < this->chnHandles.Size(); chnCount++)
    {
        nString chnName (nVariableServer::Instance()->GetVariableName(this->chnHandles.At(chnCount)));
        nGuiHoriSliderGroup* slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", chnName.Get());
        slider->SetLeftText(chnName.Get());
        slider->SetMinValue(0);
        slider->SetMaxValue(100);  
        slider->SetValue(100);
        nString rightText;
        rightText.SetFloat(((float)slider->GetValue()/100.0f));
        rightText.TerminateAtIndex(4);
        slider->SetRightText(rightText.Get());          
        slider->SetKnobSize(float(knobSize));
        slider->SetLeftWidth(leftWidth);
        slider->SetRightWidth(rightWidth);
        if (chnCount == 0)
        {
            layout->AttachWidget(slider, nGuiFormLayout::Top, this->refChnLabel, border);
        }
        else
        {
            layout->AttachWidget(slider, nGuiFormLayout::Top, prevSlider, border);
        }  
        layout->AttachWidget(slider, nGuiFormLayout::Left, this->refAnimStates, border);                
        layout->AttachForm(slider, nGuiFormLayout::Right, border);                 
        slider->OnShow();
        prevSlider = slider;
        refWeightChnListSlider.Append(slider);            
    }
    kernelServer->PopCwd();

    // update all layouts
    this->UpdateLayout(this->GetRect());
}
