#============================================================================
#  Helper functions
#
#  This file contains additional subroutines used by more than one other file.
#
#  Copyright (c) 2003 Rafael Van Daele-Hunt
#
#  This file is licensed under the terms of the Nebula License
#============================================================================
#--------------------------------------------------------------------
#   getfilenamewithextension $filename $defaultext
#   If filename has no extension, we append .$defaultext
#--------------------------------------------------------------------
proc getfilenamewithextension {filename defaultext} {
  if {[string last . $filename] <= [string last / $filename]} {
    # There is no . following the last /, so any .s indicate directories rather than prefacing an extension.  We add the default extension:
    set filename $filename.$defaultext
  }
  return $filename
}

proc writeerr {text} {
  global home
  set cid [open [cleanpath $home/buildErrors.log] a]
  puts $cid $text
  close $cid
}
#----------------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------------
