#-------------------------------------------------------------------------------
# xmltree.py
#-------------------------------------------------------------------------------

import wx

class XmlTree(wx.TreeCtrl):
    def __init__(self, parent):
        self.xmlPage = None
        wx.TreeCtrl.__init__(self, parent)
        self.createImageList()

    def update(self, page):
        self.xmlPage = page
        self.DeleteAllItems()
        if page:
            root = self.AddRoot('XML(' + self.xmlPage.xmlFile.pathName + ')', self.imgXml)

# private
    def createImageList(self):
        size = (16, 16)
        self.imageList = wx.ImageList(16, 16)
        self.imgXml = self.imageList.Add(wx.ArtProvider_GetBitmap('XML', wx.ART_OTHER, size))
        self.SetImageList(self.imageList)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------