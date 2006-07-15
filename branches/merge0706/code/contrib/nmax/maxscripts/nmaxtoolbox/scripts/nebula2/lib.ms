-------------------------------------------------------------------------------
--  lib.ms
--  
--  library for common functions.
--
--  (C)2005 Kim, Hyoun Woo.
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  Recursively collect all files which has the given pattern under the 
--  specified directory.
--
--  Parameters:
--    root : the directory which to collect the files
--    parttern : file pattern to collect
--  
--  Retrun:
--    array which contains collected files.
--
--  Usage: 
--       get all .ms files from the max installation
--
--       nGetFilesRecursive "c:/3dsmax7" "*.ms"
--
-------------------------------------------------------------------------------
fn nGetFilesRecursive root pattern =
(
    -- collect all directories under root
    dirArray = GetDirectories (root + "/*")
    
    for d in dirArray do
        join dirArray (GetDirectories (d + "/*"))

    -- collect all files which has the given pattern
    collectedFiles = #()

    for f in dirArray do
        join collectedFiles (getFiles (f + pattern))

    -- return array which contains collected files.
    collectedFiles 
)

