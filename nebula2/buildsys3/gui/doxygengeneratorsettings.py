#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import os

STR_INVALID_DIR = """\
The directory %s
does not exist or is not a directory.
Please supply a valid directory.
"""

STR_DOXYGEN_NOT_FOUND = """\
Doxygen was not found in %s
Please specify the directory where the doxygen binary resides.
"""

STR_HTML_HELP_WORKSHOP_NOT_FOUND = """\
HTML Help Workshop (hhc.exe) was not found in %s
Please specify the directory where the HTML Help Workshop
compiler binary (hhc) resides.
"""

#--------------------------------------------------------------------------
class DoxygenGeneratorSettingsDialog(wx.Dialog):

    #--------------------------------------------------------------------------
    def __init__(self, parentWindow, generator, pos = wx.DefaultPosition,
                 size = wx.DefaultSize, style = wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parentWindow, -1,
                           'Doxygen Generator Settings', pos, size, style)
        self.generator = generator
        generatorSettings = generator.GetSettings()

        autoRunStaticBox = wx.StaticBox(self, -1, 'Automatically')
        self.autoRunDoxygenCheckBox = wx.CheckBox(self, -1, 'Run Doxygen')
        self.autoRunDoxygenCheckBox.SetValue(generatorSettings['autoRunDoxygen'])
        self.autoGenerateChmCheckBox = wx.CheckBox(self, -1, 'Generate CHM')
        self.autoGenerateChmCheckBox.SetValue(generatorSettings['autoGenerateCHM'])
        locationsStaticBox = wx.StaticBox(self, -1, 'Program Directories')
        doxygenDirLabel = wx.StaticText(self, -1, 'Doxygen')
        self.doxygenDirTextBox = wx.TextCtrl(self, -1,
                                             generatorSettings['doxygenDir'],
                                             (0, 0), (310, 21))
        self.doxygenDirBtn = wx.Button(self, -1, 'Browse...')
        self.Bind(wx.EVT_BUTTON, self.OnDoxygenDirBtn, self.doxygenDirBtn)
        htmlHelpWorkshopDirLabel = wx.StaticText(self, -1,
                                                 'HTML Help Workshop')
        self.htmlHelpWorkshopDirTextBox = wx.TextCtrl(self, -1,
                                              generatorSettings['htmlHelpWorkshopDir'],
                                              (0, 0), (310, 21))
        self.htmlHelpWorkshopDirBtn = wx.Button(self, -1, 'Browse...')
        self.Bind(wx.EVT_BUTTON, self.OnHtmlHelpWorkshopDirBtn, self.htmlHelpWorkshopDirBtn)
        okBtn = wx.Button(self, -1, 'OK')
        self.Bind(wx.EVT_BUTTON, self.OnOkBtn, okBtn)
        cancelBtn = wx.Button(self, wx.ID_CANCEL, 'Cancel')
        okBtn.SetDefault()

        # layout

        # autorun area sizer
        sizerA = wx.StaticBoxSizer(autoRunStaticBox, wx.VERTICAL)
        sizerA.Add(self.autoRunDoxygenCheckBox, 0, wx.ALL, 4)
        sizerA.Add(self.autoGenerateChmCheckBox, 0, wx.ALL, 4)
        # locations area sizers
        sizerB = wx.GridBagSizer(4, 10)
        # row 0
        sizerB.Add(doxygenDirLabel, (0, 0),
                   flag = wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.doxygenDirTextBox, (0, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.doxygenDirBtn, (0, 2),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        # row 1
        sizerB.Add(htmlHelpWorkshopDirLabel, (1, 0),
                   flag = wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.htmlHelpWorkshopDirTextBox, (1, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.htmlHelpWorkshopDirBtn, (1, 2),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerC = wx.StaticBoxSizer(locationsStaticBox, wx.VERTICAL)
        sizerC.Add(sizerB, 1, wx.EXPAND|wx.ALL, 4)
        # ok & cancel buttons sizer
        sizerD = wx.BoxSizer(wx.HORIZONTAL)
        sizerD.Add(okBtn, 0, wx.ALL, 4)
        sizerD.Add(cancelBtn, 0, wx.ALL, 4)
        # top level sizer
        sizerE = wx.BoxSizer(wx.VERTICAL)
        sizerE.Add(sizerA, 0, wx.EXPAND|wx.ALL, 4)
        sizerE.Add(sizerC, 0, wx.EXPAND|wx.ALL, 4)
        sizerE.Add(sizerD, 0, wx.ALIGN_CENTER|wx.ALL, 4)
        sizerE.Fit(self)
        self.SetSizer(sizerE)
        self.Fit()

    #--------------------------------------------------------------------------
    def VerifyDirContainsDoxygen(self, doxygenDir, displayMsg = True):
        doxygenPath = os.path.join(doxygenDir, 'doxygen')
        cmdPipe = os.popen('"' + doxygenPath + '" --help', 'r')
        helpStr = cmdPipe.readline()
        cmdPipe.close()
        if '' == helpStr:
            if displayMsg:
                dlg = wx.MessageDialog(self,
                                       STR_DOXYGEN_NOT_FOUND % doxygenDir,
                                       'Doxygen Generator Settings',
                                       wx.ICON_EXCLAMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
            return False
        return True

    #--------------------------------------------------------------------------
    def VerifyDirContainsHHC(self, htmlHelpWorkshopDir, displayMsg = True):
        hhcPath = os.path.join(htmlHelpWorkshopDir, 'hhc')
        cmdPipe = os.popen('"' + hhcPath + '"', 'r')
        helpStr = cmdPipe.readline()
        cmdPipe.close()
        if '' == helpStr:
            if displayMsg:
                dlg = wx.MessageDialog(self,
                          STR_HTML_HELP_WORKSHOP_NOT_FOUND % htmlHelpWorkshopDir,
                          'Doxygen Generator Settings',
                          wx.ICON_EXCLAMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
            return False
        return True

    #--------------------------------------------------------------------------
    def OnDoxygenDirBtn(self, evt):
        dlg = wx.DirDialog(self, 'Specify the directory where doxygen is installed:',
                           self.doxygenDirTextBox.GetValue().strip())
        if dlg.ShowModal() == wx.ID_OK:
            if self.VerifyDirContainsDoxygen(dlg.GetPath(), False):
                self.doxygenDirTextBox.SetValue(dlg.GetPath())
            else:
                # on Windows at least the actual doxygen binary is in the
                # bin subfolder at the location where the user originally
                # installed doxygen, so try to find the binary there
                binDir = os.path.join(dlg.GetPath(), 'bin')
                if self.VerifyDirContainsDoxygen(binDir):
                    self.doxygenDirTextBox.SetValue(binDir)
        dlg.Destroy()

    #--------------------------------------------------------------------------
    def OnHtmlHelpWorkshopDirBtn(self, evt):
        dlg = wx.DirDialog(self, 'Specify the directory where HTML Help Workshop is installed:',
                           self.htmlHelpWorkshopDirTextBox.GetValue().strip())
        if dlg.ShowModal() == wx.ID_OK:
            if self.VerifyDirContainsHHC(dlg.GetPath()):
                self.htmlHelpWorkshopDirTextBox.SetValue(dlg.GetPath())
        dlg.Destroy()

    #--------------------------------------------------------------------------
    def OnOkBtn(self, evt):
        # get data from the GUI
        autoRunDoxygen = self.autoRunDoxygenCheckBox.IsChecked()
        autoGenerateCHM = self.autoGenerateChmCheckBox.IsChecked()
        doxygenDir = self.doxygenDirTextBox.GetValue().strip()
        htmlHelpWorkshopDir = self.htmlHelpWorkshopDirTextBox.GetValue().strip()

        # validate
        if '' != doxygenDir:
            if os.path.isdir(doxygenDir):
                if not self.VerifyDirContainsDoxygen(doxygenDir):
                    return
            else:
                dlg = wx.MessageDialog(self,
                                       STR_INVALID_DIR % doxygenDir,
                                       'Doxygen Generator Settings',
                                       wx.ICON_EXCLAMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
                return

        if '' != htmlHelpWorkshopDir:
            if os.path.isdir(htmlHelpWorkshopDir):
                if not self.VerifyDirContainsHHC(htmlHelpWorkshopDir):
                    return
            else:
                dlg = wx.MessageDialog(self,
                          STR_INVALID_DIR % htmlHelpWorkshopDir,
                          'Doxygen Generator Settings',
                          wx.ICON_EXCLAMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
                return

        # copy back the new data and save to disk
        generatorSettings = { 'doxygenDir'          : doxygenDir,
                              'autoRunDoxygen'      : autoRunDoxygen,
                              'htmlHelpWorkshopDir' : htmlHelpWorkshopDir,
                              'autoGenerateCHM'     : autoGenerateCHM }
        self.generator.SetSettings(generatorSettings)
        self.generator.SaveSettings()

        # close the dialog
        if self.IsModal():
            self.EndModal(wx.ID_OK)
        else:
            self.SetReturnCode(wx.ID_OK)
            self.Show(False)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------

