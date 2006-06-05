#-------------------------------------------------------------------------------
# leftnotebook.py
#-------------------------------------------------------------------------------

import wx
from explorerpage import ExplorerPage
from db3tree import Db3Tree
from xmltree import XmlTree

class LeftNotebook(wx.Notebook):
    def __init__(self, parent):
        wx.Notebook.__init__(self, parent)
        self.AddPage(ExplorerPage(self), 'Explorer')
        self.db3Tree = Db3Tree(self)
        self.AddPage(self.db3Tree, 'Database')
        self.xmlTree = XmlTree(self)
        self.AddPage(self.xmlTree, 'XML')
        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.onPageChanged)

# private
    def onPageChanged(self, event):
        rightPanel = self.GetParent().rightPanel
        page = event.GetSelection()
        if page == 0:
            rightPanel.sizer.Show(rightPanel.explorerPanel)
            rightPanel.sizer.Hide(rightPanel.db3Notebook)
            rightPanel.sizer.Hide(rightPanel.xmlNotebook)
        elif page == 1:
            rightPanel.sizer.Hide(rightPanel.explorerPanel)
            rightPanel.sizer.Show(rightPanel.db3Notebook)
            rightPanel.sizer.Hide(rightPanel.xmlNotebook)
        elif page == 2:
            rightPanel.sizer.Hide(rightPanel.explorerPanel)
            rightPanel.sizer.Hide(rightPanel.db3Notebook)
            rightPanel.sizer.Show(rightPanel.xmlNotebook)
        rightPanel.Layout()
        event.Skip()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------