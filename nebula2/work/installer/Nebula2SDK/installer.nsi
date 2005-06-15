;-------------------------------------------------------------------------------
;
;   Nullsoft Installer script for "Nebula2 SDK"
;
;   (C) 2004 RadonLabs GmbH
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
;   Include Modern UI

    !include "MUI.nsh"

;-------------------------------------------------------------------------------
;   Defines
    
    !define COMPANY "RadonLabs"
    !define PRODUCT "Nebula2 SDK"
    !define SRCDIR "..\..\.."
    !define STARTMENU "$SMPROGRAMS\${PRODUCT}"

;-------------------------------------------------------------------------------
;   General

    Name "${PRODUCT}"
    OutFile "${SRCDIR}\Nebula2SDK_${__DATE__}.exe"
    XPStyle on
    CrcCheck off
    
    ; default installation directory
    InstallDir "$PROGRAMFILES\${PRODUCT}"

    ; Get installation folder from registry if available
    InstallDirRegKey HKLM "Software\${COMPANY}\${PRODUCT}" "installdir"
       
;-------------------------------------------------------------------------------
;   Interface Settings

    !define MUI_ABORTWARNING
    !define MUI_ICON "install.ico"
    !define MUI_UNICON "uninstall.ico"
    !define MUI_HEADERIMAGE
    !define MUI_HEADERIMAGE_BITMAP "header.bmp"
    !define MUI_HEADERIMAGE_UNBITMAP "header.bmp"
    !define MUI_WELCOMEFINISHPAGE_BITMAP "wizard.bmp"
    !define MUI_UNWELCOMEFINISHPAGE_BITMAP "wizard.bmp"
    !define MUI_INSTFILESPAGE_PROGRESSBAR "smooth"

;--------------------------------
;   Pages

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH
  
    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH
    
;--------------------------------
;   Languages
 
    !insertmacro MUI_LANGUAGE "English"
    
;-------------------------------------------------------------------------------
;   Installer Sections     

Section "-Runtime" Section_RuntimeFiles

    ; root directory
    SetOutPath "$INSTDIR"
    
    ; bin directory
    SetOutPath "$INSTDIR\bin"
    File "${SRCDIR}\bin\*.tcl"
    File "${SRCDIR}\bin\*.txt"
    
    SetOutPath "$INSTDIR\bin\win32"
    File "${SRCDIR}\bin\win32\*.ico"
    File "${SRCDIR}\bin\win32\nviewer.exe"
    File "${SRCDIR}\bin\win32\nsh.exe"
    File "${SRCDIR}\bin\win32\nremoteshell.exe"
    File "${SRCDIR}\bin\win32\nnpktool.exe"
    File "${SRCDIR}\bin\win32\NewProjectWizard.exe"

    ; export directory    
    SetOutPath "$INSTDIR\data\shaders"
    File "${SRCDIR}\data\shaders\*.xml"

    SetOutPath "$INSTDIR\data\shaders\2.0"
    File "${SRCDIR}\data\shaders\2.0\*.fx"
    File "${SRCDIR}\data\shaders\2.0\*.xml"

    SetOutPath "$INSTDIR\data\shaders\fixed"
    File "${SRCDIR}\data\shaders\fixed\*.fx"
    File "${SRCDIR}\data\shaders\fixed\*.xml"
    SetOutPath "$INSTDIR\data\shaders\lib"

    File "${SRCDIR}\data\shaders\lib\*.fx"

    SetOutPath "$INSTDIR\export\textures\system"
    File "${SRCDIR}\export\textures\system\*.dds" 
        
