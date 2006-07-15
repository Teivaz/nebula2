-- start.lua
-- startup script for lua server test

-- you MUST load nebthunker.lua before trying to do anything else
dofile(mangle('home:code/contrib/nluaserver/bin/nebthunker.lua'))
-- you would typically load console.lua since it provides some useful commands
dofile(mangle('home:code/contrib/nluaserver/bin/console.lua'))

-- setup an assign for the luatest dir
gFileServer = lookup('/sys/servers/file2')
gFileServer:setassign('luatest', 'home:code/contrib/nluaserver/data/luatest/')

function start()
    gfxClass = 'nd3d9server'

    inputClass = 'ninputserver'
    osName = os.getenv('OS')
    if (osName ~= nil) then
        osName = string.upper(osName)
        if (string.find(osName, 'WIN', 1, true)) then
            inputClass = 'ndi8server'
        end
    end
    puts('using input server: '..inputClass..'\n')


    -- create servers

    new('nresourceserver',     '/sys/servers/resource')
    
    gGfxServer = newthunk(gfxClass,   '/sys/servers/gfx')
    -- setup assigns
    local featureSet = gGfxServer:getfeatureset()
    if ((featureSet == "dx9") or (featureSet == "dx9flt")) then
        gFileServer:setassign('shaders', 'home:data/shaders/2.0/')
        puts('Shader directory: ../data/shaders/2.0')
    else
        gFileServer:setassign('shaders', 'home:data/shaders/fixed/')
        puts('Shader directory: ../data/shaders/fixed')
    end
    gGfxServer:setdisplaymode('nLuaTest', 'windowed', 10, 10, 640, 480, false)
    gGfxServer:opendisplay()
    
    new(inputClass,            '/sys/servers/input')
    new('nconserver',          '/sys/servers/console')
    new('nmrtsceneserver',     '/sys/servers/scene')
    new('nvariableserver',     '/sys/servers/variable')
    new('nanimationserver',    '/sys/servers/anim')

    -- open display

    --gGfxServer:setdisplaymode('nLuaTest', 'windowed', 10, 10, 640, 480, false)
    --gGfxServer:setviewvolume(-0.1, 0.1, -0.075, 0.075, 0.1, 5000.0)
    --gGfxServer:setclearcolor(0.5, 0.5, 0.5, 1.0)

    -- define the script cmds
    dofile(mangle('luatest:cmds.lua'))

    -- setup test tree
    new('nroot', '/tree')
    sel('/tree')
        new('nroot', 'node0')
        new('nroot', 'node1')
        sel('node1')
            new('nroot', 'node10')
            new('nroot', 'node11')
            new('nroot', 'node12')
        up()
        new('nroot', 'node2')

    -- implement script cmds (declared in cmds.lua earlier)
    node = nebula.tree
    pin(node)

    function node:SampleCmd(num)
        return num
    end

    function node:SampleCmd2(str, num)
        if (str == 'true') then
            return true, num
        else
            return false, num
        end
    end

    node = nebula.tree.node0
    pin(node)

    function node:SampleCmd(num)
        return num + 1
    end

    function node:SampleCmd2(str, num)
        if (str == 'true') then
            return true, num + 1
        else
            return false, num + 1
        end
    end

    -- finish populating the nebula thunks table (no cmds for these cauz I'm lazy)
    node = lookup('/tree/node1'); pin(node)
    node = lookup('/tree/node1/node10'); pin(node)
    node = lookup('/tree/node1/node11'); pin(node)
    node = lookup('/tree/node1/node12'); pin(node)
    node = lookup('/tree/node2'); pin(node)
    node = nil

    -- simple test of script cmds functionality
    -- (this is the lua version of what goes on the C++ side)
    function TestScriptCmds()
        if (1 == nebula.tree:SampleCmd(1)) then
            puts('Test 1: pass\n')
        else
            puts('Test 1: fail\n')
        end
        b, i = nebula.tree:SampleCmd2('true', 1)
        if ((b == true) and (i == 1)) then
            puts('Test 2: pass\n')
        else
            puts('Test 2: fail\n')
        end
        if (2 == nebula.tree.node0:SampleCmd(1)) then
            puts('Test3: pass\n')
        else
            puts('Test3: fail\n')
        end
        b, i = nebula.tree.node0:SampleCmd2('false', 1)
        if ((b == false) and (i == 2)) then
            puts('Test4: pass\n')
        else
            puts('Test4: fail\n')
        end
    end

    function NaughtyFunctionC()
        Ooh_I_am_so_naughty()
    end

    function GoodFunctionB()
        NaughtyFunctionC()
    end

    function GoodFunctionA()
        GoodFunctionB()
    end

    function GenerateStackTrace()
        GoodFunctionA()
    end

    -- map input

    dofile(mangle('luatest:input.lua'))

    -- setup test
    new('nluatest', '/test')
    --nebula.sys.servers.gfx:opendisplay()
end

function stop()
    gfx = lookup('/sys/servers/gfx')
    if (gfx ~= nil) then
        gfx:closedisplay()
    end

    delete('/test')
    delete('/sys/servers/anim')
    delete('/sys/servers/variable')
    delete('/sys/servers/scene')
    delete('/sys/servers/console')
    delete('/sys/servers/input')
    delete('/sys/servers/gfx')
    delete('/sys/servers/resource')
end
