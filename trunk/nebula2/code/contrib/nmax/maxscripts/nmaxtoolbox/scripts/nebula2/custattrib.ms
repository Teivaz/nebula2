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
--  -31-Oct-06  Changed to delete all duplicated custom attributes not just one.
--              It fixed mesh object has several mesh options which cause 3dsmax 
--              to slow down. (fix on #380)
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
            -- since there is a chance that there are more than one custom attributes, 
            -- so we should not just delete only one duplicated custom attribute 
            -- but all duplicated custom attributes.
            for i = 1 to numCustAttr do
            (
                attrDef = custAttributes.getdef obj i
                if attrDef != undefined and attrDef.name == ca.name do
                (
                    -- the object already has custom attribute.
                    custAttributes.delete obj i
                )
            ) 
        )
    )
		
    -- add a new custom attribute, if the object has not any one.
    if foundIndex <= 0 do
    (
        custAttributes.add obj ca
        return true
    )
    
    return false
)


