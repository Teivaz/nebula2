---------------------------------------------------------------------------------
--  utilz.ms
--
--  (C)2006  Kim, Hyoun Woo
---------------------------------------------------------------------------------
            
---------------------------------------------------------------------------------
--  Put a warning if the scene to be exported has mulitple objects which have
--  same object name.
---------------------------------------------------------------------------------
fn nCheckObjectName = 
(
    -- collect all objects in the scene.
    o = #()

    for i in $* do
    (
        append o i
    )

    -- iterate collected nodes and see if there are objects which have same name.
    for i=1 to o.count do
    (
        for j=i to o.count do
        (
            -- no need to check for hidden node
            if o[j].isHidden == false then
            (
                if i != j then
                (
                    if o[i].name == o[j].name then
                    (
                        local msg = stringstream ""
                        format "There are multiple objects with the same name : %" o[i].name to:msg
                        messageBox msg title:"Warning"                
                    )
                )
            )
        )
    )
)


