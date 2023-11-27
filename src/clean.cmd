@echo off

setlocal
pushd %~dp0

set _NUGET_CACHE=%USERPROFILE%\.nuget\packages
if "%NUGET_PACKAGES%" NEQ "" set _NUGET_CACHE=%NUGET_PACKAGES%

echo Cleaning...

if exist ..\build rd /s/q ..\build
if exist ..\packages rd /s/q ..\packages

if exist Directory.Packages.props (del Directory.Packages.props)
if exist global.json (del global.json)

if exist ..\Directory.Packages.props (del ..\Directory.Packages.props)
if exist ..\global.json (del ..\global.json)

if exist "%_NUGET_CACHE%\wixinternal.basebuildtasks.sources" rd /s/q "%_NUGET_CACHE%\wixinternal.basebuildtasks.sources"
if exist "%_NUGET_CACHE%\wixinternal.testsupport" rd /s/q "%_NUGET_CACHE%\wixinternal.testsupport"
if exist "%_NUGET_CACHE%\wixinternal.core.testpackage" rd /s/q "%_NUGET_CACHE%\wixinternal.core.testpackage"
if exist "%_NUGET_CACHE%\panelswwix4.bal.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.bal.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.balutil" rd /s/q "%_NUGET_CACHE%\panelswwix4.balutil"
if exist "%_NUGET_CACHE%\panelswwix4.bextutil" rd /s/q "%_NUGET_CACHE%\panelswwix4.bextutil"
if exist "%_NUGET_CACHE%\panelswwix4.bootstrappercore.native" rd /s/q "%_NUGET_CACHE%\panelswwix4.bootstrappercore.native"
if exist "%_NUGET_CACHE%\panelswwix4.burn" rd /s/q "%_NUGET_CACHE%\panelswwix4.burn"
if exist "%_NUGET_CACHE%\panelswwix4.complus.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.complus.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.converters" rd /s/q "%_NUGET_CACHE%\panelswwix4.converters"
if exist "%_NUGET_CACHE%\panelswwix4.core" rd /s/q "%_NUGET_CACHE%\panelswwix4.core"
if exist "%_NUGET_CACHE%\panelswwix4.core.burn" rd /s/q "%_NUGET_CACHE%\panelswwix4.core.burn"
if exist "%_NUGET_CACHE%\panelswwix4.core.extensioncache" rd /s/q "%_NUGET_CACHE%\panelswwix4.core.extensioncache"
if exist "%_NUGET_CACHE%\panelswwix4.core.native" rd /s/q "%_NUGET_CACHE%\panelswwix4.core.native"
if exist "%_NUGET_CACHE%\panelswwix4.core.windowsinstaller" rd /s/q "%_NUGET_CACHE%\panelswwix4.core.windowsinstaller"
if exist "%_NUGET_CACHE%\panelswwix4.data" rd /s/q "%_NUGET_CACHE%\panelswwix4.data"
if exist "%_NUGET_CACHE%\panelswwix4.dependency.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.dependency.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.directx.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.directx.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.dnc.hostgenerator" rd /s/q "%_NUGET_CACHE%\panelswwix4.dnc.hostgenerator"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.compression" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.compression"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.compression.cab" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.compression.cab"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.compression.zip" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.compression.zip"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.customaction" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.customaction"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.resources" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.resources"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller.linq" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller.linq"
if exist "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller.package" rd /s/q "%_NUGET_CACHE%\panelswwix4.dtf.windowsinstaller.package"
if exist "%_NUGET_CACHE%\panelswwix4.dutil" rd /s/q "%_NUGET_CACHE%\panelswwix4.dutil"
if exist "%_NUGET_CACHE%\panelswwix4.extensibility" rd /s/q "%_NUGET_CACHE%\panelswwix4.extensibility"
if exist "%_NUGET_CACHE%\panelswwix4.firewall.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.firewall.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.heat" rd /s/q "%_NUGET_CACHE%\panelswwix4.heat"
if exist "%_NUGET_CACHE%\panelswwix4.http.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.http.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.iis.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.iis.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.mba.core" rd /s/q "%_NUGET_CACHE%\panelswwix4.mba.core"
if exist "%_NUGET_CACHE%\panelswwix4.msmq.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.msmq.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.netfx.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.netfx.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.powershell.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.powershell.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.sdk" rd /s/q "%_NUGET_CACHE%\panelswwix4.sdk"
if exist "%_NUGET_CACHE%\panelswwix4.sql.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.sql.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.util.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.util.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.ui.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.ui.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.versioning" rd /s/q "%_NUGET_CACHE%\panelswwix4.versioning"
if exist "%_NUGET_CACHE%\panelswwix4.visualstudio.wixext" rd /s/q "%_NUGET_CACHE%\panelswwix4.visualstudio.wixext"
if exist "%_NUGET_CACHE%\panelswwix4.wcautil" rd /s/q "%_NUGET_CACHE%\panelswwix4.wcautil"
if exist "%_NUGET_CACHE%\wix" rd /s/q "%_NUGET_CACHE%\wix"

popd
endlocal
