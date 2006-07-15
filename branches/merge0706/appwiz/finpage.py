#----------------------------------------------------------------------
#  finpage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

STR_MESSAGE = '''
Run 'update.py' to build the created applicaion.
'''
#----------------------------------------------------------------------
#  page for final result. (the last page)
#----------------------------------------------------------------------
class FinPage(wiz.WizardPageSimple):
    def __init__(self, parent, title, frame):

        wiz.WizardPageSimple.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        font = wx.Font(12, wx.DEFAULT, wx.NORMAL, wx.BOLD)

        msgLabel01 = wx.StaticText(self, -1, 'Congratulations!\n')
        msgLabel01.SetFont(font)

        font = wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.BOLD)

        label01 = wx.StaticText(self, -1, 'Output Directory : ')
        self.outputDirLabel = wx.StaticText(self, -1, '')
        #self.outputDirLabel.SetFont(font)

        label02 = wx.StaticText(self, -1, 'nApplication Class Name : ')
        self.appClassNameLabel = wx.StaticText(self, -1, '')
        self.appClassNameLabel.SetFont(font)

        label03 = wx.StaticText(self, -1, 'nAppState Class Name : ')
        self.appStateClassNameLabel = wx.StaticText(self, -1, '')
        self.appStateClassNameLabel.SetFont(font)

        label04 = wx.StaticText(self, -1, 'ScriptServer : ')
        self.scriptServerLabel = wx.StaticText(self, -1, '')
        self.scriptServerLabel.SetFont(font)

        label05 = wx.StaticText(self, -1, 'Target Name : ')
        self.targetNameLabel = wx.StaticText(self, -1, '')
        self.targetNameLabel.SetFont(font)

        msgLabel02 = wx.StaticText(self, -1, STR_MESSAGE)

        # Layout
        sizerB = wx.GridBagSizer(1, 1)

        #
        sizerB.Add(msgLabel01, (0, 0), 
                   flag = wx.ALIGN_CENTER, span = wx.GBSpan(1,3))
        #
        sizerB.Add(label01, (1, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.outputDirLabel, (1, 1), flag = wx.EXPAND)
        #
        sizerB.Add(label02, (2, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appClassNameLabel, (2, 1), flag = wx.EXPAND)
        #
        sizerB.Add(label03, (3, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appStateClassNameLabel, (3, 1), flag = wx.EXPAND)
        #
        sizerB.Add(label04, (4, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.scriptServerLabel, (4, 1), flag = wx.EXPAND)
        #
        sizerB.Add(label05, (5, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.targetNameLabel, (5, 1), flag = wx.EXPAND)
        
        sizerB.Add(msgLabel02, (6, 0), 
                   flag = wx.ALIGN_LEFT, span = wx.GBSpan(1,3))

        self.sizerA.Add(sizerB, 0, wx.ALL, 1)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #----------------------------------------------------------------------
    #  should be called before the page is changed to show the result.
    #----------------------------------------------------------------------
    def UpdatePage(self, frame):
        self.outputDirLabel.SetLabel(frame.page2.outputDirTextBox.GetValue())
        self.appClassNameLabel.SetLabel(frame.page3.classTextBox.GetValue()) 
        self.appStateClassNameLabel.SetLabel(frame.page3.appStateNameTextBox.GetValue())  
        self.scriptServerLabel.SetLabel(frame.page5.scriptComboBox.GetValue()) 
        self.targetNameLabel.SetLabel(frame.page6.targetNameTextBox.GetValue()) 

    #----------------------------------------------------------------------
    #
    #----------------------------------------------------------------------
    def validate(self):
        valid = True
        return valid

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
