import sys
import time
import wx
import pynebula as nebula

# Add some stuff to sys.path so that import can find it.
fileServer = nebula.lookup('/sys/servers/file2')
sys.path += [fileServer.manglepath('home:data/scripts/')]
sys.path += [fileServer.manglepath('home:code/contrib/wxnebula/')]

from startup import *
from wxnebula.app import NebulaWXApp

CoreOnStartup = OnStartup
def OnStartup():
    CoreOnStartup()
    # Create our application object.
    app = NebulaWXApp(0)
    # Configure the gfx server.
    gfxServer = nebula.lookup('/sys/servers/gfx')
    gfxServer.setskipmsgloop(True)
    gfxServer.setdisplaymode('test', 'childwindow', 0, 0, 640, 480, True)
    # And trigger our application once/frame so that it can respond to
    # events and stuff.
    nebula.setTrigger(app.Trigger)

