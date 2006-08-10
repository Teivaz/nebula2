#----------------------------------------------------------------------
#  appentrypage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

#----------------------------------------------------------------------
#  App Main Entry page
#----------------------------------------------------------------------
class AppEntryPage(wiz.WizardPageSimple):
    def __init__(self, parent, title):
        wiz.WizardPageSimple.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        self.appNameLabel = wx.StaticText(self, -1, 'App Name')
        self.appNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21))

        self.appObjNameLabel = wx.StaticText(self, -1, 'App Object Name')
        self.appObjNameTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21)) 

        # script server
        #twhit scriptServerList = ['ntclserver', 'npythonserver', 'nluaserver', 'nrubyserver']

        #twhit self.scriptLabel = wx.StaticText(self, -1, 'Script Server')
        #twhit self.scriptComboBox = wx.ComboBox(self, 500, 'ntclserver', (0, 0),
        #twhit     (200, 20), scriptServerList, wx.CB_DROPDOWN #|wxTE_PROCESS_ENTER
        #twhit     )

        # startup script
        '''
        self.startupScriptLabel = wx.StaticText(self, -1, 'Startup Script')
        self.startupScriptTextBox = wx.TextCtrl(self, -1, '', (0,0), (250, 21))

        self.startupScriptDirBtn = wx.Button(self, -1, 'Browse...')
        self.Bind(wx.EVT_BUTTON, self.OnStartupScriptBtn, self.startupScriptDirBtn)
        '''

        # Layout
        sizerB = wx.GridBagSizer(4, 5)

        sizerB.Add(self.appNameLabel, (0, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appNameTextBox, (0, 1), flag = wx.EXPAND)

        sizerB.Add(self.appObjNameLabel, (1, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.appObjNameTextBox, (1, 1), flag = wx.EXPAND)

        #twhit sizerB.Add(self.scriptLabel, (2, 0), 
        #twhit            flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        #twhit sizerB.Add(self.scriptComboBox, (2, 1),
        #twhit            flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)

        '''
        sizerB.Add(self.startupScriptLabel, (3, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.startupScriptTextBox, (3, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.startupScriptDirBtn, (4, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        '''

        self.sizerA.Add(sizerB, 0, wx.ALL, 5)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #----------------------------------------------------------------------
    #
    #----------------------------------------------------------------------
    '''
    def OnStartupScriptBtn(self, evt):
        wildcard = "Nebula2 files (*.n2)|*.n2|"    \
                   "Tcl files (*.tcl)|*.tcl|"  \
                   "Python files (*.py)|*.py|" \
                   "Lua files (*.lua)|*.lua|"  \
                   "Ruby file (*.rb)|*.rb|"    \
                   "All files (*.*)|*.*"

        dlg = wx.FileDialog(self, "Choose startup script file:", 
                           os.getcwd(), defaultFile='', wildcard=wildcard,
                           style=wx.OPEN | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            self.startupScriptTextBox.SetValue(dlg.GetPath())
        dlg.Destroy()
    '''

    #----------------------------------------------------------------------
    #
    #----------------------------------------------------------------------
    def validate(self):
        valid = True
        return valid

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
