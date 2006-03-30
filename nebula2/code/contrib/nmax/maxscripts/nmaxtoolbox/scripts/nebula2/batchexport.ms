-------------------------------------------------------------------------------
--  batchexport.ms
--
--  Recursively export *.max 3dsmax scene files under specified directory into 
--  Nebula2 files under "proj:export/gfxlib".
--
--  (C)2005 Kim, Hyoun Woo.
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  rollout for 'batch export'
-------------------------------------------------------------------------------
rollout rBatchExport "Batch Export" width:350 height:380
(
    GroupBox grp1 "Select Files" pos:[7,4] width:329 height:249
    -- button for selecting source directory
    button btnSelectSrcDir "Select source directory" pos:[18,27] width:305 height:25
	
    button btnOk "Ok" pos:[182,344] width:156 height:25
    button btnCancel "Cancel" pos:[10,344] width:156 height:25
    listbox lbxCollectedFiles ".max files under source directory" pos:[15,59] width:312 height:10
	
    label lblCollectedFiles "" pos:[17,223] width:308 height:20

    groupBox grp5 "Export" pos:[7,258] width:331 height:78

    -- button for batch export
    button btnExport "Export" pos:[74,273] width:187 height:25    
    progressBar prgsExport "ProgressBar" pos:[14,307] width:312 height:18
	
    on rBatchExport open do
    (
        -- 'ok' button is enabled when export is finished.
        btnOk.enabled = false
    )
		
    local srcDirectory
    local collectedFiles
	
    -- event handler when the select source directory button is pressed	
    on btnSelectSrcDir pressed do
    (
        -- get source directory from select fold dialog
        srcDirectory = getSavePath "Select source directory"
	
        if srcDirectory != undefined do
        (	
            btnSelectSrcDir.caption = srcDirectory
		
            -- recursively collect files under the source directory
            include "nebula2/lib.ms"
	    
            collectedFiles = nGetFilesRecursive srcDirectory "*.max"
	    
            -- insert collected files to the listbox
            lbxCollectedFiles.items = collectedFiles 
		
            -- specify the number of collected files to the label.
            cntFiles = collectedFiles.count as string
            lblCaption = cntFiles + " files are collected."
            lblCollectedFiles.caption = lblCaption
        )		
    )
	
    -- event handler when the 'export' button is pressed	
    on btnExport pressed do
    (		
        local fileext = ".n2"
		
        prgsExport.value = 0.0
	    
        for i=1 to collectedFiles.count do
        (
            -- export here
	        
            local filename = getFilenameFile collectedFiles[i]
            local exportFilePath = filename + fileext

            --loadMaxFile collectedFiles[i]
            --exportFile exportFilePath #noPrompt
	        
            prgsExport.value = 100. * i / collectedFiles.count
	        
	    sleep 0.5
        )

        resetMaxFile #noPrompt

        -- finished the exporting, so eable 'ok' button
        btnOk.enabled = true		
    )
	
    -- event handler when the 'ok' button is pressed	
    on btnOk pressed do
    (
        DestroyDialog rBatchExport
    )
	
    -- event handler when the 'cancel' button is pressed
    on btnCancel pressed do
    (
        -- if we finished export, just destroy the dialog
        if btnOk.enabled == true then
        (   
            DestroyDialog rBatchExport
        )
        else
        (
            -- we still exporting, so query to finish or not
            result = queryBox "Abort batch export?" title:"Warning"
            if result == true do
                DestroyDialog rBatchExport
        )
    )
)

