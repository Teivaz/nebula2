#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.mixins.listctrl as listmix
import string, thread, logging, sys, os.path
import wx.lib.newevent
import webbrowser

ABOUT_BOX_TEXT = """\
    Nebula 2 Build System GUI    
    (c) 2005 Vadim Macagon       
    
    The Nebula 2 Build System uses the nifty
    Plex module which was written by Greg Ewing.
"""

STARTUP_BOX_TEXT = """\
Please wait...

    Loading Workspaces.
"""

SUMMARY_DLG_TEXT = """\
All done!

Built %(numOfWorkspacesBuilt)i workspace(s) out of %(totalNumOfWorkspaces)i.
"""

# This creates a new Event class and an EVT binder function
(WorkspaceListLoadedEvent, EVT_WORKSPACE_LIST_LOADED) = wx.lib.newevent.NewEvent()

(CreateProgressDialogEvent,  EVT_CREATE_PROGRESS_DLG) = wx.lib.newevent.NewEvent()
(UpdateProgressDialogEvent,  EVT_UPDATE_PROGRESS_DLG) = wx.lib.newevent.NewEvent()
(DestroyProgressDialogEvent, EVT_DESTROY_PROGRESS_DLG) = wx.lib.newevent.NewEvent()

(DisplaySummaryDialogEvent, EVT_DISPLAY_SUMMARY_DLG) = wx.lib.newevent.NewEvent()

(NewBuildLogRecordEvent, EVT_NEW_BUILD_LOG_RECORD) = wx.lib.newevent.NewEvent()

#--------------------------------------------------------------------------
class BuildSummaryDialog(wx.Dialog):
    def __init__(self, parentWindow, id, title, details,
                 pos = wx.DefaultPosition, size = wx.DefaultSize, 
                 style = wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parentWindow, id, title, pos, size, style)
        line1 = wx.StaticText(self, -1, SUMMARY_DLG_TEXT % details,
                              style = wx.ALIGN_CENTER|wx.ST_NO_AUTORESIZE)
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
class LogRecordDialog(wx.Dialog):

    LOG_RECORD_INFO_TEXT = """\
%(name)s - %(levelname)s
Created: %(asctime)s

%(message)s"""
    
    def __init__(self, parentWindow, title, record):
        wx.Dialog.__init__(self, parentWindow, -1, title)
        formatter = logging.Formatter('%(message)s')
        args = { 'name' : record.name,
                 'levelname' : record.levelname,
                 'asctime' : record.asctime,
                 'message' : formatter.format(record) }
        self.panel = wx.Panel(self)
        style = wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_LINEWRAP
        self.textBox = wx.TextCtrl(self.panel, -1, 
                                   self.LOG_RECORD_INFO_TEXT % args,
                                   (0, 0), (400, 100), style)
        self.okBtn = wx.Button(self.panel, wx.ID_OK, 'OK')
        # layout
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.textBox, 0, wx.EXPAND|wx.ALL, 4)
        sizer.Add(self.okBtn, 0, wx.CENTER|wx.TOP, 8)
        sizer.Fit(self.panel)
        self.panel.SetSizer(sizer)
        self.Fit()
        

#--------------------------------------------------------------------------
class BuildLogCtrlHandler(logging.Handler):
    
    def __init__(self, buildLogCtrl, level = logging.NOTSET):
        logging.Handler.__init__(self, level)
        self.buildLogCtrl = buildLogCtrl
        
    def emit(self, logRecord):
        evt = NewBuildLogRecordEvent(record = logRecord)
        wx.PostEvent(self.buildLogCtrl, evt)

