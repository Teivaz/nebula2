-------------------------------------------------------------------------------
--  animtoxml.ms
--
--  Gether animation clips from note track and wirte those into existing 
--  anims.xml file.
--
--  (c)2007 Kim Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- extract animation clip name from note track and its key.
-------------------------------------------------------------------------------
fn getAnimName value = 
(
    ret = ""
    i = value.count
    do 
    (
        if value[i] == ":" then
            exit
        i -= 1
    )
    while (i>0)
    
    offset = value.count - i + 1
    ret = replace value i offset ""

    -- trim white space(or tabs) if it is
    ret = trimRight ret
    
    return ret
)

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
struct ExcelUtil
(
    excel,
   
    -- create 'excel' OLE object and open the given xml file.
    -- e.g. openExcel "e:\\anims.xml"
    fn openExcel xmlFile = 
    (
        excel = createOLEObject "excel.application"
        excel.workbooks.open xmlFile
        
        return excel
    ),
    
    --
    fn getExcelCell row col = 
    (
         cell = excel.application.cells row col

        return cell
    ),
    
    
    -- relaese created OLE Excel object.
    fn closeExcel = 
    (
        releaseOLEObject excel
    ),

    -- 
    fn findEmptyColumn = 
    (
        col = 1
        val-- = getExcelCell 1 col

        do 
        (
            val = getExcelCell 1 col
            col += 1
        )
        while val.value != undefined
        
        return col - 1
    )
)

-------------------------------------------------------------------------------
--  Extract key of the note track and insert it to the exist animation XML file.
--
--  e.g. appendAnimToXml "d:/nebula2/data/tables/anims.xml" "player"
-------------------------------------------------------------------------------
fn appendAnimToXml path category = 
(
    nt = getNoteTrack $bip01 1
    nt_keys = nt.keys

    animClipNames = #()
    
    for i=1 to nt_keys.count do
    (
        val = nt_keys[i].value
        if val != "" do
        (
            str = stringStream val
            line = readLine str
            if line != undefined do
            (
                animClipName = getAnimName line        
        
                --trimRight animClipName
                print animClipName + "\n"
        
                append animClipNames animClipName
            )
        )
    )
    
    -- open animation xml file.
    exlUtil = ExcelUtil()
    --exlUtil.openExcel "e:/anims.xml"
    exlUtil.openExcel path
    
    -- retrieve empty column
    col = exlUtil.findEmptyColumn()
    
    -- add character name to the first row
    title = exlUtil.getExcelCell 1 col
    --title.value = "Valerie"
    title.value = category
    
    for j=1 to animClipNames.count do
    (
        row = j + 1
        cell = exlUtil.getExcelCell row col
        cell.value = animClipNames[j]
    )
)

