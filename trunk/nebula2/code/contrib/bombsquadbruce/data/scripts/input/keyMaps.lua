keyMaps = {
    move = { 
        left = {'keyb0:a', 'keyb0:left' },
        right = { 'keyb0:d', 'keyb0:right' },
        forwards = { 'keyb0:w', 'keyb0:up' },
        backwards = { 'keyb0:s', 'keyb0:down' },
    },
    
    quit = 'keyb0:esc', -- behaviour depends on context:
     --during the game, it goes to the main menu, pausing the game
     --in main menu, this closes the app if no game is in progress,
     --or reenters the game otherwise
     
    screenshot = 'keyb0:snap',
}

if debugControlsActive then 
    keyMaps.admin = { 
        toggleConsole = 'keyb0:f8',
        showStats = 'keyb0:f2'
    }
    keyMaps.camera = {
        activateFreeCam = 'keyb0:num1',
        activatePlayerCam = 'keyb0:num2',
        activateObstacleCam = 'keyb0:num3',
        toggleControls = 'keyb0:decimal',
        drag = 'mouse0:btn0',
        rotate = 'mouse0:btn1',
        overhead = 'keyb0:num8',
        reset = 'keyb0:num5'
    }
end
