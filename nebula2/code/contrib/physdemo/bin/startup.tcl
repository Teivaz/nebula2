#-------------------------------------------------------------------------------
#   startup.tcl
#
#   This is the Elium startup scripted.  Generally sets path assigns, but may
#   do other things - just look yourself, it isn't that long, yeesh!
#
#   (C) 2004 MeganFox
#-------------------------------------------------------------------------------

set oldCwd [psel]

#-------------------------------------------------------------------------------
#   set up resource assigns
#-------------------------------------------------------------------------------
sel /sys/servers/file2
    set proj [.manglepath "proj:"]
    set home [.manglepath "home:"]
    
    .setassign "data" "home:export/"
    .setassign "nebula2" "home:../../../"
    
    if {[exists /sys/servers/gfx]} {
        set featureset [/sys/servers/gfx.getfeatureset]
        if {($featureset == "dx9") || ($featureset == "dx9flt")} {   
            .setassign "shaders" "nebula2:export/shaders/2.0/"
        } else {
            .setassign "shaders" "nebula2:export/shaders/fixed/"
        }
    } else {
        .setassign "shaders" "nebula2:export/shaders/2.0/"
        puts "Shader directory: [.getassign shaders]"
    }
    .setassign "meshes"   "data:meshes/"
    .setassign "textures" "data:textures/"
    .setassign "anims"    "data:anims/"
    .setassign "shapes"   "data:shapedefs/"
    .setassign "lights"   "data:lightdefs/"
    .setassign "gui"      "$proj/export/gui/"

