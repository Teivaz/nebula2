--------------------------------------------------------------------------------
--   bin/startup.lua
--
--   This is the central Nebula runtime startup script which is 
--   used by various tools, like the Maya plugin or the viewer.
--
--   The script mainly sets up assigns and Nebula variables.
--
--   (C) 2003 RadonLabs GmbH
---------------------------------------------------------------------------------

pushcwd('.')

sel('/sys/servers/file2')
call('setassign', 'luascript', 'home:code/contrib/nluaserver/bin/')

dofile (mangle('luascript:nebthunker.lua'))
dofile (mangle('luascript:console.lua'))


---------------------------------------------------------------------------------
--   set up resource assigns
---------------------------------------------------------------------------------

f = nebula.sys.servers.file2
proj = f:manglepath('proj:')
home = f:manglepath('home:')

f:setassign('meshes', proj .. '/export/meshes/')
f:setassign('textures', proj .. '/export/textures/')
if (exists('/sys/servers/gfx')) then
    featureSet = nebula.sys.servers.gfx:getfeatureset()
    if ((featureSet == 'dx9') or (featureSet == 'dx9flt')) then
        f:setassign('shaders', home .. '/export/shaders/2.0/')
        puts('Shader directory: ' .. home .. '/export/shaders/2.0')
    else
        f:setassign('shaders', home .. '/export/shaders/fixed/')
        puts('Shader directory: ' .. home .. '/export/shaders/fixed')
    end
else
    f:setassign('shaders', home .. '/export/shaders/2.0/')
    puts('Shader directory: ' .. home .. '/export/shaders/2.0')
end
f:setassign('anims', proj .. '/export/anims/')
f:setassign('gfxlib', proj .. '/export/gfxlib/')

-- nclod directories
f:setassign('nclodscripts',  proj .. '/code/contrib/nclodterrain/bin/')
f:setassign('nclodtextures', proj .. '/code/contrib/nclodterrain/export/textures/')
f:setassign('nclodterrains', proj .. '/code/contrib/nclodterrain/export/terrains/')
f:setassign('nclodshaders', proj .. '/code/contrib/nclodterrain/export/shaders/')

--------------------------------------------------------------------------------
--   restore original directory
--------------------------------------------------------------------------------

popcwd()
   

