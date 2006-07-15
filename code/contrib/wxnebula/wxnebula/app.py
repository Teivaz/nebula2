import wx
import pynebula as nebula

from wxnebula.controls import MainWindowFrame

class NebulaWXApp(wx.App):
    def OnInit(self):
        frame = MainWindowFrame()
        frame.SetApplication(self)
        frame.Show(True)
        self.SetTopWindow(frame)
        self.keepGoing = True
        # Set up our very own event loop...
        self.eventLoop = wx.EventLoop()
        wx.EventLoop.SetActive(self.eventLoop)
        return True

    def Trigger(self):
        while self.eventLoop.Pending():
            self.eventLoop.Dispatch()
        self.ProcessIdle()
        if self.keepGoing == False:
            nebula.exit()

