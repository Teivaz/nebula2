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
        animDir type:#string default:"" animatable:true ui:edtAnimDirSetting
    )

    rollout rOptions "Nebula2 Anim Options"
    (
        label lblAnimDirSetting "Dest Directory for a Animation : " align:#left
        edittext edtAnimDirSetting "" align:#left fieldWidth:100 across:2 
        button btnAnimDirSetting  "<<" align:#right width:20 height:18

        on btnAnimDirSetting pressed do
        (
            include "nebula2/utilz.ms"

            projDir = nGetProjDir();
            animAssign = nebula2.call "/sys/servers/file2.manglepath" "anims:"
            animAssign = nReplaceSlash animAssign[1]

            animPath = getSavePath initialDir:animAssign caption:"Select a directory where the animation file to be exported."
            if animPath != undefined then 
            (
                animPath = nReplaceSlash animPath
                findIndex = findString animPath animAssign
                if findIndex != undefined  then 
                (
                    subDir = substring animPath (animAssign.count + 2) animPath.count
                    exportAnimDir = "anims:" + subDir
                    edtAnimDirSetting.text = exportAnimDir
                )
                else
                (
                    format "(%) is not subdirectory of (%)\n" animPath animAssign
                )
            )
        )

        on edtAnimDirSetting entered newval do
        (
            include "nebula2/utilz.ms"

            projDir = nGetProjDir();
            -- NOTE: nebula2.call returns array type
            animAssign = nebula2.call "/sys/servers/file2.manglepath" "anims:"
            animAssign = nReplaceSlash animAssign[1]

            animPath = nebula2.call "/sys/servers/file2.manglepath" newval
            animPath = animPath[1]
            findIndex = findString animPath animAssign
            if findIndex != undefined  then 
            (
                subDir = substring animPath (animAssign.count + 2) animPath.count
                exportAnimDir = "anims:" + subDir
                edtAnimDirSetting.text = exportAnimDir
            )
            else
            (
                format "(%) is not subdirectory of (%)\n" animPath animAssign
                exportAnimDir = ""
                edtAnimDirSetting.text = ""
            )
        )
    )
)


