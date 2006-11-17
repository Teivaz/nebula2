/*-----------------------------------------------------------------------------
    nohviewer_demo.ms

    This demo shows Nebula's Named Object Hierarchy(NOH) on treeview.

    Put this on '$3dsmax/scripts' directory then run. Make sure before
    run this maxscript, nmaxexpose.dlu plugin should be installed on
    $3dsmax/plugins directory.

    (C) 2004 Kim, Hyoun Woo.
*/
    
-- rollout for treeview
rollout NOHTreeViewer "Nebula Object Viewer" width:355 height:540
(
    -- activeX treeview control
    ActiveXControl tv "MSComctlLib.TreeCtrl.2" pos:[5,52] width:343 height:478
	
    /*
        Add node to treeview control 
	  
	parentTreeNodeIndex parent node's index in treeview control
	parentNodeName      parent node's name
	objectName          nebula object name to insert tree 
    */
    function addChild parentTreeNodeIndex parentNodeName objectName = 
    (
        cmd = parentNodeName + ".getchildren"
        childs = nebula2.callv cmd
	
        -- we got child's node name in array 'childs'
        -- insert this to treeview
        for i in childs do
        (
            -- get name of nebula object
            -- returned object name is array type
            cmd = i + ".getname"
            objName = nebula2.callv cmd
	
            -- '4' : append node by child of this node
            o = tv.nodes.add parentTreeNodeIndex 4 "" objName[1]
			
            -- we add childs recursively
            addChild o.index i objName
        )	
    )
	
    on NOHTreeViewer open  do
    (
        nebula2.sel "/"
        root = tv.nodes.add()
        root.text = "/"
		
        r = nebula2.callv ".getchildren"
        addChild root.index root.text root.text
    )
)

rollpage = newRolloutFloater "Nebula Object Viewer" 355 540
addRollout NOHTreeViewer rollpage 
