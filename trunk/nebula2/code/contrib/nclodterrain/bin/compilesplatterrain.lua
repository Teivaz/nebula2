--
-- $parser:nluaserver$ $class:ntransformnode$
--
-- lua script to chunk up heightfield data and compile collision chunks
--
--(C) Copyright 2004 Gary Haussmann
--this code is released under the Nebula License
--

tree_depth = 3
tile_tree_depth = 3
space_error = 0.25

-- this value needs to be (size of tile map / (1<<(tile_tree_depth-1))) * 2
tqt_tilesize = 256

--------------------------------------------------------------------------------
--
-- Convert the heightfield bitmap into a quadtree of meshes described by triangle strips
--
--------------------------------------------------------------------------------
--
-- build a chunker node and configure it
--

new('nclodchunker','/usr/chunker')
chunker = nebula.usr.chunker
chunker:setchunkfilename('nclodterrains:examples/sample.clod')
chunker:settileindexfilename('nclodtextures:examples/tiles.bmp')
chunker:setspacings(1.0,1.0,0.25)
chunker:setchunkparameters(tree_depth, space_error)
chunker:setvalidateflag(true)

-- do the chunking...

chunker:compilechunksfromfile('nclodtextures:examples/heightfield.bmp')

--------------------------------------------------------------------------------
--
-- Based on the mesh chunk data we just compiled, build up collision data for
-- each chunk
--
--------------------------------------------------------------------------------
--
-- build a vertex data node to read in the compiled vertex data
--

--new('nclodvertextree','/usr/vertexdata')
--terrdata = nebula.usr.vertexdata
--terrdata:setvertexfilename('output.chu')

-- now build a compiler node and configure it

--new('nclodcompilernode','/usr/chunkcompiler')
--chunkcompiler = nebula.usr.chunkcompiler
--chunkcompiler:setsourcepath('/usr/vertexdata')
--chunkcompiler:setvalidateflag(true)

-- next line is not needed unless we use a collide server in a non-standard place
--chunkcompiler:setcollideserver('/sys/servers/collide')

-- do the compile...

--chunkcompiler:compiletrees('output.cch')


--
-- Build tqt data file as well

max_tiles = 16

new('nclodtqtsplatter','/usr/tqtcompiler')
tqtcompiler = nebula.usr.tqtcompiler
tqtcompiler:settqtparameters(tile_tree_depth, tqt_tilesize, max_tiles)
tqtcompiler:settqtfilename('nclodterrains:examples/sample.tqt')
tqtcompiler:setdetailtexture(0,'nclodtextures:terrainsplat/dirt1.bmp')
tqtcompiler:setdetailtexture(1,'nclodtextures:terrainsplat/r_rock1.bmp')
tqtcompiler:setdetailtexture(2,'nclodtextures:terrainsplat/grass7.bmp')
tqtcompiler:setdetailtexture(3,'nclodtextures:terrainsplat/grass6.bmp')
tqtcompiler:setdetailtexture(4,'nclodtextures:terrainsplat/grass1.bmp')
tqtcompiler:setdetailtexture(5,'nclodtextures:terrainsplat/mossy3.bmp')
tqtcompiler:setdetailtexture(6,'nclodtextures:terrainsplat/!water1.bmp')
tqtcompiler:setdetailtexture(7,'nclodtextures:terrainsplat/snow4.bmp')
tqtcompiler:setdetailtexture(8,'nclodtextures:terrainsplat/snow5.bmp')
tqtcompiler:setdetailtexture(9,'nclodtextures:terrainsplat/r_rock1.bmp')
tqtcompiler:setdetailtexture(10,'nclodtextures:terrainsplat/r_rock2.bmp')
tqtcompiler:setdetailtexture(11,'nclodtextures:terrainsplat/r_rock6.bmp')
tqtcompiler:setdetailtexture(12,'nclodtextures:terrainsplat/sand1.bmp')
tqtcompiler:setdetailtexture(13,'nclodtextures:terrainsplat/sand2.bmp')
tqtcompiler:setdetailtexture(14,'nclodtextures:terrainsplat/grass6.bmp')
tqtcompiler:setdetailtexture(15,'nclodtextures:terrainsplat/grass7.bmp')
tqtcompiler:compiletqtfromfile('nclodtextures:examples/tiles.bmp')

--and exit

exit()
