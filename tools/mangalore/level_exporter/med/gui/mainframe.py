#-------------------------------------------------------------------------------
# mainframe.py
#-------------------------------------------------------------------------------

import ConfigParser, os, wx
from medartprovider import MedArtProvider
from medsplitter import MedSplitter
from db3xmldialog import Db3XmlDialog
from xmldb3dialog import XmlDb3Dialog
from aboutdialog import AboutDialog
from med.mang.db3file import Db3File
from med.mang.xmlfile import XmlFile
from med.mang.mangalorelevel import MangaloreLevel

CONFIG_FILENAME = 'med.ini'

class MainFrame(wx.Frame):
    def __init__(self):
        self.config = ConfigParser.SafeConfigParser()
        self.config.read(CONFIG_FILENAME)
        wx.ArtProvider_PushProvider(MedArtProvider())
        wx.Frame.__init__(self, None, title = 'Mangalore Level Editor', size = (800, 600))
        self.SetIcon(wx.ArtProvider_GetIcon('MED', wx.ART_OTHER, (16, 16)))
        self.createMenuBar()
        self.createToolBar()
        self.CreateStatusBar()
        self.splitter = MedSplitter(self)
        sizer = wx.BoxSizer()
        sizer.Add(self.splitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.onDestroy, self)

    def enableCloseTool(self, enable = True):
        if enable:
            self.toolBar.EnableTool(self.closeToolId, True)
        else:
            self.toolBar.EnableTool(self.closeToolId, False)

    def enableSaveTool(self, enable = True):
        if enable:
            self.toolBar.EnableTool(self.saveToolId, True)
        else:
            self.toolBar.EnableTool(self.saveToolId, False)

    def enableConvertTool(self, enable = True):
        if enable:
            self.toolBar.EnableTool(self.convertToolId, True)
        else:
            self.toolBar.EnableTool(self.convertToolId, False)

    def openDb3File(self, pathName):
        self.enableCloseTool()
        self.enableConvertTool()
        self.splitter.rightPanel.db3Notebook.addPage(pathName)
        self.splitter.leftNotebook.SetSelection(1)

    def openXmlFile(self, pathName):
        self.enableCloseTool()
        self.enableConvertTool()
        self.splitter.rightPanel.xmlNotebook.addPage(pathName)
        self.splitter.leftNotebook.SetSelection(2)

    def closeAllDb3Files(self):
        self.enableCloseTool(False)
        self.enableSaveTool(False)
        self.enableConvertTool(False)
        self.splitter.leftNotebook.SetSelection(0)

    def closeAllXmlFiles(self):
        self.enableCloseTool(False)
        self.enableSaveTool(False)
        self.enableConvertTool(False)
        self.splitter.leftNotebook.SetSelection(0)

