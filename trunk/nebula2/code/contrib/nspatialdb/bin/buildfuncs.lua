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

function buildfence(xcenter, ycenter, zcenter, xblocks, zblocks, hastop, hasbottom)
    blocksize = 3.0
    northz = zcenter - zblocks*0.5*blocksize
    southz = zcenter + zblocks*0.5*blocksize
    westx = xcenter - xblocks*0.5*blocksize
    eastx = xcenter + xblocks*0.5*blocksize

    -- build the north/south sets of blocks
    gapblock = math.floor(xblocks/2)
    for block=1,xblocks-1 do
        if (block ~= gapblock) then
            buildblock(westx + block * blocksize, ycenter, northz, blocksize)
            buildblock(westx + block * blocksize, ycenter, southz, blocksize)
        end
    end

    -- build the east/west sets of blocks
    gapblock = math.floor(zblocks/2)
    for block=0,zblocks do
        if (block ~= gapblock) then
            buildblock(westx, ycenter, northz + block*blocksize, blocksize)
            buildblock(eastx, ycenter, northz + block*blocksize, blocksize)
        end
    end

end
