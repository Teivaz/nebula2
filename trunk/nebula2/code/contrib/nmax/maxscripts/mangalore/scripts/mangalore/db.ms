------------------------------------------------------------------------------- 
--  db.ms
--
--  Nebula2 SQL database utility functions.
--
--  (c)2007 Kim Hyoun Woo
------------------------------------------------------------------------------- 

-------------------------------------------------------------------------------
--  nmaxtoolbox does not have default sql database server, 
--  so we should create one before we use it.
------------------------------------------------------------------------------- 
fn nCreateSqlServer = 
(
    ret = nebula2.lookup "/sys/servers/db"
    if ret == "" then
        nebula2.new "nsqlite3server" "/sys/servers/db"
)

------------------------------------------------------------------------------- 
--  Create database file.
--  path - path of database file where to be created.
--
--  e.g. >nCreateDatabase "d:\\dev\\nebula2\\export\\db\\mydb.db3"
--       >"mydb"
------------------------------------------------------------------------------- 
fn nCreateDatabase path = 
(
    -- database is resource and all resources are create by resource server
    -- in nebula. So the resource server should be already created before
    -- any resources are created.
    ret = nebula2.lookup "/sys/servers/resource"
    if ret == "" then
        nebula2.new "nresourceserver" "/sys/servers/resource"

    nebula2.call "/sys/servers/db.newdatabase" path
    return getFilenameFile path
)

------------------------------------------------------------------------------- 
-- Create a new table in the database.
------------------------------------------------------------------------------- 
fn nCreateTable database tablename tablearray keycolumn = 
(
    cmd = "/sys/share/rsrc/db/"
    cmd += (database as string)
    cmd += ".createtable"

    inarg = #()
    append inarg tablename
    append inarg tablearray
    append inarg keycolumn

    nebula2.call cmd inarg
)

------------------------------------------------------------------------------- 
--  Delete table from the database.
------------------------------------------------------------------------------- 

fn nDeleteTable database tablename =
(
    cmd = "/sys/share/rsrc/db/"
    cmd += database
    cmd += ".deletetable"

    nebula2.call cmd tablename
)

------------------------------------------------------------------------------- 
--  Insert a new row into the database and initialize it with the given 
--  space-separated values. 
------------------------------------------------------------------------------- 
fn nInsertRow database tablename columns values = 
(
    cmd = "/sys/share/rsrc/db/"
    cmd += database
    cmd += ".insertrow"

    inarg = #()
    append inarg tablename
    append inarg columns
    append inarg values

    nebula2.call cmd inarg
)

------------------------------------------------------------------------------- 
--  Delete a row from the database.
------------------------------------------------------------------------------- 

fn nDeleteRow database tablename keycolumn keyvalue = 
(
    cmd = "/sys/share/rsrc/db/"
    cmd += database
    cmd += ".deleterow"
	
    inarg = #()
    append inarg tablename
    append inarg keycolumn
    append inarg keyvalue

    nebula2.call cmd inarg
)

