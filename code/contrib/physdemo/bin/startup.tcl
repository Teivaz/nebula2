#-------------------------------------------------------------------------------
#   startup.tcl
#
#   This is the Elium startup script.  Generally sets path assigns, but may
#   do other things - just look yourself, it isn't that long, yeesh!
#
#   (C) 2004 MeganFox
#-------------------------------------------------------------------------------

set oldCwd [psel]

#-------------------------------------------------------------------------------
#   set up resource assigns
#-------------------------------------------------------------------------------
sel /sys/servers/file2
    set home [.manglepath "home:"]
    
    .setassign "data"	  "proj:data/"
    .setassign "meshes"   "proj:export/meshes/"
    .setassign "textures" "proj:export/textures/"
    .setassign "gfxlib"   "home:gfxlib/"
    .setassign "lights"   "proj:export/lightdefs/"
    .setassign "shapes"   "proj:export/shapedefs/"
    
    if {[exists /sys/servers/gfx]} {
        set featureSet [/sys/servers/gfx.getfeatureset]
        if {($featureSet =="dx9") || ($featureSet == "dx9flt")} {   
            .setassign "shaders" "data:shaders/2.0/"
            /sys/servers/scene.setrenderpathfilename "home:data/shaders/dx9hdr_renderpath.xml"
        } else {
            .setassign "shaders" "data:shaders/fixed/"
            /sys/servers/scene.setrenderpathfilename "home:data/shaders/dx7_renderpath.xml"
        }
    } else {
        .setassign "shaders" "data:shaders/2.0/"
        /sys/servers/scene.setrenderpathfilename "home:data/shaders/dx9_renderpath.xml")
    }

    # enable zFail shadow rendering
    if {[exists /sys/servers/shadow]} {
        /sys/servers/shadow.setusezfail true
    }
    

