;-------------------------------------------------------------------------------
;
;  Nullsoft Installer script for the Nebula 2 Exporter for Lightwave 3D
;
;  (c) 2006    Vadim Macagon
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
;  Use Modern UI

!include "MUI.nsh"

;-------------------------------------------------------------------------------
;  Defines

!define PRODUCT_VERSION "0.1"
!define LONG_PRODUCT_VERSION "${PRODUCT_VERSION}.0.0"
!define COMPANY "SteelRonin"
!define PRODUCT_NAME "Nebula 2 Exporter for Lightwave"
!define PRODUCT_SHORT_NAME "N2LightwaveExporter"
!define /date DATE_TODAY "%Y_%m_%d"
!define CHM_NAME "N2LightwaveExporter.chm"
!define SRCDIR ".."
!define N2SRCDIR "..\..\..\.."
!define PRODUCT_INST_REGKEY "Software\${COMPANY}\${PRODUCT_SHORT_NAME}"
!define PRODUCT_UNINST_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

;-------------------------------------------------------------------------------
;  General Settings

SetCompressor lzma
; set the name to be used in installer title bars etc.
Name "The ${PRODUCT_NAME}"
; set the location and name for the compiled installer
OutFile "${SRCDIR}\N2LightwaveExporter_${DATE_TODAY}.exe"
; use an XP manifest thingie
XPStyle on
; default installation directory
InstallDir "$PROGRAMFILES\${PRODUCT_SHORT_NAME}"
; get installation folder from registry if available
InstallDirRegKey HKLM "${PRODUCT_INST_REGKEY}" "installdir"

;-------------------------------------------------------------------------------
;   Interface Settings

!define MUI_ABORTWARNING
!define MUI_ICON "images\install.ico"
!define MUI_UNICON "images\uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "images\header.bmp"
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEFINISHPAGE_BITMAP "images\wizard.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "images\wizard.bmp"
!define MUI_INSTFILESPAGE_PROGRESSBAR "smooth"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
; allow the user to specify the program group in the start menu
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_REGKEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
; allow user to auto-display the documentation after the installation
!define MUI_FINISHPAGE_RUN "hh.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Display Exporter Documentation"
!define MUI_FINISHPAGE_RUN_PARAMETERS "$INSTDIR\doc\${CHM_NAME}"

;--------------------------------
;  Installer Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "eula.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Var STARTMENU_FOLDER
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;--------------------------------
;  Uninstaller Pages

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;  Custom Functions

!define MUI_CUSTOMFUNCTION_GUIINIT myGuiInit

Function myGUIInit

  Push $R0

  ; Ensure only one instance of the installer is running at a time
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "N2LightwaveExporterSetup") i .r1 ?e'
  Pop $R0
  StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
  Abort ; installer already running, bail out

FunctionEnd

;--------------------------------
;  Languages

!insertmacro MUI_LANGUAGE "English"

;-------------------------------------------------------------------------------
;  Set the version info that will show up in the installer file properties

VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(c) 2006 Vadim Macagon"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_NAME}"
VIProductVersion "${LONG_PRODUCT_VERSION}"

;-------------------------------------------------------------------------------
;  Installer Sections

Section "Runtime" Section_Runtime

    SectionIn RO

    ; root directory
    SetOutPath "$INSTDIR"

    ; bin directory
    SetOutPath "$INSTDIR\bin\win32"
    File "images\install.ico"
    File "${N2SRCDIR}\bin\win32\nviewer.exe"
    File "${N2SRCDIR}\bin\win32\n2lwexporter.p"

    ; Standard lighting environment
    SetOutPath "$INSTDIR\export\gfxlib"
    ; The default stdlight.n2 means results in nViewer having a black background
    ; which makes it difficult to see models being previewed, this may get fixed
    ; File "${N2SRCDIR}\export\gfxlib\stdlight.n2"
    ; In the meantime use our own light setup instead...
    File "${SRCDIR}\export\gfxlib\stdlight.n2"

    ; Meshes
    SetOutPath "$INSTDIR\export\meshes"
    File "${N2SRCDIR}\export\meshes\skybox.n3d2"

    ; Scripts
    SetOutPath "$INSTDIR\data\scripts"
    File "${N2SRCDIR}\data\scripts\*.tcl"

    ; Shader library
    SetOutPath "$INSTDIR\data\shaders"
    File "${N2SRCDIR}\data\shaders\*.xml"

    SetOutPath "$INSTDIR\data\shaders\2.0"
    File "${N2SRCDIR}\data\shaders\2.0\*.fx"

    SetOutPath "$INSTDIR\data\shaders\fixed"
    File "${N2SRCDIR}\data\shaders\fixed\*.fx"

    SetOutPath "$INSTDIR\data\shaders\lib"
    File "${N2SRCDIR}\data\shaders\lib\*.fx"

    SetOutPath "$INSTDIR\export\textures\system"
    File "${N2SRCDIR}\export\textures\system\*.dds"

    ; Write the intall dir to the registry
    WriteRegStr HKLM "${PRODUCT_INST_REGKEY}" "installdir" "$INSTDIR"

    ; Write uninstall information to the registry
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "DisplayIcon" "$INSTDIR\bin\win32\install.ico,0"
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "HelpLink" "http://www.nebuladevice.org"
    WriteRegStr   HKLM "${PRODUCT_UNINST_REGKEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegDWORD HKLM "${PRODUCT_UNINST_REGKEY}" "NoModify" 0x1
    WriteRegDWORD HKLM "${PRODUCT_UNINST_REGKEY}" "NoRepair" 0x1

    WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Documentation" Section_Documentation

    SetOutPath "$INSTDIR\doc"
    File /nonfatal "${SRCDIR}\doc\artist\doxydoc\html\${CHM_NAME}"

