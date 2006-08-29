-------------------------------------------------------------------------------
--  animoptions_ca.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  custom attributes for animation options.
--  executed in 'Animation Options' menu item of 'Nebula2' menu
--  'Custom Attributes' are shown in modifier panel
-------------------------------------------------------------------------------
nAnimOptionsCA = attributes "nAnimOptions"
(
    parameters AnimDirSetting rollout:rOptions
    (
        animDir type:#string default:"" ui:edtAnimDirSetting
    )

    rollout rOptions "Nebula2 Anim Options"
    (
        label lblAnimDirSetting "Dest Directory for a Animation : " align:#left
        edittext edtAnimDirSetting "" align:#left fieldWidth:100 across:2 readonly:true 
        button btnAnimDirSetting  "<<" align:#right width:20 height:18
        on btnAnimDirSetting pressed do
        (
            animPath = getSavePath caption:"Select a directory where the animation file to be exported."
            if animPath != undefined then 
            (
                edtAnimDirSetting.text = animPath
            )
        )
    )
)

