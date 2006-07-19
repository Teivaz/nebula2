#ifndef N_GUIADJUSTDISPLAYFILEDIALOG_H
#define N_GUIADJUSTDISPLAYFILEDIALOG_H
//------------------------------------------------------------------------------
/**
    @class nGuiAdjustDisplayFileDialog
    @ingroup Gui

    File dialog for saving values from supported windows
    (saturation, color balance, etc...)

    (C) 2006 RadonLabs GmbH
*/
#include "gui/nguifiledialog.h"

class nGuiSettingsManagementWindow;
//------------------------------------------------------------------------------
class nGuiSettingsFileDialog : public nGuiFileDialog
{
public:
    /// constructor
    nGuiSettingsFileDialog();
    /// destructor
    virtual ~nGuiSettingsFileDialog();
    /// sets reference of adjustdisplay window (the parent)
    void SetSettingWindowRef(nGuiSettingsManagementWindow* settingsWindowTmp);
    /// overridden method
    bool OnOk();

private:
    /// reference to display adjust window
    nGuiSettingsManagementWindow* settingsWindow;
};

//------------------------------------------------------------------------------
#endif