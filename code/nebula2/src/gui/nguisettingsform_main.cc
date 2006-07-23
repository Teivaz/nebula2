//------------------------------------------------------------------------------
//  nguisettingsform_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguisettingsform.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguitextlabel.h"
#include "gui/nguitextbutton.h"
#include "gui/nguicheckbuttongroup2.h"
#include "misc/nprefserver.h"

nNebulaClass(nGuiSettingsForm, "gui::nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiSettingsForm::nGuiSettingsForm() :
    okText("Ok"),
    cancelText("Cancel"),
    optionValuesValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSettingsForm::~nGuiSettingsForm()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsForm::AddChoiceOption(const nString& name, const nString& labelText, const nArray<nString>& options, int defaultOptionIndex)
{
    struct ChoiceOption newChoice;
    newChoice.name = name;
    newChoice.labelText = labelText;
    newChoice.options = options;
    newChoice.defaultOptionIndex = defaultOptionIndex;
    this->choiceOptions.Append(newChoice);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsForm::AddSliderOption(const nString& name, const nString& labelText, int minVal, int maxVal, int defaultVal)
{
    struct SliderOption newSlider;
    newSlider.name = name;
    newSlider.labelText = labelText;
    newSlider.minVal = minVal;
    newSlider.maxVal = maxVal;
    newSlider.defaultVal = defaultVal;
    this->sliderOptions.Append(newSlider);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsForm::OnShow()
{
    nGuiFormLayout::OnShow();

    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();

    const float dividerPos = 0.4f;
    const float verticalDist = 0.01f;

    kernelServer->PushCwd(this);

    vector2 labelMinSize(0.0f, 0.0f);
    vector2 labelMaxSize(1.0f, 1.0f);

    // create the choice options
    int i;
    int num = this->choiceOptions.Size();
    for (i = 0; i < num; i++)
    {
        ChoiceOption& choiceOption = this->choiceOptions[i];

        // build label
        nString labelName = "ChoiceLabel";
        labelName.AppendInt(i);
        nGuiTextLabel* label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", labelName.Get());
        label->SetText(choiceOption.labelText.Get());
        label->SetFont(skin->GetLabelFont());
        labelMaxSize.y = label->GetTextExtent().y;
        label->SetMinSize(labelMinSize);
        label->SetMaxSize(labelMaxSize);
        label->SetAlignment(nGuiTextLabel::Right);
        label->SetColor(skin->GetLabelTextColor());
        this->AttachForm(label, Left, 0.0f);
        this->AttachPos(label, Right, dividerPos);
        if (i > 0)
        {
            this->AttachWidget(label, Top, this->choiceOptions[i - 1].refLabel, verticalDist);
        }
        else
        {
            this->AttachForm(label, Top, 0.0f);
        }
        label->OnShow();
        choiceOption.refLabel = label;

        // build checkbutton
        nString checkButtonsName = "CheckButton";
        checkButtonsName.AppendInt(i);
        nGuiCheckButtonGroup2* checkButtons = (nGuiCheckButtonGroup2*) kernelServer->New("nguicheckbuttongroup", checkButtonsName.Get());
        int optionIndex;
        int numOptions = choiceOption.options.Size();
        for (optionIndex = 0; optionIndex < numOptions; optionIndex++)
        {
            checkButtons->AppendOption(choiceOption.options[optionIndex].Get());
        }
        this->AttachWidget(checkButtons, VCenter, choiceOption.refLabel, 0.0f);
        this->AttachPos(checkButtons, Left, dividerPos);
        checkButtons->OnShow();
        choiceOption.refCheckButtons = checkButtons;
    }

    // create the slider options
    num = this->sliderOptions.Size();
    for (i = 0; i < num; i++)
    {
        SliderOption& sliderOption = this->sliderOptions[i];

        // build label
        nString labelName = "SliderLabel";
        labelName.AppendInt(i);
        nGuiTextLabel* label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", labelName.Get());
        label->SetText(sliderOption.labelText.Get());
        label->SetFont(skin->GetLabelFont());
        labelMaxSize.y = label->GetTextExtent().y;
        label->SetMinSize(labelMinSize);
        label->SetMaxSize(labelMaxSize);
        label->SetAlignment(nGuiTextLabel::Right);
        label->SetColor(skin->GetLabelTextColor());
        this->AttachForm(label, Left, 0.0f);
        this->AttachPos(label, Right, dividerPos);
        if (i > 0)
        {
            this->AttachWidget(label, Top, this->sliderOptions[i - 1].refLabel, verticalDist);
        }
        else
        {
            this->AttachWidget(label, Top, this->choiceOptions.Back().refLabel, verticalDist);
        }
        label->OnShow();
        sliderOption.refLabel = label;

        // build slider
        nString sliderName = "Slider";
        sliderName.AppendInt(i);
        nGuiHoriSliderGroup* slider;
        slider = (nGuiHoriSliderGroup*) kernelServer->New("nguihorislidergroup", sliderName.Get());
        slider->SetLeftText("");
        slider->SetRightText("%d");
        slider->SetLeftWidth(0.0f);
        slider->SetRightWidth(0.1f);
        slider->SetMinValue(float(sliderOption.minVal));
        slider->SetMaxValue(float(sliderOption.maxVal));
        slider->SetValue(0.0f);
        slider->SetKnobSize((sliderOption.maxVal - sliderOption.minVal) / 10.0f);
        slider->SetLabelFont(skin->GetLabelFont());
        this->AttachWidget(slider, nGuiFormLayout::VCenter, sliderOption.refLabel, 0.0f);
        this->AttachPos(slider, nGuiFormLayout::Left, dividerPos);
        this->AttachForm(slider, nGuiFormLayout::Right, 0.0f);
        slider->OnShow();
        sliderOption.refSlider = slider;
    }

    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // create Ok text button
    nGuiTextButton* textButton;
    textButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "OkButton");
    textButton->SetText(this->GetOkText().Get());
    textButton->SetFont(skin->GetButtonFont());
    textButton->SetAlignment(nGuiTextButton::Center);
    textButton->SetDefaultBrush("button_n");
    textButton->SetPressedBrush("button_p");
    textButton->SetHighlightBrush("button_h");
    textButton->SetMinSize(buttonSize);
    textButton->SetMaxSize(buttonSize);
    textButton->SetColor(skin->GetButtonTextColor());
    this->AttachForm(textButton, nGuiFormLayout::Left, 0.005f);
    this->AttachForm(textButton, nGuiFormLayout::Bottom, 0.005f);
    textButton->OnShow();
    this->refOkButton = textButton;

    // create Cancel text button
    textButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "CancelButton");
    textButton->SetText(this->GetCancelText().Get());
    textButton->SetFont(skin->GetButtonFont());
    textButton->SetAlignment(nGuiTextButton::Center);
    textButton->SetDefaultBrush("button_n");
    textButton->SetPressedBrush("button_p");
    textButton->SetHighlightBrush("button_h");
    textButton->SetMinSize(buttonSize);
    textButton->SetMaxSize(buttonSize);
    textButton->SetColor(skin->GetButtonTextColor());
    this->AttachForm(textButton, nGuiFormLayout::Right, 0.005f);
    this->AttachForm(textButton, nGuiFormLayout::Bottom, 0.005f);
    textButton->OnShow();
    this->refCancelButton = textButton;

    kernelServer->PopCwd();

    // register as event listener because we need to listen for events from our slider
    nGuiServer::Instance()->RegisterEventListener(this);

    // update the UI from current settings
    this->UpdateUiFromSettings();

    this->optionValuesValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsForm::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);
    this->ClearAttachRules();

    int i;
    int num = this->choiceOptions.Size();
    for (i = 0; i < num; i++)
    {
        this->choiceOptions[i].refLabel->Release();
        this->choiceOptions[i].refCheckButtons->Release();
    }
    num = this->sliderOptions.Size();
    for (i = 0; i < num; i++)
    {
        this->sliderOptions[i].refLabel->Release();
        this->sliderOptions[i].refSlider->Release();
    }
    this->refOkButton->Release();
    this->refCancelButton->Release();

    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsForm::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::Action)
    {
        if (this->refOkButton == event.GetWidget())
        {
            // ok button pressed, make settings persistent...
            this->UpdateSettingsFromUi();

            // validate the GetChoiceOption() and GetSliderOption() methods
            this->optionValuesValid = true;

            // generate a DialogOk event
            nGuiEvent dialogOkEvent(this, nGuiEvent::DialogOk);
            nGuiServer::Instance()->PutEvent(dialogOkEvent);
        }
        else if (this->refCancelButton == event.GetWidget())
        {
            // cancel button pressed, generate a DialogCancel event
            nGuiEvent dialogCancelEvent(this, nGuiEvent::DialogCancel);
            nGuiServer::Instance()->PutEvent(dialogCancelEvent);
        }
    }
    nGuiFormLayout::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Read current setting values, and update the user interface from the
    current settings. If settings don't exist, use default values.
