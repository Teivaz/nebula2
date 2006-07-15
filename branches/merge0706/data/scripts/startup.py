#-------------------------------------------------------------------------------
#   bin/startup.py
#
#   The script mainly sets up assigns and Nebula variables.
#
#   (C) 2003 RadonLabs GmbH
#-------------------------------------------------------------------------------
import pynebula as nebula

#-------------------------------------------------------------------------------
#   OnStartup
#
#   This procedure is called right after application startup. Use it 
#   to initialize Nebula assigns.
#-------------------------------------------------------------------------------
def OnStartup():
    fileServer = nebula.lookup('/sys/servers/file2')
    proj = fileServer.manglepath("proj:")
    home = fileServer.manglepath("home:")

    # setup assigns
    fileServer.setassign("data",     home + "/data/")
    fileServer.setassign("export",   proj + "/export/")
    fileServer.setassign("scripts",  "data:scripts")
    fileServer.setassign("physics",  "export:physics/")
    fileServer.setassign("meshes",   "export:meshes/")
    fileServer.setassign("textures", "export:textures/")
    fileServer.setassign("anims",    "export:anims/")
    fileServer.setassign("gfxlib",   "export:gfxlib/")
    fileServer.setassign("lights",   "export:lightlib/")
    fileServer.setassign("levels",   "export:levels/")
    fileServer.setassign("sound",    "export:audio/")

#-------------------------------------------------------------------------------
#   OnShutdown
#   
#   This procedure is called when the application shuts down.
#-------------------------------------------------------------------------------
def OnShutdown():
    pass

#-------------------------------------------------------------------------------
#   OnGraphicsStartup
#
#   This method is called right after graphics is initialized.
#-------------------------------------------------------------------------------
def OnGraphicsStartup():
    if not nebula.exists('/sys/servers/scene'):
        print "The scene server hasn't been created yet?"
        return
    sceneServer = nebula.lookup('/sys/servers/scene')
    if nebula.exists('/sys/servers/gfx'):
        featureSet = nebula.lookup('/sys/servers/gfx').getfeatureset()
        if featureSet == "dx9":
            # DX9 hardware without floating point render targets
            sceneServer.setrenderpathfilename("data:shaders/dx9_renderpath.xml")
        elif featureSet == "dx9flt":
            # DX9 hardware with floating point render targets,
            # use HDR render path
            sceneServer.setrenderpathfilename("data:shaders/dx9hdr_renderpath.xml")
        else:
            # non-DX9 hardware, use fixed function render path
            sceneServer.setrenderpathfilename("data:shaders/dx7_renderpath.xml")
    else:
        sceneServer.setrenderpathfilename("data:shaders/dx9_renderpath.xml")

    # enable zFail shadow rendering
    if nebula.exists('/sys/servers/shadow'):
        nebula.lookup('/sys/servers/shadow').setusezfail(True)

#-------------------------------------------------------------------------------
#   OnGraphicsShutdown
#
#   This method is not yet called.
#-------------------------------------------------------------------------------
def OnGraphicsShutdown():
    pass

