-------------------------------------------------------------------------------
--  dbbuilder.ms 
--
--  (c)2007 Kim Hyoun Woo
-------------------------------------------------------------------------------

include "mangalore/db.ms"
--include "mangalore/newdb.ms"

-------------------------------------------------------------------------------
fn mlCreateDatabase = 
(
    -- retrieve database which we write tables and columns into.
    mangaloreIniFile = getDir #plugcfg
    mangaloreIniFile += "\\"
    mangaloreIniFile += "mangalore.ini"

    path =  getINISetting mangaloreIniFile "Database" "filepath"
    fname = getINISetting mangaloreIniFile "Database" "filename"

    print path
    print fname

    dbPath = path + "\\" + fname
    if path == "" or fname == "" then
    (
        print "Incorrect DB path or filename.\n"
        return undefined
    )

    -- create nsqllite3server via nmaxexpose.
    nCreateSqlServer()

    -- create .db3 file and retrieve an instance of database object.
    database = nCreateDatabase (dbPath)

    -- write tables and columns to the given database.
    dir = getDir #scripts
    genFile = getfiles (dir + "\\mangalore\\newdb.ms")
    print genFile
    if genFile.count != 0 then
    (
        print "newdb.ms file exist"
        include "mangalore/newdb.ms"
        nCreateTablesAndColumns database
    )
    else
        print "There is no newdb.ms file!\n"
)

