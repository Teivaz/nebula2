-------------------------------------------------------------------------------
--  dbbuilder.ms 
--
--  (c)2007 Kim Hyoun Woo
-------------------------------------------------------------------------------

include "mangalore/db.ms"
include "mangalore/custattr.ms"

-------------------------------------------------------------------------------
--  custom attributes which is used for containing database name.
--  
-------------------------------------------------------------------------------
mlDatabaseCA = attributes "MangaloreDatabase"
(
    parameters MangaloreDatabase rollout:rDatabase 
    (
        -- mangalore database parameter does not have default value
        -- because that there is no way to make it animatable with specified
        -- ui control even it is not possible to connect the parameter to ui.
        mlDatabase  type:#string default:"" animatable:false 
    )
   
    rollout rDatabase "Mangalore Database"
    (
    )
)

-------------------------------------------------------------------------------
--  create a new database file.
--
-------------------------------------------------------------------------------
fn mlCreateDatabase = 
(
    filepath = getSaveFileName caption:"Create Mangalore Database File" \
                    types:"Database(*.db3)|*.db3|All|*.*|"

    if filepath != undefined do
    (
        dbPath = filepath

        --
        mlAddCustomAttributes rootNode mlDatabaseCA 
        rootNode.mlDatabase = getFilenameFile dbPath

        -- create nsqllite3server via nmaxexpose.
        nCreateSqlServer()

        -- create .db3 file and retrieve an instance of database object.
        database = nCreateDatabase (dbPath)

        -- write tables and columns to the given database.
        dir = getDir #scripts
        genFile = getfiles (dir + "\\mangalore\\newdb.ms")

        if genFile.count != 0 then
        (
            include "mangalore/newdb.ms"
            nCreateTablesAndColumns database

            format "%s database file was created.\n" dbPath to:listener
        )
        else
        (
            print "There is no newdb.ms file!\n"
        )
    )
)

-------------------------------------------------------------------------------
--  open selected database file.
--  A database file should be opened before we export a scene. 
-------------------------------------------------------------------------------
fn mlOpenDatabase = 
(
    filepath = getOpenFileName caption:"Open Mangalore Database File" \
                    types:"Database(*.db3)|*.db3|All|*.*|"

    if filepath != undefined do
    (
        dbPath = filepath

        -- check and force remove if there is any exist custom attributes 
        -- and then recreate it.
        mlAddCustomAttributes rootNode mlDatabaseCA 

        -- save database to the rootnode to get it lately
        rootNode.mlDatabase = getFilenameFile dbPath

        -- create nsqllite3server via nmaxexpose.
        nCreateSqlServer()

        -- create .db3 file and retrieve an instance of database object.
        database = nCreateDatabase (dbPath)
    )
)
-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
