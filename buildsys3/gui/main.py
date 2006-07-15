#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.newevent
import string, thread, logging, sys, os.path, time
import webbrowser
from workspacespanel import WorkspacesPanel
from workspacespanel import WorkspaceListLoadedEvent
from classbuilderpanel import ClassBuilderPanel
from cmdeditorpanel import CmdEditorPanel
from buildlog import BuildLogPanel
from externaltaskdialog import ExternalTaskDialogProxy

(CreateExternalTaskDialogProxyEvent, EVT_CREATE_EXTERN_TASK_DLG_PROXY) = wx.lib.newevent.NewEvent()

ABOUT_BOX_TEXT = """\
    Nebula 2 Build System GUI    
    (c) 2005 Vadim Macagon       
    
    The Nebula 2 Build System uses the nifty
    Plex module which was written by Greg Ewing.
"""

#--------------------------------------------------------------------------
class AboutDialog(wx.Dialog):
    def __init__(self, parentWindow, id, title, pos = wx.DefaultPosition,
                 size = wx.DefaultSize, style = wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parentWindow, id, title, pos, size, style)
        line1 = wx.StaticText(self, -1, ABOUT_BOX_TEXT, 
                              style = wx.ALIGN_CENTRE|wx.ST_NO_AUTORESIZE)
        okBtn = wx.Button(self, wx.ID_OK, 'OK')
        okBtn.SetDefault()
        # layout
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(line1, 0, wx.ALIGN_CENTRE|wx.ALL, 10)
        sizer.Add(okBtn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        self.Fit()

#--------------------------------------------------------------------------
class MainFrame(wx.Frame):

    #--------------------------------------------------------------------------
    def __init__(self, buildSys, generatorName, workspaceNames):
        wx.Frame.__init__(self, None, -1, 'Nebula 2 Build System', 
                          style = wx.MINIMIZE_BOX | wx.SYSTEM_MENU | 
                                  wx.CAPTION | wx.CLOSE_BOX |
                                  wx.CLIP_CHILDREN | wx.RESIZE_BORDER)
        # load & set the Nebula icon
        try:
            self.SetIcon(wx.Icon(os.path.join('bin', 'win32', 'nebula.ico'),
                                 wx.BITMAP_TYPE_ICO))
        finally:
            pass
        
        self.buildSys = buildSys
        # create the main menu
        self.mainMenu = wx.MenuBar()
        # main menu -> File
        self.mainMenuFile = wx.Menu()
        self.mainMenuFile.Append(wx.ID_EXIT, 'E&xit\tAlt-X', 
                                 'Exit the application.')
        self.Bind(wx.EVT_MENU, self.OnMainMenuFileExit, id = wx.ID_EXIT)
        # main menu -> Help
        self.mainMenuHelp = wx.Menu()
        self.mainMenuHelp.Append(wx.ID_HELP, '&Manual', 'Display user manual.')
        self.Bind(wx.EVT_MENU, self.OnMainMenuHelpManual, id = wx.ID_HELP)
        self.mainMenuHelp.Append(wx.ID_ABOUT, '&About', 'Display about box.')
        self.Bind(wx.EVT_MENU, self.OnMainMenuHelpAbout, id = wx.ID_ABOUT)
        # attach the menus
        self.mainMenu.Append(self.mainMenuFile, '&File')
        self.mainMenu.Append(self.mainMenuHelp, '&Help')
        self.SetMenuBar(self.mainMenu)
        
        # create the status bar
        self.CreateStatusBar()
        
        # A Panel for all the controls
        self.mainPanel = wx.Panel(self)

        # Line to separate the main menu from the controls
        self.staticLine = wx.StaticLine(self.mainPanel, -1, 
                                        style = wx.HORIZONTAL)

        # The tab group
        self.tabGroup = wx.Notebook(self.mainPanel, -1)
        self.workspacesTab = WorkspacesPanel(self.tabGroup, self.buildSys,
                                             generatorName, workspaceNames)
        self.classBuilderTab = ClassBuilderPanel(self.tabGroup, self.buildSys)
        self.cmdEditorTab = CmdEditorPanel(self.tabGroup, self.buildSys)
        self.tabGroup.AddPage(self.workspacesTab, 'Workspace Generation')
        self.tabGroup.AddPage(self.classBuilderTab, 'Class Builder')
        self.tabGroup.AddPage(self.cmdEditorTab, 'Cmd Editor')

        # Log window
        self.buildLogPanel = BuildLogPanel(self.mainPanel, self.buildSys)
        
        self.closeBtn = wx.Button(self.mainPanel, -1, 'Close')
        self.Bind(wx.EVT_BUTTON, self.OnCloseBtn, self.closeBtn)
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        
        # More Events
        self.Bind(EVT_CREATE_EXTERN_TASK_DLG_PROXY, 
                  self.OnCreateExternalTaskDialogProxy)
        
        # Layout the controls...
        # top level sizer
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(self.staticLine, 0, wx.EXPAND)
        sizerA.Add(self.tabGroup, 1, wx.EXPAND)
        sizerA.Add(self.buildLogPanel, 0, wx.EXPAND|wx.ALL, 4)
        sizerA.Add(self.closeBtn, 0, wx.CENTER|wx.ALL, 5)
        # get the sizers to sort out the frame's size
        sizerA.Fit(self.mainPanel)
        self.mainPanel.SetSizer(sizerA)
        self.Fit()
        # if we wanted to set the size of the frame manually, we'd do this:
        #self.mainPanel.SetSizer(sizerA)
        #self.mainPanel.Layout()
        #self.Fit()
        
        # Load the workspace list in a separate thread
        thread.start_new_thread(self.LoadWorkspaceList, ())
        
    #--------------------------------------------------------------------------
    # Called when File->Exit is selected from the main menu.
    def OnMainMenuFileExit(self, evt):
        self.Close()
        
    #--------------------------------------------------------------------------
    # Called when Help->Manual is selected from the main menu.
    def OnMainMenuHelpManual(self, evt):
        webbrowser.open(self.buildSys.GetAbsPathFromRel(
            os.path.join('buildsys3', 'docs', 'Manual.html')), True)
        
    #--------------------------------------------------------------------------
    # Called when Help->About is selected from the main menu.
    def OnMainMenuHelpAbout(self, evt):
        dlg = AboutDialog(self, -1, "About")
        dlg.CenterOnScreen()
        # this does not return until the dialog is closed.
        dlg.ShowModal()
        dlg.Destroy()
                        
    #--------------------------------------------------------------------------
    # Called when the Close button is clicked.
    def OnCloseBtn(self, evt):
        self.Close()
                        
    #--------------------------------------------------------------------------
    # Called when the frame is closed.
    def OnClose(self, evt):
        # need to wait for the workspace loading thread to stop before
        # killing the application, otherwise bad things happen
        if not self.workspacesTab.WorkspaceLoaderThreadDone():
            dlg = wx.MessageDialog(self, 
                                   'Please wait for the workspace list to load.',
                                   'Nebula 2 Build System',
                                   wx.ICON_EXCLAMATION|wx.OK)
            dlg.ShowModal()
            dlg.Destroy()
        else:
            sys.stderr.flush()
            sys.stdout.flush()
            self.Destroy()
    
    #--------------------------------------------------------------------------
    # Obtain a list of available workspaces and update the workspace
    # check-list-box when done.
    def LoadWorkspaceList(self):
        success = self.buildSys.Prepare()
        evt = WorkspaceListLoadedEvent(listLoaded = success)
        wx.PostEvent(self.workspacesTab, evt)
        
    #--------------------------------------------------------------------------
    # Create an external task dialog proxy, the callback function will be 
    # called with the new instance of the dialog proxy.
    # This can be called from any thread.
    def CreateExternalTaskDialogProxy(self, dlgTitle, OnCreateDlg, 
                                      OnOkBtn, OnCancelBtn):
        if OnCreateDlg != None:
            evt = CreateExternalTaskDialogProxyEvent(dlgTitle = dlgTitle,
                                                     OnCreateDlg = OnCreateDlg,
                                                     OnOkBtn = OnOkBtn,
                                                     OnCancelBtn = OnCancelBtn)
            wx.PostEvent(self, evt)
            
    #--------------------------------------------------------------------------
    # Never call directly! 
    # This will be called indirectly within the GUI thread's context 
    # by self.CreateExternalTaskDialogProxy().
    def OnCreateExternalTaskDialogProxy(self, evt):
        proxy = ExternalTaskDialogProxy(self, evt.dlgTitle, 
                                        evt.OnOkBtn, evt.OnCancelBtn)
        evt.OnCreateDlg(proxy)
        
    #--------------------------------------------------------------------------
    # Destroy a dialog proxy previously created by 
    # self.CreateExternalTaskDialogProxy().
    def DestroyExternalTaskDialogProxy(self, proxy):
        proxy.Destroy()

#--------------------------------------------------------------------------
class MainApp(wx.App):
    
    #--------------------------------------------------------------------------
    def __init__(self, buildSys, generatorName, workspaceNames, 
                 redirect = False, filename = None, useBestVisual = False, 
                 clearSigInt = True):
        self.buildSys = buildSys
        self.curGeneratorName = generatorName
        self.curWorkspaceNames = workspaceNames
        wx.App.__init__(self, redirect, filename, useBestVisual, clearSigInt)
        
    #--------------------------------------------------------------------------
    def OnInit(self):
        mainFrame = MainFrame(self.buildSys, self.curGeneratorName, 
                              self.curWorkspaceNames)
        self.buildSys.SetMainFrame(mainFrame)
        self.SetTopWindow(mainFrame)
        mainFrame.CenterOnScreen()
        mainFrame.Show(True)
        return True
        
#--------------------------------------------------------------------------
def DisplayGUI(buildSys, generatorName, workspaceNames):
    app = MainApp(buildSys, generatorName, workspaceNames,
                  redirect=True, filename='gui.log')
    app.MainLoop()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------

