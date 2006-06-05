#-------------------------------------------------------------------------------
# xmledit.py
#-------------------------------------------------------------------------------

import os, wx
import wx.stc as stc
from med.mang.xmlfile import XmlFile
from med.mang.db3file import Db3File
from med.mang.mangalorelevel import MangaloreLevel

class XmlEdit(stc.StyledTextCtrl):
    def __init__(self, pathName, parent):
        self.xmlFile = XmlFile()
        self.xmlFile.open(pathName)
        stc.StyledTextCtrl.__init__(self, parent)
        self.SetLexer(stc.STC_LEX_XML)
        self.StyleSetSpec(stc.STC_STYLE_DEFAULT, "size:9")
        self.StyleClearAll()
        self.StyleSetSpec(stc.STC_H_XMLSTART, "fore:#0000ff,bold")
        self.StyleSetSpec(stc.STC_H_XMLEND, "fore:#0000ff,bold")
        self.StyleSetSpec(stc.STC_H_TAG, "fore:#0000ff,bold")
        self.StyleSetSpec(stc.STC_H_TAGEND, "fore:#0000ff,bold")
        self.StyleSetSpec(stc.STC_H_ATTRIBUTE, "fore:#ff0000,bold")
        self.SetText(self.xmlFile.content)
        self.EmptyUndoBuffer()
        self.Bind(stc.EVT_STC_CHANGE, self.onChange)

    def save(self):
        self.xmlFile.content = self.GetText()
        self.xmlFile.save()

    def saveAs(self):
        pathName = self.xmlFile.pathName
        wildCard = 'Mangalore XML file (*.xml)|*.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(self, message = "Save file as...", defaultDir = os.path.dirname(pathName),
                            defaultFile = pathName, wildcard = wildCard, style = wx.SAVE)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            pathName = dlg.GetPath()
            self.xmlFile.saveAs(pathName)
            self.GetParent().SetPageText(self.GetParent().GetSelection(), pathName)
            mainFrame = self.GetParent().GetParent().GetParent().GetParent()
            mainFrame.enableSaveTool(False)
            mainFrame.splitter.leftNotebook.xmlTree.update(self)
        dlg.Destroy()

    def convert(self):
        xmlPathName = self.xmlFile.pathName
        root, ext = os.path.splitext(xmlPathName)
        db3PathName = root + '.db3'
        wildCard = 'SQLite datebase file (*.db3)|*.db3|All files (*.*)|*.*'
        dlg = wx.FileDialog(self, message = "Convert file to...", defaultDir = os.path.dirname(xmlPathName),
                            defaultFile = db3PathName, wildcard = wildCard, style = wx.SAVE)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            db3PathName = dlg.GetPath()
            level = MangaloreLevel()
            level.loadXml(xmlPathName)
            db3File = Db3File()
            db3File.open(db3PathName)
            db3File.createEmpty()
            level.saveDb3(db3File)
            db3File.save()
            db3File.close()
            mainFrame = self.GetParent().GetParent().GetParent().GetParent()
            mainFrame.openDb3File(db3PathName)
        dlg.Destroy()

# private
    def onChange(self, event):
        mainFrame = self.GetParent().GetParent().GetParent().GetParent()
        if self.CanUndo():
            mainFrame.enableSaveTool()
        else:
            mainFrame.enableSaveTool(False)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------