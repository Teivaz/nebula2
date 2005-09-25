#--------------------------------------
# shdctrls.py
#--------------------------------------

import pynebula as nebula
import wx
from shdpanel import *

#--------------------------------------

fileSvr = nebula.lookup('/sys/servers/file2')
napp = nebula.lookup('/app')

#--------------------------------------

class MainFrame(wx.Frame):
    def __init__(self, app):
        wx.Frame.__init__(self, None, title = 'Shader Tuner', size = (1024, 768))
        self.wxApp = app

        self.SetMenuBar(self.CreateMenuBar())
        self.CreateStatusBar(2, wx.ST_SIZEGRIP)

        self.infoPanel = InfoPanel(self)
        self.shdPanel = ShaderPanel(self)
        self.renderPanel = RenderPanel(self, self.infoPanel, self.shdPanel)
        self.infoPanel.SetRenderPanel(self.renderPanel)
        self.shdPanel.SetRenderPanel(self.renderPanel)

        sizer1 = wx.BoxSizer(wx.VERTICAL)
        sizer1.Add(self.renderPanel, 0)
        sizer1.Add(self.infoPanel, 1, wx.EXPAND)
        sizer = wx.BoxSizer()
        sizer.Add(sizer1, 0, wx.EXPAND)
        sizer.Add(self.shdPanel, 1, wx.EXPAND)
        self.SetSizer(sizer)

        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def CreateMenuBar(self):
        menuBar = wx.MenuBar()
        fileMenu = wx.Menu()
        MENU_FILE_RESET = wx.NewId()
        fileMenu.Append(MENU_FILE_RESET, "&Reset", "Remove loaded objects and reset scene")
        MENU_FILE_LOAD = wx.NewId()
        fileMenu.Append(MENU_FILE_LOAD, "&Load", "Load nebula object from file")
        menuBar.Append(fileMenu, "&File")
        helpMenu = wx.Menu()
        MENU_HELP_ABOUT = wx.NewId()
        helpMenu.Append(MENU_HELP_ABOUT, "&About", "About shader tuner")
        menuBar.Append(helpMenu, "&Help")
        self.Bind(wx.EVT_MENU, self.OnFileReset, id = MENU_FILE_RESET)
        self.Bind(wx.EVT_MENU, self.OnFileLoad, id = MENU_FILE_LOAD)
        self.Bind(wx.EVT_MENU, self.OnHelpAbout, id = MENU_HELP_ABOUT)
        return menuBar

    def OnFileReset(self, event):
        napp.resetscene()
        self.infoPanel.Reset()
        self.shdPanel.Reset()
        self.renderPanel.Reset()

    def OnFileLoad(self, event):
        dlg = wx.FileDialog(self, message = "Choose a nebula object file",
                            defaultDir = fileSvr.manglepath('gfxlib:'),
                            wildcard = "Nebula object file (*.n2)|*.n2|All files (*.*)|*.*")
        if dlg.ShowModal() == wx.ID_OK:
            napp.loadobject(dlg.GetPath().encode('ascii'))
        dlg.Destroy()

    def OnHelpAbout(self, event):
        self.renderPanel.ShowInfoPopup(False)
        dlg = AboutDialog(self)
        dlg.ShowModal()
        dlg.Destroy()
        self.renderPanel.ShowInfoPopup(True)

    def OnCloseWindow(self, event):
        self.wxApp.Shutdown()
        self.Destroy()

#--------------------------------------

class RenderPanel(wx.Panel):
    def __init__(self, parent, infoPanel, shdPanel):
        wx.Panel.__init__(self, parent, size = (800, 600))
        nebula.lookup('/sys/env/parent_hwnd').seti(self.GetHandle())
        self.infoPanel = infoPanel
        self.shdPanel = shdPanel
        self.infoPopup = None

    def Reset(self):
        self.DestroyInfoPopup()

    def OnMouseClick(self, button, x, y):
        self.DestroyInfoPopup()
        id = napp.getpickedobject()
        if id != 0:
            if button == 0:
                node = napp.getobjectnode(id)
                pos = napp.getobjectposition(id)
                size = napp.getobjectsize(id)
                self.infoPopup = self.CreateInfoPopup(node.getname(), (x, y))
                self.matNodelist = []
                self.FindMaterialNode(node)
                self.shdPanel.SetObject(self.matNodelist[0])
            elif button == 1:
                pass
        else:
            self.shdPanel.Reset()

    def CreateInfoPopup(self, text, pos):
        infoPopup = wx.PopupWindow(self, flags = wx.SIMPLE_BORDER)
        text = wx.StaticText(infoPopup, label = text, pos = (5, 5))
        size = text.GetBestSize()
        infoPopup.SetSize((size.width + 10, size.height + 10))
        infoPopup.Position(self.ClientToScreen((0, 0)), pos)
        infoPopup.Show(True)
        return infoPopup

    def ShowInfoPopup(self, show):
        if self.infoPopup != None:
            self.infoPopup.Show(show)

    def DestroyInfoPopup(self):
        if self.infoPopup != None:
            self.infoPopup.Show(False)
            self.infoPopup.Destroy()
            self.infoPopup = None

    def FindMaterialNode(self, node):
        if node.isa('nmaterialnode'):
            self.matNodelist.append(node)
        child = node.gethead()
        while child != None:
            self.FindMaterialNode(child)
            child = child.getsucc()

#--------------------------------------

class InfoPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

    def SetRenderPanel(self, renderPanel):
        self.renderPanel = renderPanel;

    def Reset(self):
        pass

#--------------------------------------

class AboutDialog(wx.Dialog):
    def __init__(self, parent):
        pre = wx.PreDialog()
        pre.Create(parent)
        self.PostCreate(pre)

        text = wx.StaticText(self, label = "Shader Tuner v0.01\n\nNebula2 Community SDK\n\nSeptember 2005", size = (200, 100), style = wx.ALIGN_CENTRE)
        btn = wx.Button(self, wx.ID_OK)
        btn.SetDefault()
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text, 1, wx.ALIGN_CENTRE | wx.TOP, 25)
        sizer.Add(btn, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 25)
        self.SetSizer(sizer)

#--------------------------------------
# Eof
#--------------------------------------