#-------------------------------------------------------------------------------
# medsplash.py
#-------------------------------------------------------------------------------

import os, wx
from mainframe import MainFrame

class MedSplash(wx.SplashScreen):
    def __init__(self):
        bmp = wx.Image(os.path.join('res', 'medsplash.png')).ConvertToBitmap()
        wx.SplashScreen.__init__(self, bmp, wx.SPLASH_CENTRE_ON_SCREEN | wx.SPLASH_TIMEOUT, 5000, None)
        self.futureCall = wx.FutureCall(2000, self.showMainFrame)
        self.Bind(wx.EVT_CLOSE, self.onClose)

# private
    def showMainFrame(self):
        mainFrame = MainFrame()
        mainFrame.CenterOnScreen()
        mainFrame.Show()
        if self.futureCall.IsRunning():
            self.Raise()

    def onClose(self, event):
        self.Hide()
        if self.futureCall.IsRunning():
            self.futureCall.Stop()
            self.showMainFrame()
        event.Skip()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------