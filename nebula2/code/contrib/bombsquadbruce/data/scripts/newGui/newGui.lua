--newGui.lua
runscript('newGui/OFMMenuT.lua')
runscript('newGui/mainMenu.lua')
--"inherited" functions, called automatically by universal guiServer
function guiServer.AddSharedBrushes()
    skin:addbrush('inactiveItemTopLeft','menu/components'   ,0, 0, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemTopMid','menu/components'        ,32,0, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemTopRight','menu/components',64,0, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemLeft','menu/components'        ,0, 32,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemMid','menu/components'         ,32,32,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemRight','menu/components',      64,32,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemLowLeft','menu/components',   0, 64,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemLowMid','menu/components',        32,64,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('inactiveItemLowRight','menu/components', 64,64,32,32,1.0,1.0,1.0,1.0)
    
    skin:addbrush('invalidItemTopLeft','menu/components',     160,0,  32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemTopMid','menu/components',          192,0,  32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemTopRight','menu/components',   224,0,  32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemLeft','menu/components',          160,32, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemMid','menu/components',          192,32, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemRight','menu/components',        224,32, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemLowLeft','menu/components',    160,64, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemLowMid','menu/components',          192,64, 32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('invalidItemLowRight','menu/components',   224,64, 32,32,1.0,1.0,1.0,1.0)

    skin:addbrush('activeItemTopLeft','menu/components',     160,160,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemTopMid','menu/components',     192,160,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemTopRight','menu/components',   224,160,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemLeft','menu/components',          160,192,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemMid','menu/components',          192,192,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemRight','menu/components',        224,192,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemLowLeft','menu/components',    160,224,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemLowMid','menu/components',     192,224,32,32,1.0,1.0,1.0,1.0)
    skin:addbrush('activeItemLowRight','menu/components',   224,224,32,32,1.0,1.0,1.0,1.0)

    --skin:addbrush('knobright', 'menu/components',                  191, 95, 65, 65, 1.0,1.0,1.0,1.0)
    skin:addbrush('knobLeft', 'menu/components',                  126, 95, 66, 66, 1.0,1.0,1.0,1.0) --?!
    skin:addbrush('barKnobRight', 'menu/components',            95, 96, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barKnobBottom', 'menu/components',         96, 64, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barVertical', 'menu/components',                96, 32, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barHorizontal', 'menu/components',            96, 0, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barTTop', 'menu/components',                   128, 0, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barTLeft', 'menu/components',                   128, 32, 32, 32, 1.0,1.0,1.0,1.0)
    skin:addbrush('barTRight', 'menu/components',                128, 64, 32, 32, 1.0,1.0,1.0,1.0)
   
    skin:addbrush( 'msgTopLeft', 'GUI-Stats_Window',           0, 64, 21, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgTopMid', 'GUI-Stats_Window',           20, 64, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgTopRight', 'GUI-Stats_Window',         40, 64, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgLeft', 'GUI-Stats_Window',                 0, 104, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgMid', 'GUI-Stats_Window',                10, 104, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgRight', 'GUI-Stats_Window',              20, 104, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgLowLeft', 'GUI-Stats_Window',           0, 84, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgLowMid', 'GUI-Stats_Window',           20, 84, 20, 20, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'msgLowRight', 'GUI-Stats_Window',           40, 84, 20, 20, 1.0, 1.0, 1.0, 1.0 )
end

function guiServer:AddSounds()
    skin:setsound( 'ButtonClick', 'sounds:selectTool.wav' )
end

function guiServer.DefineCursor()
    guiServer.cursor = { texture = 'gui:mousecursor.dds', hotspot = {0,0} }
end

function guiServer:CreateGameSpecific()
    self.fontColour = { r = 1, g = 1, b = 0, a = 1 }
    self.disabledFontColour = { r = 0.6, g = 0.6, b = 0, a = 1 }
    
end

function guiServer:AddSharedFonts()
    self.object:addcustomfont( 'Huge', 'data:fonts/jesus.ttf', 'little tubby jesus', 48 * win.xres / 640, false, false, false )
end

function guiServer:Up()
    if guiServer.curWindow ~= nil and guiServer.modes[guiServer.curWindow].Up ~= nil then
        guiServer.modes[guiServer.curWindow]:Up()
    else
        guiServer:Activate( "mainMenu" )
    end
end