*/
void
nGuiSettingsForm::UpdateUiFromSettings()
{
    nPrefServer* prefServer = nPrefServer::Instance();
    prefServer->SetCompanyName(this->GetCompanyName());
    prefServer->SetApplicationName(this->GetAppName());
    if (prefServer->Open())
    {
        // handle choice options
        int i;
        int num = this->choiceOptions.Size();
        for (i = 0; i < num; i++)
        {
            const ChoiceOption& choiceOption = this->choiceOptions[i];
            if (prefServer->KeyExists(choiceOption.name))
            {
                int choiceValue = prefServer->ReadInt(choiceOption.name);
                choiceOption.refCheckButtons->SetSelection(choiceValue);
            }
            else
            {
                // no prefs item exists yet, set to default value
                choiceOption.refCheckButtons->SetSelection(choiceOption.defaultOptionIndex);
            }
        }

        // handle choice sliders
        num = this->sliderOptions.Size();
        for (i = 0; i < num; i++)
        {
            const SliderOption& sliderOption = this->sliderOptions[i];
            if (prefServer->KeyExists(sliderOption.name))
            {
                int sliderValue = prefServer->ReadInt(sliderOption.name);
                sliderOption.refSlider->SetValue(float(sliderValue));
            }
            else
            {
                // no prefs item exists yet, set to default value
                sliderOption.refSlider->SetValue(float(sliderOption.defaultVal));
            }
        }
        prefServer->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Read the current UI values and write them to the prefs server.
*/
void
nGuiSettingsForm::UpdateSettingsFromUi()
{
    nPrefServer* prefServer = nPrefServer::Instance();
    prefServer->SetCompanyName(this->GetCompanyName());
    prefServer->SetApplicationName(this->GetAppName());
    if (prefServer->Open())
    {
        this->optionValuesValid = true;

        // handle choice options
        int i;
        int num = this->choiceOptions.Size();
        for (i = 0; i < num; i++)
        {
            const ChoiceOption& choiceOption = this->choiceOptions[i];
            prefServer->WriteInt(choiceOption.name, choiceOption.refCheckButtons->GetSelection());
        }
        num = this->sliderOptions.Size();
        for (i = 0; i < num; i++)
        {
            const SliderOption& sliderOption = this->sliderOptions[i];
            prefServer->WriteInt(sliderOption.name, int(sliderOption.refSlider->GetValue()));
        }
        prefServer->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Get preferences choice value. Value is only valid after
    the DialogOk event is sent from this form widget.
*/
const nString&
nGuiSettingsForm::GetChoiceOption(const nString& name) const
{
    n_assert2(this->optionValuesValid, "nGuiSettingsForm values only valid after nGuiEvent::DialogOk event!");
    nPrefServer* prefServer = nPrefServer::Instance();
    prefServer->SetCompanyName(this->GetCompanyName());
    prefServer->SetApplicationName(this->GetAppName());
    if (prefServer->Open())
    {
        int index = this->FindChoiceOptionIndexByName(name);
        n_assert2(index != -1, "nGuiSettingsForm::GetChoiceOption(): invalid option name!");
        n_assert(prefServer->KeyExists(name));
        const nString& optionString = this->choiceOptions[index].options[prefServer->ReadInt(name)];
        prefServer->Close();
        return optionString;
    }
    // can't happen
    static const nString dummyStr = "Can't Happen";
    return dummyStr;
}

//------------------------------------------------------------------------------
/**
    Get preferences slider option value. Value is only valid after
    the DialogOk event is sent from this form widget.
*/
int
nGuiSettingsForm::GetSliderOption(const nString& name) const
{
    n_assert2(this->optionValuesValid, "nGuiSettingsForm values only valid after nGuiEvent::DialogOk event!");
    nPrefServer* prefServer = nPrefServer::Instance();
    prefServer->SetCompanyName(this->GetCompanyName());
    prefServer->SetApplicationName(this->GetAppName());
    if (prefServer->Open())
    {
        int index = this->FindSliderOptionIndexByName(name);
        n_assert2(index != -1, "nGuiSettingsForm::GetSliderOption(): invalid option name!");
        n_assert(prefServer->KeyExists(name));
        int sliderValue = prefServer->ReadInt(name);
        prefServer->Close();
        return sliderValue;
    }
    // can't happen
    return -1;
}

//------------------------------------------------------------------------------
/**
    Find a choice option index by name, return -1, if not found.
*/
int
nGuiSettingsForm::FindChoiceOptionIndexByName(const nString& name) const
{
    int i;
    int num = this->choiceOptions.Size();
    for (i = 0; i < num; i++)
    {
        if (name == this->choiceOptions[i].name)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Find a slider option index by name, return -1, if not found.
*/
int
nGuiSettingsForm::FindSliderOptionIndexByName(const nString& name) const
{
    int i;
    int num = this->choiceOptions.Size();
    for (i = 0; i < num; i++)
    {
        if (name == this->sliderOptions[i].name)
        {
            return i;
        }
    }
    return -1;
}