SectionEnd

Section "Samples" Section_Samples

    ; source files
    
    CreateDirectory "$INSTDIR\work\scenes"
    CreateDirectory "$INSTDIR\work\objects"
    CreateDirectory "$INSTDIR\work\images"
    
    SetOutPath "$INSTDIR\work"
    File "${SRCDIR}\N2LightwaveExporter.xml"

    ; scenes
    SetOutPath "$INSTDIR\work\scenes\examples"
    File /nonfatal "${SRCDIR}\work\scenes\examples\*.lws"

    ; objects
    SetOutPath "$INSTDIR\work\objects\examples"
    File /nonfatal "${SRCDIR}\work\objects\examples\*.lwo"

    ; images
    SetOutPath "$INSTDIR\work\images\examples"
    File /nonfatal "${SRCDIR}\work\images\examples\*.*"
    
    ; exported files
    
    CreateDirectory "$INSTDIR\export\gfxlib"
    CreateDirectory "$INSTDIR\export\anims"
    CreateDirectory "$INSTDIR\export\meshes"
    CreateDirectory "$INSTDIR\export\textures"
    
    SetOutPath "$INSTDIR\export"
    File "${SRCDIR}\export\readme.txt"
    
    ; anims
    SetOutPath "$INSTDIR\export\anims"
    File "${SRCDIR}\export\anims\readme.txt"
    SetOutPath "$INSTDIR\export\anims\examples"
    File /nonfatal "${SRCDIR}\export\anims\examples\*.nax2"

    ; gfxlib
    SetOutPath "$INSTDIR\export\gfxlib"
    File "${SRCDIR}\export\gfxlib\readme.txt"
    SetOutPath "$INSTDIR\export\gfxlib\examples"
    File /nonfatal "${SRCDIR}\export\gfxlib\examples\*.n2"

    ; meshes
    SetOutPath "$INSTDIR\export\meshes"
    File "${SRCDIR}\export\meshes\readme.txt"
    SetOutPath "$INSTDIR\export\meshes\examples"
    File /nonfatal "${SRCDIR}\export\meshes\examples\*.nvx2"

    ; textures
    SetOutPath "$INSTDIR\export\textures"
    File "${SRCDIR}\export\textures\readme.txt"
    SetOutPath "$INSTDIR\export\textures\examples"
    File /nonfatal "${SRCDIR}\export\textures\examples\*.*"

SectionEnd

; Hidden section - always runs, user can't deselect
Section "-StartMenuEntries" Section_StartMenuEntries

    ; Delete old Start Menu Shortcuts
    IfFileExists "$SMPROGRAMS\$STARTMENU_FOLDER" 0 +2
        RmDir /r "$SMPROGRAMS\$STARTMENU_FOLDER"

    ; Set the "current working directory" for the shortcuts
    SetOutPath "$INSTDIR\bin\win32"
    
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    
    CreateShortCut '$SMPROGRAMS\$STARTMENU_FOLDER\Nebula 2 Viewer.lnk' \
                   '$INSTDIR\bin\win32\nviewer.exe' '-projdir "$INSTDIR"' \
                   '$INSTDIR\bin\win32\nviewer.exe' 0

    IfFileExists "$INSTDIR\doc\${CHM_NAME}" 0 endOfDocumentation
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Help.lnk" "hh.exe" "$INSTDIR\doc\${CHM_NAME}"
    endOfDocumentation:

    ; Links to related web sites
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER\Links"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\Nebula Community Website.lnk" "http://www.nebuladevice.org/"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\Radon Labs GmbH Website.lnk" "http://www.radonlabs.de/"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\Nebula on SourceForge.lnk" "http://www.sourceforge.net/projects/nebuladevice"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\Report a Bug.lnk" "http://nebuladevice.cubik.org/bugs/"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\Mailing Lists.lnk" "http://sourceforge.net/mail/?group_id=7192"
    ; Site hasn't been updated in ages, and is down at the moment
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Links\SteelRonin Website.lnk" "http://www.steelronin.com/"

    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0

    !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;-------------------------------------------------------------------------------
;   Section Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

!insertmacro MUI_DESCRIPTION_TEXT ${Section_Runtime} "Core Lightwave plugins."
!insertmacro MUI_DESCRIPTION_TEXT ${Section_Documentation} "User documentation."
!insertmacro MUI_DESCRIPTION_TEXT ${Section_Samples} "Sample Lightwave scenes."

!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-------------------------------------------------------------------------------
;   Uninstaller Section

Section "Uninstall"

    ; Delete registry keys
    DeleteRegKey HKLM "${PRODUCT_INST_REGKEY}"
    DeleteRegKey /ifempty HKLM "SOFTWARE\${COMPANY}"

    ; Remove the installation directory
    RMDir /r "$INSTDIR"

    ; Delete Start Menu Shortcuts
    !insertmacro MUI_STARTMENU_GETFOLDER Application $STARTMENU_FOLDER
    RmDir /r "$SMPROGRAMS\$STARTMENU_FOLDER"

    ; Delete Installer And Unistall Registry Entries
    DeleteRegKey HKEY_LOCAL_MACHINE "${PRODUCT_INST_REGKEY}"
    DeleteRegKey HKEY_LOCAL_MACHINE "${PRODUCT_UNINST_REGKEY}"

SectionEnd

;-------------------------------------------------------------------------------
;  EOF
;-------------------------------------------------------------------------------