#--------------------------------------------------------------------------
class BuildLogCtrl(wx.ListCtrl, listmix.ListCtrlAutoWidthMixin):
    
    [ID_CLEAR_LOG, ID_VIEW_RECORD, ID_COPY_RECORDS
    ] = map(lambda init_ids: wx.NewId(), range(3))
    
    #--------------------------------------------------------------------------
    def __init__(self, parentWindow, id):
        wx.ListCtrl.__init__(self, parentWindow, id,
            style = wx.LC_NO_HEADER|wx.LC_REPORT|wx.LC_VIRTUAL|wx.LC_HRULES)
        listmix.ListCtrlAutoWidthMixin.__init__(self) # autosizes the column
        self.InsertColumn(0, 'description')
        self.SetColumnWidth(0, 450)
        self.SetItemCount(0)
        self.warningAttr = wx.ListItemAttr()
        self.warningAttr.SetBackgroundColour('yellow')
        self.errorAttr = wx.ListItemAttr()
        self.errorAttr.SetBackgroundColour('red')
        self.logHandler = None
        self.showInfo = True
        self.showErrors = True
        self.showWarnings = True
        self.logRecs = []
        self.infoRecIndices = []
        self.errorRecIndices = []
        self.warningRecIndices = []
        self.displayedRecIndices = []
        self.Bind(EVT_NEW_BUILD_LOG_RECORD, self.OnNewLogRecord)
        self.Bind(wx.EVT_COMMAND_RIGHT_CLICK, self.OnRightMouseUp) # for wxMSW
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightMouseUp) # for wxGTK
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightMouseDown)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnItemActivated)
        self.Bind(wx.EVT_MENU, self.OnClearLog, id = self.ID_CLEAR_LOG)
        self.Bind(wx.EVT_MENU, self.OnViewRecord, id = self.ID_VIEW_RECORD)
        self.Bind(wx.EVT_MENU, self.OnCopyRecords, id = self.ID_COPY_RECORDS)
        
    #--------------------------------------------------------------------------
    def RefreshRecs(self):
        self.displayedRecIndices = []
        if self.showInfo:
            self.displayedRecIndices.extend(self.infoRecIndices)
        if self.showWarnings:
            self.displayedRecIndices.extend(self.warningRecIndices)
        if self.showErrors:
            self.displayedRecIndices.extend(self.errorRecIndices)
        self.displayedRecIndices.sort()
        self.SetItemCount(len(self.displayedRecIndices))
        self.Refresh()
        
    #--------------------------------------------------------------------------
    def GetShowInfo(self):
        return self.showInfo
        
    #--------------------------------------------------------------------------
    # TODO: turn this into an event so it can be safely called from non-gui
    # threads
    def SetShowInfo(self, show):
        self.showInfo = show
        self.RefreshRecs()
        
    #--------------------------------------------------------------------------
    def GetShowWarnings(self):
        return self.showWarnings
        
    #--------------------------------------------------------------------------
    # TODO: turn this into an event so it can be safely called from non-gui
    # threads
    def SetShowWarnings(self, show):
        self.showWarnings = show
        self.RefreshRecs()
        
    #--------------------------------------------------------------------------
    def GetShowErrors(self):
        return self.showErrors
        
    #--------------------------------------------------------------------------
    # TODO: turn this into an event so it can be safely called from non-gui
    # threads
    def SetShowErrors(self, show):
        self.showErrors = show
        self.RefreshRecs()
        
    #--------------------------------------------------------------------------
    # These 3 methods are callbacks for implementing the "virtualness" of the 
    # list.
    def OnGetItemText(self, item, col):
        assert len(self.displayedRecIndices) > 0
        text =  self.logHandler.format(
                    self.logRecs[self.displayedRecIndices[item]])
        lines = string.split(text, '\n')
        if len(lines) > 0:
            return lines[0]
        return ''

    def OnGetItemImage(self, item):
        # TODO ?
        return -1

    def OnGetItemAttr(self, item):
        # TODO ?
        return None
        
    #--------------------------------------------------------------------------
    def OnRightMouseDown(self, evt):
        self.mouseX = evt.GetX()
        self.mouseY = evt.GetY()
        evt.Skip()
        
    #--------------------------------------------------------------------------
    # Get the indices of all currently selected items.
    def getSelectedItems(self):
        selectedItems = []
        for i in range(self.GetItemCount()):
            if self.GetItemState(i, wx.LIST_STATE_SELECTED):
                selectedItems.append(i)
        return selectedItems
        
    #--------------------------------------------------------------------------
    # Pop up the right click menu.
    def OnRightMouseUp(self, evt):
        menu = wx.Menu()
        hitItem, itemFlags = self.HitTest((self.mouseX, self.mouseY))
        selectedItems = self.getSelectedItems()
        
        # an item is under the cursor?
        if itemFlags & wx.LIST_HITTEST_ONITEM: 
            if len(selectedItems) > 0:
                if hitItem in selectedItems:
                    if len(selectedItems) == 1:
                        menu.Append(self.ID_VIEW_RECORD, 'View')
                    menu.Append(self.ID_COPY_RECORDS, 'Copy')
                    menu.AppendSeparator()
                else:
                    # deselect all
                    for i in range(len(selectedItems)):
                        self.SetItemState(i, 0, wx.LIST_STATE_SELECTED)
            else:
                self.Select(hitItem)
                menu.Append(self.ID_VIEW_RECORD, 'View')
                menu.AppendSeparator()
        else:
            # deselect all
            for i in range(len(selectedItems)):
                self.SetItemState(i, 0, wx.LIST_STATE_SELECTED)
        
        menu.Append(self.ID_CLEAR_LOG, 'Clear Window')
        self.PopupMenu(menu, (self.mouseX, self.mouseY))
        menu.Destroy()
        
    #--------------------------------------------------------------------------
    # Called when a new record is logged.
    def OnNewLogRecord(self, evt):
        self.logRecs.append(evt.record)
        if logging.WARNING == evt.record.levelno:
            self.warningRecIndices.append(len(self.logRecs)-1)
        elif logging.ERROR == evt.record.levelno:
            self.errorRecIndices.append(len(self.logRecs)-1)
        else:
            self.infoRecIndices.append(len(self.logRecs)-1)
        
        self.RefreshRecs()
        
    #--------------------------------------------------------------------------
    def AttachToLogger(self, currentHandler):
        logger = logging.getLogger('N2-BuildSystem')
        self.logHandler = BuildLogCtrlHandler(self)
        formatter = logging.Formatter('%(asctime)s |%(levelname)s| %(message)s', 
                                      '%X')
        self.logHandler.setFormatter(formatter)
        #if currentHandler != None:
        #    logger.removeHandler(currentHandler)
        logger.addHandler(self.logHandler)
    
    #--------------------------------------------------------------------------
    def OnClearLog(self, evt):
        self.SetItemCount(0)
        self.Refresh()
        self.logRecs = []
        self.infoRecIndices = []
        self.errorRecIndices = []
        self.warningRecIndices = []
        self.displayedRecIndices = []
        
    #--------------------------------------------------------------------------
    def OnViewRecord(self, evt):
        items = self.getSelectedItems()
        if len(items) == 1:
            dlg = LogRecordDialog(self, 'Log Entry Details', 
                      self.logRecs[self.displayedRecIndices[items[0]]])
            dlg.ShowModal()
            dlg.Destroy()
        
    #--------------------------------------------------------------------------
    def OnItemActivated(self, evt):
        assert evt.m_itemIndex < len(self.displayedRecIndices)
        dlg = LogRecordDialog(self, 'Log Entry Details',
                  self.logRecs[self.displayedRecIndices[evt.m_itemIndex]])
        dlg.ShowModal()
        dlg.Destroy()
        
    #--------------------------------------------------------------------------
    def OnCopyRecords(self, evt):
        text = ''
        items = self.getSelectedItems()
        for i in items:
            text += self.logHandler.format(
                        self.logRecs[self.displayedRecIndices[i]]) + '\n'
        if '' != text:
            dataObj = wx.TextDataObject()
            dataObj.SetText(text)
            wx.TheClipboard.Open()
            wx.TheClipboard.SetData(dataObj)
            wx.TheClipboard.Close()