; only change start menu/registry stuff if no silent install (silent installs are extract-only) 
;   create the start-menu items
IfSilent endOfRuntimeSection

    SetOutPath "$INSTDIR\bin\win32"

    ; Write file associations
    WriteRegStr HKCR ".n2" "" "Nebula2.Object"
    WriteRegStr HKCR ".nvx2" "" "Nebula2.BinMesh"
    WriteRegStr HKCR ".n3d2" "" "Nebula2.AsciiMesh"
    WriteRegStr HKCR ".nax2" "" "Nebula2.BinAnim"
    WriteRegStr HKCR ".nanim2" "" "Nebula2.AsciiAnim"

    WriteRegStr HKCR "Nebula2.Object" "" "Nebula2 Object File"
    WriteRegStr HKCR "Nebula2.Object\DefaultIcon" "" "$INSTDIR\bin\win32\nebula.ico"
    WriteRegStr HKCR "Nebula2.Object\shell\open\command" "" '$INSTDIR\bin\win32\nviewer.exe -view "%1"'

    WriteRegStr HKCR "Nebula2.BinMesh" "" "Nebula2 Binary Mesh File"
    WriteRegStr HKCR "Nebula2.BinMesh\DefaultIcon" "" "$INSTDIR\bin\win32\nebula.ico"

    WriteRegStr HKCR "Nebula2.AsciiMesh" "" "Nebula2 Ascii Mesh File"
    WriteRegStr HKCR "Nebula2.AsciiMesh\DefaultIcon" "" "$INSTDIR\bin\win32\nebula.ico"

    WriteRegStr HKCR "Nebula2.BinAnim" "" "Nebula2 Binary Animation File"
    WriteRegStr HKCR "Nebula2.BinAnim\DefaultIcon" "" "$INSTDIR\bin\win32\nebula.ico"

    WriteRegStr HKCR "Nebula2.AsciiAnim" "" "Nebula2 Ascii Animation File"
    WriteRegStr HKCR "Nebula2.AsciiAnim\DefaultIcon" "" "$INSTDIR\bin\win32\nebula.ico"    

;   write registry keys, do not overwrite previous project setting
    Push $0
    WriteRegStr HKLM "SOFTWARE\${COMPANY}\${PRODUCT}" "installdir" "$INSTDIR"

