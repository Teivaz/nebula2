;-------------------------------------------------------------------------------
;
;  Nullsoft Installer script for the Nebula 2 Exporter for 3dsmax
;
;  (c) 2006    Kim Hyoun Woo
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
;  Use Modern UI

!include "MUI.nsh"

;-------------------------------------------------------------------------------
;  Defines

!define PRODUCT_VERSION "0.1"
!define LONG_PRODUCT_VERSION "${PRODUCT_VERSION}.0.0"
!define COMPANY "Kim Hyouon Woo"
!define PRODUCT_NAME "Nebula2 3dsmax Toolkit"
!define PRODUCT_SHORT_NAME "nmaxtoobox"
!define /date DATE_TODAY "%Y_%m_%d"
!define CHM_NAME "nmaxtoolbox.chm"
!define SRCDIR ".."
!define N2SRCDIR "..\..\..\.."

;-------------------------------------------------------------------------------
;  General Settings

SetCompressor lzma
; set the name to be used in installer title bars etc.
Name "The ${PRODUCT_NAME}"
; set the location and name for the compiled installer
OutFile "${SRCDIR}\nmaxtoolbox_${DATE_TODAY}.exe"
; use an XP manifest thingie
XPStyle on
; default installation directory
InstallDir "c:\3dsmax8"
; get installation folder from registry if available
;InstallDirRegKey HKLM "${PRODUCT_INST_REGKEY}" "installdir"

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
!define MUI_FINISHPAGE_RUN "hh.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Display Exporter Documentation"
!define MUI_FINISHPAGE_RUN_PARAMETERS "$INSTDIR\scripts\nebula2\${CHM_NAME}"

;--------------------------------
;  Installer Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "eula.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
;Var STARTMENU_FOLDER
;!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;--------------------------------
;  Custom Functions

!define MUI_CUSTOMFUNCTION_GUIINIT myGuiInit

Function myGUIInit

  Push $R0

  ; Ensure only one instance of the installer is running at a time
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "N23dsmaxtoolkitSetup") i .r1 ?e'
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
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(c) 2006 Kim Hyoun Woo"
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
    SetOutPath "$INSTDIR\plugins"
    File "${N2SRCDIR}\bin\win32\nmaxtoolbox.dlu"
    File "${N2SRCDIR}\bin\win32\nmaximport.dli"


    ; 3dsmax/plugcfg direcotry
    SetOutPath "$INSTDIR\plugcfg"
    File "${N2SRCDIR}\code\contrib\nmax\maxscripts\nmaxtoolbox\plugcfg\*.ini"

    ; 3dsmax/scripts/startup direcotry
    SetOutPath "$INSTDIR\scripts\startup"
    File "${N2SRCDIR}\code\contrib\nmax\maxscripts\nmaxtoolbox\scripts\startup\nmaxtoolbox.ms"

    ; 3dsmax/scripts/nebula2 direcotry
    SetOutPath "$INSTDIR\scripts\nebula2"
    File "${N2SRCDIR}\code\contrib\nmax\maxscripts\nmaxtoolbox\scripts\nebula2\*.ms"

    SetOutPath "$INSTDIR\scripts\nebula2"
    File "${N2SRCDIR}\code\contrib\nmax\maxscripts\nmaxtoolbox\scripts\nebula2\*.bmp"

    SetOutPath "$INSTDIR\scripts\nebula2"
    File "${N2SRCDIR}\code\contrib\nmax\maxscripts\nmaxtoolbox\scripts\nebula2\*.txt"

SectionEnd

Section "Documentation" Section_Documentation

    SetOutPath "$INSTDIR\scripts\nebula2"
    File /nonfatal "${SRCDIR}\doc\artistdoc\doxydoc\toolkit\html\${CHM_NAME}"

SectionEnd

Section "Samples" Section_Samples

SectionEnd

; It may need this section to register custom activex control
Section "ActiveX" Section_ActiveX

    SectionIn RO

    ; root directory
    SetOutPath "$INSTDIR"

    ; bin directory
    SetOutPath "$INSTDIR\plugins"
    File "${N2SRCDIR}\bin\win32\nmaxenvelopecurve.ocx"

    ; register activex used for envelope curver cotnrol
    Exec '"$INSTDIR\regsvr32.exe /s nmaxenvelopecurve.ocx"'

SectionEnd

;-------------------------------------------------------------------------------
;   Section Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

!insertmacro MUI_DESCRIPTION_TEXT ${Section_Runtime} "Core 3dsmax plugins."
!insertmacro MUI_DESCRIPTION_TEXT ${Section_Documentation} "User documentation."
;!insertmacro MUI_DESCRIPTION_TEXT ${Section_Samples} "Sample 3dsmax scenes."

!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-------------------------------------------------------------------------------
;  EOF
;-------------------------------------------------------------------------------
