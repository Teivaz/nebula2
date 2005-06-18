import wx
import pynebula as nebula

from wxnebula.controls import MainWindowFrame

class NebulaWXApp(wx.App):
    def OnInit(self):
        frame = MainWindowFrame()
        frame.Show(True)
        self.SetTopWindow(frame)
        self.keepGoing = True
        return True

    def Trigger(self):
        while self.Pending():
            self.Dispatch()
        self.ProcessIdle()
        if self.keepGoing == False:
            nebula.exit()

