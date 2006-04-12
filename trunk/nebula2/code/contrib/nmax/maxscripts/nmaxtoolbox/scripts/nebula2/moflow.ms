-------------------------------------------------------------------------------
--  moflow.ms
--
--  Motion Flow utility function library
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  struct for animation state.
-------------------------------------------------------------------------------
struct animState 
(
    stateName,
    startFrame,
    fadeinTime,
    duration,
    repeat,
    clipArray
)

-------------------------------------------------------------------------------
--  Retrieves biped root node from all objects in the given scene.
--
-------------------------------------------------------------------------------
fn getBipedRootNode = 
(
    for o in objects where classof o == Biped_Object do
    (
        return o.controller.RootNode 
    )    
    -- no biped root node in the scene.
    return undefined
)

-------------------------------------------------------------------------------
--  Return animation state array which is created based on clips in motion flow
--  
--  -12-Apr-06  kims  fixed to extract exact start-frame and duraion 
-------------------------------------------------------------------------------
fn createAnimStatesFromMotionFlow = 
(
    -- retrieves root biped object
    bipedRootNode = getBipedRootNode()
    if bipedRootNode == undefined then
        return #()
    
    -- retrieves motion flow object from controller of the root biped node.
    moFlow = bipedRootNode.controller.motionFlow
    if moFlow == undefined then
        return #()
    
    -- retrieves current active motion flow script.
    activeScript = moFlow.activeScript

    -- retrieves all snippets (clip in motion flow)		
    moFlowSnippets = moFlow.snippets

    numSnippets = moFlowSnippets.count

    -- array which to contain created animation states
    collectedAnimStates = #()

    count = numSnippets - 1   

    index = numSnippets	
	
    -- animation length accumulator 
    -- which is used for start frame of next animation state
    lengthAccum = 0
	
    -- previous destination start frame 
    -- which is start frame of this source snippet
    prevDestStart = 0
	
    for i = 1 to count do
    (
        -- snippets in moFlowSnippet are reversed order.
        snippetFrom = moFlowSnippets[index]
        snippetTo   = moFlowSnippets[index - 1]
		
        index -= 1
        
        thisTrans = undefined
        for trans in snippetFrom.transitions where (trans.toSnippet == snippetTo) do
        (
            thisTrans = trans
            break
        )
        
        whichTrans = activeScript.tranIndices[1]						
        ti = thisTrans.tranInfos[whichTrans]
				
        numAnimStates = collectedAnimStates.count
	local prevAnim
        if (numAnimStates > 1) then
            prevAnim = collectedAnimStates[numAnimStates-1]
		
        -- create animation state struct
        anim = animState()
        anim.stateName = snippetFrom.clipName
		
        if (numAnimStates > 0) then
            anim.startFrame = lengthAccum + 1
	else
            anim.startFrame = 0

        anim.fadeinTime = 0.0
        -- not always the first clip start from 0
        anim.duration = ti.sourceStart - snippetFrom.start - prevDestStart + 1 
        anim.repeat = 1
        anim.clipArray = #()
		       
        -- append created animation state       
        append collectedAnimStates anim
		
        prevDestStart = ti.destStart
        lengthAccum += anim.duration
    )
    
    return collectedAnimStates
)