;   write uninstall information to the registry    
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT} (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayIcon" "$INSTDIR\bin\win32\nebula.ico,0"

    WriteUninstaller "$INSTDIR\Uninstall.exe"

endOfRuntimeSection:
SectionEnd

Section "Documentation" Section_Documentation

    SetOutPath "$INSTDIR\doc"
    File /nonfatal "${SRCDIR}\doc\nebula2api\html\nebula2api.chm"
    File /nonfatal "${SRCDIR}\doc\sdk\html\nebula2sdk.chm"

    SetOutPath "$INSTDIR\code\doxygen"
    File "${SRCDIR}\code\doxygen\*.html"
    File "${SRCDIR}\code\doxygen\*.cfg"
    File "${SRCDIR}\code\doxygen\*.css"
    File "${SRCDIR}\code\doxygen\*.txt"

SectionEnd

Section "Doxygen Sources" Section_Doxygen

    SetOutPath "$INSTDIR\doc\nebula2api"
    File "${SRCDIR}\doc\nebula2api\*.dox"
    SetOutPath "$INSTDIR\doc\sdk"
    File "${SRCDIR}\doc\sdk\*.dox"

SectionEnd

Section "VStudio.NET Project Files" Section_VStudioFiles

    SetOutPath "$INSTDIR\code\nebula2\vstudio"
    File "${SRCDIR}\code\nebula2\vstudio\*.vcproj"
    File "${SRCDIR}\code\nebula2\vstudio\*.sln"

SectionEnd

Section "Examples" Section_ExampleFiles
    ; examples
    ; anims
    SetOutPath "$INSTDIR\export\anims\examples"
    File /nonfatal "${SRCDIR}\export\anims\examples\*.nax2"
    
    ; gfxlib
    SetOutPath "$INSTDIR\export\gfxlib\examples"
    File /nonfatal "${SRCDIR}\export\gfxlib\examples\*.n2"
    
    ; meshes
    SetOutPath "$INSTDIR\export\meshes\examples"
    File /nonfatal "${SRCDIR}\export\meshes\examples\*.nvx2"
    
    ; textures
    !define TEXDST "$INSTDIR\export\textures"
    !define TEXSRC "${SRCDIR}\export\textures"

    SetOutPath "${TEXDST}\buildings"
    File /nonfatal "${TEXSRC}\buildings\*.dds"
    SetOutPath "${TEXDST}\charaktere"
    File /nonfatal "${TEXSRC}\charaktere\*.dds"
    SetOutPath "${TEXDST}\environment"
    File /nonfatal "${TEXSRC}\environment\*.dds"    
    SetOutPath "${TEXDST}\examples"
    File /nonfatal "${TEXSRC}\examples\*.dds"
    SetOutPath "${TEXDST}\lightmaps"
    File /nonfatal "${TEXSRC}\lightmaps\*.dds"
    SetOutPath "${TEXDST}\materials"
    File /nonfatal "${TEXSRC}\materials\*.dds"

    ; "work" directory structure
    SetOutPath "$INSTDIR\work"
    File "${SRCDIR}\work\readme.txt"
    SetOutPath "$INSTDIR\work\gfxlib"
    File "${SRCDIR}\work\gfxlib\readme.txt"
    SetOutPath "$INSTDIR\work\textures"
    File "${SRCDIR}\work\textures\readme.txt"
        


SectionEnd

Section "Source Code" Section_SourceCode

    !define INC "${SRCDIR}\code\nebula2\inc"
    !define SRC "${SRCDIR}\code\nebula2\src"
    !define DSTINC "$INSTDIR\code\nebula2\inc"
    !define DSTSRC "$INSTDIR\code\nebula2\src"
    
    ; inc directory
    SetOutPath "${DSTINC}"
    File "${INC}\*.h"
    SetOutPath "${DSTINC}\anim2"
    File "${INC}\anim2\*.h"
    SetOutPath "${DSTINC}\application"
    File "${INC}\application\*.h"    
    SetOutPath "${DSTINC}\audio3"
    File "${INC}\audio3\*.h"
    SetOutPath "${DSTINC}\character"
    File "${INC}\character\*.h"
    SetOutPath "${DSTINC}\file"
    File "${INC}\file\*.h"
    SetOutPath "${DSTINC}\gfx2"
    File "${INC}\gfx2\*.h"    
    SetOutPath "${DSTINC}\gui"
    File "${INC}\gui\*.h"
    SetOutPath "${DSTINC}\il"
    File "${INC}\il\*.h"
    SetOutPath "${DSTINC}\input"
    File "${INC}\input\*.h"
    SetOutPath "${DSTINC}\kernel"
    File "${INC}\kernel\*.h"
    SetOutPath "${DSTINC}\locale"
    File "${INC}\locale\*.h"
    SetOutPath "${DSTINC}\mathlib"
    File "${INC}\mathlib\*.h"
    SetOutPath "${DSTINC}\misc"
    File "${INC}\misc\*.h"
    SetOutPath "${DSTINC}\nature"
    File "${INC}\nature\*.h"
    SetOutPath "${DSTINC}\ncterrain2"
    File "${INC}\ncterrain2\*.h"
    SetOutPath "${DSTINC}\network"
    File "${INC}\network\*.h"
    SetOutPath "${DSTINC}\particle"
    File "${INC}\particle\*.h"
    SetOutPath "${DSTINC}\resource"
    File "${INC}\resource\*.h"
    SetOutPath "${DSTINC}\scene"
    File "${INC}\scene\*.h"
    SetOutPath "${DSTINC}\script"
    File "${INC}\script\*.h"
    SetOutPath "${DSTINC}\shadow"
    File "${INC}\shadow\*.h"
    SetOutPath "${DSTINC}\tinyxml"
    File "${INC}\tinyxml\*.h"
    File "${INC}\tinyxml\*.txt"
    SetOutPath "${DSTINC}\tools"
    File "${INC}\tools\*.h"
    SetOutPath "${DSTINC}\util"
    File "${INC}\util\*.h"
    SetOutPath "${DSTINC}\variable"
    File "${INC}\variable\*.h"
    SetOutPath "${DSTINC}\video"
    File "${INC}\video\*.h"
    SetOutPath "${DSTINC}\xml"
    File "${INC}\xml\*.h"
    ; src directory
    SetOutPath "${DSTSRC}"
    File "${SRC}\dummy.cc"
    File "${SRC}\microtcl.epk"
    File "${SRC}\napplication.epk"
    File "${SRC}\ncterrain2.epk"
    File "${SRC}\ndinput8.epk"
    File "${SRC}\ndirect3d9.epk"
    File "${SRC}\ndshow.epk"
    File "${SRC}\ndsound.epk"
    File "${SRC}\nebulalib.epk"
    File "${SRC}\ngui.epk"
    File "${SRC}\nkernel.epk"
    File "${SRC}\nlocale.epk"
    File "${SRC}\nnebula.epk"
    File "${SRC}\nnetwork.epk"
    File "${SRC}\ntoollib.epk"
    File "${SRC}\tools.epk"
    File "${SRC}\util.epk"
    File "${SRC}\xml.epk"
    File "${SRC}\update.tcl"
    
    SetOutPath "${DSTSRC}\anim2"
    File "${SRC}\anim2\*.cc"
    SetOutPath "${DSTSRC}\application"
    File "${SRC}\application\*.cc"
    SetOutPath "${DSTSRC}\audio3"
    File "${SRC}\audio3\*.cc"
    SetOutPath "${DSTSRC}\character"
    File "${SRC}\character\*.cc"
    SetOutPath "${DSTSRC}\file"
    File "${SRC}\file\*.cc"
    SetOutPath "${DSTSRC}\gfx2"
    File "${SRC}\gfx2\*.cc"
    SetOutPath "${DSTSRC}\gui"
    File "${SRC}\gui\*.cc"
    SetOutPath "${DSTSRC}\input"
    File "${SRC}\input\*.cc"
    SetOutPath "${DSTSRC}\kernel"
    File "${SRC}\kernel\*.cc"
    SetOutPath "${DSTSRC}\locale"
    File "${SRC}\locale\*.cc"
    SetOutPath "${DSTSRC}\microtcl"
    File "${SRC}\microtcl\*.cc"
    File "${SRC}\microtcl\*.h"
    SetOutPath "${DSTSRC}\misc"
    File "${SRC}\misc\*.cc"
    SetOutPath "${DSTSRC}\nature"
    File "${SRC}\nature\*.cc"
    SetOutPath "${DSTSRC}\ncterrain2"
    File "${SRC}\ncterrain2\*.cc"
    SetOutPath "${DSTSRC}\network"
    File "${SRC}\network\*.cc"
    SetOutPath "${DSTSRC}\packages"
    File "${SRC}\packages\*.txt"
    File "${SRC}\packages\*.cc"
    SetOutPath "${DSTSRC}\particle"
    File "${SRC}\particle\*.cc"    
    SetOutPath "${DSTSRC}\resource"
    File "${SRC}\resource\*.cc"
    SetOutPath "${DSTSRC}\scene"
    File "${SRC}\scene\*.cc"
    SetOutPath "${DSTSRC}\script"
    File "${SRC}\script\*.cc"
    SetOutPath "${DSTSRC}\shadow"
    File "${SRC}\shadow\*.cc"
    SetOutPath "${DSTSRC}\tinyxml"
    File "${SRC}\tinyxml\*.cc"
    File "${SRC}\tinyxml\*.txt"
    SetOutPath "${DSTSRC}\tools"
    File "${SRC}\tools\*.cc"
    SetOutPath "${DSTSRC}\util"
    File "${SRC}\util\*.cc"    
    SetOutPath "${DSTSRC}\variable"
    File "${SRC}\variable\*.cc"
    SetOutPath "${DSTSRC}\video"
    File "${SRC}\video\*.cc"
    SetOutPath "${DSTSRC}\xml"
    File "${SRC}\xml\*.cc"

    SetOutPath "$INSTDIR\code\nebula2"
    File "${SRCDIR}\code\nebula2\readme.txt"

    SetOutPath "$INSTDIR\code\nebula2\vstudio"
    File "${SRCDIR}\code\nebula2\vstudio\readme.txt"
    
    SetOutPath "$INSTDIR\code\nebula2\res"
    File "${SRCDIR}\code\nebula2\res\*.ico"
    File "${SRCDIR}\code\nebula2\res\*.rc"

    SetOutPath "$INSTDIR\doc"
    File "${SRCDIR}\doc\nebula2\license.txt"

    SetOutPath "$INSTDIR\code\nebula2\lib\win32_vc_i386"
    File "${SRCDIR}\code\nebula2\lib\win32_vc_i386\*.lib"
    
SectionEnd

Section "Installer Files" Section_InstallerFiles

    SetOutPath "$INSTDIR\work\installer\Nebula2SDK"
    File "${SRCDIR}\work\installer\Nebula2SDK\*.bmp"
    File "${SRCDIR}\work\installer\Nebula2SDK\*.ico"
    File "${SRCDIR}\work\installer\Nebula2SDK\*.nsi"

    SetOutPath "$INSTDIR\work\installer\NewProjectWizard"
    File "${SRCDIR}\work\installer\NewProjectWizard\*.bmp"
    File "${SRCDIR}\work\installer\NewProjectWizard\*.ico"
    File "${SRCDIR}\work\installer\NewProjectWizard\*.nsi"

SectionEnd 

Section "-StartMenuEntries" Section_StartMenuEntries

IfSilent endOfStartMenuSection

    ; Delete Old Start Menu Shortcuts
    IfFileExists "$SMPROGRAMS\${PRODUCT}" "" +2
        RmDir /r "$SMPROGRAMS\${PRODUCT}"

    ; this sets the "current working directory" for the shortcuts
    SetOutPath "$INSTDIR\bin\win32"
    CreateDirectory "${STARTMENU}"

    CreateShortCut "${STARTMENU}\Browse Nebula2 SDK Directory.lnk" "$WINDIR\explorer.exe" "$INSTDIR"
    CreateShortCut "${STARTMENU}\License.lnk" "$INSTDIR\doc\license.txt"

    CreateDirectory "${STARTMENU}\Tools"
        CreateShortCut "${STARTMENU}\Tools\Nebula2 Viewer.lnk" "$INSTDIR\bin\win32\nviewer.exe" "" "$INSTDIR\bin\win32\nviewer.exe" 0
        CreateShortCut "${STARTMENU}\Tools\Nebula2 Shell.lnk" "$INSTDIR\bin\win32\nsh.exe" "" "$INSTDIR\bin\win32\nsh.exe" 0
        CreateShortCut "${STARTMENU}\Tools\New Project Wizard.lnk" "$INSTDIR\bin\win32\NewProjectWizard.exe" "" "$INSTDIR\bin\win32\NewProjectWizard.exe" 0
    
    CreateDirectory "${STARTMENU}\Documentation"
        IfFileExists "$INSTDIR\doc\nebula2sdk.chm" "" +2
            CreateShortCut "${STARTMENU}\Documentation\Nebula2 SDK Readme.lnk" "$INSTDIR\doc\nebula2sdk.chm"
        IfFileExists "$INSTDIR\doc\nebula2api.chm" "" +2
            CreateShortCut "${STARTMENU}\Documentation\Nebula2 API Documentation.lnk" "$INSTDIR\doc\nebula2api.chm"
        CreateShortCut "${STARTMENU}\Documentation\Radon Labs GmbH Website.lnk" "http://www.radonlabs.de"
        CreateShortCut "${STARTMENU}\Documentation\Nebula on SourceForge.lnk" "http://www.sourceforge.net/projects/nebuladevice"
        CreateShortCut "${STARTMENU}\Documentation\Nebula Community Website.lnk" "http://www.nebuladevice.org"    

    IfFileExists "$INSTDIR\code\nebula2\vstudio\tools.sln" "" endOfVStudio
        CreateDirectory "${STARTMENU}\VStudio Solutions"
        CreateShortCut "${STARTMENU}\VStudio Solutions\Nebula Static Link Lib.lnk" "$INSTDIR\code\nebula2\vstudio\nebulalib.sln" 
        CreateShortCut "${STARTMENU}\VStudio Solutions\Nebula Tools.lnk" "$INSTDIR\code\nebula2\vstudio\tools.sln" 

endOfVStudio:

    IfFileExists "$INSTDIR\export\gfxlib\examples\bouncingball.n2" "" endOfSamples
        CreateDirectory "${STARTMENU}\Example Objects"
        CreateShortCut "${STARTMENU}\Example Objects\Hierarchical Animation.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\bouncingball.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Animated Character.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\eagle.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Environment Mapping.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\envmapping.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Fire Particle System.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\fire.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Smoke Particle System.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\smoke.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Lightmapping.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\lightmapping.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Ocean Shader (DX9 only).lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\ocean.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Opel Blitz Truck.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\opelblitz.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Tiger Tank.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\tiger.n2" -projdir "$INSTDIR"'
        CreateShortCut "${STARTMENU}\Example Objects\Dynamic Tree.lnk" "$INSTDIR\bin\win32\nviewer.exe" '-view "$INSTDIR\export\gfxlib\examples\tree.n2" -projdir "$INSTDIR"'
endOfSamples:

    CreateShortCut "${STARTMENU}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0

endOfStartMenuSection:
SectionEnd
;-------------------------------------------------------------------------------
;   Section Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_Documentation} "This component includes the user documentation, including web links and the Nebula2 API docs."
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_SourceCode} "This component includes the Nebula2 source code."
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_ExampleFiles} "This component includes sample 3d objects and textures."
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_InstallerFiles} "This component includes the NSIS installer source files for the SDK installer and the New Project Wizard."
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_VStudioFiles} "This component includes pre-generated VStudio.NET project files."
    !insertmacro MUI_DESCRIPTION_TEXT ${Section_Doxygen} "This component includes the documentation source files for Doxygen, these are necessary if you want to build the Nebula2 API docs yourself."
!insertmacro MUI_FUNCTION_DESCRIPTION_END
    
;-------------------------------------------------------------------------------
;   Uninstaller Section  
Section "Uninstall"

;   Delete registry keys
    DeleteRegKey HKLM "SOFTWARE\${COMPANY}\${PRODUCT}"
    DeleteRegKey /ifempty HKLM "SOFTWARE\${COMPANY}"

;   Remove the installation directory
    RMDir /r "$INSTDIR"

;   Delete Start Menu Shortcuts
    RmDir  /r "$SMPROGRAMS\${PRODUCT}"
  
;   Delete Uninstaller And Unistall Registry Entries
    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${COMPANY}\${PRODUCT}"
    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"  
  
SectionEnd

