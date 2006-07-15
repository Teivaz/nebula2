--------------------------------------------------------------------------------
-- Standard input definitions
--------------------------------------------------------------------------------

i = lookup('/sys/servers/input')
i:beginmap()
    i:map('keyb0:space.down',       'reset')
    i:map('keyb0:esc.down',         'script:nebula.sys.servers.gui:togglesystemgui()')
    i:map('keyb0:f11.down',         'console')

-- chase camera controls
    i:map('relmouse0:btn0.pressed', 'look')
    i:map('relmouse0:btn1.pressed', 'zoom')
    i:map('relmouse0:btn2.pressed', 'pan')
    i:map('relmouse0:-x',           'left')
    i:map('relmouse0:+x',           'right')
    i:map('relmouse0:-y',           'up')
    i:map('relmouse0:+y',           'down')
    i:map('relmouse0:+zbtn.down',   'zoomIn')
    i:map('relmouse0:-zbtn.down',   'zoomOut')

-- play camera controls
    i:map('keyb0:left.pressed',        'turnleft')
    i:map('keyb0:right.pressed',       'turnright')
    i:map('keyb0:ctrl.pressed',        'strafemode')
    i:map('keyb0:up.pressed',          'moveforward')
    i:map('keyb0:down.pressed',         'movebackward')

-- manipulate state; which camera is active, etc.
    i:map('keyb0:1.down',           'activatecam1')
    i:map('keyb0:2.down',           'activatecam2')
    i:map('keyb0:f1.down',          'changeclipstyle')
    i:map('keyb0:f2.down',          'flycontrols')
    i:map('keyb0:f3.down',          'mayacontrols')
    i:map('keyb0:f4.down',          'cycleviscamera')
    i:map('keyb0:f5.down',          'cycleviewcamera')
    i:map('keyb0:f6.down',          'toggledebugviz')
    i:map('keyb0:f7.down',          'otherview')
    i:map('keyb0:f12.down',         'screenshot')
i:endmap()
