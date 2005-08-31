#ifndef N_GUISETTINGSFORM_H
#define N_GUISETTINGSFORM_H
//------------------------------------------------------------------------------
/**
    @class nGuiSettingsForm
    @ingroup Gui

    @brief Implements a form layout with user customizable settings widgets.
    Uses persistency server for storing the settings.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"

class nGuiTextLabel;
class nGuiCheckButtonGroup2;
class nGuiHoriSliderGroup;
class nGuiTextButton;

//------------------------------------------------------------------------------
class nGuiSettingsForm : public nGuiFormLayout
{
public:
    /// constructor
    nGuiSettingsForm();
    /// destructor
    virtual ~nGuiSettingsForm();
    /// add a choice settings
    void AddChoiceOption(const nString& name, const nString& labelText, const nArray<nString>& options, int defaultOptionIndex);
    /// add a slider setting
    void AddSliderOption(const nString& name, const nString& labelText, int minVal, int maxVal, int defaultVal);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// set company name
    void SetCompanyName(const nString& n);
    /// get company name
    const nString& GetCompanyName() const;
    /// set application name
    void SetAppName(const nString& n);
    /// get application name
    const nString& GetAppName() const;
    /// set ok text
    void SetOkText(const nString& t);
    /// get ok text
    const nString& GetOkText() const;
    /// set cancel text
    void SetCancelText(const nString& t);
    /// get cancel text
    const nString& GetCancelText() const;
    /// get selected choice option (only after DialogOk event has been sent)
    const nString& GetChoiceOption(const nString& name) const;
    /// get slider option value (only after DialogOk event has been sent)
    int GetSliderOption(const nString& name) const;

private:
    /// update the user interface from the settings
    void UpdateUiFromSettings();
    /// update persistent settings from the user interface
    void UpdateSettingsFromUi();
    /// find choice option index by name
    int FindChoiceOptionIndexByName(const nString& n) const;
    /// find slider option index by name
    int FindSliderOptionIndexByName(const nString& n) const;
        
    struct ChoiceOption
    {
        nString name;
        nString labelText;
        nArray<nString> options;
        int defaultOptionIndex;
        nRef<nGuiTextLabel> refLabel;
        nRef<nGuiCheckButtonGroup2> refCheckButtons;
    };
    struct SliderOption
    {
        nString name;
        nString labelText;
        int minVal;
        int maxVal;
        int defaultVal;
        nRef<nGuiTextLabel> refLabel;
        nRef<nGuiHoriSliderGroup> refSlider;
    };
    nArray<ChoiceOption> choiceOptions;
    nArray<SliderOption> sliderOptions;
    nRef<nGuiTextButton> refOkButton;
    nRef<nGuiTextButton> refCancelButton;
    nString companyName;
    nString appName;
    nString okText;
    nString cancelText;
    bool optionValuesValid;
};

nSetter(nGuiSettingsForm::SetCompanyName, const nString&, companyName);
nGetter(const nString&, nGuiSettingsForm::GetCompanyName, companyName);
nSetter(nGuiSettingsForm::SetAppName, const nString&, appName);
nGetter(const nString&, nGuiSettingsForm::GetAppName, appName);
nSetter(nGuiSettingsForm::SetOkText, const nString&, okText);
nGetter(const nString&, nGuiSettingsForm::GetOkText, okText);
nSetter(nGuiSettingsForm::SetCancelText, const nString&, cancelText);
nGetter(const nString&, nGuiSettingsForm::GetCancelText, cancelText);
//------------------------------------------------------------------------------
#endif    

