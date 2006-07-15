#-------------------------------------------------------------------------------
# db3notebook.py
#-------------------------------------------------------------------------------

import wx
from db3page import Db3Page

class Db3Notebook(wx.Notebook):
    def __init__(self, parent):
        wx.Notebook.__init__(self, parent)
        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.onPageChanged)
        self.Bind(wx.EVT_LEFT_DCLICK, self.onLeftDClick)

    def addPage(self, pathName):
        self.AddPage(Db3Page(pathName, self), pathName)
        self.SetSelection(self.GetPageCount() - 1)

    def deleteSelectedPage(self):
        self.deletePage(self.GetSelection())

    def getSelectedPage(self):
        return self.GetPage(self.GetSelection())

# private
    def onPageChanged(self, event):
        page = self.GetPage(event.GetSelection())
        mainFrame = self.GetParent().GetParent().GetParent()
        if page.changed:
            mainFrame.enableSaveTool()
        else:
            mainFrame.enableSaveTool(False)
        mainFrame.splitter.leftNotebook.db3Tree.update(page)
        event.Skip()

    def onLeftDClick(self, event):
        page, flags = self.HitTest(event.GetPosition())
        self.deletePage(page)

    def deletePage(self, page):
        self.DeletePage(page)
        if not self.GetPageCount():
            leftNotebook = self.GetParent().GetParent().leftNotebook
            leftNotebook.db3Tree.update(None)
            if self.GetParent().xmlNotebook.GetPageCount():
                leftNotebook.SetSelection(2)
            else:
                mainFrame = self.GetParent().GetParent().GetParent()
                mainFrame.closeAllDb3Files()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------