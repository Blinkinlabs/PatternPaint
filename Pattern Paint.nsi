############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
#
# Modified to also install a driver, in the method suggested here:
# http://stackoverflow.com/questions/2464843/installing-drivers-from-nsis-installer-in-x64-system
############################################################################################

# Avoid windows installation workaround, so we put the start menu items in the right place?
RequestExecutionLevel admin

!include "x64.nsh"

!define APP_NAME "Pattern Paint"
!define COMP_NAME "Blinkinlabs"
!define WEB_SITE "http://blinkinlabs.com"
!define VERSION "VERSION_STRING"
!define COPYRIGHT "Blinkinlabs © 2016"
!define DESCRIPTION "Application"
!define INSTALLER_NAME "PatternPaint Windows Installer.exe"
!define MAIN_APP_EXE "PatternPaint.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\Pattern Paint"

######################################################################

!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!insertmacro MUI_PAGE_DIRECTORY

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Pattern Paint"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SMPROGRAMS
!endif

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_APP_EXE}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"

File "PatternPaintWindows\libgcc_s_dw2-1.dll"
File "PatternPaintWindows\libstdc++-6.dll"
File "PatternPaintWindows\libwinpthread-1.dll"
File "PatternPaintWindows\PatternPaint.exe"
File "PatternPaintWindows\Qt5Core.dll"
File "PatternPaintWindows\Qt5Gui.dll"
File "PatternPaintWindows\Qt5SerialPort.dll"
File "PatternPaintWindows\Qt5Widgets.dll"
File "PatternPaintWindows\WinSparkle.dll"
File "PatternPaintWindows\libusb-1.0.dll"
File "PatternPaintWindows\libblinky.dll"
SetOutPath "$INSTDIR\platforms"
File "PatternPaintWindows\platforms\qwindows.dll"
SetOutPath "$INSTDIR\imageformats"
File "PatternPaintWindows\imageformats\qgif.dll"
File "PatternPaintWindows\imageformats\qjpeg.dll"
File "PatternPaintWindows\imageformats\qsvg.dll"
File "PatternPaintWindows\imageformats\qtiff.dll"
SetOutPath "$INSTDIR\driver"
File "PatternPaintWindows\driver\dpinst32.exe"
File "PatternPaintWindows\driver\dpinst64.exe"
SetOutPath "$INSTDIR\driver\blinkytape"
File "PatternPaintWindows\driver\blinkytape\blinkinlabs.cat"
File "PatternPaintWindows\driver\blinkytape\blinkinlabs.inf"
SetOutPath "$INSTDIR\driver\lightbuddy"
File "PatternPaintWindows\driver\lightbuddy\lightbuddy_serial.inf"
File "PatternPaintWindows\driver\lightbuddy\lightbuddy_DFU_runtime.inf"
File "PatternPaintWindows\driver\lightbuddy\lightbuddy_DFU.inf"
File "PatternPaintWindows\driver\lightbuddy\libusb_device.cat"
File "PatternPaintWindows\driver\lightbuddy\blinkinlabs.cat"
SetOutPath "$INSTDIR\driver\lightbuddy\x86"
File "PatternPaintWindows\driver\lightbuddy\x86\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\lightbuddy\x86\WdfCoInstaller01009.dll"
SetOutPath "$INSTDIR\driver\lightbuddy\amd64"
File "PatternPaintWindows\driver\lightbuddy\amd64\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\lightbuddy\amd64\WdfCoInstaller01009.dll"
SetOutPath "$INSTDIR\driver\blinkypendant"
File "PatternPaintWindows\driver\blinkypendant\blinkypendant_serial.inf"
File "PatternPaintWindows\driver\blinkypendant\blinkypendant_DFU_runtime.inf"
File "PatternPaintWindows\driver\blinkypendant\blinkypendant_DFU.inf"
File "PatternPaintWindows\driver\blinkypendant\libusb_device.cat"
File "PatternPaintWindows\driver\blinkypendant\blinkinlabs.cat"
SetOutPath "$INSTDIR\driver\blinkypendant\x86"
File "PatternPaintWindows\driver\blinkypendant\x86\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\blinkypendant\x86\WdfCoInstaller01009.dll"
SetOutPath "$INSTDIR\driver\blinkypendant\amd64"
File "PatternPaintWindows\driver\blinkypendant\amd64\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\blinkypendant\amd64\WdfCoInstaller01009.dll"
SetOutPath "$INSTDIR\driver\eightbyeight"
File "PatternPaintWindows\driver\eightbyeight\eightbyeight_serial.inf"
File "PatternPaintWindows\driver\eightbyeight\eightbyeight_DFU_runtime.inf"
File "PatternPaintWindows\driver\eightbyeight\eightbyeight_DFU.inf"
File "PatternPaintWindows\driver\eightbyeight\libusb_device.cat"
File "PatternPaintWindows\driver\eightbyeight\blinkinlabs.cat"
SetOutPath "$INSTDIR\driver\eightbyeight\x86"
File "PatternPaintWindows\driver\eightbyeight\x86\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\eightbyeight\x86\WdfCoInstaller01009.dll"
SetOutPath "$INSTDIR\driver\eightbyeight\amd64"
File "PatternPaintWindows\driver\eightbyeight\amd64\winusbcoinstaller2.dll"
File "PatternPaintWindows\driver\eightbyeight\amd64\WdfCoInstaller01009.dll"
SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\"
CreateShortCut "$SMPROGRAMS\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\"
CreateShortCut "$SMPROGRAMS\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

