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

            itms = pbArray[a]

            /*
            item[1] : name
            itms[2] : id 
            itms[3] : owners reference number 
            itms[4] : parameter block keywords
            */
            for y = 5 to itms.Count do
            (
                cmd = "ca."
                cmd += itms[y][1] as string
                result = execute cmd
                
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

                -- get value
                values += result as string
                values += " "
            )
        )
    )

    append ret columns
    append ret values
    return ret
)

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
fn mlExport = 
(
    try
    (
        local database  = 
        local tablename = 

        for obj in objects do
        (
            if ((classof obj) == XRefObject) then
            (
                inargs = nParseCustomAttrs (obj) 

                -- export not a part of customattributes such as transform etc.
                inargs[1] += "Transform"
                inargs[1] += " "

                tm = nConvertTM obj.transform
                inagrs[2] += tm 
                inagrs[2] += " "

                nInsertRow database tablename inagrs[1] inagrs[2]

                echo #INFO ("exported : " + obj.name)
            )
        )
    )
    catch
    (
        format "ERROR: Unexpected error when export % \n" obj.name to:listener
    )
)

