-- This file include various functions to make it easier to build
-- some standard visibility test configurations with script

--
-- function to build an occluding block, including a visible mesh and
-- an occluder.
--

function buildblock(blockx, blocky, blockz, blocksize)
    r = nebula.world.rootsector
    blocksizeh = blocksize * 0.5
    r:addoccludingobject('fenceoccblock', 
        blockx-blocksizeh, blocky-blocksizeh, blockz-blocksizeh,
        blockx+blocksizeh, blocky+blocksizeh, blockz+blocksizeh)
    r:addvisibleobject('fenceblock', '/usr/occluderobject', blockx, blocky, blockz, blocksizeh)
end


--
-- simple script to build a "fence" of occluder boxes around a point. 
--
-- pass in:
--  - the x,y,z center of the fence
--  - the number of blocks to use in the x & z directions to build the fence
--  - whether the fence has a bottom floor and/or top ceiling
--
-- A fence of size M by N is a ring of blocks that surrounds a rectangular
-- region of size (M-2) by (N-2), with 1-block gaps in the center of the
-- four walls.  The fence is built in the x-z plane.
-- For instance, a 7 by 5 fence would look like this from above: (on the y axis)
--
--   ###.###
--   #.....#
--   .......
--   #.....#
--   ###.###
--
-- where '#' marks the positions of the block occluders
-- the blocks are always of size 3x3, except the top and bottom which are
-- sized to cover the whole area if possible

function buildfence(xcenter, ycenter, zcenter, xblocks, zblocks, blocksize, hastop, hasbottom)
    r = nebula.world.rootsector

    blocksizeh = blocksize/2.0
    northz = zcenter - zblocks*0.5*blocksize
    southz = zcenter + zblocks*0.5*blocksize
    westx = xcenter - xblocks*0.5*blocksize
    eastx = xcenter + xblocks*0.5*blocksize
    upy = ycenter - blocksizeh
    downy = ycenter + blocksizeh
    xgapblock = math.floor(xblocks/2)
    zgapblock = math.floor(zblocks/2)

    -- build the north/south sets of blocks
    for block=1,xblocks-2 do
        if (block ~= xgapblock) then
            blockx = westx + (block+0.5) * blocksize
            blocky = ycenter
            r:addvisibleobject('fenceblock', '/usr/occluderobject', blockx, blocky, northz+blocksizeh, blocksizeh)
            r:addvisibleobject('fenceblock', '/usr/occluderobject', blockx, blocky, southz-blocksizeh, blocksizeh)
        end
    end

    -- build the east/west sets of blocks
    for block=0,zblocks-1 do
        if (block ~= zgapblock) then
            blocky = ycenter
            blockz = northz + (block+0.5) * blocksize
            r:addvisibleobject('fenceblock', '/usr/occluderobject', westx+blocksizeh, blocky, blockz, blocksizeh)
            r:addvisibleobject('fenceblock', '/usr/occluderobject', eastx-blocksizeh, blocky, blockz, blocksizeh)
        end
    end

    -- add in the occluders, 8 of them
    r:addoccludingobject('fenceoccblock', westx, upy, northz, westx + (xgapblock) * blocksize, downy, northz + blocksize)
    r:addoccludingobject('fenceoccblock', westx, upy, southz - blocksize, westx + (xgapblock) * blocksize, downy, southz)
    r:addoccludingobject('fenceoccblock', westx + (xgapblock+1) * blocksize, upy, northz, eastx, downy, northz + blocksize)
    r:addoccludingobject('fenceoccblock', westx + (xgapblock+1) * blocksize, upy, southz - blocksize, eastx, downy, southz)

    r:addoccludingobject('fenceoccblock', westx, upy, northz, westx + blocksize, downy, northz + (zgapblock) * blocksize)
    r:addoccludingobject('fenceoccblock', westx, upy, northz + (zgapblock+1) * blocksize, westx + blocksize, downy, southz)
    r:addoccludingobject('fenceoccblock', eastx-blocksize, upy, northz, eastx, downy, northz + (zgapblock) * blocksize)
    r:addoccludingobject('fenceoccblock', eastx-blocksize, upy, northz + (zgapblock+1) * blocksize, eastx, downy, southz)

    -- add in top/bottom if needed
    if (xblocks > zblocks) then
        maxdim = xblocks * blocksize
    else
        maxdim = zblocks * blocksize
    end
    maxdimh = maxdim / 2.0
    if (hastop) then
        topycenter = ycenter - blocksizeh - maxdimh
        r:addvisibleobject('ceilingblock','/usr/occluderobject',xcenter,topycenter,zcenter, maxdimh)
        r:addoccludingobject('ceilingoccblock', 
            xcenter - maxdimh, topycenter - maxdimh, zcenter - maxdimh,
            xcenter + maxdimh, topycenter + maxdimh, zcenter + maxdimh)
    end
    if (hasbottom) then
        botycenter = ycenter + blocksizeh + maxdimh
        r:addvisibleobject('floorblock','/usr/occluderobject',xcenter,botycenter,zcenter, maxdimh)
        r:addoccludingobject('flooroccblock', 
            xcenter - maxdimh, botycenter - maxdimh, zcenter - maxdimh,
            xcenter + maxdimh, botycenter + maxdimh, zcenter + maxdimh)
    end