#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is opened.
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
    # set texture path pre- and postfix (NOTE: don't change path to textures:system!!!)
    .settextureprefix "home:export/textures/system/"
    .settexturepostfix ".dds"

    # active and inactive window modulation color
    .setactivewindowcolor 1.0 1.0 1.0 1.0
    .setinactivewindowcolor 0.6 0.6 0.6 0.6
    .setbuttontextcolor 0.0 0.0 0.0 1.0
    .settitletextcolor  0.0 0.0 0.0 1.0
    .setlabeltextcolor  0.0 0.0 0.0 1.0
    .setentrytextcolor  0.0 0.0 0.0 1.0
    .settextcolor       0.0 0.0 0.0 1.0
    .setmenutextcolor   0.0 0.0 0.0 1.0

    # define brushes
    .beginbrushes

    # window title bar, window background, tooltip background
    .addbrush titlebar skin  66 152 10 20 1.0 1.0 1.0 1.0
    .addbrush window   skin   8 154  4  4 1.0 1.0 1.0 1.0
    .addbrush tooltip  skin   8 154  4  4 1.0 1.0 0.878 0.8
    .addbrush pink     skin   8 154  4  4 1.0 0.0 1.0 1.0
    .addbrush dragbox  skin   8 154  4  4 1.0 0.8 0.8 0.5
    
    # text entry field
    .addbrush textentry_n skin 446 124 8 8 0.7 0.7 0.7 1.0
    .addbrush textentry_p skin 446 124 8 8 0.8 0.8 0.8 1.0
    .addbrush textentry_h skin 446 124 8 8 0.9 0.9 0.9 1.0
    .addbrush textcursor  skin 446 124 8 8 0.4 0.4 0.4 1.0
    
    # the window close button
    .addbrush close_n skin 388 40 16 16 1.0 1.0 1.0 1.0
    .addbrush close_h skin 404 40 16 16 1.0 1.0 1.0 1.0
    .addbrush close_p skin 420 40 16 16 1.0 1.0 1.0 1.0
    
    # the window size button
    .addbrush size_n skin 372 40 16 16 1.0 1.0 1.0 1.0
    .addbrush size_h skin 372 40 16 16 1.0 1.0 1.0 1.0
    .addbrush size_p skin 372 40 16 16 1.0 1.0 1.0 1.0
    
    # arrows
    .addbrush arrowleft_n  skin  68 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowleft_h  skin  84 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowleft_p  skin 100 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_n skin 116 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_h skin 132 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_p skin 148 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_n    skin 164 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_h    skin 180 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_p    skin 196 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_n  skin  20 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_h  skin  36 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_p  skin  52 40 16 16 1.0 1.0 1.0 1.0

    # sliders
    .addbrush sliderbg         skin 228  40 16 16 0.5 0.5 0.5 1.0
    .addbrush sliderknobhori_n skin 276  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobhori_h skin 292  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobhori_p skin 308  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_n skin 324  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_h skin 340  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_p skin 356  40 16 16 1.0 1.0 1.0 1.0
            
    # standard buttons
    .addbrush button_n skin 192 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_h skin 288 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_p skin 384 152 96 20 1.0 1.0 1.0 1.0
    
    .addbrush menu_n skin 192 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_h skin 288 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_p skin 384 172 96 16 1.0 1.0 1.0 1.0
        
    .addbrush button_64x16_n skin   0 0 64 16 1.0 1.0 1.0 1.0
    .addbrush button_64x16_h skin  64 0 64 16 1.0 1.0 1.0 1.0
    .addbrush button_64x16_p skin 128 0 64 16 1.0 1.0 1.0 1.0

    .addbrush button_96x16_n skin 192 0 96 16 1.0 1.0 1.0 1.0
    .addbrush button_96x16_h skin 288 0 96 16 1.0 1.0 1.0 1.0
    .addbrush button_96x16_p skin 384 0 96 16 1.0 1.0 1.0 1.0

    .addbrush button_64x24_n skin   0 16 64 24 1.0 1.0 1.0 1.0
    .addbrush button_64x24_h skin  64 16 64 24 1.0 1.0 1.0 1.0
    .addbrush button_64x24_p skin 128 16 64 24 1.0 1.0 1.0 1.0

    .addbrush button_96x24_n skin 192 16 96 24 1.0 1.0 1.0 1.0
    .addbrush button_96x24_h skin 288 16 96 24 1.0 1.0 1.0 1.0
    .addbrush button_96x24_p skin 384 16 96 24 1.0 1.0 1.0 1.0

    .addbrush button_64x20_n skin 192 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_64x20_h skin 288 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_64x20_p skin 384 152 96 20 1.0 1.0 1.0 1.0

    .addbrush menu_64x16_n skin   0 172 64 16 1.0 1.0 1.0 1.0
    .addbrush menu_64x16_h skin  64 172 64 16 1.0 1.0 1.0 1.0
    .addbrush menu_64x16_p skin 128 172 64 16 1.0 1.0 1.0 1.0

    .addbrush menu_128x16_n skin 192 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_128x16_h skin 288 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_128x16_p skin 384 172 96 16 1.0 1.0 1.0 1.0

    # list views
    .addbrush list_background skin 446  72 8 8 1.0 1.0 1.0 1.0
    .addbrush list_selection  skin  64 172 64 16 1.0 1.0 1.0 1.0
 
    # icons
    .addbrush console_n skin     0  56 48 48 1.0 1.0 1.0 1.0
    .addbrush console_p skin     0  56 48 48 0.5 0.5 0.5 1.0
    .addbrush console_h skin     0 104 48 48 1.0 1.0 1.0 1.0

    .addbrush texbrowser_n skin  48  56 48 48 1.0 1.0 1.0 1.0
    .addbrush texbrowser_p skin  48  56 48 48 0.5 0.5 0.5 1.0
    .addbrush texbrowser_h skin  48 104 48 48 1.0 1.0 1.0 1.0

    .addbrush gfxbrowser_n skin  96  56 48 48 1.0 1.0 1.0 1.0
    .addbrush gfxbrowser_p skin  96  56 48 48 0.5 0.5 0.5 1.0
    .addbrush gfxbrowser_h skin  96 104 48 48 1.0 1.0 1.0 1.0

    .addbrush dbgwindow_n skin  144  56 48 48 1.0 1.0 1.0 1.0
    .addbrush dbgwindow_p skin  144  56 48 48 0.5 0.5 0.5 1.0
    .addbrush dbgwindow_h skin  144 104 48 48 1.0 1.0 1.0 1.0

    .addbrush syswindow_n skin  192  56 48 48 1.0 1.0 1.0 1.0
    .addbrush syswindow_p skin  192  56 48 48 0.5 0.5 0.5 1.0
    .addbrush syswindow_h skin  192 104 48 48 1.0 1.0 1.0 1.0

    .addbrush hidegui_n skin    240  56 48 48 1.0 1.0 1.0 1.0
    .addbrush hidegui_p skin    240  56 48 48 0.5 0.5 0.5 1.0
    .addbrush hidegui_h skin    240 104 48 48 1.0 1.0 1.0 1.0

    .addbrush quit_n skin       288  56 48 48 1.0 1.0 1.0 1.0
    .addbrush quit_p skin       288  56 48 48 0.5 0.5 0.5 1.0
    .addbrush quit_h skin       288 104 48 48 1.0 1.0 1.0 1.0

    .addbrush hardpoint_n skin       0  188 48 48 1.0 1.0 1.0 1.0
    .addbrush hardpoint_p skin       0  188 48 48 0.5 0.5 0.5 1.0
    .addbrush hardpoint_h skin       48 188 48 48 1.0 1.0 1.0 1.0

    .addbrush disp_n skin       384  56 48 48 1.0 1.0 1.0 1.0
    .addbrush disp_p skin       384  56 48 48 0.5 0.5 0.5 1.0
    .addbrush disp_h skin       384 104 48 48 1.0 1.0 1.0 1.0

    # the left and right logos
    .addbrush n2logo n2logo 0 0 64 64 1.0 1.0 1.0 0.5

    .endbrushes

    /sys/servers/gui.setsystemskin $skin
    /sys/servers/gui.setskin $skin
    
    # create the Nebula dock window
    /sys/servers/gui.newwindow nguidockwindow true

    sel $cwd
}

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is closed.
#-------------------------------------------------------------------------------
proc OnGuiServerClose {} {

}
