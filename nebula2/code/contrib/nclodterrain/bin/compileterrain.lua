--
-- lua script to chunk up heightfield data and compile collision chunks
--
--(C) Copyright 2004 Gary Haussmann
--this code is released under the Nebula License
--

tree_depth = 2
space_error = 1.00
tqt_tilesize = 128
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
chunker:setchunkfilename('home:export/terrains/examples/sample.clod')
chunker:setspacings(0.5,0.5,0.5)
chunker:setchunkparameters(tree_depth, space_error)
chunker:setvalidateflag(true)

-- do the chunking...

chunker:compilechunksfromfile('home:work/textures/heightfield.bmp')

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
new('nclodtqtmaker','/usr/tqtcompiler')
tqtcompiler = nebula.usr.tqtcompiler
tqtcompiler:settqtparameters(tree_depth, tqt_tilesize)
tqtcompiler:settqtfilename('home:export/terrains/examples/sample.tqt')
tqtcompiler:compiletqtfromfile('home:work/textures/cloddiffuse.bmp')

--and exit

exit()