# private
    def createMenuBar(self):
        menuBar = wx.MenuBar()
        menu = wx.Menu()
        menuId = wx.NewId()
        menu.Append(menuId, 'Open...', 'Open mangalore level...')
        self.Bind(wx.EVT_MENU, self.onOpen, id = menuId)
        menuId = wx.NewId()
        menu.Append(menuId, 'Close', 'Close mangalore level')
        self.Bind(wx.EVT_MENU, self.onClose, id = menuId)
        menuId = wx.NewId()
        menu.Append(menuId, 'Save', 'Save mangalore level')
        self.Bind(wx.EVT_MENU, self.onSave, id = menuId)
        menuId = wx.NewId()
        menu.Append(menuId, 'Save as...', 'Save mangalore level as...')
        self.Bind(wx.EVT_MENU, self.onSaveAs, id = menuId)
        menuBar.Append(menu, 'File')

        menu = wx.Menu()
        menuId = wx.NewId()
        menu.Append(menuId, 'Preferences...', 'Mangalore level editor preferences...')
        self.Bind(wx.EVT_MENU, self.onPreferences, id = menuId)
        menuBar.Append(menu, 'Edit')

        menu = wx.Menu()
        menuId = wx.NewId()
        menu.Append(menuId, 'Db3 to Xml...', 'Convert mangalore level from .db3 to.xml format')
        self.Bind(wx.EVT_MENU, self.onDb3Xml, id = menuId)
        menu.Append(menuId, 'Xml to Db3...', 'Convert mangalore level from .xml to.db3 format')
        self.Bind(wx.EVT_MENU, self.onXmlDb3, id = menuId)
        menuBar.Append(menu, 'Tool')

        self.winMenu = wx.Menu()
        self.showMenuId = wx.NewId()
        self.winMenu.AppendCheckItem(self.showMenuId, 'Show left panel', 'Show left tree panel')
        self.winMenu.Check(self.showMenuId, True)
        self.Bind(wx.EVT_MENU, self.onShowLeftPanel, id = self.showMenuId)
        self.hideMenuId = wx.NewId()
        self.winMenu.AppendCheckItem(self.hideMenuId, 'Hide left panel', 'Hide left tree panel')
        self.Bind(wx.EVT_MENU, self.onHideLeftPanel, id = self.hideMenuId)
        menuBar.Append(self.winMenu, 'Window')

        menu = wx.Menu()
        menuId = wx.NewId()
        menu.Append(menuId, 'About...', 'About mangalore level editor...')
        self.Bind(wx.EVT_MENU, self.onAbout, id = menuId)
        menuBar.Append(menu, 'Help')
        self.SetMenuBar(menuBar)

    def createToolBar(self):
        self.toolBar = self.CreateToolBar()
        size = (16, 16)
        toolId = wx.NewId()
        bmp = wx.ArtProvider_GetBitmap('OPEN', wx.ART_OTHER, size)
        self.toolBar.AddSimpleTool(toolId, bmp, 'Open...', 'Open mangalore level...')
        self.Bind(wx.EVT_TOOL, self.onOpen, id = toolId)

        self.closeToolId = wx.NewId()
        bmp = wx.ArtProvider_GetBitmap('CLOSE', wx.ART_OTHER, size)
        self.toolBar.AddSimpleTool(self.closeToolId, bmp, 'Close', 'Close mangalore level')
        tool = self.toolBar.FindById(self.closeToolId)
        bmp = wx.ArtProvider_GetBitmap('CLOSE_DISABLED', wx.ART_OTHER, size)
        tool.SetDisabledBitmap(bmp)
        self.toolBar.EnableTool(self.closeToolId, False)
        self.Bind(wx.EVT_TOOL, self.onClose, id = self.closeToolId)

        self.saveToolId = wx.NewId()
        bmp = wx.ArtProvider_GetBitmap('SAVE', wx.ART_OTHER, size)
        self.toolBar.AddSimpleTool(self.saveToolId, bmp, 'Save', 'Save mangalore level')
        tool = self.toolBar.FindById(self.saveToolId)
        bmp = wx.ArtProvider_GetBitmap('SAVE_DISABLED', wx.ART_OTHER, size)
        tool.SetDisabledBitmap(bmp)
        self.toolBar.EnableTool(self.saveToolId, False)
        self.Bind(wx.EVT_TOOL, self.onSave, id = self.saveToolId)

        self.convertToolId = wx.NewId()
        bmp = wx.ArtProvider_GetBitmap('CONVERT', wx.ART_OTHER, size)
        self.toolBar.AddSimpleTool(self.convertToolId, bmp, 'Convert', 'Convert mangalore level format')
        tool = self.toolBar.FindById(self.convertToolId)
        bmp = wx.ArtProvider_GetBitmap('CONVERT_DISABLED', wx.ART_OTHER, size)
        tool.SetDisabledBitmap(bmp)
        self.toolBar.EnableTool(self.convertToolId, False)
        self.Bind(wx.EVT_TOOL, self.onConvert, id = self.convertToolId)

        self.splitToolId = wx.NewId()
        bmp1 = wx.ArtProvider_GetBitmap('UNSPLIT', wx.ART_OTHER, size)
        bmp2 = wx.ArtProvider_GetBitmap('SPLIT', wx.ART_OTHER, size)
        self.toolBar.AddCheckTool(self.splitToolId, bmp1, bmp2, 'Show/Hide left panel', 'Show or hide left tree panel')
        self.splitTool = self.toolBar.FindById(self.splitToolId)
        self.Bind(wx.EVT_TOOL, self.onShowHideLeftPanel, id = self.splitToolId)

        toolId = wx.NewId()
        bmp = wx.ArtProvider_GetBitmap('ABOUT', wx.ART_OTHER, size)
        self.toolBar.AddSimpleTool(toolId, bmp, 'About...', 'About mangalore level editor...')
        self.Bind(wx.EVT_TOOL, self.onAbout, id = toolId)
        self.toolBar.Realize()

    def onOpen(self, event):
        wildCard = 'Mangalore XML file (*.xml)|*.xml|SQLite datebase file (*.db3)|*.db3|All files (*.*)|*.*'
        dlg = wx.FileDialog(self, message = "Choose a Mangalore level file", defaultDir = os.getcwd(), defaultFile = "",
                            wildcard = wildCard, style = wx.OPEN | wx.CHANGE_DIR)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            pathName = dlg.GetPath()
            root, ext = os.path.splitext(pathName)
            if ext == '.db3':
                self.openDb3File(pathName)
            elif ext == '.xml':
                self.openXmlFile(pathName)
        dlg.Destroy()

    def onClose(self, event):
        rightPanel = self.splitter.rightPanel
        if rightPanel.db3Notebook.IsShown():
            rightPanel.db3Notebook.deleteSelectedPage()
        elif rightPanel.xmlNotebook.IsShown():
            rightPanel.xmlNotebook.deleteSelectedPage()

    def onSave(self, event):
        rightPanel = self.splitter.rightPanel
        if rightPanel.db3Notebook.IsShown():
            rightPanel.db3Notebook.getSelectedPage().db3Grid.save()
        elif rightPanel.xmlNotebook.IsShown():
            rightPanel.xmlNotebook.getSelectedPage().save()
        self.enableSaveTool(False)

    def onSaveAs(self, event):
        rightPanel = self.splitter.rightPanel
        if rightPanel.db3Notebook.IsShown():
            rightPanel.db3Notebook.getSelectedPage().saveAs()
        elif rightPanel.xmlNotebook.IsShown():
            rightPanel.xmlNotebook.getSelectedPage().saveAs()
        self.enableSaveTool(False)

    def onDb3Xml(self, event):
        dlg = Db3XmlDialog(self.config.get('DEFAULT', 'db3path'), self.config.get('DEFAULT', 'xmlpath'), self)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            db3PathName = dlg.db3FileBrowse.GetValue()
            xmlPathName = dlg.xmlFileBrowse.GetValue()
            level = MangaloreLevel()
            db3File = Db3File(db3PathName)
            level.loadDb3(db3File)
            db3File.close()
            level.saveXml(xmlPathName)
        dlg.Destroy()

    def onXmlDb3(self, event):
        dlg = XmlDb3Dialog(self.config.get('DEFAULT', 'xmlpath'), self.config.get('DEFAULT', 'db3path'), self)
        dlg.CenterOnScreen()
        ret = dlg.ShowModal()
        if ret == wx.ID_OK:
            xmlPathName = dlg.xmlFileBrowse.GetValue()
            db3PathName = dlg.db3FileBrowse.GetValue()
            level = MangaloreLevel()
            level.loadXml(xmlPathName)
            db3File = Db3File(db3PathName)
            db3File.createEmpty()
            level.saveDb3(db3File)
            db3File.save()
            db3File.close()
        dlg.Destroy()

    def onConvert(self, event):
        rightPanel = self.splitter.rightPanel
        if rightPanel.db3Notebook.IsShown():
            rightPanel.db3Notebook.getSelectedPage().convert()
        elif rightPanel.xmlNotebook.IsShown():
            rightPanel.xmlNotebook.getSelectedPage().convert()

    def onPreferences(self, event):
        pass

    def onShowLeftPanel(self, event):
        self.winMenu.Check(self.hideMenuId, False)
        self.splitTool.SetNormalBitmap(wx.ArtProvider_GetBitmap('UNSPLIT', wx.ART_OTHER, (16, 16)))
        self.toolBar.ToggleTool(self.splitToolId, False)
        self.toolBar.Realize()
        self.splitter.SplitVertically(self.splitter.leftNotebook, self.splitter.rightPanel, -600)
        self.splitter.rightPanel.Refresh()

    def onHideLeftPanel(self, event):
        self.winMenu.Check(self.showMenuId, False)
        self.splitTool.SetNormalBitmap(wx.ArtProvider_GetBitmap('SPLIT', wx.ART_OTHER, (16, 16)))
        self.toolBar.ToggleTool(self.splitToolId, True)
        self.toolBar.Realize()
        self.splitter.Unsplit(self.splitter.leftNotebook)

    def onShowHideLeftPanel(self, event):
        size = (16, 16)
        if self.splitTool.IsToggled():
            self.splitTool.SetNormalBitmap(wx.ArtProvider_GetBitmap('SPLIT', wx.ART_OTHER, size))
            self.winMenu.Check(self.hideMenuId, True)
            self.winMenu.Check(self.showMenuId, False)
            self.splitter.Unsplit(self.splitter.leftNotebook)
        else:
            self.splitTool.SetNormalBitmap(wx.ArtProvider_GetBitmap('UNSPLIT', wx.ART_OTHER, size))
            self.winMenu.Check(self.hideMenuId, False)
            self.winMenu.Check(self.showMenuId, True)
            self.splitter.SplitVertically(self.splitter.leftNotebook, self.splitter.rightPanel, -600)
            self.splitter.rightPanel.Refresh()
        self.toolBar.Realize()

    def onAbout(self, event):
        dlg = AboutDialog(self)
        dlg.CenterOnScreen()
        dlg.ShowModal()
        dlg.Destroy()

    def onDestroy(self, event):
        wx.ArtProvider_PopProvider()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------