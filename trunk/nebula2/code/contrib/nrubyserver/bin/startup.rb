#-------------------------------------------------------------------------------
#   bin/startup.tcl
#
#   This is the central Nebula runtime startup script which is 
#   used by various tools, like the Maya plugin or the viewer.
#
#   The script mainly sets up assigns and Nebula variables.
#
#   (C) 2003 RadonLabs GmbH
#-------------------------------------------------------------------------------
include Nebula

oldCwd = psel

#-------------------------------------------------------------------------------
#   set up resource assigns
#-------------------------------------------------------------------------------
sel '/sys/servers/file2'
    home = manglepath "home:"
    proj = manglepath "proj:"
    
    setassign "meshes",   "proj:export/meshes/"
    setassign "textures", "proj:export/textures/"
 #   if ('/sys/servers/gfx'.exists)
 #       if ('/sys/servers/gfx'.getfeatureset == "dx9")
 #           setassign "shaders", "#{home}/export/shaders/2.0/"
 #           puts "Shader directory: #{home}/export/shaders/2.0"
 #       else
 
	 # I changed this as my own card does not support the full dx9 feature set :-(
            setassign "shaders", "proj:export/shaders/fixed/"
            puts "Shader directory: #{proj}/export/shaders/fixed"
 #       end
 #   else
 #       setassign "shaders", "#{home}/export/shaders/2.0/"
 #       puts "Shader directory: #{home}/export/shaders/2.0"
 #  end
    setassign "anims",    "proj:export/anims/"
    setassign "gfxlib",   "proj:export/gfxlib/"

#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel oldCwd
   
