#-------------------------------------------------------------------------------
# explorerpage.py
#-------------------------------------------------------------------------------

import wx
from explorertree import ExplorerTree
from explorerchoice import ExplorerChoice

class ExplorerPage(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.explorerTree = ExplorerTree(self)
        self.exploreChoice = ExplorerChoice(self)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.explorerTree, 1, wx.EXPAND)
        sizer.Add(self.exploreChoice, 0, wx.EXPAND)
        self.SetSizer(sizer)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------