#--------------------------------------------------------------------------
class MainFrame(wx.Frame):

    #--------------------------------------------------------------------------
    def __init__(self, buildSys, title):
        wx.Frame.__init__(self, None, -1, title, 
                          style = wx.MINIMIZE_BOX | wx.SYSTEM_MENU | 
                                  wx.CAPTION | wx.CLOSE_BOX |
                                  wx.CLIP_CHILDREN | wx.STATIC_BORDER)
        # load & set the Nebula icon
        try:
            self.SetIcon(wx.Icon(os.path.join('bin', 'win32', 'nebula.ico'),
                                 wx.BITMAP_TYPE_ICO))
        finally:
            pass
            
        self.progressDialog = None
        self.cancelProgressDlg = False
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

        # Generator controls
        self.generatorStaticBox = wx.StaticBox(self.mainPanel, -1, 'Generator',
                                               style = wx.SIMPLE_BORDER)
        generators = buildSys.GetGenerators()
        self.generatorDesc = {}
        for generatorName, generator in generators.items():
            self.generatorDesc[generatorName] = string.replace(
                                                    generator.description, 
                                                    '&', '&&')
        self.generatorComboBox = wx.ComboBox(self.mainPanel, -1, '', 
                                             (0, 0), (166, 20),
                                             self.generatorDesc.keys(), 
                                             wx.CB_READONLY)
        self.Bind(wx.EVT_COMBOBOX, self.OnSelectGenerator, 
                  self.generatorComboBox)
        self.generatorDescBox = wx.StaticText(self.mainPanel, -1, 
                                              'Please select a generator.', 
                                              (0, 0), (300, 40),
                                              wx.ST_NO_AUTORESIZE)
        
        # Workspace controls
        self.workspacesStaticBox = wx.StaticBox(self.mainPanel, -1, 
                                                'Workspaces',
                                                style = wx.SIMPLE_BORDER)
        self.workspaceListBox = wx.CheckListBox(self.mainPanel, -1,
                                                (0, 0), (150, 200), 
                                                [],
                                                style = wx.LB_SINGLE)
        self.Bind(wx.EVT_LISTBOX, self.OnSelectWorkspace, 
                  self.workspaceListBox)
        self.Bind(wx.EVT_CHECKLISTBOX, self.OnTickWorkspace, 
                  self.workspaceListBox)
        self.selectAllWorkspacesBtn = wx.Button(self.mainPanel, -1, 
                                                'All')
        self.Bind(wx.EVT_BUTTON, self.OnSelectAllWorkspaces, 
                  self.selectAllWorkspacesBtn)
        self.deselectAllWorkspacesBtn = wx.Button(self.mainPanel, -1,
                                                  'None')
        self.Bind(wx.EVT_BUTTON, self.OnDeselectAllWorkspaces,
                  self.deselectAllWorkspacesBtn)
        self.workspaceDescBox = wx.StaticText(self.mainPanel, -1,
                                              STARTUP_BOX_TEXT, 
                                              (0, 0), (150, 50),
                                              wx.ST_NO_AUTORESIZE)
        self.bldFilenameBox = wx.StaticText(self.mainPanel, -1, '',
                                            (0, 0), (150, 50),
                                            wx.ST_NO_AUTORESIZE)

        # Log window
        self.logStaticBox = wx.StaticBox(self.mainPanel, -1, 'Log',
                                         style = wx.SIMPLE_BORDER)
        self.buildLogCtrl = BuildLogCtrl(self.mainPanel, -1)
        self.buildLogCtrl.AttachToLogger(self.buildSys.logHandler)
        self.infoCheckBox = wx.CheckBox(self.mainPanel, -1, 'Show Info')
        self.infoCheckBox.SetValue(self.buildLogCtrl.GetShowInfo())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowInfo, self.infoCheckBox)
        self.warningsCheckBox = wx.CheckBox(self.mainPanel, -1, 'Show Warnings')
        self.warningsCheckBox.SetValue(self.buildLogCtrl.GetShowWarnings())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowWarnings, 
                  self.warningsCheckBox)
        self.errorsCheckBox = wx.CheckBox(self.mainPanel, -1, 'Show Errors')
        self.errorsCheckBox.SetValue(self.buildLogCtrl.GetShowErrors())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowErrors, self.errorsCheckBox)
        
        # Run button
        self.runBtn = wx.Button(self.mainPanel, -1, 'Run', (0, 0))
        self.Bind(wx.EVT_BUTTON, self.OnRun, self.runBtn)
        
        # Bind Progress Dialog Events
        self.Bind(EVT_CREATE_PROGRESS_DLG, self.OnCreateProgressDialog)
        self.Bind(EVT_UPDATE_PROGRESS_DLG, self.OnUpdateProgressDialog)
        self.Bind(EVT_DESTROY_PROGRESS_DLG, self.OnDestroyProgressDialog)
        
        # Bind Summary Dialog Event
        self.Bind(EVT_DISPLAY_SUMMARY_DLG, self.OnDisplaySummaryDialog)
        
        # Disable controls that shouldn't be messed with until we have a list
        # of available workspaces
        self.selectAllWorkspacesBtn.Disable()
        self.deselectAllWorkspacesBtn.Disable()
        self.runBtn.Disable()
        
        # Layout the controls...
        # generators area sizer
        sizerD = wx.StaticBoxSizer(self.generatorStaticBox, wx.HORIZONTAL)
        sizerD.Add(self.generatorComboBox, 0, wx.ALL, 4)
        sizerD.Add(self.generatorDescBox, 1, wx.EXPAND|wx.ALL, 4)
        # workspaces area sizers
        sizerG = wx.BoxSizer(wx.HORIZONTAL)
        sizerG.Add(self.selectAllWorkspacesBtn, 0, wx.ALIGN_CENTER|wx.ALL, 4)
        sizerG.Add(self.deselectAllWorkspacesBtn, 0, wx.ALIGN_CENTER|wx.ALL, 4)
        sizerF = wx.BoxSizer(wx.VERTICAL)
        sizerF.Add(self.workspaceListBox, 0, wx.EXPAND|wx.BOTTOM, 4)
        sizerF.Add(sizerG, 1, wx.EXPAND)
        sizerH = wx.BoxSizer(wx.VERTICAL)
        sizerH.Add(self.workspaceDescBox, 1, wx.EXPAND|wx.ALL, 4)
        sizerH.Add(self.bldFilenameBox, 1, wx.EXPAND|wx.ALL, 4)
        sizerE = wx.StaticBoxSizer(self.workspacesStaticBox, wx.HORIZONTAL)
        sizerE.Add(sizerF, 0, wx.ALL, 4)
        sizerE.Add(sizerH, 1, wx.EXPAND|wx.ALL, 4)
        # generators + workspaces area sizer
        sizerB = wx.BoxSizer(wx.VERTICAL)
        sizerB.Add(sizerD, 0, wx.EXPAND|wx.ALL, 4)
        sizerB.Add(sizerE, 0, wx.EXPAND|wx.ALL, 4)
        # log area sizer
        sizerC = wx.StaticBoxSizer(self.logStaticBox, wx.VERTICAL)
        sizerC.Add(self.buildLogCtrl, 1, wx.EXPAND|wx.ALL, 4)
        sizerC.Add(self.infoCheckBox, 0, wx.ALIGN_LEFT|wx.LEFT, 4)
        sizerC.Add(self.warningsCheckBox, 0, wx.ALIGN_LEFT|wx.LEFT|wx.TOP, 4)
        sizerC.Add(self.errorsCheckBox, 0, wx.ALIGN_LEFT|wx.ALL, 4)
        # top level sizer
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(self.staticLine, 0, wx.EXPAND)
        sizerA.Add(sizerB, 0, wx.EXPAND)
        sizerA.Add(sizerC, 1, wx.EXPAND|wx.ALL, 4)
        sizerA.Add(self.runBtn, 0, wx.ALIGN_CENTER|wx.ALL, 5)
        # get the sizers to sort out the frame's size
        sizerA.Fit(self.mainPanel)
        self.mainPanel.SetSizer(sizerA)
        self.Fit()
        # if we wanted to set the size of the frame manually, we'd do this:
        #self.mainPanel.SetSizer(self.sizerA)
        #self.mainPanel.Layout()
        #self.Fit()
        
        # Load the workspace list in a separate thread
        self.Bind(EVT_WORKSPACE_LIST_LOADED, self.OnWorkspaceListLoaded)
        thread.start_new_thread(self.LoadWorkspaceList, ())
        
    #--------------------------------------------------------------------------
    # Called when File->Exit is selected from the main menu.
    def OnMainMenuFileExit(self, evt):
        sys.stderr.flush()
        sys.stdout.flush()
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
    # Called when a generator is selected from the generator combo-box.
    def OnSelectGenerator(self, evt):
        self.generatorDescBox.SetLabel(
            self.generatorDesc[self.generatorComboBox.GetValue()])
        
    #--------------------------------------------------------------------------
    # Called when a workspace is selected in the workspace check-list-box.
    def OnSelectWorkspace(self, evt):
        workspace = self.buildSys.workspaces[
                        self.workspaceListBox.GetStringSelection()]
        self.workspaceDescBox.SetLabel(string.replace(workspace.annotation, 
                                                      '&', '&&'))
        #self.bldFilenameBox.SetLabel('Found in: ' \
        #    + self.buildSys.GetAbsPathFromRel(workspace.bldFile))
        self.bldFilenameBox.SetLabel('Found in: ' + workspace.bldFile)
    
    #--------------------------------------------------------------------------
    # Called when a workspace is ticked in the workspace check-list-box.
    def OnTickWorkspace(self, evt):
        # highlight the item that's been ticked/unticked
        index = evt.GetSelection()
        self.workspaceListBox.SetSelection(index)
        # have to do this manually, SetSelection() above doesn't seem to call
        # OnSelectWorkspace
        self.OnSelectWorkspace(None)
        
    #--------------------------------------------------------------------------
    # Called when the All button is clicked in the workspace area.
    def OnSelectAllWorkspaces(self, evt):
        for i in range(self.workspaceListBox.GetCount()):
            self.workspaceListBox.Check(i, True)
        
    #--------------------------------------------------------------------------
    # Called when the None button is clicked in the workspace area.
    def OnDeselectAllWorkspaces(self, evt):
        for i in range(self.workspaceListBox.GetCount()):
            self.workspaceListBox.Check(i, False)
    
    #--------------------------------------------------------------------------
    # Called when the list of available workspaces has been obtained.
    def OnWorkspaceListLoaded(self, evt):
        if len(self.buildSys.workspaces) > 0:
            self.workspaceListBox.InsertItems(self.buildSys.workspaces.keys(), 
                                              0)
            self.workspaceListBox.SetSelection(0)
            self.OnSelectWorkspace(None)
            
        # re-enable controls we disabled previously
        self.selectAllWorkspacesBtn.Enable(True)
        self.deselectAllWorkspacesBtn.Enable(True)
        self.runBtn.Enable(True)
        
    #--------------------------------------------------------------------------
    # Called when the Show Warnings check-box is ticked or unticked.
    def OnTickShowInfo(self, evt):
        self.buildLogCtrl.SetShowInfo(evt.IsChecked())
        
    #--------------------------------------------------------------------------
    # Called when the Show Warnings check-box is ticked or unticked.
    def OnTickShowWarnings(self, evt):
        self.buildLogCtrl.SetShowWarnings(evt.IsChecked())
        
    #--------------------------------------------------------------------------
    # Called when the Show Errors check-box is ticked or unticked.
    def OnTickShowErrors(self, evt):
        self.buildLogCtrl.SetShowErrors(evt.IsChecked())
        
    #--------------------------------------------------------------------------
    # Called when the Run button is clicked.
    def OnRun(self, evt):
        generatorName = self.generatorComboBox.GetValue()
        if '' == generatorName:
            dlg = wx.MessageDialog(self, 'Please select a generator.',
                                   'Insufficient Parameters',
                                   style = wx.ICON_EXCLAMATION|wx.OK)
            dlg.ShowModal()
            dlg.Destroy()
            return
            
        workspaceNames = []
        for i in range(self.workspaceListBox.GetCount()):
            if self.workspaceListBox.IsChecked(i):
                workspaceNames.append(self.workspaceListBox.GetString(i))
        if len(workspaceNames) < 1:
            dlg = wx.MessageDialog(self, 
                                   'Please select at least one workspace.',
                                   'Insufficient Parameters',
                                   style = wx.ICON_EXCLAMATION|wx.OK)
            dlg.ShowModal()
            dlg.Destroy()
            return
            
        # run the build system in a separate thread
        thread.start_new_thread(self.RunBuildSys, (generatorName, 
                                                   workspaceNames))
                
    #--------------------------------------------------------------------------
    # Obtain a list of available workspaces and update the workspace
    # check-list-box when done.
    def LoadWorkspaceList(self):
        if (self.buildSys.Prepare()):
            #self.buildSys.logger.info('LoadWorkspaceList() done!')
            evt = WorkspaceListLoadedEvent()
            wx.PostEvent(self, evt)
            
    #--------------------------------------------------------------------------
    # Run the build system for the specified generator and workspaces.
    def RunBuildSys(self, generatorName, workspaceNames):
        self.buildSys.AttachProgressDialog(self.CreateProgressDialog,
                                           self.UpdateProgressDialog,
                                           self.ProgressDialogCancelled,
                                           self.DestroyProgressDialog)
        self.buildSys.ShowProgressDialog(True)
        self.buildSys.AttachSummaryDialog(self.DisplaySummaryDialog)
        self.buildSys.Run(generatorName, workspaceNames)
        
    #--------------------------------------------------------------------------
    # Create and show the progress dialog.
    # This can be safely called from any thread.
    def CreateProgressDialog(self, title, message, maxVal = 100, 
                             canAbort = True):
        assert None == self.progressDialog
        style = wx.PD_AUTO_HIDE|wx.PD_APP_MODAL
        if canAbort:
            style |= wx.PD_CAN_ABORT
        evt = CreateProgressDialogEvent(dlgTitle = title, dlgMsg = message,
                                        dlgMaxVal = maxVal, dlgStyle = style)
        wx.PostEvent(self, evt)
        # TODO: block a non-gui thread until the dialog has actually been 
        # created? Using threading.Event
        
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnCreateProgressDialog(self, evt):
        assert None == self.progressDialog
        self.progressDialog = wx.ProgressDialog(evt.dlgTitle, evt.dlgMsg,
                                                evt.dlgMaxVal, None, 
                                                evt.dlgStyle)
        self.cancelProgressDlg = False
                                                
    #--------------------------------------------------------------------------
    # Update the progress dialog.
    # This can be safely called from any thread.
    def UpdateProgressDialog(self, val, message):
        evt = UpdateProgressDialogEvent(dlgVal = val, dlgMsg = message)
        wx.PostEvent(self, evt)
    
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnUpdateProgressDialog(self, evt):
        if self.progressDialog != None:
            self.cancelProgressDlg = not self.progressDialog.Update(evt.dlgVal, 
                                                                    evt.dlgMsg)

    #--------------------------------------------------------------------------
    # Check if the user pressed the cancel button on the progress dialog.
    # This can be called from any thread.
    def ProgressDialogCancelled(self):
        return self.cancelProgressDlg

    #--------------------------------------------------------------------------
    # Destroy the progress dialog.
    # This can be safely called from any thread.
    def DestroyProgressDialog(self):
        evt = DestroyProgressDialogEvent()
        wx.PostEvent(self, evt)
        
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnDestroyProgressDialog(self, evt):
        if self.progressDialog != None:
            self.progressDialog.Destroy()
            self.progressDialog = None
            
    #--------------------------------------------------------------------------
    # This can be safely called from any thread.
    def DisplaySummaryDialog(self, evtDetails):
        evt = DisplaySummaryDialogEvent(details = evtDetails)
        wx.PostEvent(self, evt)
        
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnDisplaySummaryDialog(self, evt):
        dlg = BuildSummaryDialog(self, -1, "Build Summary", evt.details)
        dlg.CenterOnParent()
        dlg.ShowModal()
        dlg.Destroy()

#--------------------------------------------------------------------------
class MainApp(wx.App):
    
    #--------------------------------------------------------------------------
    def __init__(self, buildSys, redirect = False, filename = None, 
                 useBestVisual = False, clearSigInt = True):
        self.buildSys = buildSys
        wx.App.__init__(self, redirect, filename, useBestVisual, clearSigInt)
        
    #--------------------------------------------------------------------------
    def OnInit(self):
        mainFrame = MainFrame(self.buildSys, 'Nebula 2 Build System')
        self.SetTopWindow(mainFrame)
        mainFrame.CenterOnScreen()
        mainFrame.Show(True)
        return True
        
#--------------------------------------------------------------------------
def DisplayGUI(buildSys):
    app = MainApp(buildSys, redirect=True, filename='gui.log')
    app.MainLoop()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
