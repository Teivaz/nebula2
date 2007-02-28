-------------------------------------------------------------------------------
--  export.ms
--
--  (c)2007 Kim Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  It converts 3dsmax trnsform to nebula2's one and returns it by string type
--  e.g. str = nConvertTM $box01.transform
-------------------------------------------------------------------------------
fn nConvertTM m =
(
    local point3 p 
    local ret = ""

    p = m.row1
    ret += p.x as string + ","
    ret += p.z as string + ","
    ret += p.y as string + ","
    ret += "0.0,"

    -- swap z and y
    p = m.row3
    ret += p.x as string + ","
    ret += p.z as string + ","
    ret += p.y as string + ","
    ret += "0.0,"

    p = m.row2
    ret += p.x as string + ","
    ret += p.z as string + ","
    ret += p.y as string + ","
    ret += "0.0,"

    p = m.row4
    ret += p.x as string + ","
    ret += p.z as string + ","
    ret += p.y as string + ","
    ret += "1.0"

    return ret
)

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
fn nStripPrefix str =
(
    ret = ""
    cnt = str.count
    for i=2 to cnt do
    (
        ret += str[i]
    )
    return ret
)

-------------------------------------------------------------------------------
--  Parse the given max object and extracts and classify custom attributes 
--  and its values.
--  It returns MAXScript array contains the following two elements:
--      retrun[1] : space-seperated columns.
--      return[2] : space-seperated values of the columns.
-------------------------------------------------------------------------------
fn nParseCustomAttrs obj = 
(
    columns = ""
    values = ""

    ret = #()

    for objDef in (custAttributes.getDefs obj) do
    (
        pbArray = custAttributes.getPBlockDefs objdef
        for a = 1 to pbArray.count do 
        (
            ca = custAttributes.get obj a
			
            if ca == undefined do
                continue

            itms = pbArray[1]

            /*
            item[1] : name
            itms[2] : id 
            itms[3] : owners reference number 
            itms[4] : parameter block keywords
            */
            for y = 5 to itms.Count do
            (                
                if (itms[y][2][2] != #color) and (itms[y][2][2] != #stringTab) then
                (
                
                --format  " type: %" itms[y][2][2] 
                --print y
            
                cmd = "ca."
                cmd += itms[y][1] as string
                result = execute cmd
                
                if result == undefined do
                (
                    print result
                    continue
                )
                -- get column 
                -- we dont append a column which does not have a value.
                -- it might cause an assertion when we call ninsertrow nebula2 
                -- expose function.
                exp = result as string
                if exp != "" then
                (
                    str = itms[y][1] as string
                    --col = nStripPrefix str 
                    col = ""
                    cnt = str.count
                    for i=2 to cnt do
                    (
                        col += str[i]
                    )                    
                    columns += col
                    columns += " "
                )
                                                
                if itms[y][2][2] == #integer then
                (
                    for z=1 to itms[y][2].Count by 2 do
                    (
                        if itms[y][2][z] == #ui then
                        (
                            -- see if the given ui is dropdownlist.
                            str = itms[y][2][z+1] as string
                            index = findString str "dlist"
                            if index != undefined then
                            (
                                /*
                                --format "index : %" index
                                tmp = ""
                                for i = 5 to str.count-5 do
                                (
                                    tmp += str[index + i]
                                )
                                print tmp
                                */
                                /*
                                -- remove 'c' prefix.
                                s = ""
                                tmp = itms[y][1] as string
                                for i=2 to tmp.Count do
                                (
                                    s += tmp[i]
                                )
                                */
                                -- e.g. ca.strtab_Type[1]
                                cmd = "ca."
                                cmd += "strtab"
                                cmd += itms[y][1] as string 
                                cmd += "["
                                cmd += result as string
                                cmd += "]"
                                result = execute cmd
                                print cmd
                                print result
                            )
                        )
                    )
                )
                -- get value
                values += result as string
                values += " "					

                )
            )
        )
    )

    append ret columns
    append ret values
    return ret
)

-- the below 'parse.ms' file is automatically generated by runing build.py
include "mangalore/parse.ms"

-------------------------------------------------------------------------------
--  Export entities.
-- 
-------------------------------------------------------------------------------
fn mlExport = 
(
    try
    (
        local database  = ""

        -- check the rootnode has custom attribute which name of it is "MangaloreDatabase".
        numCustAttr = custAttributes.count rootNode
        foundIndex = -1
		
        -- check that the object already has the given custom attribute.
        for i = 1 to numCustAttr do
        (
            -- retrieves the attribute definition for a given custom attribute 
            -- set in an object
            attrDef = custAttributes.getdef rootNode i
            if attrDef != undefined and attrDef.name == "MangaloreDatabase" do
            (
                -- the object already has custom attribute.
                foundIndex = i
            )
        ) 

        -- no database is opened, any database should be opened before we export a scene.
        if foundIndex == -1 do
        (
            msg = "No database is opened, any database should be opened before we export a scene."
            messageBox msg title:"Warning"
            return
        )
        else
        (
            -- retrieve database from roonode's custom attribute
            database = (rootNode.mlDatabase as string)
        )

        if database = "" do
        (
            print "Error: Unexpected behavior. Database is exist but we cannot retrieve its name.\n"
            return
        )

        --FIXME: export entities only at the moment. 
        local tablename = "_Entities"

        for obj in objects do
        (
            if ((classof obj) == XRefObject) then
            (
                -- retrieve columns and values of the given node.
                inargs = nParseMangaloreEntity obj 

                -- insert values to database.
                nInsertRow database tablename inargs[1] inargs[2]
            )
            else
            (
                format "WARNING: %s is not a XRef object.\n" obj.name to:listener
            )
        )
    )
    catch
    (
        format "ERROR: Unexpected error when export % \n" obj.name to:listener
    )
)
-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
