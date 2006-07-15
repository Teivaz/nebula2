#-------------------------------------------------------------------------------
# db3page.py
#-------------------------------------------------------------------------------

import os, wx
from db3grid import Db3Grid
from med.mang.db3file import Db3File
from med.mang.mangalorelevel import MangaloreLevel

class Db3Page(wx.Panel):
    def __init__(self, pathName, parent):
        self.db3File = Db3File()
        self.db3File.open(pathName)
        wx.Panel.__init__(self, parent)
        self.db3Grid = None
        self.changed = False
        self.Bind(wx.EVT_WINDOW_DESTROY, self.onDestroy, self)

    def createTableGrid(self, table):
        if self.db3Grid:
            self.db3Grid.Destroy()
        self.db3Grid = Db3Grid(table, self)
        sizer = wx.BoxSizer()
        sizer.Add(self.db3Grid, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def saveAs(self):
        pathName = self.db3File.pathName
        wildCard = 'SQLite datebase file (*.db3)|*.db3|All files (*.*)|*.*'
        dlg = wx.FileDialog(self, message = "Save file as...", defaultDir = os.path.dirname(pathName),
                            defaultFile = pathName, wildcard = wildCard, style = wx.SAVE)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            pathName = dlg.GetPath()
            self.db3File.saveAs(pathName)
            self.GetParent().SetPageText(self.GetParent().GetSelection(), pathName)
            mainFrame = self.GetParent().GetParent().GetParent().GetParent()
            mainFrame.enableSaveTool(False)
            mainFrame.splitter.leftNotebook.db3Tree.update(self)
        dlg.Destroy()

    def convert(self):
        db3PathName = self.db3File.pathName
        root, ext = os.path.splitext(db3PathName)
        xmlPathName = root + '.xml'
        wildCard = 'Mangalore XML file (*.xml)|*.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(self, message = "Convert file to ...", defaultDir = os.path.dirname(db3PathName),
                            defaultFile = xmlPathName, wildcard = wildCard, style = wx.SAVE)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            xmlPathName = dlg.GetPath()
            level = MangaloreLevel()
            level.loadDb3(self.db3File)
            level.saveXml(xmlPathName)
            mainFrame = self.GetParent().GetParent().GetParent().GetParent()
            mainFrame.openXmlFile(xmlPathName)
        dlg.Destroy()

# private
    def onDestroy(self, event):
        self.db3File.close()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------