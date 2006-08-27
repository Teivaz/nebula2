#ifndef N_GUISETTINGSMANAGEMENTWINDOW_H
#define N_GUISETTINGSMANAGEMENTWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiSettingsManagementWindow
    @ingroup Gui

    Window to save values from display adjustments window and
    scene controls window

    (C) 2006 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiHoriSliderGroup;
class nGuiColorSliderGroup;
class nGuiTextButton;
class nGuiMessageBox;
class nGuiTextEntry;
class nGuiTextView;
class nPrefServer;
class nStream;
class nGuiAdjustDisplayWindow;
class nGuiSceneControlWindow;
//------------------------------------------------------------------------------
class nGuiSettingsManagementWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiSettingsManagementWindow();
    /// destructor
    virtual ~nGuiSettingsManagementWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// pseudo callback method (file selection dialog filles the entry with selected file)
    void SetSelectedFilename(const nString filename);

private:
   /// reset to defaults
    void ResetValues();
    /// creates a file dialog to select file to load
    void CreateFileDialog();
    /// loads the values from xml file
    void LoadValuesFromFile(nString filename);
    /// saves the values to xml file
    void SaveValuesToFile(nString filename);
    /// saves the values as default values to the registry
    void SaveValuesAsDefault();
    /// sets the info text to the label
    void SetInfoText(const nString message, bool criticalError = false);
    /// loads values from registry and sets up the display adjustment controls
    bool ResetDisplayAdjustments(nPrefServer* prefServer);
    /// loads values from registry and sets up the display adjustment controls
    bool ResetSceneControls(nPrefServer* prefServer);
    /// loads the scene control content from the stream
    bool LoadSceneControlsFromStream(nStream* stream);
    /// loads the display adjustment content from the stream
    bool LoadDisplayAdjustmentsFromStream(nStream* stream);
    /// writes the display adjustments content to the strem
    void WriteXMLfromDisplayAdjustments(nStream* stream, nGuiAdjustDisplayWindow* adjustWindow);
    /// writes the scene controls content to the stream
    void WriteXMLfromSceneControls(nStream* stream, nGuiSceneControlWindow* sceneWindow);
    /// adds the each window with "save" support
    void AddSupportedWindowsToList();
    /// checks if the directory exists, if not create it
    bool CheckDirAndCreate();

    nRef<nGuiTextButton>       refResetButton;
	nRef<nGuiTextButton>       refSaveButton;
    nRef<nGuiTextButton>       refLoadButton;
    nRef<nGuiTextButton>       refBrowseButton;
    nRef<nGuiTextButton>       refSetDefaultButton;

    nRef<nGuiTextEntry>        refFilenameEntry;

    nRef<nGuiMessageBox>       refMessageBox;

    nRef<nGuiTextLabel>        refWindowSelectionLabel;
    nRef<nGuiTextLabel>        refFileLabel;
    nRef<nGuiTextLabel>        refDefaultLabel;
    nRef<nGuiTextLabel>        refInfoLabel;

    nRef<nGuiTextView>         refWindowSelectionTextView;
};
//------------------------------------------------------------------------------
#endif