SectionEnd

######################################################################

Section Driver
${If} ${RunningX64}
       ExecWait '"$INSTDIR\driver\dpinst64.exe" /sa /sw /PATH "$INSTDIR\driver\blinkytape"'
       ExecWait '"$INSTDIR\driver\dpinst64.exe" /sa /sw /PATH "$INSTDIR\driver\lightbuddy"'
       ExecWait '"$INSTDIR\driver\dpinst64.exe" /sa /sw /PATH "$INSTDIR\driver\blinkypendant"'
       ExecWait '"$INSTDIR\driver\dpinst64.exe" /sa /sw /PATH "$INSTDIR\driver\eightbyeight"'
	   ${Else}
       ExecWait '"$INSTDIR\driver\dpinst32.exe" /sa /sw /PATH "$INSTDIR\driver\blinkytape"'
       ExecWait '"$INSTDIR\driver\dpinst32.exe" /sa /sw /PATH "$INSTDIR\driver\lightbuddy"'
       ExecWait '"$INSTDIR\driver\dpinst32.exe" /sa /sw /PATH "$INSTDIR\driver\blinkypendant"'
       ExecWait '"$INSTDIR\driver\dpinst32.exe" /sa /sw /PATH "$INSTDIR\driver\eightbyeight"'
	   ${EndIf}
SectionEnd
	
######################################################################

Section Uninstall
${INSTALL_TYPE}
Delete "$INSTDIR\libgcc_s_dw2-1.dll"
Delete "$INSTDIR\libstdc++-6.dll"
Delete "$INSTDIR\libwinpthread-1.dll"
Delete "$INSTDIR\PatternPaint.exe"
Delete "$INSTDIR\Qt5Core.dll"
Delete "$INSTDIR\Qt5Gui.dll"
Delete "$INSTDIR\Qt5SerialPort.dll"
Delete "$INSTDIR\Qt5Widgets.dll"
Delete "$INSTDIR\WinSparkle.dll"
Delete "$INSTDIR\libusb-1.0.dll"
Delete "$INSTDIR\libblinky.dll"
Delete "$INSTDIR\platforms\qwindows.dll"
Delete "$INSTDIR\imageformats\qgif.dll"
Delete "$INSTDIR\imageformats\qjpeg.dll"
Delete "$INSTDIR\imageformats\qsvg.dll"
Delete "$INSTDIR\imageformats\qtiff.dll"
Delete "$INSTDIR\driver\blinkytape\blinkinlabs.cat"
Delete "$INSTDIR\driver\blinkytape\blinkinlabs.inf"
Delete "$INSTDIR\driver\lightbuddy\x86\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\lightbuddy\x86\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\lightbuddy\amd64\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\lightbuddy\amd64\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\lightbuddy\lightbuddy_serial.inf"
Delete "$INSTDIR\driver\lightbuddy\lightbuddy_DFU_runtime.inf"
Delete "$INSTDIR\driver\lightbuddy\lightbuddy_DFU.inf"
Delete "$INSTDIR\driver\lightbuddy\libusb_device.cat"
Delete "$INSTDIR\driver\lightbuddy\blinkinlabs.cat"
Delete "$INSTDIR\driver\blinkypendant\x86\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\blinkypendant\x86\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\blinkypendant\amd64\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\blinkypendant\amd64\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\blinkypendant\blinkypendant_serial.inf"
Delete "$INSTDIR\driver\blinkypendant\blinkypendant_DFU_runtime.inf"
Delete "$INSTDIR\driver\blinkypendant\blinkypendant_DFU.inf"
Delete "$INSTDIR\driver\blinkypendant\libusb_device.cat"
Delete "$INSTDIR\driver\blinkypendant\blinkinlabs.cat"
Delete "$INSTDIR\driver\eightbyeight\x86\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\eightbyeight\x86\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\eightbyeight\amd64\winusbcoinstaller2.dll"
Delete "$INSTDIR\driver\eightbyeight\amd64\WdfCoInstaller01009.dll"
Delete "$INSTDIR\driver\eightbyeight\eightbyeight_serial.inf"
Delete "$INSTDIR\driver\eightbyeight\eightbyeight_DFU_runtime.inf"
Delete "$INSTDIR\driver\eightbyeight\eightbyeight_DFU.inf"
Delete "$INSTDIR\driver\eightbyeight\libusb_device.cat"
Delete "$INSTDIR\driver\eightbyeight\blinkinlabs.cat"
Delete "$INSTDIR\driver\dpinst32.exe"
Delete "$INSTDIR\driver\dpinst64.exe"

RmDir "$INSTDIR\driver\lightbuddy\x86"
RmDir "$INSTDIR\driver\lightbuddy\amd64"
RmDir "$INSTDIR\driver\lightbuddy"
RmDir "$INSTDIR\driver\blinkypendant\x86"
RmDir "$INSTDIR\driver\blinkypendant\amd64"
RmDir "$INSTDIR\driver\blinkypendant"
RmDir "$INSTDIR\driver\eightbyeight\x86"
RmDir "$INSTDIR\driver\eightbyeight\amd64"
RmDir "$INSTDIR\driver\eightbyeight"
RmDir "$INSTDIR\driver\blinkytape"
RmDir "$INSTDIR\driver"
RmDir "$INSTDIR\imageformats"
RmDir "$INSTDIR\platforms"
 
Delete "$INSTDIR\uninstall.exe"

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SMPROGRAMS
Delete "$SMPROGRAMS\${APP_NAME}.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\${APP_NAME}.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

