-------------------------------------------------------------------------------
--  terraineditor.ms
--
--  (C)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- rollout for terrain 
-------------------------------------------------------------------------------
rollout rTerrainEditor "Terrain Editor" width:216 height:180
(
    GroupBox grpHeightmape "Height Map" pos:[10,7] width:195 height:159
    spinner spnWorldSize "Terrain World Size:" pos:[35,24] width:143 height:16 range:[1,1e+006,100] scale:1
    spinner spnScale "Terrain Scale:        " pos:[34,49] width:144 height:16 range:[1,100000,10] scale:1
    colorPicker cpWireClr "Wireframe color:     " pos:[27,81] width:147 height:16
    button btnImport "Import Terrain" pos:[29,130] width:150 height:23
    checkbox chkOptimize "optimizing" pos:[28,104] width:150 height:16

    -- import a terrain from selected heightmap.	
    on btnImport pressed do
    (
        include "nebula2/terrain.ms"

        heightmapFilename = nImportTerrain spnWorldSize.value spnScale.value cpWireClr.color chkOptimize.checked
		
        -- change button text to filename of the loaded heightmap.
        if heigihtmapFilename != undefined do
            btnImport.text = filenameFromPath heightmapFilename
    )
)

