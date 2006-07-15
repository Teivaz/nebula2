#----------------------------------------------------------------------
#  appmodepage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

STR_MISSING_PARAMS = """\
Please ensure you've entered both of the class names.
"""

STR_INVALID_CLASS_PREFIX = """\
The class prefix must be shorter than the class name.
"""

#----------------------------------------------------------------------
#  App Module specification page.
#----------------------------------------------------------------------
class AppModPage(wiz.WizardPageSimple):
    def __init__(self, parent, title):
        wiz.WizardPageSimple.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        # class prefix
        self.classPrefixLabel = wx.StaticText(self, -1, 'Class Prefix')
        self.classPrefixTextBox = wx.TextCtrl(self, -1, 'n', (0, 0), (200, 21))
        # napplication derived class name
        self.classNameLabel = wx.StaticText(self, -1, 'Application Class Name')
        self.classTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))

        self.appObjNameLabel = wx.StaticText(self, -1, 'Application Object Name')
        self.appObjNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))

        self.appBriefDocLabel = wx.StaticText(self, -1, 'Application Brief Description')
        self.appBriefDocTextBox = wx.TextCtrl(self, -1, 
                                           'a brief description of the class', 
                                           (0, 0), (200, 40),
                                           wx.TE_MULTILINE|wx.TE_WORDWRAP)
        # nappstate derived class name
        self.appStateNameLabel = wx.StaticText(self, -1, 'App State Class Name')
        self.appStateNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))

        self.appStateObjNameLabel = wx.StaticText(self, -1, 'App State Object Name')
        self.appStateObjNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))

        # nappstate derived class brief doc
        self.appStateBriefDocLabel = wx.StaticText(self, -1, 'App State Brief Description')
        self.appStateBriefDocTextBox = wx.TextCtrl(self, -1, 
                                           'a brief description of the class', 
                                           (0, 0), (200, 40),
                                           wx.TE_MULTILINE|wx.TE_WORDWRAP)

        '''
        self.useDefaultInputCheckBox = wx.CheckBox(self, -1, 'Add Default Input', 
                                                   style=wx.ALIGN_RIGHT)
        '''

        # Layout
        sizerB = wx.GridBagSizer(4, 8)

        #
        sizerB.Add(self.classPrefixLabel, (0, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.classPrefixTextBox, (0, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.classNameLabel, (1, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.classTextBox, (1, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.appObjNameLabel, (2, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appObjNameTextBox, (2, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        #
        sizerB.Add(self.appBriefDocLabel, (3, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appBriefDocTextBox, (3, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.appStateNameLabel, (4, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appStateNameTextBox, (4, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        #
        sizerB.Add(self.appStateObjNameLabel, (5, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appStateObjNameTextBox, (5, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        # 
        sizerB.Add(self.appStateBriefDocLabel, (6, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appStateBriefDocTextBox, (6, 1), flag = wx.EXPAND)
        # 
        '''
        sizerB.Add(self.useDefaultInputCheckBox, (7, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        '''

        self.sizerA.Add(sizerB, 0, wx.ALL, 8)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #----------------------------------------------------------------------
    #  Check the validation of the page before to go to the next page.
    #----------------------------------------------------------------------
    def validate(self):
        valid = True

        className = self.classTextBox.GetValue().strip()
        if '' == className:
            valid = False 

        appStateClassName = self.appStateNameTextBox.GetValue().strip()
        if '' == appStateClassName:
            valid = False

        if not valid:
            dlg = wx.MessageDialog(self, STR_MISSING_PARAMS,
                                   'AppWizard', 
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy() 

        classPrefix = self.classPrefixTextBox.GetValue().strip()
        if len(classPrefix) >= len(className):
            valid = False
            dlg = wx.MessageDialog(self, STR_INVALID_CLASS_PREFIX,
                                   'AppWizard', 
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy()

        return valid

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