#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is opened
#-------------------------------------------------------------------------------
proc OnGuiServerOpen {} {

    set cwd [psel]

    # initialize the default tooltip
    set guiRoot [/sys/servers/gui.getrootpath]
    sel $guiRoot
    new nguitooltip Tooltip
        sel Tooltip
        .setdefaultbrush "tooltip"
        .setfont "GuiSmall"
        .setcolor 0 0 0 1 
        .setalignment "left"
        .setborder 0.005 0.005
    sel ..
        
    # define the system skin
    set skin [/sys/servers/gui.newskin system]
    sel $skin
            
        # set texture path pre- and postfix
        .settextureprefix "nebula2:export/textures/system/"
        .settexturepostfix ".dds"
        
        # active and inactive window modulation color
        .setactivewindowcolor 1.0 1.0 1.0 0.9
        .setinactivewindowcolor 0.6 0.6 0.6 0.6
    
        # define brushes  
        .beginbrushes

        # window title bar, window background, tooltip background
        .addbrush titlebar skin  0 52 43 20 1.0 1.0 1.0 1.0
        .addbrush window   skin  0 77 15 13 1.0 1.0 1.0 1.0
        .addbrush tooltip  skin  0 77 15 13 1.0 1.0 1.0 1.0
        .addbrush pink     skin  0 97 14 14 1.0 1.0 1.0 1.0

        # text entry field
        .addbrush textentry_n skin 0 52 43 20 0.7 0.7 0.7 1.0
        .addbrush textentry_p skin 0 52 43 20 0.7 0.7 1.0 1.0
        .addbrush textentry_h skin 0 52 43 20 0.9 0.9 1.0 1.0
        .addbrush textcursor skin 0 97 14 14  1.0 1.0 1.0 1.0

        # the window close button
        .addbrush close_n skin  0 29 16 17 1.0 1.0 1.0 1.0
        .addbrush close_h skin 22 29 16 17 1.3 1.3 1.3 1.0
        .addbrush close_p skin 43 28 16 17 1.0 1.0 1.0 1.0

        # the window size button
        .addbrush size_n skin 352 136 16 16 1.0 1.0 1.0 1.0
        .addbrush size_h skin 352 136 16 16 1.3 1.3 1.3 1.0
        .addbrush size_p skin 352 136 16 16 1.2 1.2 1.2 1.0

        # arrows
        .addbrush arrowleft_n  skin   0 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowleft_h  skin  16 136 16 16 1.3 1.3 1.3 1.0
        .addbrush arrowleft_p  skin  32 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowright_n skin  64 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowright_h skin  80 136 16 16 1.3 1.3 1.3 1.0
        .addbrush arrowright_p skin  96 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowup_n    skin 128 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowup_h    skin 144 136 16 16 1.3 1.3 1.3 1.0
        .addbrush arrowup_p    skin 160 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowdown_n  skin 192 136 16 16 1.0 1.0 1.0 1.0
        .addbrush arrowdown_h  skin 208 136 16 16 1.3 1.3 1.3 1.0
        .addbrush arrowdown_p  skin 224 136 16 16 1.0 1.0 1.0 1.0

        # sliders
        .addbrush sliderbg     skin   0  77 15 13 0.8 0.8 0.8 1.0
        .addbrush sliderknobhori_n skin 256 136 16 16 1.0 1.0 1.0 1.0
        .addbrush sliderknobhori_p skin 272 136 16 16 1.0 1.0 1.0 1.0
        .addbrush sliderknobhori_h skin 288 136 16 16 1.3 1.3 1.3 1.0
        .addbrush sliderknobvert_n skin 304 136 16 16 1.0 1.0 1.0 1.0
        .addbrush sliderknobvert_p skin 320 136 16 16 1.0 1.0 1.0 1.0
        .addbrush sliderknobvert_h skin 336 136 16 16 1.3 1.3 1.3 1.0

        # standard buttons
        .addbrush button_n skin 226 0 73 24 1.0 1.0 1.0 1.0
        .addbrush button_p skin  76 0 73 24 1.0 1.0 1.0 1.0
        .addbrush button_h skin   1 0 73 24 1.3 1.3 1.3 1.0

        # icons
        .addbrush terminal_n skin  49 52 32 32 1.0 1.0 1.0 1.0
        .addbrush terminal_p skin  48 51 32 32 1.0 1.0 1.0 1.0
        .addbrush terminal_h skin  49 52 32 32 1.3 1.3 1.3 1.0

        .addbrush quit_n     skin 122 52 32 32 1.0 1.0 1.0 1.0
        .addbrush quit_p     skin 121 51 32 32 1.0 1.0 1.0 1.0
        .addbrush quit_h     skin 122 52 32 32 1.3 1.3 1.3 1.0

        .addbrush computer_n skin 198 52 32 32 1.0 1.0 1.0 1.0
        .addbrush computer_p skin 197 51 32 32 1.0 1.0 1.0 1.0
        .addbrush computer_h skin 198 52 32 32 1.3 1.3 1.3 1.0

        .addbrush info_n     skin 277 52 32 32 1.0 1.0 1.0 1.0
        .addbrush info_p     skin 276 51 32 32 1.0 1.0 1.0 1.0
        .addbrush info_h     skin 277 52 32 32 1.3 1.3 1.3 1.0

        .addbrush settings_n skin  49 91 32 32 1.0 1.0 1.0 1.0
        .addbrush settings_p skin  48 90 32 32 1.0 1.0 1.0 1.0
        .addbrush settings_h skin  49 91 32 32 1.3 1.3 1.3 1.0

        .addbrush desktop_n  skin 122 91 32 32 1.0 1.0 1.0 1.0
        .addbrush desktop_p  skin 121 90 32 32 1.0 1.0 1.0 1.0
        .addbrush desktop_h  skin 122 91 32 32 1.3 1.3 1.3 1.0

        .addbrush eject_n    skin 198 91 32 32 1.0 1.0 1.0 1.0
        .addbrush eject_p    skin 197 90 32 32 1.0 1.0 1.0 1.0
        .addbrush eject_h    skin 198 91 32 32 1.3 1.3 1.3 1.0

        .addbrush n2logo n2logo 0 0 64 64 1.0 1.0 1.0 0.5

        .endbrushes

    /sys/servers/gui.setsystemskin $skin
    /sys/servers/gui.setskin $skin

    /sys/servers/gui.newwindow nguidockwindow true

    sel $cwd
}

#-------------------------------------------------------------------------------
#   This proceducre is called when the gui server is closed
#-------------------------------------------------------------------------------
proc OnGuiServerClose {} {
    # Empty
}
   
