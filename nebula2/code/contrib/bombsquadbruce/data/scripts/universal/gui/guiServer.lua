runscript( 'universal/gui/itemManagerT.lua' )
runscript( 'universal/gui/ItemT.lua' )
runscript( 'universal/gui/menuT.lua' )
runscript( 'universal/gui/buttonT.lua' )

--protected

guiServer = { 
    rootWindow = { name = 'SystemRootWindow' }, -- fixed by the C++   
    modes = {}, -- where the various game-specific guis will be stored
    defaultFont = 'ChipmunkDefault',
    monospaceFont = 'MonospaceDefault',
    fontColour = { r=1, g=1, b=1, a=1 },
    disabledFontColour = { r = 0.6, g = 0.6, b = 0.6, a = 1 },
    backStack = {} --a stack of windows that we've opened and want to be able to return to
}

--public

--The game should call this when it is Run() 
--N.B. It doesn't work if you put it in OnGuiServerOpen()
function guiServer:Create()
    self:InitRootWindow()
    self:CreateGameSpecific()
    for modeName, mode in guiServer.modes do
        mode:Create()
    end
end

--newModeName is the entry in guiServer.modes that we want to activate
function guiServer:Activate( newModeName, noBackStack )
    if newModeName ~= guiServer.curWindow then
        if noBackStack ~= true then
            table.insert( guiServer.backStack, guiServer.curWindow )
        end

        if guiServer.curWindow ~= nil then
            guiServer.modes[guiServer.curWindow]:Deactivate(newModeName)
        end
  
        if newModeName == nil then
            nebula.sys.servers.gfx:setcursorvisibility('none')
            guiServer.backStack = {}
        else
            for modeName, mode in guiServer.modes do
                if modeName == newModeName then
                    if guiServer.cursor == nil then
                        nebula.sys.servers.gfx:setcursorvisibility('system')
                    else
                        nebula.sys.servers.gfx:setcursorvisibility('custom')
                    end
                    mode:Activate(guiServer.curWindow)
                    break
                end
            end    
        end
        guiServer.curWindow = newModeName
    end
end

function guiServer:Deactivate()
    self:Activate()
end

function guiServer:GoBack()
    guiServer:Activate( table.remove( guiServer.backStack ), true ) 
end

--private
-------------------------------------------------------------------------------
--  OnGuiServerOpen()   
-- This procedure is called automatically by the C++
-- Rename it not!
-------------------------------------------------------------------------------

function OnGuiServerOpen()   
    guiServer.object = nebula.sys.servers.gui
    guiServer.rootPath = guiServer.object:getrootpath()   
    local cwd = psel()
    sel( guiServer.rootPath )
        guiServer.object:setdisplaysize( win.xres, win.yres )
        guiServer:SetSkin()
        guiServer:AddFonts()
        guiServer:DefineCursor()
        if guiServer.cursor ~= nil then
            nebula.sys.servers.gfx:setmousecursor( guiServer.cursor.texture, guiServer.cursor.hotspot[1], guiServer.cursor.hotspot[2] )
        end
    sel(cwd)
end

function OnGuiServerClose()
    --empty, but must be present because 
    -- the C++ calls it automatically -- don't rename!
end

function guiServer:SetSkin()
    -- define the system skin
    skin = self.object:newskin("system")
            
        -- set texture path pre- and postfix
        skin:settextureprefix("gui:")
        skin:settexturepostfix(".dds")
        
        -- active window modulation color
        skin:setactivewindowcolor(1.0,1.0,1.0,1.0)
    
        -- define brushes  
        skin:beginbrushes()
        guiServer:AddBrushes()
        skin:endbrushes()
        
        --define sounds
        if guiServer.AddSounds then
            guiServer:AddSounds()
        end

    self.object:setsystemskin(skin)
    self.object:setskin(skin)
end

function guiServer:InitRootWindow()
    self.rootWindow.path = self.rootPath..'/'..self.rootWindow.name
    self.rootWindow.object = lookup( self.rootWindow.path )
    self.object:setrootwindow( self.rootWindow.name )
    self.rootWindow.object:show()
end

function guiServer:AddBrushes()        
    -- window title bar, window background, tooltip background
    --skin:addbrush('titlebar','skin',0,52,43,20,1.0,1.0,1.0,1.0)
    --skin:addbrush('window','skin',0,77,15,13,1.0,1.0,1.0,1.0)
    guiServer.AddSharedBrushes()
    for modeName, mode in guiServer.modes do
        if mode.AddBrushes ~= nil then
            mode.AddBrushes()
        end
    end
end

function guiServer:AddFonts()
    self.bigFontSize = 24 * win.xres / 640    
    self.fontSize = 20 * win.xres / 640
    guiServer.object:addcustomfont( 'ChipmunkDefault', 'data:fonts/jesus.ttf', 'little tubby jesus', self.bigFontSize, false, false, false )
    guiServer.object:addsystemfont( 'MonospaceDefault', 'courier new', self.bigFontSize, true, false, false )
    guiServer:AddSharedFonts()
end

--helper function for widgets
InitFont = function( font, name )
    if font == nil then 
        font = {}
    end
    if font.name == nil then
        if name == nil then
            name = 'defaultFont'
        end
        font.name = guiServer[name]
    end
    if font.colour == nil then
        font.colour = Instantiate( guiServer.fontColour )
    end
    if font.disabledColour == nil then
        font.disabledColour = Instantiate( guiServer.disabledFontColour )
    end
    return font
end
    
function guiServer:CalcBrushSize( brushName )
    local size = {self.object:computescreenspacebrushsize(brushName)}
    size[1] = size[1] * win.xres / 1280
    size[2] = size[2] * win.yres / 1024
    return size
end
