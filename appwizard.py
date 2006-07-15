#----------------------------------------------------------------------
#  appwizard.py
#
#  nApplication wizard dialog which helps to generate napplication
#  devied class.
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import zlib
import  wx
from  appwiz.appframe import *

#----------------------------------------------------------------------
#  wxPyton Application.
#----------------------------------------------------------------------
class WizApp(wx.App):
    def __init__(self, name):
        self.name = name
        wx.App.__init__(self, redirect=False)

    def OnInit(self):
        frame = AppWizFrame(None, -1, "AppWizard", 
                            pos=(0,0), size=(400,200), 
                            style=wx.DEFAULT_FRAME_STYLE)
 
        return True

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
def main(argv):
    app = WizApp("AppWizard")
    app.MainLoop()

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
if __name__ == '__main__':
    import sys,os
    main(sys.argv)

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
