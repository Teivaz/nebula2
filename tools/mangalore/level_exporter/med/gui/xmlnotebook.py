#-------------------------------------------------------------------------------
# xmlnotebook.py
#-------------------------------------------------------------------------------

import wx
from xmledit import XmlEdit

class XmlNotebook(wx.Notebook):
    def __init__(self, parent):
        wx.Notebook.__init__(self, parent)
        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.onPageChanged)
        self.Bind(wx.EVT_LEFT_DCLICK, self.onLeftDClick)

    def addPage(self, pathName):
        self.AddPage(XmlEdit(pathName, self), pathName)
        self.SetSelection(self.GetPageCount() - 1)

    def deleteSelectedPage(self):
        self.deletePage(self.GetSelection())

    def getSelectedPage(self):
       return self.GetPage(self.GetSelection())

# private
    def onPageChanged(self, event):
        page = self.GetPage(event.GetSelection())
        mainFrame = self.GetParent().GetParent().GetParent()
        if page.CanUndo():
            mainFrame.enableSaveTool()
        else:
            mainFrame.enableSaveTool(False)
        mainFrame.splitter.leftNotebook.xmlTree.update(page)
        event.Skip()

    def onLeftDClick(self, event):
        page, flags = self.HitTest(event.GetPosition())
        self.deletePage(page)

    def deletePage(self, page):
        self.DeletePage(page)
        if not self.GetPageCount():
            leftNotebook = self.GetParent().GetParent().leftNotebook
            leftNotebook.xmlTree.update(None)
            if self.GetParent().db3Notebook.GetPageCount():
                leftNotebook.SetSelection(1)
            else:
                mainFrame = self.GetParent().GetParent().GetParent()
                mainFrame.closeAllXmlFiles()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------