-------------------------------------------------------------------------------
--  attach_ca.ms
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
        attachmentnode type:#boolean default:true align:#left ui:chkAttachment
    )

    rollout rAttach "Nebula2 Attachment Node"
    (
        checkbox chkAttachment "Attachment Node" checked:true enabled:false
    )
)
