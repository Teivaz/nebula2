#-------------------------------------------------------------------------------
#   Defines the standard lighting for npyviewer.exe
#-------------------------------------------------------------------------------

stdLightTransform = new("ntransformnode", "/usr/scene/stdlight")
stdLight = new("nlightnode", "/usr/scene/stdlight/l")
stdLight.setposition(1000, 500, 1000)
stdLight.setvector("LightDiffuse",   1, 1, 1, 1)
stdLight.setvector("LightSpecular",  1, 1, 1, 1)
stdLight.setvector("LightAmbient",   1, 1, 1, 1)
stdLight.setvector("LightDiffuse1",  0.3, 0.3, 0.3, 1.0)
stdLight.setvector("LightSpecular1", 0.3, 0.3, 0.3, 1.0)

