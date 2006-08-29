-------------------------------------------------------------------------------
--  custattrib.ms
--
--  Defines Nebula2 node custom attributes utilities.
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  Add the given custom attributes 'ca' to the 'obj'
-- 
-------------------------------------------------------------------------------
fn AddCustomAttributes obj ca = 
(
    numCustAttr = custAttributes.count obj
    foundIndex = -1
		
    -- check that the object already has the given custom attribute.
    for i = 1 to numCustAttr do
    (
        -- retrieves the attribute definition for a given custom attribute 
        -- set in an object
        attrDef = custAttributes.getdef obj i
        if attrDef != undefined and attrDef.name == ca.name do
        (
            -- the object already has custom attribute.
            foundIndex = i
        )
    ) 

    -- if the object already has custom attribute
    if foundIndex > 0 do
    (
        strContents = "Remove Custom Attributes from '"
        strContents = strContents + (obj.name as string)
        strContents = strContents + "'?"
        bRemove = queryBox strContents title:"Warning" beep:false 

        if bRemove do
        (
            -- remove exist custom attributes.
            custAttributes.delete obj foundIndex
        )
    )
		
    -- add a new custom attribute, if the object has none.
    if foundIndex <= 0 do
    (
        custAttributes.add obj ca
    )
)