end

--
-- script to build a grid of visible objects.  strew these around for dramatic visibility demos
--

--
-- basically you specify a bounding box that wil be filled with objects, and the spacing and size of the objects
--
function buildgridofstuff(xlow,ylow,zlow, xhigh,yhigh,zhigh, spacing, objectsize)
    r = nebula.world.rootsector

    xobjects = math.floor((xhigh-xlow)/spacing)
    yobjects = math.floor((yhigh-ylow)/spacing)
    zobjects = math.floor((zhigh-zlow)/spacing)

    if (xobjects < 1) then xobjects=1 end
    if (yobjects < 1) then yobjects=1 end
    if (zobjects < 1) then zobjects=1 end

    for xobject=1,xobjects do
      for yobject=1,yobjects do
        for zobject=1,zobjects do
            xc = xlow + (xobject-1) * spacing
            yc = ylow + (yobject-1) * spacing
            zc = zlow + (zobject-1) * spacing
            r:addvisibleobject('showthing', '/usr/testobject', xc,yc,zc, objectsize)
        end
      end
    end 
    
end


--
--
--
function cleanrootsector()
    r = nebula.world.rootsector
    r:clearobjects()
end

--
-- construct test configuration one, with a large amount of objects spread out
--
function testconfig1()
    cleanrootsector()

    -- should be 25*25*25 = 15,625 objects, but they are spaced pretty far apart,
    -- so not very many are visible at once
    buildgridofstuff(-120,-120,-120, 120,120,120, 10, 1)
end

--
-- construct test configuration two, which is like config one but packed into a smaller space
-- so that more are visible at one time
--
function testconfig2()
    cleanrootsector()

    -- should be 16*16*16 = 4096 object, spaced closely together, so a lot of them
    -- will be displayed at once
    buildgridofstuff(-40,-40,-40, 40,40,40, 5, 0.8)
end

--
-- test configuration 3, with several occluders, a few of which are visible at once
--
function testconfig3()
    cleanrootsector()

    buildfence(0,0,0, 9,9, 3, true, false)
    buildfence(50,0,0, 9,9, 3, true, false)
    buildfence(0,50,0, 9,9, 3, true, false)
    buildfence(0,0,50, 9,9, 3, true, false)
    buildfence(-50,0,0, 9,9, 3, true, false)
    buildfence(0,-50,0, 9,9, 3, true, false)
    buildfence(0,0,-50, 9,9, 3, true, false)

    buildfence(100,0,0, 9,9, 3, true, false)
    buildfence(0,100,0, 9,9, 3, true, false)
    buildfence(0,0,100, 9,9, 3, true, false)
    buildfence(-100,0,0, 9,9, 3, true, false)
    buildfence(0,-100,0, 9,9, 3, true, false)
    buildfence(0,0,-100, 9,9, 3, true, false)
end

--
-- test configuration 4, with several occluders, several of which are visible at once
-- and a buttload of objects
--
function testconfig4()
    cleanrootsector()

    buildfence(0,0,0, 9,9, 3, true, false)
    buildfence(27,0,0, 9,9, 3, true, false)
    buildfence(0,0,27, 9,9, 3, true, false)
    buildfence(-27,0,0, 9,9, 3, true, false)
    buildfence(0,0,-27, 9,9, 3, true, false)
    buildfence(27,0, 27, 9,9, 3, true, false)
    buildfence(27,0, -27, 9,9, 3, true, false)
    buildfence(-27,0, 27, 9,9, 3, true, false)
    buildfence(27,0, -27, 9,9, 3, true, false)

    buildgridofstuff(-54,0,-54,54,0,54, 7, 1)
    buildgridofstuff(-52,0,-52,52,0,52, 11, 1)

end