#-------------------------------------------------------------------------------
#   OnMapInput is called back by npyviewer when the input mapping should be
#   defined.
#-------------------------------------------------------------------------------
def OnViewerMapInput():
    inputServer = nebula.lookup('/sys/servers/input')
    inputServer.beginmap()
    inputServer.map("keyb0:space.down",       "reset")
    inputServer.map("keyb0:esc.down",
                    "script:lookup('/sys/servers/gui').togglesystemgui()")
    inputServer.map("keyb0:f11.down",         "console")
    inputServer.map("keyb0:t.down",
                    "script:lookup('/sys/servers/time').resettime()")
    inputServer.map("relmouse0:btn0.pressed", "look")
    inputServer.map("relmouse0:btn1.pressed", "zoom")
    inputServer.map("relmouse0:btn2.pressed", "pan")
    inputServer.map("relmouse0:+zbtn.down",   "ScrollDown")
    inputServer.map("relmouse0:-zbtn.down",   "ScrollUp")
    inputServer.map("relmouse0:-x",           "left")
    inputServer.map("relmouse0:+x",           "right")
    inputServer.map("relmouse0:-y",           "up")
    inputServer.map("relmouse0:+y",           "down")
    inputServer.map("relmouse0:-z",           "zoomIn")
    inputServer.map("relmouse0:+z",           "zoomOut")
    inputServer.map("keyb0:f1.down",          "mayacontrols")
    inputServer.map("keyb0:f2.down",          "flycontrols")
    inputServer.map("keyb0:f12.down",         "screenshot")
    inputServer.endmap()

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is opened.
#-------------------------------------------------------------------------------
def OnGuiServerOpen():
    # initialize the default tooltip
    guiServer = nebula.lookup('/sys/servers/gui')
    guiRoot = guiServer.getrootpath()
    oldCwd = nebula.psel()
    nebula.sel(guiRoot)

    toolTip = nebula.new("nguitooltip", "Tooltip")
    toolTip.setdefaultbrush("tooltip")
    toolTip.setfont("GuiSmall")
    toolTip.setcolor(0, 0, 0, 1)
    toolTip.setalignment("left")
    toolTip.setborder(0.005, 0.005)

    # define the system skin
    skin = guiServer.newskin('system')
    # set texture path pre- and postfix (NOTE: don't change path to textures:system!!!)
    skin.settextureprefix("home:export/textures/system/")
    skin.settexturepostfix(".dds")

    # active and inactive window modulation color
    skin.setactivewindowcolor(1.0, 1.0, 1.0, 1.0)
    skin.setinactivewindowcolor(0.6, 0.6, 0.6, 0.6)
    skin.setbuttontextcolor(0.0, 0.0, 0.0, 1.0)
    skin.settitletextcolor(0.0, 0.0, 0.0, 1.0)
    skin.setlabeltextcolor(0.0, 0.0, 0.0, 1.0)
    skin.setentrytextcolor(0.0, 0.0, 0.0, 1.0)
    skin.settextcolor(0.0, 0.0, 0.0, 1.0)
    skin.setmenutextcolor(0.0, 0.0, 0.0, 1.0)

    # define brushes
    skin.beginbrushes()

    # window title bar, window background, tooltip background
    skin.addbrush('titlebar','skin',66,152,10,20,1.0,1.0,1.0,1.0)
    skin.addbrush('window','skin',8,154,4,4,1.0,1.0,1.0,1.0)
    skin.addbrush('tooltip','skin',8,154,4,4,1.0,1.0,0.878,0.8)
    skin.addbrush('pink','skin',8,154,4,4,1.0,1.0,1.0,1.0)
    skin.addbrush('dragbox','skin',8,154,4,4,1.0,0.8,0.8,0.5)

    # text entry field
    skin.addbrush('textentry_n','skin',446,124,8,8,0.7,0.7,0.7,1.0)
    skin.addbrush('textentry_p','skin',446,124,8,8,0.8,0.8,0.8,1.0)
    skin.addbrush('textentry_h','skin',446,124,8,8,0.9,0.9,0.9,1.0)
    skin.addbrush('textcursor','skin',446,124,8,8,1.0,0.8,0.8,0.5)

    # the window close button
    skin.addbrush('close_n','skin',388,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('close_h','skin',388,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('close_p','skin',388,40,16,16,1.0,1.0,1.0,1.0)

    # the window size button
    skin.addbrush('size_n','skin',372,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('size_h','skin',372,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('size_p','skin',372,40,16,16,1.0,1.0,1.0,1.0)

    # arrows
    skin.addbrush('arrowleft_n','skin',68,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowleft_h','skin',84,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowleft_p','skin',100,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowright_n','skin',116,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowright_h','skin',132,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowright_p','skin',148,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowup_n','skin',164,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowup_h','skin',180,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowup_p','skin',196,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowdown_n','skin',20,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowdown_h','skin',36,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('arrowdown_p','skin',52,40,16,16,1.0,1.0,1.0,1.0)

    # sliders
    skin.addbrush('sliderhori_bg','skin', 228, 40, 16, 16, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('slidervert_bg','skin', 228, 40, 16, 16, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('sliderknobhori_n','skin',276,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('sliderknobhori_p','skin',292,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('sliderknobhori_h','skin',308,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('sliderknobvert_n','skin',324,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('sliderknobvert_p','skin',340,40,16,16,1.0,1.0,1.0,1.0)
    skin.addbrush('sliderknobvert_h','skin',356,40,16,16,1.0,1.0,1.0,1.0)

    # standard buttons
    skin.addbrush('button_n','skin',192,152,96,20,1.0,1.0,1.0,1.0)
    skin.addbrush('button_h','skin',288,152,96,20,1.0,1.0,1.0,1.0)
    skin.addbrush('button_p','skin',384,152,96,20,1.0,1.0,1.0,1.0)

    skin.addbrush('menu_n','skin',192,172,96,16,1.0,1.0,1.0,1.0)
    skin.addbrush('menu_h','skin',288,172,96,16,1.0,1.0,1.0,1.0)
    skin.addbrush('menu_p','skin',384,172,96,16,1.0,1.0,1.0,1.0)

    skin.addbrush('button_64x16_n', 'skin',  0,  0, 64, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x16_h', 'skin', 64,  0, 64, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x16_p', 'skin', 128, 0, 64, 16, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('button_96x16_n', 'skin', 192, 0, 96, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_96x16_h', 'skin', 288, 0, 96, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_96x16_p', 'skin', 384, 0, 96, 16, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('button_64x24_n', 'skin',   0, 16, 64, 24, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x24_h', 'skin',  64, 16, 64, 24, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x24_p', 'skin', 128, 16, 64, 24, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('button_96x24_n', 'skin', 192, 16, 96, 24, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_96x24_h', 'skin', 288, 16, 96, 24, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_96x24_p', 'skin', 384, 16, 96, 24, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('button_64x20_n', 'skin', 192, 152, 96, 20, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x20_h', 'skin', 288, 152, 96, 20, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('button_64x20_p', 'skin', 384, 152, 96, 20, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('menu_64x16_n', 'skin',   0, 172, 64, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('menu_64x16_h', 'skin',  64, 172, 64, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('menu_64x16_p', 'skin', 128, 172, 64, 16, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('menu_128x16_n', 'skin', 192, 172, 96, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('menu_128x16_h', 'skin', 288, 172, 96, 16, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('menu_128x16_p', 'skin', 384, 172, 96, 16, 1.0, 1.0, 1.0, 1.0)

    # list views
    skin.addbrush('list_background', 'skin', 446,  72,  8,  8, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('list_selection' , 'skin',  64, 172, 64, 16, 1.0, 1.0, 1.0, 1.0)

    # icons
    skin.addbrush('console_n', 'skin',     0,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('console_p', 'skin',     0,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('console_h', 'skin',     0, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('texbrowser_n', 'skin',  48,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('texbrowser_p', 'skin',  48,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('texbrowser_h', 'skin',  48, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('gfxbrowser_n', 'skin',  96,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('gfxbrowser_p', 'skin',  96,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('gfxbrowser_h', 'skin',  96, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('dbgwindow_n', 'skin',  144,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('dbgwindow_p', 'skin',  144,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('dbgwindow_h', 'skin',  144, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('syswindow_n', 'skin',  192,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('syswindow_p', 'skin',  192,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('syswindow_h', 'skin',  192, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('contrwindow_n', 'skin',  336,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('contrwindow_p', 'skin',  336,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('contrwindow_h', 'skin',  336, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('hidegui_n', 'skin',    240,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('hidegui_p', 'skin',    240,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('hidegui_h', 'skin',    240, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('quit_n', 'skin',       288,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('quit_p', 'skin',       288,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('quit_h', 'skin',       288, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('hardpoint_n', 'skin',    0, 188, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('hardpoint_p', 'skin',    0, 188, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('hardpoint_h', 'skin',   48, 188, 48, 48, 1.0, 1.0, 1.0, 1.0)

    skin.addbrush('disp_n', 'skin',       384,  56, 48, 48, 1.0, 1.0, 1.0, 1.0)
    skin.addbrush('disp_p', 'skin',       384,  56, 48, 48, 0.5, 0.5, 0.5, 1.0)
    skin.addbrush('disp_h', 'skin',       384, 104, 48, 48, 1.0, 1.0, 1.0, 1.0)

    # the left and right logos
    skin.addbrush('n2logo', 'n2logo', 0, 0, 64, 64, 1.0, 1.0, 1.0, 0.5)

    # the color hexagon for the colorpicker
    skin.addbrush('colorhex', 'colorhexagon', 0, 0, 170, 141, 1.0, 1.0, 1.0, 1.0)

    skin.endbrushes()

    guiServer.setsystemskin(skin)
    guiServer.setskin(skin)

    # create the Nebula dock window
    guiServer.newwindow('nguidockwindow', True)

    nebula.sel(oldCwd)

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is closed.
#-------------------------------------------------------------------------------
def OnGuiServerClose():
    pass

