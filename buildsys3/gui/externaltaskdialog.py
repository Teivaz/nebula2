#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
# External Task Output Dialog
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.newevent

(DisplayDialogEvent, EVT_DISPLAY_DLG) = wx.lib.newevent.NewEvent()
(DestroyDialogEvent, EVT_DESTROY_DLG) = wx.lib.newevent.NewEvent()
(AppendTextEvent,    EVT_APPEND_TEXT) = wx.lib.newevent.NewEvent()
(EnableButtonsEvent, EVT_ENABLE_BUTTONS) = wx.lib.newevent.NewEvent()

#--------------------------------------------------------------------------
# This class should never be instantiated 
# (except by ExternalTaskDialogProxy).
class ExternalTaskDialog(wx.Dialog):
    def __init__(self, parentWindow, title, pos = wx.DefaultPosition,
                 size = wx.DefaultSize,
                 style = wx.CAPTION|wx.RESIZE_BORDER):
        wx.Dialog.__init__(self, parentWindow, -1, title, pos, size, style)
        outputTextBoxStyle = wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_BESTWRAP
        self.outputTextBox = wx.TextCtrl(self, -1, '', wx.DefaultPosition,
                                         (500, 300), outputTextBoxStyle)
        self.okBtn = wx.Button(self, wx.ID_OK, 'OK')
        self.cancelBtn = wx.Button(self, wx.ID_CANCEL, 'Cancel')
        
        # set default button states
        self.okBtn.Disable()
        self.cancelBtn.SetDefault()
        
        # bind events
        self.Bind(EVT_DISPLAY_DLG, self.OnDisplay)
        self.Bind(EVT_DESTROY_DLG, self.OnDestroy)
        self.Bind(EVT_APPEND_TEXT, self.OnAppendText)
        self.Bind(EVT_ENABLE_BUTTONS, self.OnEnableButtons)
        
        # button handlers will be set later
        self.okBtnHandler = None
        self.cancelBtnHandler = None
        
        # layout
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        mainSizer.Add(self.outputTextBox, 1, wx.EXPAND|wx.ALL, 10)
        buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
        buttonSizer.Add(self.okBtn, 0, wx.ALL, 5)
        buttonSizer.Add(self.cancelBtn, 0, wx.ALL, 5)
        mainSizer.Add(buttonSizer, 0, wx.ALIGN_CENTRE)
        self.SetSizer(mainSizer)
        self.Fit()
        
    #--------------------------------------------------------------------------
    # Set the function to be called after the user presses the OK button.
    def SetOkBtnHandler(self, handler):
        self.okBtnHandler = handler
        
    #--------------------------------------------------------------------------
    # Set the function to be called after the user presses the Cancel button.
    def SetCancelBtnHandler(self, handler):
        self.cancelBtnHandler = handler
        
    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------
        
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnDisplay(self, evt):
        self.CenterOnParent()
        retval = self.ShowModal()
                
        if wx.ID_OK == retval:
            if self.okBtnHandler != None:
                self.okBtnHandler()
        else:
            if self.cancelBtnHandler != None:
                self.cancelBtnHandler()
                
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnDestroy(self, evt):
        self.Destroy()
        
    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnAppendText(self, evt):
        self.outputTextBox.AppendText(evt.dlgTextToAppend)

    #--------------------------------------------------------------------------
    # This will always be called in the GUI thread's context.
    def OnEnableButtons(self, evt):
        self.okBtn.Enable(evt.enableOkBtn)
        self.cancelBtn.Enable(evt.enableCancelBtn)

#--------------------------------------------------------------------------
# This is a proxy for an actual dialog, the proxy ensures the real dialog
# is only manipulated within the GUI thread's context.
# 
# This class must be instantiated only within the GUI thread's context,
# this should generally be done by calling 
# buildsys3.main.MainFrame.CreateExternalTaskDialogProxy() 
# (which can be called from any thread).
#
# Note that once an instance of this class has been created all the methods 
# can be called from any thread.
class ExternalTaskDialogProxy:
    
    #--------------------------------------------------------------------------
    def __init__(self, parentWindow, dlgTitle,
                 okBtnHandler = None, cancelBtnHandler = None):
        self.dialog = ExternalTaskDialog(parentWindow, dlgTitle)
        self.dialog.SetOkBtnHandler(okBtnHandler)
        self.dialog.SetCancelBtnHandler(cancelBtnHandler)
        
    #--------------------------------------------------------------------------
    # Display a dialog that will be used to display output from an external
    # application.
    # This can be safely called from any thread.
    def Display(self):
        if self.dialog != None:
            evt = DisplayDialogEvent()
            wx.PostEvent(self.dialog, evt)
    
    #--------------------------------------------------------------------------
    # Destroy the underlying dialog.
    # This can be safely called from any thread.
    def Destroy(self):
        if self.dialog != None:
            evt = DestroyDialogEvent()
            wx.PostEvent(self.dialog, evt)
            self.dialog = None
        
    #--------------------------------------------------------------------------
    # Append text to the dialog that is used to display output from an external
    # application.
    # This can be safely called from any thread.
    def AppendText(self, text):
        if self.dialog != None:
            evt = AppendTextEvent(dlgTextToAppend = text)
            wx.PostEvent(self.dialog, evt)
        
    #--------------------------------------------------------------------------
    # Enable/Disable the OK and Cancel buttons on the underlying dialog.
    # This can be safely called from any thread.
    def EnableButtons(self, enableOk, enableCancel):
        if self.dialog != None:
            evt = EnableButtonsEvent(enableOkBtn = enableOk, 
                                     enableCancelBtn = enableCancel)
            wx.PostEvent(self.dialog, evt)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------

