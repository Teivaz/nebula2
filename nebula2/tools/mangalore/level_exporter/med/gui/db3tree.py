#-------------------------------------------------------------------------------
# db3tree.py
#-------------------------------------------------------------------------------

import wx

class Db3Tree(wx.TreeCtrl):
    def __init__(self, parent):
        self.db3Page = None
        wx.TreeCtrl.__init__(self, parent)
        self.createImageList()
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.onSelChanged)

    def update(self, page):
        self.db3Page = page
        self.DeleteAllItems()
        if page:
            root = self.AddRoot('Database(' + page.db3File.pathName + ')', self.imgDb3)
            for i in page.db3File.listTables():
                child = self.AppendItem(root, i, self.imgTable, data = wx.TreeItemData('table'))
            for i in page.db3File.listIndecies():
                child = self.AppendItem(root, i, self.imgIndex, data = wx.TreeItemData('index'))
            self.Expand(root)

# private
    def createImageList(self):
        size = (16, 16)
        self.imageList = wx.ImageList(16, 16)
        self.imgDb3 = self.imageList.Add(wx.ArtProvider_GetBitmap('DB3', wx.ART_OTHER, size))
        self.imgTable = self.imageList.Add(wx.ArtProvider_GetBitmap('TABLE', wx.ART_OTHER, size))
        self.imgIndex = self.imageList.Add(wx.ArtProvider_GetBitmap('INDEX', wx.ART_OTHER, size))
        self.SetImageList(self.imageList)

    def onSelChanged(self, event):
        item = event.GetItem()
        if self.GetItemData(item).GetData() == 'table':
            self.db3Page.createTableGrid(self.GetItemText(item))

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------