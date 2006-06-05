#-------------------------------------------------------------------------------
# medsplitter.py
#-------------------------------------------------------------------------------

import wx
from leftnotebook import LeftNotebook
from rightpanel import RightPanel

class MedSplitter(wx.SplitterWindow):
    def __init__(self, parent):
        wx.SplitterWindow.__init__(self, parent, style = wx.SP_LIVE_UPDATE)
        self.leftNotebook = LeftNotebook(self)
        self.rightPanel = RightPanel(self)
        self.SplitVertically(self.leftNotebook, self.rightPanel, -600)
        self.Bind(wx.EVT_SPLITTER_DCLICK, self.onSplitterDClick)

# private
    def onSplitterDClick(self, event):
        self.Unsplit(self.leftNotebook)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------