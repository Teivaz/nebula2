#-------------------------------------------------------------------------------
# rightpanel.py
#-------------------------------------------------------------------------------

import wx
from explorerpanel import ExplorerPanel
from db3notebook import Db3Notebook
from xmlnotebook import XmlNotebook

class RightPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.explorerPanel = ExplorerPanel(self)
        self.db3Notebook = Db3Notebook(self)
        self.xmlNotebook = XmlNotebook(self)
        self.sizer = wx.BoxSizer()
        self.sizer.Add(self.explorerPanel, 1, wx.EXPAND)
        self.sizer.Add(self.db3Notebook, 1, wx.EXPAND)
        self.sizer.Add(self.xmlNotebook, 1, wx.EXPAND)
        self.sizer.Hide(self.db3Notebook)
        self.sizer.Hide(self.xmlNotebook)
        self.SetSizer(self.sizer)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------