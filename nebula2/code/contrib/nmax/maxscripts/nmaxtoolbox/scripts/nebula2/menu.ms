---------------------------------------------------------------------------------
--  menu.ms
--
--  3dsmax script for 'Nebula2' menu
--
--  (c)2004 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
-- macro script for 'Export' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_Export
category:"nmaxtoolbox" 
buttonText:"Export"
tooltip:"Export" 
( 	   
    on execute do
    (
        include "nebula2/export.ms"
        
        nExport()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Batch Export' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_BatchExport
category:"nmaxtoolbox" 
buttonText:"Batch Export..."
tooltip:"Batch Export" 
( 
    on execute do
    (    	        
        include "nebula2/batchexport.ms"
    
        -- create 'Batch Export' Dialog
        CreateDialog rBatchExport
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Material Editor' menu item.
--
-- Open material editor then set Nebula custom material if it is not evaluated yet.
-- If the Nebula custom material aleady exist, just open material editor.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_MaterialEditor
category:"nmaxtoolbox" 
buttonText:"Material Editor..."
tooltip:"Open Nebula Custom Material Editor" 
( 	    
    on execute do
    (
        include "nebula2/materialeditor.ms"
        
        nMaterialEditor()        
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Mesh Options' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_MeshOptions
category:"nmaxtoolbox" 
buttonText:"Mesh Options"
tooltip:"Specify Mesh Options" 
( 	
    include "nebula2/meshoptions_fn.ms"
	
    on execute do
    (
        AddMeshOptionsCustomAttributes()
		
        -- Sets Modify command mode activate.
        max modify mode
    )
)

---------------------------------------------------------------------------------
-- macro script for 'LOD' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_LOD
category:"nmaxtoolbox" 
buttonText:"LOD"
tooltip:"Specify LOD Options" 
( 	
    include "nebula2/lod_fn.ms"
	
    on execute do
    (
        nAddLODCustomAttributes()

        -- Sets Modify command mode activate.
        max modify mode
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Animation' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_Animation
category:"nmaxtoolbox" 
buttonText:"Animation Editor..."
tooltip:"Specify Animations States and Clips" 
( 	
    --include "nebula2/animation.ms"

    on execute do 
    (
        --r_dialogue = newrolloutfloater "Animation Utility" 400 700
        --addRollout nMaxAnimStateTreeViewRollout r_dialogue rolledUp:false
        messageBox "Not available yet." title:"Nebula2 Animation Editor"
    )
)

---------------------------------------------------------------------------------
-- macro script for 'LOD' menu item
---------------------------------------------------------------------------------
/*
macroScript NebulaToolBox_Tools
category:"nmaxtoolbox" 
buttonText:"LOD Options"
tooltip:"Level Of Details" 
( 	
    include "nebula2/lod_fn.ms"

    on execute do 
    (
        AddLODCustomAttributes()
    )
)
*/
---------------------------------------------------------------------------------
-- macro script for 'Directory Setting' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_SetDirectory
category:"nmaxtoolbox" 
buttonText:"Directory Setting..."
tooltip:"specifies assignes and pathes" 
( 
    on execute do 
    (
        nShowDirSettingDlg()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Help' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_Help
category:"nmaxtoolbox" 
buttonText:"Help..."
tooltip:"User Reference" 
( 
    on execute do 
    (
        include "nebula2/help.ms"
        
        nShowHelp()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'About' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_About
category:"nmaxtoolbox" 
buttonText:"About..."
tooltip:"About Box for Nebula 3DS Max Tookit" 
( 
    on execute do 
    (
        include "nebula2/about.ms"
        
        CreateDialog rAboutRollout
    )
)

---------------------------------------------------------------------------------
-- Add menu items to 'menuMan'
---------------------------------------------------------------------------------
fn AddMenu =
(
    local bCreateMenu = false
	
    if menuMan.registerMenuContext 0x53c71f39 then 
        bCreateMenu = true
    else 
    (	
        if (menuMan.findMenu "Nebula2") == undefined then 
            bCreateMenu = true
    )

    if bCreateMenu then
    ( 
        local mainMenu = menuMan.getMainMenuBar()
        local nebula2Menu = menuMan.createMenu "Nebula2"
	
        local ItemsTemp = #()
        ItemsTemp[1] = menuMan.createActionItem "NebulaToolBox_Export" "nmaxtoolbox"
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_BatchExport" "nmaxtoolbox")		
        append ItemsTemp (menuMan.createSeparatorItem())	
		
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MaterialEditor" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Animation" "nmaxtoolbox")		
        append ItemsTemp (menuMan.createSeparatorItem())

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MeshOptions" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_LOD" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())	

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_SetDirectory" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())
        
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Help" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_About" "nmaxtoolbox")
					
        for i=1 to itemsTemp.count do 
            nebula2Menu.addItem ItemsTemp[i] -1
		
        local subMenuItem = menuMan.createSubMenuItem "Nebula2" nebula2Menu
        local subMenuIndex = mainMenu.numItems()
        mainMenu.addItem subMenuItem subMenuIndex
        menuMan.updateMenuBar()
    ) 
)

---------------------------------------------------------------------------------
-- Remove menu item from 'menuMan'
---------------------------------------------------------------------------------
fn RemoveMenu = 
(
    local menu = menuMan.findMenu "Nebula2";
    if menu != undefined then
    (
        menuMan.unRegisterMenu menu;
        menuMan.updateMenuBar();
    )
)

---------------------------------------------------------------------------------
-- Load menu
---------------------------------------------------------------------------------
fn nLoadMenu =
(
    RemoveMenu()
    AddMenu()
)


