#----------------------------------------------------------------------
#  bldpage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

STR_MISSING_PARAMS = """\
Please ensure you've entered the target name.
"""

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
class BldPage(wiz.WizardPageSimple):
    def __init__(self, parent, title):
        wiz.WizardPageSimple.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        # target name of the app in the bld file.
        self.targetNameLabel = wx.StaticText(self, -1, 'Target Name')
        self.targetNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21))

        # annotation for the target in the bld file.
        self.targetAnnotationLabel = wx.StaticText(self, -1, 'Target Annotation')
        self.targetAnnotationTextBox = wx.TextCtrl(self, -1, 
                                           'Insert a brief annotation of the target', 
                                           (0, 0), (200, 40),

                                           wx.TE_MULTILINE|wx.TE_WORDWRAP)
        # Layout
        sizerB = wx.GridBagSizer(4, 6)

        sizerB.Add(self.targetNameLabel, (0, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_TOP)
        sizerB.Add(self.targetNameTextBox, (0, 1), flag = wx.EXPAND)

        sizerB.Add(self.targetAnnotationLabel, (1, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_TOP)
        sizerB.Add(self.targetAnnotationTextBox, (1, 1), flag = wx.EXPAND)

        self.sizerA.Add(sizerB, 0, wx.ALL, 6)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #----------------------------------------------------------------------
    #  Check the validation of the page.
    #----------------------------------------------------------------------
    def validate(self):
        valid = True

        targetName = self.targetNameTextBox.GetValue().strip()
        if '' == targetName:
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
