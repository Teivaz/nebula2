-------------------------------------------------------------------------------
--  terrain.ms
--
--  Import/Export heightmap for a terrain
--
--  (C)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  read the given heightmap then create editable mesh from the heightmap
-------------------------------------------------------------------------------
fn nImportTerrain terrainWorldSize terrainScale wColor doOptimize = 
(
    -- open image file browser
    b = selectbitmap() 
		
    -- get only file name which striped extension out
    if b != undefined then
        bname = (getfilenamefile b.filename) 
    else
        return undefined

    -- get properties of bitmap
    w = b.width 
    h = b.height
	
    local terrainObject
    local temp
    
    -- check all nodes in the secene that there is already terrain mesh node 
    -- which name of "terrain"	
    for i = 1 to rootnode.children.count do
        if ( rootnode.children[i].name == "terrain" ) then 
            temp =  rootnode.children[i]	
        else
            temp = undefined
    
    if temp != undefined then
    (
        if (temp.verts.count == (w * h)) then
        (
            terrainObject = temp
        )
        else
        (
            terrainObject = mesh length:terrainWorldSize \
                                 width:terrainWorldSize \
                                 lengthsegs:(w - 1) \
                                 widthsegs:(w - 1) \
                                 wireColor: blue
            terrainObject.wireColor = wcolor
        )
    )
    else
    (
        terrainObject = mesh length:terrainWorldSize \ 
                             width:terrainWorldSize \ 
                             lengthsegs:(w - 1) \
                             widthsegs:(w - 1) \
                             wireColor: blue
        terrainObject.wireColor = wcolor                             
    )

    terrainObject.name = "terrain"
    clearUndoBuffer()
    
    --local terrainScale = 10
	
    terrainScale = (terrainScale / 256.0)
    
    tmp2 = (w * h)
	
    -- for each row in the bitmap
    for r=0 to h-1 do 
    ( 
        -- retrieves pixels in the given row.
        pixels = getpixels b [0,r] w 
        
        -- read in the column of pixels
        for c=1 to w do 
        ( 
            -- get the pixel
            p = pixels[c] 

            -- adjust a height of the created mesh.
			-- use red component for height.
            terrainObject.verts[c + r*h].pos.z = ((p.r as integer) * terrainScale)
        )
    )	

    clearUndoBuffer()
    
    if (doOptimize) do
    (
        addModifier terrainObject (Optimize())
        clearUndoBuffer()
    )
    
    collapseStack terrainObject
    clearUndoBuffer()

    addModifier terrainObject (Uvwmap())
    collapseStack terrainObject
    clearUndoBuffer()

    select terrainObject
	
    return b.filename
)

