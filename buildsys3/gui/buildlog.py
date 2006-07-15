#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.mixins.listctrl as listmix
import wx.lib.newevent
import logging, string

# This creates a new Event class and an EVT binder function
(NewBuildLogRecordEvent, EVT_NEW_BUILD_LOG_RECORD) = wx.lib.newevent.NewEvent()

#--------------------------------------------------------------------------
class BuildLogListCtrlHandler(logging.Handler):
    
    def __init__(self, buildLogCtrl, level = logging.NOTSET):
        logging.Handler.__init__(self, level)
        self.buildLogCtrl = buildLogCtrl
        
    def emit(self, logRecord):
        evt = NewBuildLogRecordEvent(record = logRecord)
        wx.PostEvent(self.buildLogCtrl, evt)

#--------------------------------------------------------------------------
class BuildLogListCtrl(wx.ListCtrl, listmix.ListCtrlAutoWidthMixin):
    
    LOG_RECORD_INFO_TEXT = '''\
[%(asctime)s] %(levelname)s
%(message)s'''
    
    [ID_CLEAR_LOG, ID_COPY_RECORDS
    ] = map(lambda init_ids: wx.NewId(), range(2))
    
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
        self.recDescTextBox = None
        self.logRecs = []
        self.infoRecIndices = []
        self.errorRecIndices = []
        self.warningRecIndices = []
        self.displayedRecIndices = []
        self.Bind(EVT_NEW_BUILD_LOG_RECORD, self.OnNewLogRecord)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected)
        self.Bind(wx.EVT_COMMAND_RIGHT_CLICK, self.OnRightMouseUp) # for wxMSW
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightMouseUp) # for wxGTK
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightMouseDown)
        self.Bind(wx.EVT_MENU, self.OnClearLog, id = self.ID_CLEAR_LOG)
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
                    menu.Append(self.ID_COPY_RECORDS, 'Copy')
                    menu.AppendSeparator()
                else:
                    # deselect all
                    for i in range(len(selectedItems)):
                        self.SetItemState(i, 0, wx.LIST_STATE_SELECTED)
            else:
                self.Select(hitItem)
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
        self.logHandler = BuildLogListCtrlHandler(self)
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
        if self.recDescTextBox != None:
            self.recDescTextBox.SetValue('')
        
    #--------------------------------------------------------------------------
    # Set the TextBox that will be populated with the details of the currently
    # selected log record.
    def AttachToDescTextBox(self, textBox):
        self.recDescTextBox = textBox
        
    #--------------------------------------------------------------------------
    def OnItemSelected(self, evt):
        if self.recDescTextBox != None:
            record = self.logRecs[self.displayedRecIndices[evt.m_itemIndex]]
            formatter = logging.Formatter('%(message)s')
            args = { 'levelname' : record.levelname,
                     'asctime' : record.asctime,
                     'message' : formatter.format(record) }
            text = self.LOG_RECORD_INFO_TEXT % args
            self.recDescTextBox.SetValue(text)
        
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
class BuildLogPanel(wx.Panel):
    def __init__(self, parentWindow, buildSys):
        wx.Panel.__init__(self, parentWindow)
        self.logStaticBox = wx.StaticBox(self, -1, 'Log')
        self.buildLogCtrl = BuildLogListCtrl(self, -1)
        self.buildLogCtrl.AttachToLogger(buildSys.logHandler)
        self.infoCheckBox = wx.CheckBox(self, -1, 'Show Info')
        self.infoCheckBox.SetValue(self.buildLogCtrl.GetShowInfo())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowInfo, self.infoCheckBox)
        self.warningsCheckBox = wx.CheckBox(self, -1, 'Show Warnings')
        self.warningsCheckBox.SetValue(self.buildLogCtrl.GetShowWarnings())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowWarnings, 
                  self.warningsCheckBox)
        self.errorsCheckBox = wx.CheckBox(self, -1, 'Show Errors')
        self.errorsCheckBox.SetValue(self.buildLogCtrl.GetShowErrors())
        self.Bind(wx.EVT_CHECKBOX, self.OnTickShowErrors, self.errorsCheckBox)
        self.logRecTextBox = wx.TextCtrl(self, -1, '', 
                                         (0, 0), (380, 50), 
                                         wx.TE_MULTILINE
                                         |wx.TE_READONLY
                                         |wx.TE_LINEWRAP)
        self.buildLogCtrl.AttachToDescTextBox(self.logRecTextBox)
        
        # Layout the controls...
        sizerC = wx.BoxSizer(wx.VERTICAL)
        sizerC.Add(self.infoCheckBox, 0, wx.ALIGN_LEFT)
        sizerC.Add(self.warningsCheckBox, 0, wx.ALIGN_LEFT|wx.TOP, 4)
        sizerC.Add(self.errorsCheckBox, 0, wx.ALIGN_LEFT|wx.TOP, 4)
        sizerB = wx.BoxSizer(wx.HORIZONTAL)
        sizerB.Add(sizerC, 0, wx.ALIGN_TOP|wx.LEFT, 4)
        sizerB.Add(self.logRecTextBox, 1, wx.EXPAND|wx.LEFT|wx.RIGHT, 4)
        sizerA = wx.StaticBoxSizer(self.logStaticBox, wx.VERTICAL)
        sizerA.Add(self.buildLogCtrl, 1, wx.EXPAND|wx.ALL, 4)
        sizerA.Add(sizerB, 0, wx.EXPAND|wx.TOP|wx.BOTTOM, 4)
        sizerA.Fit(self)
        self.SetSizer(sizerA)
        
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
# EOF
#--------------------------------------------------------------------------
