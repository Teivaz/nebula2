#--------------------------------------
# shdtuner.py
#--------------------------------------

import pynebula as nebula
import sys, wx

#--------------------------------------

fileSvr = nebula.lookup('/sys/servers/file2')
sys.path += [fileSvr.manglepath('home:data/scripts/shdtuner')]

from shdctrls import *

#--------------------------------------

class ShdTunerApp(wx.App):
    def __init__(self):
        wx.App.__init__(self, redirect = True, filename = 'shdtuner.log')

    def OnInit(self):
        global mainFrame
        mainFrame = MainFrame(self)
        self.SetTopWindow(mainFrame)
        mainFrame.Show(True)
        self.isRunning = True
        self.eventLoop = wx.EventLoop()
        wx.EventLoop.SetActive(self.eventLoop)
        nebula.setTrigger(self.Trigger)
        return True

    def Shutdown(self):
        self.isRunning = False

    def Trigger(self):
        while self.eventLoop.Pending():
            self.eventLoop.Dispatch()
        self.ProcessIdle()
        if self.isRunning == False:
            nebula.exit()

#--------------------------------------

def OnShowWindow():
    app = ShdTunerApp()

#--------------------------------------

def OnMouseClick(button, x, y):
    global mainFrame
    mainFrame.renderPanel.OnMouseClick(button, x, y)

#--------------------------------------

def OnGuiServerOpen():
    pass

#--------------------------------------

def OnGuiServerClose():
    pass

#--------------------------------------
# Eof
#--------------------------------------