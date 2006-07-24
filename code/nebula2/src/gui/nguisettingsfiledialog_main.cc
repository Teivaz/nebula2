//------------------------------------------------------------------------------
//  nguiadjustdisplayfiledialog_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguisettingsfiledialog.h"
#include "gui/nguisettingsmanagementwindow.h"

nNebulaClass(nGuiSettingsFileDialog, "nguifiledialog");

//------------------------------------------------------------------------------
/**
    Constructor
*/
nGuiSettingsFileDialog::nGuiSettingsFileDialog()
{
    this->SetSaveMode(false);
    this->SetDirectory("user:options");
    this->SetTitle("Please select a file");
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGuiSettingsFileDialog::~nGuiSettingsFileDialog()
{
}

//------------------------------------------------------------------------------
/**
    Overridden OnOk() method
*/
bool
nGuiSettingsFileDialog::OnOk()
{
    // get variables
    nString dirPath = this->GetDirectory();
    nString fileName = this->GetFilename();

    // check parent
    n_assert(this->settingsWindow);

    // call parent
    this->settingsWindow->SetSelectedFilename(fileName);

    return true;
}

//------------------------------------------------------------------------------
/**
    Saves reference to adjustdisplay window (used for give back selected filename)
*/
void
nGuiSettingsFileDialog::SetSettingWindowRef(nGuiSettingsManagementWindow* settingsWindowTmp)
{
    this->settingsWindow = settingsWindowTmp;
}
