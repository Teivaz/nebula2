//------------------------------------------------------------------------------
//  nguisettingsmanagementwindow_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguisettingsmanagementwindow.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguicolorslidergroup.h"
#include "gui/nguisettingsfiledialog.h"
#include "gui/nguiscenecontrolwindow.h"
#include "gui/nguimessagebox.h"
#include "gui/nguitextentry.h"
#include "gui/nguitextview.h"
#include "gui/nguiadjustdisplaywindow.h"
#include "variable/nvariableserver.h"
#include "util/nstream.h"
#include "misc/nprefserver.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nGuiSettingsManagementWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
// constant strings
const char * displayAdjustmentString    = "Display Adjustment";
const char * sceneControlString         = "Scene Control";
const char * sliderString               = "Slider";
const char * colorSliderString          = "ColorSlider";

// display adjustment keys for registry
const char * daSaturateSlider           = "DisplaySetting.Saturate.Slider";
const char * daBalanceColorSlider       = "DisplaySetting.Balance.ColorSlider";
const char * daHDRIntesitySlider        = "DisplaySetting.HDR_Intensity.Slider";
const char * daHDRThresholdSlider       = "DisplaySetting.HDR_Threshold.Slider";
const char * daHDROffsetSlider          = "DisplaySetting.HDR_Offset.Slider";
const char * daFogColorSlider           = "DisplaySetting.Fog_Color.ColorSlider";
const char * daFogNearSlider            = "DisplaySetting.Fog_Near.Slider";
const char * daFogFarSlider             = "DisplaySetting.Fog_Far.Slider";
const char * daFocusDistSlider          = "DisplaySetting.Focus_Dist.Slider";
const char * daFocusLengthSlider        = "DisplaySetting.Focus_Length.Slider";
const char * daNoiseItensitySlider      = "DisplaySetting.Noise_Intensity.Slider";
const char * daNoiseScaleSlider         = "DisplaySetting.Noise_Scale.Slider";
const char * daNoiseFreqSlider          = "DisplaySetting.Noise_Freq.Slider";

// scene control keys for registry
const char * scAmbientColorSlider       = "SceneControl.Ambient_Color.ColorSlider";
const char * scDiffueColorSlider        = "SceneControl.Diffuse_Color.ColorSlider";
const char * scSpeculaColorSlider       = "SceneControl.Specular_Color.ColorSlider";
const char * scLightDirectionSlider     = "SceneControl.Light_Direction.Slider";
const char * scLightHeightSlider        = "SceneControl.Light_Height.Slider";

// scene controls keys for xml
const char * scXMLRoot                  = "Scene_Control";
const char * scXMLAmbientColor          = "Ambient_Color";
const char * scXMLDiffuceColor          = "Diffuse_Color";
const char * scXMLSpecularColor         = "Specular_Color";
const char * scXMLLightDirection        = "Light_Direction";
const char * scXMLLightHeight           = "Light_Height";

// display adjustments keys for xml
const char * daXMLRoot                  = "Display_Adjustments";
const char * daXMLSaturate              = "Saturate";
const char * daXMLBalance               = "Balance";
const char * daXMLHDRIntesity           = "HDR_Intensity";
const char * daXMLHDRThreshold          = "HDR_Threshold";
const char * daXMLHDROffset             = "HDR_Offset";
const char * daXMLFogColor              = "Fog_Color";
const char * daXMLFogNear               = "Fog_Near";
const char * daXMLFogFar                = "Fog_Far";
const char * daXMLFocusDist             = "Focus_Dist";
const char * daXMLFocusLenght           = "Focus_Length";
const char * daXMLNoiseIntesity         = "Noise_Intensity";
const char * daXMLNoiseScale            = "Noise_Scale";
const char * daXMLNoiseFreq             = "Noise_Freq";

