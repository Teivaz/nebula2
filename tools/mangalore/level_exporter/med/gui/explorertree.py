#-------------------------------------------------------------------------------
# explorertree.py
#-------------------------------------------------------------------------------

import os, glob, win32api, win32file, wx

class ExplorerTree(wx.TreeCtrl):
    def __init__(self, parent):
        wx.TreeCtrl.__init__(self, parent, style = wx.TR_HIDE_ROOT)
        self.filter = '*.*'
        self.expandedDirs = []
        self.createImageList()
        self.lastDrive = self.loadDrives()
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.onItemExpanding)
        self.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.onItemExpanded)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.onItemCollapsed)
        self.Bind(wx.EVT_LEFT_DCLICK, self.onLeftDClick)

    def update(self, filter):
        self.filter = filter
        leafDirs = list(self.expandedDirs)
        found = True
        while found:
            found = False
            for i in leafDirs:
                for j in leafDirs:
                    if i != j:
                        k = os.path.commonprefix((i, j))
                        if i == k:
                            leafDirs.remove(i)
                            found = True
                        elif j == k:
                            leafDirs.remove(j)
                            found = True
        stack = []
        i = self.lastDrive
        while True:
            path, clean = self.GetItemData(i).GetData()
            self.SetItemData(i, wx.TreeItemData((path, False)))
            if path in leafDirs:
                self.rebuildItem(i)
            if self.GetLastChild(i):
                stack.append(i)
            sibling = self.GetPrevSibling(i)
            if sibling:
                i = sibling
            else:
                if len(stack):
                    i = self.GetLastChild(stack.pop())
                else:
                    return

# private
    def createImageList(self):
        size = (16, 16)
        self.imageList = wx.ImageList(16, 16)
        self.imgFloppy = self.imageList.Add(wx.ArtProvider_GetBitmap('FLOPPY', wx.ART_OTHER, size))
        self.imgHardDisk = self.imageList.Add(wx.ArtProvider_GetBitmap('HARDDISK', wx.ART_OTHER, size))
        self.imgCDROM = self.imageList.Add(wx.ArtProvider_GetBitmap('CDROM', wx.ART_OTHER, size))
        self.imgDir = self.imageList.Add(wx.ArtProvider_GetBitmap('DIR', wx.ART_OTHER, size))
        self.imgFile = self.imageList.Add(wx.ArtProvider_GetBitmap('FILE', wx.ART_OTHER, size))
        self.imgDb3 = self.imageList.Add(wx.ArtProvider_GetBitmap('DB3', wx.ART_OTHER, size))
        self.imgXml = self.imageList.Add(wx.ArtProvider_GetBitmap('XML', wx.ART_OTHER, size))
        self.SetImageList(self.imageList)

    def loadDrives(self):
        child = None
        root = self.AddRoot('')
        for i in win32api.GetLogicalDriveStrings().split('\0')[:-1]:
            type = win32file.GetDriveType(i)
            itemData = wx.TreeItemData((i, False))
            if type == win32file.DRIVE_REMOVABLE:
                child = self.AppendItem(root, '[' + i.replace('\\', ']'), self.imgFloppy, data = itemData)
            elif type == win32file.DRIVE_FIXED:
                child = self.AppendItem(root, '[' + i.replace('\\', ']'), self.imgHardDisk, data = itemData)
            elif type == win32file.DRIVE_CDROM:
                child = self.AppendItem(root, '[' + i.replace('\\', ']'), self.imgCDROM, data = itemData)
            self.SetItemHasChildren(child, True)
        return child

    def onItemExpanding(self, event):
        item = event.GetItem()
        path, clean = self.GetItemData(item).GetData()
        if not clean:
            self.rebuildItem(item)

    def onItemExpanded(self, event):
        item = event.GetItem()
        path, clean = self.GetItemData(item).GetData()
        if path not in self.expandedDirs:
            self.expandedDirs.append(path)

    def onItemCollapsed(self, event):
        item = event.GetItem()
        path, clean = self.GetItemData(item).GetData()
        if path in self.expandedDirs:
            self.expandedDirs.remove(path)

    def onLeftDClick(self, event):
        item, flags = self.HitTest(event.GetPosition())
        path, clean = self.GetItemData(item).GetData()
        if os.path.isfile(path):
            mainFrame = self.GetParent().GetParent().GetParent().GetParent()
            root, ext = os.path.splitext(path)
            if ext == '.db3':
                mainFrame.openDb3File(path)
            elif ext == '.xml':
                mainFrame.openXmlFile(path)
        else:
            event.Skip()

    def rebuildItem(self, item):
        self.DeleteChildren(item)
        path, clean = self.GetItemData(item).GetData()
        for i in glob.glob(os.path.join(path, '*')):
            if os.path.isdir(i):
                itemData = wx.TreeItemData((i, False))
                child = self.AppendItem(item, os.path.basename(i) + '/', self.imgDir, data = itemData)
                self.SetItemHasChildren(child, True)
        for i in glob.glob(os.path.join(path, self.filter)):
            itemData = wx.TreeItemData((i, True))
            root, ext = os.path.splitext(i)
            if ext == '.db3':
                child = self.AppendItem(item, os.path.basename(i), self.imgDb3, data = itemData)
            elif ext == '.xml':
                child = self.AppendItem(item, os.path.basename(i), self.imgXml, data = itemData)
            else:
                child = self.AppendItem(item, os.path.basename(i), self.imgFile, data = itemData)
        self.SetItemData(item, wx.TreeItemData((path, True)))

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------