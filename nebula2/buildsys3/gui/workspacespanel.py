#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.newevent
import string, thread

STARTUP_BOX_TEXT = """\
Please wait...

    Loading Workspaces.
"""

SUMMARY_DLG_TEXT = """\
All done!

Built %(numOfWorkspacesBuilt)i workspace(s) out of %(totalNumOfWorkspaces)i.
"""

(WorkspaceListLoadedEvent, EVT_WORKSPACE_LIST_LOADED) = wx.lib.newevent.NewEvent()

(CreateProgressDialogEvent,  EVT_CREATE_PROGRESS_DLG) = wx.lib.newevent.NewEvent()
(UpdateProgressDialogEvent,  EVT_UPDATE_PROGRESS_DLG) = wx.lib.newevent.NewEvent()
(DestroyProgressDialogEvent, EVT_DESTROY_PROGRESS_DLG) = wx.lib.newevent.NewEvent()

(DisplaySummaryDialogEvent, EVT_DISPLAY_SUMMARY_DLG) = wx.lib.newevent.NewEvent()

#--------------------------------------------------------------------------
class WorkspacesPanel(wx.Panel):
    
    def __init__(self, parentWindow, buildSys, generatorName, workspaceNames):
        wx.Panel.__init__(self, parentWindow)
        self.buildSys = buildSys
        self.defaultGeneratorName = generatorName
        self.defaultWorkspaceNames = workspaceNames
        
        self.progressDialog = None
        self.cancelProgressDlg = False
        
        # Generator controls
        self.generatorStaticBox = wx.StaticBox(self, -1, 'Generator',
                                               style = wx.SIMPLE_BORDER)
        generators = buildSys.GetGenerators()
        self.generatorDesc = {}
        for generatorName, generator in generators.items():
            self.generatorDesc[generatorName] = string.replace(
                                                    generator.description, 
                                                    '&', '&&')
        generatorComboBoxVal = ''
        generatorDescBoxVal = 'Please select a generator.'
        if self.defaultGeneratorName in self.generatorDesc:
            generatorComboBoxVal = generatorName
            generatorDescBoxVal = self.generatorDesc[self.defaultGeneratorName]
        generatorNames = self.generatorDesc.keys()
        generatorNames.sort()
        self.generatorComboBox = wx.ComboBox(self, -1, 
                                             generatorComboBoxVal, 
                                             (0, 0), (166, 20),
                                             generatorNames, 
                                             wx.CB_READONLY)
        self.Bind(wx.EVT_COMBOBOX, self.OnSelectGenerator, 
                  self.generatorComboBox)
        self.generatorDescBox = wx.StaticText(self, -1, 
                                              generatorDescBoxVal, 
                                              (0, 0), (300, 40),
                                              wx.ST_NO_AUTORESIZE)
        
        # Workspace controls
        self.workspacesStaticBox = wx.StaticBox(self, -1, 
                                                'Workspaces',
                                                style = wx.SIMPLE_BORDER)
        self.workspaceListBox = wx.CheckListBox(self, -1,
                                                (0, 0), (150, 200), 
                                                [],
                                                style = wx.LB_SINGLE)
        self.Bind(wx.EVT_LISTBOX, self.OnSelectWorkspace, 
                  self.workspaceListBox)
        self.Bind(wx.EVT_CHECKLISTBOX, self.OnTickWorkspace, 
                  self.workspaceListBox)
        self.selectAllWorkspacesBtn = wx.Button(self, -1, 
                                                'All')
        self.Bind(wx.EVT_BUTTON, self.OnSelectAllWorkspaces, 
                  self.selectAllWorkspacesBtn)
        self.deselectAllWorkspacesBtn = wx.Button(self, -1,
                                                  'None')
        self.Bind(wx.EVT_BUTTON, self.OnDeselectAllWorkspaces,
                  self.deselectAllWorkspacesBtn)
        self.workspaceDescBox = wx.StaticText(self, -1,
                                              STARTUP_BOX_TEXT, 
                                              (0, 0), (150, 50),
                                              wx.ST_NO_AUTORESIZE)
        self.bldFilenameBox = wx.StaticText(self, -1, '',
                                            (0, 0), (150, 50),
                                            wx.ST_NO_AUTORESIZE)
        
        # Run button
        self.runBtn = wx.Button(self, -1, 'Run', (0, 0))
        self.Bind(wx.EVT_BUTTON, self.OnRun, self.runBtn)
        
        # Disable controls that shouldn't be messed with until we have a list
        # of available workspaces
        self.selectAllWorkspacesBtn.Disable()
        self.deselectAllWorkspacesBtn.Disable()
        self.runBtn.Disable()

        # Bind Progress Dialog Events
        self.Bind(EVT_CREATE_PROGRESS_DLG, self.OnCreateProgressDialog)
        self.Bind(EVT_UPDATE_PROGRESS_DLG, self.OnUpdateProgressDialog)
        self.Bind(EVT_DESTROY_PROGRESS_DLG, self.OnDestroyProgressDialog)
        
        # Bind Summary Dialog Event
        self.Bind(EVT_DISPLAY_SUMMARY_DLG, self.OnDisplaySummaryDialog)
        
        # Bind Workspace List Loaded Event
        self.Bind(EVT_WORKSPACE_LIST_LOADED, self.OnWorkspaceListLoaded)
        
        # Layout the controls...
        # generators area sizer
        sizerB = wx.StaticBoxSizer(self.generatorStaticBox, wx.HORIZONTAL)
        sizerB.Add(self.generatorComboBox, 0, wx.ALL, 4)
        sizerB.Add(self.generatorDescBox, 1, wx.EXPAND|wx.ALL, 4)
        # workspaces area sizers
        sizerE = wx.BoxSizer(wx.HORIZONTAL)
        sizerE.Add(self.selectAllWorkspacesBtn, 0, wx.ALIGN_CENTER|wx.ALL, 4)
        sizerE.Add(self.deselectAllWorkspacesBtn, 0, wx.ALIGN_CENTER|wx.ALL, 4)
        sizerD = wx.BoxSizer(wx.VERTICAL)
        sizerD.Add(self.workspaceListBox, 0, wx.EXPAND|wx.BOTTOM, 4)
        sizerD.Add(sizerE, 1, wx.EXPAND)
        sizerF = wx.BoxSizer(wx.VERTICAL)
        sizerF.Add(self.workspaceDescBox, 1, wx.EXPAND|wx.ALL, 4)
        sizerF.Add(self.bldFilenameBox, 1, wx.EXPAND|wx.ALL, 4)
        sizerC = wx.StaticBoxSizer(self.workspacesStaticBox, wx.HORIZONTAL)
        sizerC.Add(sizerD, 0, wx.ALL, 4)
        sizerC.Add(sizerF, 1, wx.EXPAND|wx.ALL, 4)
        # generators + workspaces area sizer
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(sizerB, 0, wx.EXPAND|wx.ALL, 4)
        sizerA.Add(sizerC, 0, wx.EXPAND|wx.ALL, 4)
        sizerA.Add(self.runBtn, 0, wx.ALIGN_CENTER|wx.ALL, 5)
        sizerA.Fit(self)
        self.SetSizer(sizerA)

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
    # Called when the Run button is clicked.
    def OnRun(self, evt):
        generatorName = self.generatorComboBox.GetValue()
        if '' == generatorName:
            dlg = wx.MessageDialog(self, 'Please select a generator.',
                                   'Insufficient Parameters',
                                   wx.ICON_EXCLAMATION|wx.OK)
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
                                   wx.ICON_EXCLAMATION|wx.OK)
            dlg.ShowModal()
            dlg.Destroy()
            return
            
        # run the build system in a separate thread
        thread.start_new_thread(self.RunBuildSys, (generatorName, 
                                                   workspaceNames))

    #--------------------------------------------------------------------------
    # Called when the list of available workspaces has been obtained.
    def OnWorkspaceListLoaded(self, evt):
        workspaceNames = self.buildSys.workspaces.keys()
        workspaceNames.sort()
        
        # populate the list box
        if len(workspaceNames) > 0:
            self.workspaceListBox.InsertItems(workspaceNames, 0)
            self.workspaceListBox.SetSelection(0)
            self.OnSelectWorkspace(None)
            
        # tick the workspaces passed in via cmd line args
        workspaceIdx = 0
        for workspaceName in workspaceNames:
            if workspaceName in self.defaultWorkspaceNames:
                self.workspaceListBox.Check(workspaceIdx, True)
            workspaceIdx += 1
            
        # re-enable controls we disabled previously
        self.selectAllWorkspacesBtn.Enable(True)
        self.deselectAllWorkspacesBtn.Enable(True)
        self.runBtn.Enable(True)
        
    #--------------------------------------------------------------------------
    # Return True if the workspace list has been populated, False otherwise.
    def WorkspaceListLoaded(self):
        # The Run button is disabled while the list is being loaded, so we
        # can assume when it's enabled the list has already been loaded.
        return self.runBtn.IsEnabled()

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
# EOF
#--------------------------------------------------------------------------
