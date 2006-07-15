#-------------------------------------------------------------------------------
# xmldb3dialog.py
#-------------------------------------------------------------------------------

import os, wx
import wx.lib.filebrowsebutton as filebrowse

class XmlDb3Dialog(wx.Dialog):
    def __init__(self, xmlPath, db3Path, parent):
        pre = wx.PreDialog()
        pre.Create(parent, title = 'Convert mangalore level from .xml to .db3')
        self.PostCreate(pre)
        xmlWildCard = 'Mangalore XML file (*.xml)|*.xml|All files (*.*)|*.*'
        db3WildCard = 'SQLite datebase file (*.db3)|*.db3|All files (*.*)|*.*'
        if os.path.isdir(xmlPath):
            self.xmlFileBrowse = filebrowse.FileBrowseButtonWithHistory(self, labelText = 'Mangalore level .xml file:',
                                                                        startDirectory = xmlPath, fileMask = xmlWildCard)
        else:
            self.xmlFileBrowse = filebrowse.FileBrowseButtonWithHistory(self, labelText = 'Mangalore level .xml file:',
                                                                        initialValue = xmlPath, fileMask = xmlWildCard)
        if os.path.isdir(db3Path):
            self.db3FileBrowse = filebrowse.FileBrowseButtonWithHistory(self, labelText = 'Mangalore level .db3 file:',
                                                                        startDirectory = db3Path, fileMask = db3WildCard)
        else:
            self.db3FileBrowse = filebrowse.FileBrowseButtonWithHistory(self, labelText = 'Mangalore level .db3 file:',
                                                                        initialValue = db3Path, fileMask = db3WildCard)
        line = wx.StaticLine(self, size = (300, -1))
        okBtn = wx.Button(self, wx.ID_OK)
        cancelBtn = wx.Button(self, wx.ID_CANCEL)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.xmlFileBrowse, 1, wx.ALIGN_RIGHT | wx.ALL, 10)
        sizer.Add(self.db3FileBrowse, 1, wx.ALIGN_RIGHT | wx.LEFT | wx.RIGHT | wx.DOWN, 10)
        sizer.Add(line, 0, wx.LEFT | wx.RIGHT, 20)
        bsizer = wx.StdDialogButtonSizer()
        bsizer.AddButton(okBtn)
        bsizer.AddButton(cancelBtn)
        bsizer.Realize()
        sizer.Add(bsizer, 0, wx.ALIGN_CENTER | wx.ALL, 15)
        self.SetSizer(sizer)
        sizer.Fit(self)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------