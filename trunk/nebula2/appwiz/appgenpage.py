#----------------------------------------------------------------------
#  appgenpage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

STR_MISSING_PARAMS = """\
Please ensure you've selected the output directory 
"""

#----------------------------------------------------------------------
#  Application general specification page.
#----------------------------------------------------------------------
class AppGenPage(wiz.PyWizardPage):
    def __init__(self, parent, title, homeDir):
        #wiz.WizardPageSimple.__init__(self, parent)
        wiz.PyWizardPage.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        defaultOutputDir = os.path.join(homeDir, 'code')

        self.outputDirLabel = wx.StaticText(self, -1, 'Output Dir')
        self.outputDirTextBox = wx.TextCtrl(self, -1, defaultOutputDir, 
                                            (0, 0), (250, 21))

        self.outputDirBtn = wx.Button(self, -1, 'Browse...')
        self.Bind(wx.EVT_BUTTON, self.OnOutputDirBtn, self.outputDirBtn)

        self.moduleSubdirLabel = wx.StaticText(self, -1, 'Module Subdir')
        self.moduleSubdirTextBox = wx.TextCtrl(self, -1, '', 
                                               (0, 0), (200, 21))

        self.companyNameLabel = wx.StaticText(self, -1, 'Company Name')
        self.companyNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21))

        self.authorLabel = wx.StaticText(self, -1, 'Author')
        self.authorTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21))

        self.docGroupLabel = wx.StaticText(self, -1, 'Doxygen Group')
        self.docGroupTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))

        # Layout
        sizerB = wx.GridBagSizer(4, 6)

        # row 0
        sizerB.Add(self.outputDirLabel, (0, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.outputDirTextBox, (0, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        # row 1
        sizerB.Add(self.outputDirBtn, (1, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        # row 2
        sizerB.Add(self.moduleSubdirLabel, (2, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.moduleSubdirTextBox, (2, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.companyNameLabel, (3, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.companyNameTextBox, (3, 1), flag = wx.EXPAND)      
        # row 5 
        sizerB.Add(self.authorLabel, (4, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.authorTextBox, (4, 1), flag = wx.EXPAND)
        # row 6 
        sizerB.Add(self.docGroupLabel, (5, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.docGroupTextBox, (5, 1), flag = wx.EXPAND)
        # row 7

        self.sizerA.Add(sizerB, 0, wx.ALL, 6)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #--------------------------------------------------------------------------
    # Called when the Browse... button is clicked.
    #--------------------------------------------------------------------------
    def OnOutputDirBtn(self, evt):
        dlg = wx.DirDialog(self, "Choose output directory:", 
                           self.outputDirTextBox.GetValue().strip(),
                           style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)
        if dlg.ShowModal() == wx.ID_OK:
            self.outputDirTextBox.SetValue(dlg.GetPath())
        dlg.Destroy()

    def SetPrev(self, prev):
        self.prev = prev

    def SetNext(self, next):
        self.next = next

    def GetNext(self):
        return self.next

    def GetPrev(self):
        return self.prev

    #--------------------------------------------------------------------------
    # Check the validation of this page.
    #--------------------------------------------------------------------------
    def validate(self):
        valid = True
        if self.outputDirTextBox.GetValue().strip() == '':
            valid = False

        if not valid:
            dlg = wx.MessageDialog(self, STR_MISSING_PARAMS,
                                   'AppWizard', 
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy() 

        return valid

#----------------------------------------------------------------------
# EOF 
#----------------------------------------------------------------------
