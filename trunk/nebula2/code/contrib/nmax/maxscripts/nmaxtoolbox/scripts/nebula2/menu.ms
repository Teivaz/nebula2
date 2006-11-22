---------------------------------------------------------------------------------
--  menu.ms
--
--  3dsmax script for 'Nebula2' menu
--
--  (c)2004 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
-- macro script for 'Preview' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_Preview
category:"nmaxtoolbox" 
buttonText:"Preview"
tooltip:"Preview" 
( 	   
    on execute do
    (
        include "nebula2/preview.ms"
        
        nPreview()
    )
)

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
-- macro script for 'Scene Options' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_SceneOptions
category:"nmaxtoolbox" 
buttonText:"Scene Options"
tooltip:"Specify Scene Options" 
( 	
    include "nebula2/sceneoptions_fn.ms"
	
    on execute do
    (
        AddSceneOptionsCustomAttributes()
		
        -- Sets Modify command mode activate.
        max utility mode
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
-- macro script for 'Anim Options' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_AnimOptions
category:"nmaxtoolbox" 
buttonText:"Anim Options"
tooltip:"Specify Animation Options" 
( 	
    include "nebula2/animoptions_fn.ms"
	
    on execute do
    (
        AddAnimOptionsCustomAttributes()
		
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
    include "nebula2/character.ms"

    on execute do 
    (
        createDialog rAnimStateEditor width:439 height:312
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Terrain Editor' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_TerrainEditor
category:"nmaxtoolbox" 
buttonText:"Terrain Editor..."
tooltip:"import/export heightmap for terrain" 
( 
    on execute do 
    (
        include "nebula2/terraineditor.ms"      
        createDialog rTerrainEditor width:216 height:180
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Directory Setting' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_SetDirectory
category:"nmaxtoolbox" 
buttonText:"Directory Setting..."
tooltip:"specifies assignes and paths" 
( 
    on execute do 
    (
        nShowDirSettingDlg()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Connect IPC' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_ConnectIPC
category:"nmaxtoolbox" 
buttonText:"Connect IPC"
tooltip:"IPC for remote access" 
( 
    on execute do 
    (
        include "nebula2/ipc.ms"
        
        nIPCConnect()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Disconnect IPC' menu item.
---------------------------------------------------------------------------------
macroScript NebulaToolBox_DisonnectIPC
category:"nmaxtoolbox" 
buttonText:"Disconnect IPC"
tooltip:"IPC for remote access" 
( 
    on execute do 
    (
        include "nebula2/ipc.ms"
        
        nIPCDisconnect()
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
        ItemsTemp[1] = menuMan.createActionItem "NebulaToolBox_Preview" "nmaxtoolbox"
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Export" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_BatchExport" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())	
		
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MaterialEditor" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Animation" "nmaxtoolbox")		
        append ItemsTemp (menuMan.createSeparatorItem())

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_SceneOptions" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MeshOptions" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_AnimOptions" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())	

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_TerrainEditor" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_SetDirectory" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_ConnectIPC" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_DisonnectIPC" "nmaxtoolbox") 
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


