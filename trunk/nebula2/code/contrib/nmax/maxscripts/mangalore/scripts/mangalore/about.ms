---------------------------------------------------------------------------------
--  about.ms
--
--  (c)2007 Kim Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  rollout for 'About' dialogbox in 'Mangalore' menu.
---------------------------------------------------------------------------------
rollout mlAboutRollout "About Mangalore Tool for 3dsMax" width:324 height:170
(
    GroupBox grp1 "" pos:[4,7] width:316 height:124
    label lblDesc "Mangalore for Nebula2 3DS Max Toolkit." pos:[90,26] width:217 height:20
    label lblVersion "Release Version 0.1" pos:[90,49] width:210 height:20
    label lblCopyWrite "Copyright (c) 2007 Kim Hyoun Woo." pos:[89,102] width:210 height:20
    label lblLicense "This product is licensed under the terms of the Nebula License." pos:[89,70] width:221 height:31
    bitmap bmpN2Logo pos:[15,26] width:64 height:64 fileName:"nebula2/n2logo256.bmp"

    button btnOk "Ok" pos:[234,137] width:84 height:24
	
    on btnOk pressed do
    (
        DestroyDialog mlAboutRollout
    )
)

