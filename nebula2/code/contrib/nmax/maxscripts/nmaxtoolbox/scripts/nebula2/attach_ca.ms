-------------------------------------------------------------------------------
--  attach_ca.ms
--
--  Add/Remove custom attributes for a joint attachment of a node.
--
--  (C)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- custom attributes for attchment node.
-------------------------------------------------------------------------------
nAttachCA = attributes "nAttach"
(
    parameters AttachParam rollout:rAttach
    (
        --attachmentnode type:#boolean default:true align:#left ui:chkAttachment
        attachmentjoint type:#string align:#left ui:lblJointName
    )

    rollout rAttach "Nebula2 Attachment Node"
    (
        --checkbox chkAttachment "Attachment Node" checked:true enabled:false

        group "Joint attchment"
        (
            button btnSelectJoint "Select Joint" align:#left \
                   tooltip:"Select any bone node for the joint of the attachment node"

            label lblTitle "Selected Joint:" align:#left
            label lblJointName "None" align:#left offset:[10,0]
        )
		
        on btnSelectJoint pressed do
        (
            -- filter function which is needed to show all geometries in 
            -- selection dialog even including biped etc.
            fn shape_filt obj = isKindOf obj GeometryClass
			
            selectedJoint = selectByName filter:shape_filt \
                                         title:"Select a bone node for a attachment node" \
                                         showHidden:true \ 
                                         single:true
 
            --FIXME: check the type of the 'selectedJoint' which is any bone node or not.
            
            if selectedJoint != undefined do
            (
                lblJointName.text = selectedJoint.name
                attachmentjoint = selectedJoint.name
            )
        )		
    )
)
-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