// char for directory
const char * defaultDirectory           = "user:options/";
//------------------------------------------------------------------------------
/**
*/
nGuiSettingsManagementWindow::nGuiSettingsManagementWindow()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiSettingsManagementWindow::~nGuiSettingsManagementWindow()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsManagementWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    this->SetResizable(true);
    this->SetTitle("Settings Management");
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // constants...
    const float border = 0.005f;

    // text label for display adjustment settings
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "WindowSettingsSelcetionLabel");
    textLabel->SetText("Select control set");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Center);
    vector2 textSize = textLabel->GetTextExtent();
    vector2 textMinSize(0.0f, textSize.y * 1.25f);
    vector2 textMaxSize(1.0f, textSize.y * 1.25f);
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Top, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
    textLabel->OnShow();
    this->refWindowSelectionLabel = textLabel;

    // create text view field for widget selection
    nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "WindowSettingsSelectionTextView");
    n_assert(textView);
    textView->SetSelectionEnabled(true);
    textView->SetHighlightBrush("textentry_h");
    textView->SetDefaultBrush("list_background");
    textView->SetHighlightBrush("list_selection");
    textView->SetMaxSize(vector2(1.0f, 0.1f));
    layout->AttachWidget(textView, nGuiFormLayout::Top, this->refWindowSelectionLabel, border);
    layout->AttachForm(textView, nGuiFormLayout::Left, border);
    layout->AttachForm(textView, nGuiFormLayout::Right, border);
    textView->OnShow();
    this->refWindowSelectionTextView = textView;

    // text label for display adjustment settings
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "FileLabel");
    textLabel->SetText("Specify file for save/load");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Center);
    textSize = textLabel->GetTextExtent();
    textMinSize.y = textSize.y * 1.25f;
    textMaxSize.y = textSize.y * 1.25f;
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, border);
    layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refWindowSelectionTextView, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
    textLabel->OnShow();
    this->refFileLabel = textLabel;

    // browse button, with new size
    buttonSize.x = 0.03f;
    nGuiTextButton* btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "BrowseButton");
    btn->SetText("...");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    layout->AttachForm(btn, nGuiFormLayout::Right, border);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refFileLabel, border);
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    btn->OnShow();
    this->refBrowseButton = btn;

    // text label, for filename
    nGuiTextEntry * textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "FilenameEntry");
    textEntry->SetText("");
    textEntry->SetFont("GuiSmall");
    textEntry->SetAlignment(nGuiTextEntry::Left);
    textEntry->SetDefaultBrush("textentry_n");
    textEntry->SetPressedBrush("textentry_p");
    textEntry->SetHighlightBrush("textentry_h");
    textEntry->SetCursorBrush("textcursor");
    buttonSize.x = 0.3f;
    textEntry->SetMinSize(buttonSize);
    buttonSize.x = 1.0f;
    textEntry->SetMaxSize(buttonSize);
    textEntry->SetFileMode(true);
    textEntry->SetInitialCursorPos(nGuiTextLabel::Right);
    layout->AttachWidget(textEntry, nGuiFormLayout::Top, this->refFileLabel, border);
    layout->AttachForm(textEntry, nGuiFormLayout::Left, border);
    layout->AttachWidget(textEntry, nGuiFormLayout::Right, this->refBrowseButton, border);
    textEntry->OnShow();
    this->refFilenameEntry = textEntry;

    this->AddSupportedWindowsToList();

    // load button
    buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "LoadButton");
    btn->SetText("Load");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachForm(btn, nGuiFormLayout::Left, border);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refFilenameEntry, border);
    btn->OnShow();
    this->refLoadButton = btn;

    // save button
    btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "SaveButton");
    btn->SetText("Save as");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachForm(btn, nGuiFormLayout::Right, border);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refFilenameEntry, border);
    btn->OnShow();
    this->refSaveButton = btn;

    // text label for default settings
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "DefaultLabel");
    textLabel->SetText("Manage default settings");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Center);
    textSize = textLabel->GetTextExtent();
    textMinSize.y = textSize.y * 1.25f;
    textMaxSize.y = textSize.y * 1.25f;
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, border);
    layout->AttachWidget(textLabel, nGuiFormLayout::Top, this->refSaveButton, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
    textLabel->OnShow();
    this->refDefaultLabel = textLabel;

    // reset button
    btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "ResetButton");
    btn->SetText("Reset");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refDefaultLabel, border);
    layout->AttachForm(btn, nGuiFormLayout::Left, border);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, border);
    btn->OnShow();
    this->refResetButton = btn;

    // default button
    btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", "SetDefaultButton");
    btn->SetText("Set default");
    btn->SetDefaultBrush("button_n");
    btn->SetPressedBrush("button_p");
    btn->SetHighlightBrush("button_h");
    btn->SetMinSize(buttonSize);
    btn->SetMaxSize(buttonSize);
    layout->AttachWidget(btn, nGuiFormLayout::Top, this->refDefaultLabel, border);
    layout->AttachForm(btn, nGuiFormLayout::Right, border);
    btn->OnShow();
    this->refSetDefaultButton = btn;

    // text label for default settings
    textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "InfoLabel");
    textLabel->SetText("");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Center);
    textSize = textLabel->GetTextExtent();
    textMinSize.y = textSize.y * 1.25f;
    textMaxSize.y = textSize.y * 2.0f;
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
    layout->AttachForm(textLabel, nGuiFormLayout::Bottom, border);
    textLabel->OnShow();
    this->refInfoLabel = textLabel;

    kernelServer->PopCwd();

    // try to create default directory "home:work/save"
    if (!this->CheckDirAndCreate())
    {
        this->SetInfoText("Default directory could not be created!", true);
    }

    // set new window rect
    float vSize =   11 * (border + this->refLoadButton->GetMaxSize().y) +                   // each button, text entry or label
                    1 * (border + this->refWindowSelectionTextView->GetMinSize().y);        // each textview (with selection)
    rectangle rect(vector2(0.0f, 0.0f), vector2(0.38f, vSize));
    this->SetRect(rect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsManagementWindow::OnHide()
{
    if (this->refWindowSelectionLabel.isvalid())
    {
        this->refWindowSelectionLabel->Release();
    }
    if (this->refWindowSelectionTextView.isvalid())
    {
        this->refWindowSelectionTextView->Release();
    }
    if (this->refResetButton.isvalid())
    {
        this->refResetButton->Release();
    }
    if (this->refSaveButton.isvalid())
    {
        this->refSaveButton->Release();
    }
    if (this->refLoadButton.isvalid())
    {
        this->refLoadButton->Release();
    }
    if (this->refBrowseButton.isvalid())
    {
        this->refBrowseButton->Release();
    }
    if (this->refSetDefaultButton.isvalid())
    {
        this->refSetDefaultButton->Release();
    }
    if (this->refFilenameEntry.isvalid())
    {
        this->refFilenameEntry->Release();
    }
    if (this->refDefaultLabel.isvalid())
    {
        this->refDefaultLabel->Release();
    }
    if (this->refFileLabel.isvalid())
    {
        this->refFileLabel->Release();
    }
    if (this->refMessageBox.isvalid())
    {
        this->refMessageBox->Release();
    }
    if (this->refInfoLabel.isvalid())
    {
        this->refInfoLabel->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSettingsManagementWindow::OnEvent(const nGuiEvent& event)
{
    if ((event.GetType() == nGuiEvent::ButtonUp) &&
       (event.GetWidget() == this->refResetButton))
    {
        this->ResetValues();
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) &&
            (event.GetWidget() == this->refBrowseButton))
    {
        this->CreateFileDialog();
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) &&
            (event.GetWidget() == this->refLoadButton))
    {
        if (!strlen(refFilenameEntry->GetText()) == 0)
        {
            this->LoadValuesFromFile(refFilenameEntry->GetText());
        }
        else
        {
            this->SetInfoText("Please specify a filename");
        }
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) &&
            (event.GetWidget() == this->refSaveButton))
    {
        if (!strlen(refFilenameEntry->GetText()) == 0)
        {
            this->SaveValuesToFile(refFilenameEntry->GetText());
        }
        else
        {
            this->SetInfoText("Please specify a filename");
        }
    }
    else if ((event.GetType() == nGuiEvent::ButtonUp) &&
            (event.GetWidget() == this->refSetDefaultButton))
    {
        this->SaveValuesAsDefault();
    }

    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Reset display adjustment values.

    Opens the preferences server and calls methods to readvalues from server
    and assing values to window
*/
void
nGuiSettingsManagementWindow::ResetValues()
{
    // temp security var
    int iCount = 0;

    // first try to load values from registry
    nPrefServer* prefServer = nPrefServer::Instance();
    if (!prefServer->IsOpen())
    {
        if (prefServer->Open())
        {
            if (strcmp(refWindowSelectionTextView->GetSelection(), sceneControlString) == 0)
            {
                if (false == (this->ResetSceneControls(prefServer)))
                {
                    // error occured
                    prefServer->Close();
                    return;
                }
            }
            else if (strcmp(refWindowSelectionTextView->GetSelection(), displayAdjustmentString) == 0)
            {
                if (false == (this->ResetDisplayAdjustments(prefServer)))
                {
                    // error occured
                    prefServer->Close();
                    return;
                }
            }
            else
            {
                prefServer->Close();
                this->SetInfoText("No window/controls set has been selected!\n Please select one from the upper list.");
                return;
            }
        }
        else
        {
            this->SetInfoText("Registry could not be opened!\nAborting!", true);
            return;
        }
    }

    // all went good
    prefServer->Close();
    this->SetInfoText("Registry successfully read!");
}

//------------------------------------------------------------------------------
/**
    Resets the values for the display adjustments window
    Returns true if everything is ok
*/
bool
nGuiSettingsManagementWindow::ResetDisplayAdjustments(nPrefServer* prefServer)
{
    // temp var
    int iCount = 0;

    // check paramter
    n_assert(prefServer);

    // get pointer to display adjustments window and check
    nGuiAdjustDisplayWindow* adjustWindow = (nGuiAdjustDisplayWindow*)nGuiServer::Instance()->FindWindowByClass("nguiadjustdisplaywindow", 0);
    if (0 == adjustWindow)
    {
        this->SetInfoText("No 'Display Adjustments' window was found!\nMaybe no one is open?", true);
        return false;
    }

    // check again
    n_assert(adjustWindow);

    // read from registry and assign to window
    if (prefServer->KeyExists(daSaturateSlider))
    {
        adjustWindow->refSaturateSlider->SetValue(prefServer->ReadFloat(daSaturateSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daBalanceColorSlider))
    {
        adjustWindow->refBalanceSlider->SetColor(prefServer->ReadVector4(daBalanceColorSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daHDRIntesitySlider))
    {
        adjustWindow->refHdrBloomIntensitySlider->SetValue(prefServer->ReadFloat(daHDRIntesitySlider));
        iCount++;
    }
    if (prefServer->KeyExists(daHDRThresholdSlider))
    {
        adjustWindow->refHdrBrightPassThresholdSlider->SetValue(prefServer->ReadFloat(daHDRThresholdSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daHDROffsetSlider))
    {
        adjustWindow->refHdrBrightPassOffsetSlider->SetValue(prefServer->ReadFloat(daHDROffsetSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daFogColorSlider))
    {
        adjustWindow->refFogColorSlider->SetColor(prefServer->ReadVector4(daFogColorSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daFogNearSlider))
    {
        adjustWindow->refFogNearSlider->SetValue(prefServer->ReadFloat(daFogNearSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daFogFarSlider))
    {
        adjustWindow->refFogFarSlider->SetValue(prefServer->ReadFloat(daFogFarSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daFocusDistSlider))
    {
        adjustWindow->refFocusDistSlider->SetValue(prefServer->ReadFloat(daFocusDistSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daFocusLengthSlider))
    {
        adjustWindow->refFocusLengthSlider->SetValue(prefServer->ReadFloat(daFocusLengthSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daNoiseItensitySlider))
    {
        adjustWindow->refNoiseIntensitySlider->SetValue(prefServer->ReadFloat(daNoiseItensitySlider));
        iCount++;
    }
    if (prefServer->KeyExists(daNoiseScaleSlider))
    {
        adjustWindow->refNoiseScaleSlider->SetValue(prefServer->ReadFloat(daNoiseScaleSlider));
        iCount++;
    }
    if (prefServer->KeyExists(daNoiseFreqSlider))
    {
        adjustWindow->refNoiseFrequencySlider->SetValue(prefServer->ReadFloat(daNoiseFreqSlider));
        iCount++;
    }

    adjustWindow->UpdateValuesFromSliders();

    // check if all done
    if (13 != iCount)
    {
        this->SetInfoText("The number of read values differs the number of values in dialog!\nMaybe no default settings were never saved!", true);
        return false;
    }

    // everything ok
    return true;
}

//------------------------------------------------------------------------------
/**
    Resets the values for the scene controls window
    Returns true if everything is ok
*/
bool
nGuiSettingsManagementWindow::ResetSceneControls(nPrefServer* prefServer)
{
    // temp var
    int iCount = 0;

    // check paramter
    n_assert(prefServer);

    // get pointer to scene window and check
    nGuiSceneControlWindow* sceneWindow = (nGuiSceneControlWindow*)nGuiServer::Instance()->FindWindowByClass("nguiscenecontrolwindow", 0);
    if (0 == sceneWindow)
    {
        this->SetInfoText("No 'Scene Control' window was found!\nMaybe no one is open?", true);
        return false;
    }

    // check again
    n_assert(sceneWindow);

    // read keys from registry and assign to window
    if (prefServer->KeyExists(scAmbientColorSlider))
    {
        sceneWindow->refAmbientSlider->SetColor(prefServer->ReadVector4(scAmbientColorSlider));
        iCount++;
    }
    if (prefServer->KeyExists(scDiffueColorSlider))
    {
        sceneWindow->refDiffuseSlider->SetColor(prefServer->ReadVector4(scDiffueColorSlider));
        iCount++;
    }
    if (prefServer->KeyExists(scSpeculaColorSlider))
    {
        sceneWindow->refSpecularSlider->SetColor(prefServer->ReadVector4(scSpeculaColorSlider));
        iCount++;
    }
    if (prefServer->KeyExists(scLightDirectionSlider))
    {
        sceneWindow->refLightDirection->SetValue(prefServer->ReadFloat(scLightDirectionSlider));
        iCount++;
    }
    if (prefServer->KeyExists(scLightHeightSlider))
    {
        sceneWindow->refLightHeight->SetValue(prefServer->ReadFloat(scLightHeightSlider));
        iCount++;
    }

     // check if all done
    if (5 != iCount)
    {
        this->SetInfoText("The number of read values differs the number of values in dialog!\nMaybe no default settings were never saved!", true);
        return false;
    }

    // all went good
    return true;
}

//------------------------------------------------------------------------------
/**
    Opens a file dialog
*/
void
nGuiSettingsManagementWindow::CreateFileDialog()
{
    // try to create default directory "home:work/save"
    if (!this->CheckDirAndCreate())
    {
        this->SetInfoText("Default directory could not be created!", true);
        return;
    }

    nGuiSettingsFileDialog* dialog = (nGuiSettingsFileDialog*) nGuiServer::Instance()->NewWindow("nguisettingsfiledialog", true);
    n_assert(dialog);
    dialog->SetSettingWindowRef(this);
}

//------------------------------------------------------------------------------
/**
    Loads specified xml file and assigns values to controls
*/
void
nGuiSettingsManagementWindow::LoadValuesFromFile(nString filename)
{
    // try to create default directory "home:work/save"
    if (!this->CheckDirAndCreate())
    {
        this->SetInfoText("Default directory could not be created!", true);
        return;
    }

    // prepend work directory
    filename = defaultDirectory +filename;

    // check if file exists
    if (!(nFileServer2::Instance()->FileExists(filename)))
    {
        this->SetInfoText("The specified file does not exists!", true);
        return;
    }

    // check if file is a xml file
    if (!filename.CheckExtension("xml"))
    {
        this->SetInfoText("The specified file is no xml file!", true);
        return;
    }

    // create stream and write stuff
    nStream stream;
    stream.SetFilename(filename);
    stream.Open(nStream::Read);

    // open file
    if (!stream.IsOpen())
    {
        this->SetInfoText("The xml file could not be opened, or is not well-formed!", true);
        return;
    }

    // check root element
    if (!stream.HasNode("/Settings"))
    {
        this->SetInfoText("The xml file doesn't contain the root element <Settings>!", true);
        stream.Close();
        return;
    }
    stream.SetToNode("/Settings");

    // check which part should be loaded
    if (strcmp(refWindowSelectionTextView->GetSelection(), sceneControlString) == 0)
    {
        if (false == (this->LoadSceneControlsFromStream(&stream)))
        {
            stream.Close();
            return;
        }
    }
    else if (strcmp(refWindowSelectionTextView->GetSelection(), displayAdjustmentString) == 0)
    {
        if (false == (this->LoadDisplayAdjustmentsFromStream(&stream)))
        {
            stream.Close();
            return;
        }
    }
    else
    {
        this->SetInfoText("No window/controls set has been selected!\n Please select one from the upper list.");
        stream.Close();
        return;
    }

    this->SetInfoText("All settings successfully read!");
    stream.Close();
}

//------------------------------------------------------------------------------
/**
    Reads the scene control contet from given stream
    Assigns the read values to the scene control window
*/
bool
nGuiSettingsManagementWindow::LoadSceneControlsFromStream(nStream* stream)
{
    // temp variables
    int iFoundEntries = 0;

    // get the pointer to window and check
    nGuiSceneControlWindow* sceneWindow = (nGuiSceneControlWindow*)nGuiServer::Instance()->FindWindowByClass("nguiscenecontrolwindow", 0);
    if (0 == sceneWindow)
    {
        this->SetInfoText("No 'Scene Control' window was found!\nMaybe no one is open?", true);
        return false;
    }
    n_assert(sceneWindow);

    // check if the node exists
    if (!stream->HasNode(scXMLRoot))
    {
        this->SetInfoText("No 'Scene Control' window was found!\nMaybe no one is open?", true);
        return false;
    }
    stream->SetToNode(scXMLRoot);

    // try to set first child
    if (stream->SetToFirstChild())
    {
        for (int iCount = 0; iCount < 5; iCount++)
        {
            if (stream->GetCurrentNodeName() == scXMLAmbientColor && stream->GetAttrs()[0] == colorSliderString)
            {
                sceneWindow->refAmbientSlider->SetColor(stream->GetVector4(colorSliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == scXMLDiffuceColor && stream->GetAttrs()[0] == colorSliderString)
            {
                sceneWindow->refDiffuseSlider->SetColor(stream->GetVector4(colorSliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == scXMLSpecularColor && stream->GetAttrs()[0] == colorSliderString)
            {
                sceneWindow->refSpecularSlider->SetColor(stream->GetVector4(colorSliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == scXMLLightDirection && stream->GetAttrs()[0] == sliderString)
            {
                sceneWindow->refLightDirection->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == scXMLLightHeight && stream->GetAttrs()[0] == sliderString)
            {
                sceneWindow->refLightHeight->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }

            // trie to set next child
            if (5 > iFoundEntries)
            {
                if (!stream->SetToNextChild())
                {
                    this->SetInfoText("The childs of the sub element <Scene_Control>\ncontains not the required amount of elements!", true);
                    return false;
                }
            }
        }

        // check if the number of read values equals the number of needed values
        if (5 != iFoundEntries)
        {
            this->SetInfoText("Not all required settings could be read!", true);
            return false;
        }
    }
    else
    {
        this->SetInfoText("The sub element <Scene_Control> contains no childs!", true);
        return false;
    }

    // everything ok
    return true;
}

//------------------------------------------------------------------------------
/**
    Reads the display adjustment contet from given stream
    Assigns the read values to the display adjustment window
*/
bool
nGuiSettingsManagementWindow::LoadDisplayAdjustmentsFromStream(nStream* stream)
{
    // temp variables
    int iFoundEntries = 0;

    // get the pointer to the window and check
    nGuiAdjustDisplayWindow* adjustWindow = (nGuiAdjustDisplayWindow*)nGuiServer::Instance()->FindWindowByClass("nguiadjustdisplaywindow", 0);
    if (0 == adjustWindow)
    {
        this->SetInfoText("No 'Display Adjustments' window was found!\nMaybe no one is open?", true);
        return false;
    }
    n_assert(adjustWindow);

    // check the node, if exists, assign as actuall
    if (!stream->HasNode(daXMLRoot))
    {
        this->SetInfoText("The xml file doesn't contain the \nsub element <DisplayAdjustments>!", true);
        return false;
    }
    stream->SetToNode(daXMLRoot);

    // try to get childs
    if (stream->SetToFirstChild())
    {
        // repeat for each
        for (int iCount = 0; iCount < 13; iCount++)
        {
            if (stream->GetCurrentNodeName() == daXMLSaturate && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refSaturateSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLBalance && stream->GetAttrs()[0] == colorSliderString)
            {
                adjustWindow->refBalanceSlider->SetColor(stream->GetVector4(colorSliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLHDRIntesity && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refHdrBloomIntensitySlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLHDRThreshold && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refHdrBrightPassThresholdSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLHDROffset && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refHdrBrightPassOffsetSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLFogColor && stream->GetAttrs()[0] == colorSliderString)
            {
                adjustWindow->refFogColorSlider->SetColor(stream->GetVector4(colorSliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLFogNear && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refFogNearSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLFogFar && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refFogFarSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLFocusDist && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refFocusDistSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLFocusLenght && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refFocusLengthSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLNoiseIntesity && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refNoiseIntensitySlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLNoiseScale && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refNoiseScaleSlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }
            else if (stream->GetCurrentNodeName() == daXMLNoiseFreq && stream->GetAttrs()[0] == sliderString)
            {
                adjustWindow->refNoiseFrequencySlider->SetValue(stream->GetFloat(sliderString));
                iFoundEntries++;
            }

            // trie to set next child
            if (13 > iFoundEntries)
            {
                if (!stream->SetToNextChild())
                {
                    this->SetInfoText("The childs of the sub element <Display_Adjustments>\ncontains not the required amount of elements!", true);
                    return false;
                }
            }
        }
        // check if the number of read values is correct
        if (13 != iFoundEntries)
        {
            this->SetInfoText("Not all required settings could be read!", true);
            return false;
        }
    }
    else
    {
        this->SetInfoText("The sub element <Display_Adjustments> contains no childs!", true);
        return false;
    }

    // everything ok
    return true;
}

//------------------------------------------------------------------------------
/**
    Gets values from controls and save them into specified xml file
    ATTENTION: All controls will be saved into this file, but they can be read
    spereatly!
*/
void
nGuiSettingsManagementWindow::SaveValuesToFile(nString filename)
{
    // try to create default directory "user:options"
    if (!this->CheckDirAndCreate())
    {
        this->SetInfoText("Default directory could not be created!", true);
        return;
    }

    // get pointer to window
    nGuiSceneControlWindow* sceneWindow = (nGuiSceneControlWindow*)nGuiServer::Instance()->FindWindowByClass("nguiscenecontrolwindow", 0);
    if (0 == sceneWindow)
    {
        this->SetInfoText("No 'Scene Control' window was found!\nMaybe no one is open?", true);
        return;
    }
    n_assert(sceneWindow);

    // get pointer to window
    nGuiAdjustDisplayWindow* adjustWindow = (nGuiAdjustDisplayWindow*)nGuiServer::Instance()->FindWindowByClass("nguiadjustdisplaywindow", 0);
    if (0 == adjustWindow)
    {
        this->SetInfoText("No 'Display Adjustments' window was found!\nMaybe no one is open?", true);
        return;
    }
    n_assert(adjustWindow);

    // check extension
    if (!filename.CheckExtension("xml"))
    {
        filename.StripExtension();
        filename.Append(".xml");

        refFilenameEntry->SetText(filename.Get());
    }

    // prepend work path
    filename = defaultDirectory + filename;

    // create stream and write stuff
    nStream stream;
    stream.SetFilename(filename);
    stream.Open(nStream::Write);
    if (!stream.IsOpen())
    {
        this->SetInfoText("The file could not be opended. Check your permissions!", true);
        return;
    }

    // write root node with attributes for schema
    stream.BeginNode("Settings");
//    stream.SetString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
//    stream.SetString("xsi:noNamespaceSchemaLocation", "schema.xsd");

    // write window content
    this->WriteXMLfromDisplayAdjustments(&stream, adjustWindow);
    this->WriteXMLfromSceneControls(&stream, sceneWindow);

    // close stream
    stream.Close();

    // all went good
    this->SetInfoText("All settings written to\n'" + filename + "'");
}

//------------------------------------------------------------------------------
/**
    Writes the <Display_Adjustments> note to given stream
*/
void
nGuiSettingsManagementWindow::WriteXMLfromDisplayAdjustments(nStream* stream, nGuiAdjustDisplayWindow* adjustWindow)
{
    stream->BeginNode(daXMLRoot);
        stream->BeginNode(daXMLSaturate);
            stream->SetFloat(sliderString,           adjustWindow->refSaturateSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLBalance);
            stream->SetVector4(colorSliderString,    adjustWindow->refBalanceSlider->GetColor());
        stream->EndNode();
        stream->BeginNode(daXMLHDRIntesity);
            stream->SetFloat(sliderString,           adjustWindow->refHdrBloomIntensitySlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLHDRThreshold);
            stream->SetFloat(sliderString,           adjustWindow->refHdrBrightPassThresholdSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLHDROffset);
            stream->SetFloat(sliderString,           adjustWindow->refHdrBrightPassOffsetSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLFogColor);
            stream->SetVector4(colorSliderString,    adjustWindow->refFogColorSlider->GetColor());
        stream->EndNode();
        stream->BeginNode(daXMLFogNear);
            stream->SetFloat(sliderString,           adjustWindow->refFogNearSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLFogFar);
            stream->SetFloat(sliderString,           adjustWindow->refFogFarSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLFocusDist);
            stream->SetFloat(sliderString,           adjustWindow->refFocusDistSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLFocusLenght);
            stream->SetFloat(sliderString,           adjustWindow->refFocusLengthSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLNoiseIntesity);
            stream->SetFloat(sliderString,           adjustWindow->refNoiseIntensitySlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLNoiseScale);
            stream->SetFloat(sliderString,           adjustWindow->refNoiseScaleSlider->GetValue());
        stream->EndNode();
        stream->BeginNode(daXMLNoiseFreq);
            stream->SetFloat(sliderString,           adjustWindow->refNoiseFrequencySlider->GetValue());
        stream->EndNode();
    stream->EndNode();
}

//------------------------------------------------------------------------------
/**
    Writes the <Scene_Control> note to given stream
*/
void
nGuiSettingsManagementWindow::WriteXMLfromSceneControls(nStream* stream, nGuiSceneControlWindow* sceneWindow)
{
    stream->BeginNode(scXMLRoot);
        stream->BeginNode(scXMLAmbientColor);
            stream->SetVector4(colorSliderString,    sceneWindow->refAmbientSlider->GetColor());
        stream->EndNode();
        stream->BeginNode(scXMLDiffuceColor);
            stream->SetVector4(colorSliderString,    sceneWindow->refDiffuseSlider->GetColor());
        stream->EndNode();
        stream->BeginNode(scXMLSpecularColor);
            stream->SetVector4(colorSliderString,    sceneWindow->refSpecularSlider->GetColor());
        stream->EndNode();
        stream->BeginNode(scXMLLightDirection);
            stream->SetFloat(sliderString,           sceneWindow->refLightDirection->GetValue());
        stream->EndNode();
        stream->BeginNode(scXMLLightHeight);
            stream->SetFloat(sliderString,           sceneWindow->refLightHeight->GetValue());
        stream->EndNode();
    stream->EndNode();
}

//------------------------------------------------------------------------------
/**
    Saves the values as default values to the registry
*/
void
nGuiSettingsManagementWindow::SaveValuesAsDefault()
{
    // get pref server, and check if open, if not open
    nPrefServer* prefServer = nPrefServer::Instance();

    if (!prefServer->IsOpen())
    {
        if (prefServer->Open())
        {
            if (strcmp(refWindowSelectionTextView->GetSelection(), sceneControlString) == 0)
            {
                nGuiSceneControlWindow* sceneWindow = (nGuiSceneControlWindow*)nGuiServer::Instance()->FindWindowByClass("nguiscenecontrolwindow", 0);
                if (0 == sceneWindow)
                {
                    this->SetInfoText("No 'Scene Control' window was found!\nMaybe no one is open?", true);
                    prefServer->Close();
                    return;
                }

                prefServer->WriteVector4(scAmbientColorSlider,  sceneWindow->refAmbientSlider->GetColor());
                prefServer->WriteVector4(scDiffueColorSlider,  sceneWindow->refDiffuseSlider->GetColor());
                prefServer->WriteVector4(scSpeculaColorSlider, sceneWindow->refSpecularSlider->GetColor());
                prefServer->WriteFloat(scLightDirectionSlider,       sceneWindow->refLightDirection->GetValue());
                prefServer->WriteFloat(scLightHeightSlider,          sceneWindow->refLightHeight->GetValue());
            }
            else if (strcmp(refWindowSelectionTextView->GetSelection(), displayAdjustmentString) == 0)
            {
                nGuiAdjustDisplayWindow* adjustWindow = (nGuiAdjustDisplayWindow*)nGuiServer::Instance()->FindWindowByClass("nguiadjustdisplaywindow", 0);
                if (0 == adjustWindow)
                {
                    this->SetInfoText("No 'Display Adjustments' window was found!\nMaybe no one is open?", true);
                    prefServer->Close();
                    return;
                }

                adjustWindow->UpdateSlidersFromValues();

                prefServer->WriteFloat(daSaturateSlider,        adjustWindow->refSaturateSlider->GetValue());
                prefServer->WriteVector4(daBalanceColorSlider,  adjustWindow->refBalanceSlider->GetColor());
                prefServer->WriteFloat(daHDRIntesitySlider,   adjustWindow->refHdrBloomIntensitySlider->GetValue());
                prefServer->WriteFloat(daHDRThresholdSlider,   adjustWindow->refHdrBrightPassThresholdSlider->GetValue());
                prefServer->WriteFloat(daHDROffsetSlider,      adjustWindow->refHdrBrightPassOffsetSlider->GetValue());
                prefServer->WriteVector4(daFogColorSlider,adjustWindow->refFogColorSlider->GetColor());
                prefServer->WriteFloat(daFogNearSlider,        adjustWindow->refFogNearSlider->GetValue());
                prefServer->WriteFloat(daFogFarSlider,         adjustWindow->refFogFarSlider->GetValue());
                prefServer->WriteFloat(daFocusDistSlider,      adjustWindow->refFocusDistSlider->GetValue());
                prefServer->WriteFloat(daFocusLengthSlider,    adjustWindow->refFocusLengthSlider->GetValue());
                prefServer->WriteFloat(daNoiseItensitySlider, adjustWindow->refNoiseIntensitySlider->GetValue());
                prefServer->WriteFloat(daNoiseScaleSlider,     adjustWindow->refNoiseScaleSlider->GetValue());
                prefServer->WriteFloat(daNoiseFreqSlider,      adjustWindow->refNoiseFrequencySlider->GetValue());
            }
            else
            {
                this->SetInfoText("No window/controls set has been selected!\n Please select one from the upper list.");
                return;
            }
        }
        else
        {
            this->SetInfoText("Registry could not be opened!", true);
            return;
        }
    }

    // all went good, close the pref server
    prefServer->Close();
    this->SetInfoText("Successfully written to registry!");
}

//------------------------------------------------------------------------------
/**
    Sets the filename wich was selected in filedialog to the text label
*/
void
nGuiSettingsManagementWindow::SetSelectedFilename(const nString filename)
{
    refFilenameEntry->SetText(filename.Get());
}

//------------------------------------------------------------------------------
/**
    Sets the info label with given text
*/
void
nGuiSettingsManagementWindow::SetInfoText(const nString filename, bool criticalError)
{
    // set color to red if this is an critical error
    if (true == criticalError)
    {
        refInfoLabel->SetColor(vector4(0.8f, 0.0f, 0.0f, 1.0f));
    }
    else
    {
        refInfoLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    refInfoLabel->SetText(filename.Get());
}

//------------------------------------------------------------------------------
/**
    Appends a string for each supported window to the list view
*/
void
nGuiSettingsManagementWindow::AddSupportedWindowsToList()
{
    // hard coded, for each supported window
    this->refWindowSelectionTextView->AppendLine(displayAdjustmentString);
    this->refWindowSelectionTextView->AppendLine(sceneControlString);
}

//------------------------------------------------------------------------------
/**
    Checks if the directory exists and creates if it doesnt exist
*/
bool
nGuiSettingsManagementWindow::CheckDirAndCreate()
{
    if (!(nFileServer2::Instance()->DirectoryExists(defaultDirectory)))
    {
        return nFileServer2::Instance()->MakePath(defaultDirectory);
    }
    return true;
}