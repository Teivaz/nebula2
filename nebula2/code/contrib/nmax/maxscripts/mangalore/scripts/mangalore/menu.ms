---------------------------------------------------------------------------------
--  menu.ms
--
--  3dsmax script for 'Mangalore' menu
--
--  (c)2007 Kim, Hyoun Woo
---------------------------------------------------------------------------------

fileIn "mangalore/export.ms"
fileIn "mangalore/dbbuilder.ms"

---------------------------------------------------------------------------------
-- macro script for 'Export' menu item.
---------------------------------------------------------------------------------
macroScript Mangalore_Export
category:"mangalore" 
buttonText:"Export"
tooltip:"Export" 
( 	   
    on execute do
    (
        mlExport()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Batch Export' menu item.
---------------------------------------------------------------------------------
macroScript Mangalore_CreateDatabase
category:"mangalore" 
buttonText:"Create Database..."
tooltip:"Create Database File" 
( 
    on execute do
    (    	        
        mlCreateDatabase()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Preferences' menu item.
---------------------------------------------------------------------------------
macroScript Mangalore_Preferences
category:"mangalore" 
buttonText:"Preferences..."
tooltip:"Preferences" 
( 
    on execute do 
    (
        include "mangalore/preferences.ms"
    )
)

---------------------------------------------------------------------------------
-- macro script for 'Help' menu item.
---------------------------------------------------------------------------------
macroScript Mangalore_Help
category:"mangalore" 
buttonText:"Help..."
tooltip:"User Reference" 
( 
    on execute do 
    (
        include "mangalore/help.ms"
        mlShowHelp()
    )
)

---------------------------------------------------------------------------------
-- macro script for 'About' menu item.
---------------------------------------------------------------------------------
macroScript Mangalore_About
category:"mangalore" 
buttonText:"About..."
tooltip:"About Box for Nebula 3DS Max Tookit" 
( 
    on execute do 
    (
        include "mangalore/about.ms"
        CreateDialog mlAboutRollout
    )
)

---------------------------------------------------------------------------------
-- Add menu items to 'menuMan'
---------------------------------------------------------------------------------
fn mlAddMenu =
(
    local bCreateMenu = false
	
    if menuMan.registerMenuContext 0x53c71f39 then 
        bCreateMenu = true
    else 
    (	
        if (menuMan.findMenu "Mangalore") == undefined then 
            bCreateMenu = true
    )

    if bCreateMenu then
    ( 
        local mainMenu = menuMan.getMainMenuBar()
        local mangaloreMenu = menuMan.createMenu "Mangalore"
	
        local ItemsTemp = #()
        ItemsTemp[1] = menuMan.createActionItem "Mangalore_Export" "mangalore"
        append ItemsTemp (menuMan.createSeparatorItem())	

        append ItemsTemp (menuMan.createActionItem "Mangalore_CreateDatabase" "mangalore")
        append ItemsTemp (menuMan.createSeparatorItem())	
		
        append ItemsTemp (menuMan.createActionItem "Mangalore_Preferences" "mangalore")
        append ItemsTemp (menuMan.createActionItem "Mangalore_Help" "mangalore")
        append ItemsTemp (menuMan.createActionItem "Mangalore_About" "mangalore")
					
        for i=1 to itemsTemp.count do 
            mangaloreMenu.addItem ItemsTemp[i] -1
		
        local subMenuItem = menuMan.createSubMenuItem "Mangalore" mangaloreMenu
        local subMenuIndex = mainMenu.numItems()
        mainMenu.addItem subMenuItem subMenuIndex
        menuMan.updateMenuBar()
    ) 
)

---------------------------------------------------------------------------------
-- Remove menu item from 'menuMan'
---------------------------------------------------------------------------------
fn mlRemoveMenu = 
(
    local menu = menuMan.findMenu "Mangalore";
    if menu != undefined then
    (
        menuMan.unRegisterMenu menu;
        menuMan.updateMenuBar();
    )
)

---------------------------------------------------------------------------------
-- Load menu
---------------------------------------------------------------------------------
fn mlLoadMenu =
(
    mlRemoveMenu()
    mlAddMenu()
)

