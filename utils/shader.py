#--------------------------------------
# shader.py
#--------------------------------------

import os, wx
import wx.grid as gridlib
import wx.lib.mixins.grid as mixins
from xml.dom import minidom, Node

SHADERS_PATH = "../data/shaders/"

#--------------------------------------

class AboutDialog(wx.Dialog):
    def __init__(self, parent, id = -1,
                 title = wx.EmptyString,
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.DEFAULT_DIALOG_STYLE,
                 name = wx.DialogNameStr):
        pre = wx.PreDialog()
        pre.Create(parent, id, title, pos, size, style)
        self.PostCreate(pre)

        text = wx.StaticText(self, label = "Simple Shader Browser v0.01\n\nNebula2 Community SDK\n\nSeptember 2005", size = (200, 100), style = wx.ALIGN_CENTRE)
        btn = wx.Button(self, wx.ID_OK)
        btn.SetDefault()
        s = wx.BoxSizer(wx.VERTICAL)
        s.Add(text, 1, wx.ALIGN_CENTRE | wx.TOP, 25)
        s.Add(btn, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 25)
        self.SetSizer(s)

#--------------------------------------

class TextPanel(wx.Panel):
    def __init__(self, parent, id = -1,
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.TAB_TRAVERSAL | wx.NO_BORDER,
                 name = wx.PanelNameStr):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)
        box = wx.StaticBox(self, label = "Shader Basics")
        s1 = wx.StaticBoxSizer(box)

        label1 = wx.StaticText(self, label = "name")
        font = label1.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label1.SetFont(font)
        self.text1 = wx.TextCtrl(self, size = (120, -1), style = wx.TE_READONLY)

        label2 = wx.StaticText(self, label = "shadeType")
        font = label2.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label2.SetFont(font)
        self.text2 = wx.TextCtrl(self, size = (120, -1), style = wx.TE_READONLY)

        label3 = wx.StaticText(self, label = "meshType")
        font = label3.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label3.SetFont(font)
        self.text3 = wx.TextCtrl(self, size = (120, -1), style = wx.TE_READONLY)

        label4 = wx.StaticText(self, label = "alias")
        font = label4.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label4.SetFont(font)
        self.text4 = wx.TextCtrl(self, size = (120, -1), style = wx.TE_READONLY)

        s1.Add(label1, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add(self.text1, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add((0, 0), 1)
        s1.Add(label2, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add(self.text2, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add((0, 0), 1)
        s1.Add(label3, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add(self.text3, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add((0, 0), 1)
        s1.Add(label4, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add(self.text4, 0, wx.CENTRE | wx.LEFT, 5)
        s1.Add((0, 0), 1)
        s2 = wx.BoxSizer()
        s2.Add(s1, 1, wx.EXPAND | wx.ALL, 5)
        self.SetSizer(s2)

    def Load(self, shader):
        self.text1.SetValue(shader.attributes['name'].value)
        self.text2.SetValue(shader.attributes['shaderType'].value)
        self.text3.SetValue(shader.attributes['meshType'].value)
        self.text4.SetValue(shader.attributes['file'].value)

    def Unload(self):
        self.text1.Clear()
        self.text2.Clear()
        self.text3.Clear()
        self.text4.Clear()

#--------------------------------------

class GridPanel(wx.Panel):
    def __init__(self, parent, id = -1,
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.TAB_TRAVERSAL | wx.NO_BORDER,
                 name = wx.PanelNameStr):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)

        self.box = wx.StaticBox(self, label = "Shader Parameters")
        self.grid = gridlib.Grid(self)
        self.grid.CreateGrid(32, 9)
        #self.grid.EnableEditing(False)
        self.grid.SetColLabelValue(0, "name")
        self.grid.SetColLabelValue(1, "label")
        self.grid.SetColLabelValue(2, "type")
        self.grid.SetColLabelValue(3, "gui")
        self.grid.SetColLabelValue(4, "export")
        self.grid.SetColLabelValue(5, "enum")
        self.grid.SetColLabelValue(6, "min")
        self.grid.SetColLabelValue(7, "max")
        self.grid.SetColLabelValue(8, "default")
        for i in range(32):
            self.grid.SetRowLabelValue(i, "param %d" % i)
            self.grid.SetCellOverflow(i, 5, False)
        self.grid.SetRowLabelSize(62)
        self.grid.SetColSize(0, 120)
        self.grid.SetColSize(1, 120)
        self.grid.SetColSize(3, 40)
        self.grid.SetColSize(4, 50)
        self.grid.SetColSize(5, 120)
        self.grid.SetColSize(6, 40)
        self.grid.SetColSize(7, 40)
        self.grid.SetColSize(8, 120)
        self.grid.SetRowLabelAlignment(wx.ALIGN_LEFT, wx.ALIGN_CENTRE)
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def Load(self, shader):
        self.Unload()
        i = 0
        for child in shader.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.nodeName == "param":
                self.grid.SetCellValue(i, 0, child.attributes['name'].value)
                self.grid.SetCellValue(i, 1, child.attributes['label'].value)
                self.grid.SetCellValue(i, 2, child.attributes['type'].value)
                self.grid.SetCellValue(i, 3, child.attributes['gui'].value)
                self.grid.SetCellValue(i, 4, child.attributes['export'].value)
                if child.attributes.has_key('enum'):
                    self.grid.SetCellValue(i, 5, child.attributes['enum'].value)
                if child.attributes.has_key('min'):
                    self.grid.SetCellValue(i, 6, child.attributes['min'].value)
                if child.attributes.has_key('max'):
                    self.grid.SetCellValue(i, 7, child.attributes['max'].value)
                if child.attributes.has_key('def'):
                    self.grid.SetCellValue(i, 8, child.attributes['def'].value)
                i = i + 1

    def Unload(self):
        self.grid.ClearGrid()

    def OnSize(self, event):
        w, h = self.GetClientSizeTuple()
        self.box.SetDimensions(5, 5, w - 10, h - 10)
        self.grid.SetDimensions(10, 20, w - 20, h - 30)

#--------------------------------------

class TreePanel(wx.Panel):
    def __init__(self, parent, textPanel, gridPanel, id = -1,
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.TAB_TRAVERSAL | wx.NO_BORDER,
                 name = wx.PanelNameStr):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)
        self.textPanel = textPanel
        self.gridPanel = gridPanel
        self.tree = wx.TreeCtrl(self, style = wx.TR_HAS_BUTTONS)

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self.tree)

    def Load(self, doc):
        self.Unload()
        size = (16, 16)
        self.il = wx.ImageList(size[0], size[1])
        folder = self.il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER, wx.ART_OTHER, size))
        fileOpen = self.il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN, wx.ART_OTHER, size))
        file = self.il.Add(wx.ArtProvider_GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, size))
        self.tree.SetImageList(self.il)
        self.root = self.tree.AddRoot("All Shaders")
        self.tree.SetPyData(self.root, None)
        self.tree.SetItemImage(self.root, folder, wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fileOpen, wx.TreeItemIcon_Expanded)
        for child in doc.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.nodeName == "shader":
                item = self.tree.AppendItem(self.root, child.attributes['name'].value)
                self.tree.SetItemPyData(item, child)
                self.tree.SetItemImage(item, folder, wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(item, fileOpen, wx.TreeItemIcon_Expanded)
        self.tree.SortChildren(self.root)
        self.tree.Expand(self.root)

        if self.tree.ItemHasChildren(self.root):
            item, ignore = self.tree.GetFirstChild(self.root)
            self.tree.SelectItem(item)


    def Unload(self):
        self.tree.DeleteAllItems()
        self.textPanel.Unload()
        self.gridPanel.Unload()

    def OnSize(self, event):
        w, h = self.GetClientSizeTuple()
        self.tree.SetDimensions(0, 0, w, h)

    def OnSelChanged(self, event):
        item = event.GetItem()
        if item:
            child = self.tree.GetItemPyData(item)
            self.textPanel.Load(child)
            self.gridPanel.Load(child)

#--------------------------------------

class MainFrame(wx.Frame):
    def __init__(self, parent, id = -1,
                 title = wx.EmptyString,
                 pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.DEFAULT_FRAME_STYLE,
                 name = wx.FrameNameStr):
        wx.Frame.__init__(self, parent, id, title, pos, size, style, name)
        menuBar = wx.MenuBar()
        menu1 = wx.Menu()
        menu1.Append(101, "&Open", "Open shader xml file")
        menu1.Append(102, "&Close", "Close shader xml file")
        menuBar.Append(menu1, "&File")
        menu2 = wx.Menu()
        menu2.Append(201, "&About", "About simple shader browser")
        menuBar.Append(menu2, "&Help")
        self.SetMenuBar(menuBar)
        self.CreateStatusBar()

        self.textPanel = TextPanel(self)
        self.gridPanel = GridPanel(self)
        self.treePanel = TreePanel(self, self.textPanel, self.gridPanel)

        s1 = wx.BoxSizer(wx.HORIZONTAL)
        s1.Add(self.treePanel, 1, wx.EXPAND)
        s2 = wx.BoxSizer(wx.VERTICAL)
        s2.Add(self.textPanel, 0, wx.EXPAND)
        s2.Add(self.gridPanel, 1, wx.EXPAND)
        s1.Add(s2, 4, wx.EXPAND)
        self.SetSizer(s1)

        self.Bind(wx.EVT_MENU, self.Menu101, id = 101)
        self.Bind(wx.EVT_MENU, self.Menu102, id = 102)
        self.Bind(wx.EVT_MENU, self.Menu201, id = 201)

        doc = minidom.parse(SHADERS_PATH + "shaders.xml").documentElement
	self.treePanel.Load(doc)

    def Menu101(self, event):
        dlg = wx.FileDialog(self,
              message = "Choose a shader xml file",
              defaultDir = SHADERS_PATH,
              defaultFile = "",
              wildcard = "Shader xml file (*.xml)|*.xml|All files (*.*)|*.*")
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            doc = minidom.parse(path).documentElement
            self.treePanel.Load(doc)
        dlg.Destroy()

    def Menu102(self, event):
        self.treePanel.Unload()

    def Menu201(self, event):
        dlg = AboutDialog(self, title = "About", size = (350, 200))
        dlg.CenterOnScreen()
        if dlg.ShowModal() == wx.ID_OK:
            pass
        dlg.Destroy()

#--------------------------------------

class MyApp(wx.App):
    def OnInit(self):
        frame = MainFrame(None, title = "Simple Shader Browser", size = (1024, 768))
        self.SetTopWindow(frame)
        frame.Show(True)
        return True

app = MyApp(redirect = True, filename = "shader.log")
app.MainLoop()

#--------------------------------------
# Eof
#--------------------------------------