-- setup gui
-------------------------------------------------------------------------------
--   This procedure is called when the gui server is opened
-------------------------------------------------------------------------------
function OnGuiServerOpen()

    local cwd, guiroot

    cwd = psel()

    -- initialize the default tooltip
    guiroot = nebula.sys.servers.gui:getrootpath()
    sel(guiroot)

    new('nguitooltip', 'Tooltip')
    tt = lookup('Tooltip')
    tt:setdefaultbrush("tooltip")
    tt:setfont("GuiSmall")
    tt:setcolor(0,0,0,1)
    tt:setalignment("left")
    tt:setborder(0.005,0.005)
        
    -- define the system skin
    skin = nebula.sys.servers.gui:newskin("system")
    --skin = lookup(skinpath) 
    --skin = nebula.sys.share.rsrc.skins.system
            
        -- set texture path pre- and postfix
        skin:settextureprefix("home:export/textures/system/")
        skin:settexturepostfix(".dds")
        
        -- active and inactive window modulation color
        skin:setactivewindowcolor(1.0,1.0,1.0,0.9)
        skin:setinactivewindowcolor(0.6,0.6,0.6,0.6)
    
        -- define brushes  
        skin:beginbrushes()

        -- window title bar, window background, tooltip background
        skin:addbrush('titlebar','skin',0,52,43,20,1.0,1.0,1.0,1.0)
        skin:addbrush('window','skin',0,77,15,13,1.0,1.0,1.0,1.0)
        skin:addbrush('tooltip','skin',0,77,15,13,1.0,1.0,1.0,1.0)
        skin:addbrush('pink','skin',0,97,14,14,1.0,1.0,1.0,1.0)

        -- text entry field
        skin:addbrush('textentry_n','skin',0,52,43,20,0.7,0.7,0.7,1.0)
        skin:addbrush('textentry_p','skin',0,52,43,20,0.7,0.7,1.0,1.0)
        skin:addbrush('textentry_h','skin',0,52,43,20,0.9,0.9,1.0,1.0)
        skin:addbrush('textcursor','skin',0,97,14,14,1.0,1.0,1.0,1.0)

        -- the window close button
        skin:addbrush('close_n','skin',0,29,16,17,1.0,1.0,1.0,1.0)
        skin:addbrush('close_h','skin',22,29,16,17,1.3,1.3,1.3,1.0)
        skin:addbrush('close_p','skin',43,28,16,17,1.0,1.0,1.0,1.0)

        -- the window size button
        skin:addbrush('size_n','skin',352,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('size_h','skin',352,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('size_p','skin',352,136,16,16,1.2,1.2,1.2,1.0)

        -- arrows
        skin:addbrush('arrowleft_n','skin',0,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowleft_h','skin',16,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('arrowleft_p','skin',32,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowright_n','skin',64,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowright_h','skin',80,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('arrowright_p','skin',96,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowup_n','skin',128,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowup_h','skin',144,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('arrowup_p','skin',160,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowdown_n','skin',192,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('arrowdown_h','skin',208,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('arrowdown_p','skin',224,136,16,16,1.0,1.0,1.0,1.0)

        -- sliders
        skin:addbrush('sliderbg','skin',0,77,15,13,0.8,0.8,0.8,1.0)
        skin:addbrush('sliderknobhori_n','skin',256,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('sliderknobhori_p','skin',272,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('sliderknobhori_h','skin',288,136,16,16,1.3,1.3,1.3,1.0)
        skin:addbrush('sliderknobvert_n','skin',304,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('sliderknobvert_p','skin',320,136,16,16,1.0,1.0,1.0,1.0)
        skin:addbrush('sliderknobvert_h','skin',336,136,16,16,1.3,1.3,1.3,1.0)

        -- standard buttons
        skin:addbrush('button_n','skin',226,0,73,24,1.0,1.0,1.0,1.0)
        skin:addbrush('button_p','skin',76,0,73,24,1.0,1.0,1.0,1.0)
        skin:addbrush('button_h','skin',1,0,73,24,1.3,1.3,1.3,1.0)

        -- icons
        skin:addbrush('terminal_n','skin',49,52,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('terminal_p','skin',48,51,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('terminal_h','skin',49,52,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('quit_n','skin',122,52,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('quit_p','skin',121,51,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('quit_h','skin',122,52,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('computer_n','skin',198,52,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('computer_p','skin',197,51,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('computer_h','skin',198,52,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('info_n','skin',277,52,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('info_p','skin',276,51,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('info_h','skin',277,52,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('settings_n','skin',49,91,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('settings_p','skin',48,90,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('settings_h','skin',49,91,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('desktop_n','skin',122,91,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('desktop_p','skin',121,90,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('desktop_h','skin',122,91,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('eject_n','skin',198,91,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('eject_p','skin',197,90,32,32,1.0,1.0,1.0,1.0)
        skin:addbrush('eject_h','skin',198,91,32,32,1.3,1.3,1.3,1.0)

        skin:addbrush('n2logo','n2logo',0,0,64,64,1.0,1.0,1.0,0.5)

        skin:endbrushes()

    nebula.sys.servers.gui:setsystemskin(skin)
    nebula.sys.servers.gui:setskin(skin)
    nebula.sys.servers.gui:newwindow('nguidockwindow', true)

    sel(cwd)
end

--------------------------------------------------------------------------------
--   This proceducre is called when the gui server is closed
--------------------------------------------------------------------------------
function OnGuiServerClose()
    -- Empty
    local a
    a = 0
end

