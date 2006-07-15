-------------------------------------------------------------------------------
--  character.ms
--
--  3DS Max script for Nebula2 characer animation state dialog.
--
--  - Create animation states from clips of Motion Flow.
--  - Create note track and its keys for root node of biped object. 
--    The exporter refers created note track when it exports character. 
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

include "nebula2/moflow.ms"

-------------------------------------------------------------------------------
--  animation state rollout
--  
-------------------------------------------------------------------------------
rollout rAnimStateEditor "Nebula2 Animation State Dialog" width:439 height:312
(
    ActiveXControl animStateTreeView "MSComctlLib.TreeCtrl" pos:[13,21] width:219 height:236
    label lblFirstFrame "First Frame : " pos:[253,40] width:70 height:15
    label lblFrameLength "Frame Length : " pos:[251,74] width:70 height:15
    edittext edtFirstFrame "" pos:[337,37] width:70 height:20
    edittext edtFrameLength "" pos:[337,70] width:70 height:20
    label lblChannelName "Character State Channel Name" pos:[251,132] width:161 height:20
    edittext edtChannelName "" pos:[249,156] width:155 height:20
    GroupBox grpAnimState "Animation State" pos:[240,15] width:190 height:177
    label lblFadeinTime "Fadein Time : " pos:[251,103] width:70 height:16
    edittext edtFadeinTime "" pos:[338,102] width:70 height:20
    button btnCreateNoteTrack "Create Note Track" pos:[253,219] width:159 height:27
    button btnOk "Ok" pos:[348,270] width:80 height:30
	
    local animStateArray = #()
	
    fn initTreeView tv = 
    (
        tv.LineStyle = #tvwTreeLines 
    )
	
    fn addChildren tv node children = 
    (
        for c in children do
        (
            newNode = tv.Nodes.add node.index 4 "" c.stateName 0
            addChildren tv newNode c.children
        )
    )
	
    fn fillInTreeView tv animStates = 
    (
        theRootNode = tv.Nodes.add()
        theRootNode.text = "Animation States"
        theRootNode.expanded = true

        for o in animStates do
        (
            newNode = tv.Nodes.add theRootNode.index 4 "" o.stateName 0
        )
    )
		
    on rAnimStateEditor open do
    (	    
        -- retrieves animation state array from motion flow
        animStateArray = createAnimStatesFromMotionFlow()

        if animStateArray.count > 0 then
        (
            -- initialize tree view
	    initTreeView animStateTreeView
		
            -- fill tree view with retrieved animation states
            fillInTreeView animStateTreeView animStateArray
        )
    )
	
    on animStateTreeView nodeClick theNode do
    (
        for o in animStateArray where o.stateName == theNode.text do
        (
            edtFirstFrame.text  = o.startFrame as string
            edtFrameLength.text = o.duration as string 
            edtFadeinTime.text  = o.fadeinTime as string
            --edtChannelName 
        )	
    )
	
    -- create note track and add it to biped root node
    on btnCreateNoteTrack pressed  do
    (
        print animStateArray
		
        bipedRoot = getBipedRootNode()
        if bipedRoot == undefined then
        (
            messageBox "No biped objects are available." title:"Warning"
        )
        else
        (
            -- check the biped root node has any note track
            if hasNoteTracks bipedRoot then
            (
                existNoteTrack = getNoteTrack bipedRoot 1
                deleteNoteTrack bipedRoot existNoteTrack
            )
            
            -- create note track 
            newNoteTrack = notetrack "note track"
            
            -- add the created note track to biped root node
            addNoteTrack bipedRoot newNoteTrack
            
            -- create note key array
            ntKeys = newNoteTrack.keys
            
            -- create note track keys
            i = 1
            for a in animStateArray do
            (
                newKey = addNewNoteKey ntKeys animStateArray[i].startFrame
                newKey.value = animStateArray[i].stateName
                i += 1
            )
        )
    )

    on btnOk pressed do
    (
        destroyDialog rAnimStateEditor
    )	
)

