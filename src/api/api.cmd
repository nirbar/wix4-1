@setlocal
@pushd %~dp0

@set _C=Debug
@set _L=%~dp0..\..\build\logs

:parse_args
@if /i "%1"=="release" set _C=Release
@if /i "%1"=="inc" set _INC=1
@if /i "%1"=="clean" set _CLEAN=1
@if not "%1"=="" shift & goto parse_args

@set _B=%~dp0..\..\build\api\%_C%

:: Clean

@if "%_INC%"=="" call :clean
@if NOT "%_CLEAN%"=="" goto :end

@echo Building api %_C%

:: Restore
:: Build
:: Pack

msbuild api_t.proj -p:Configuration=%_C% -tl -nologo -warnaserror -bl:%_L%\api_build.binlog || exit /b

:: Test
dotnet test ^
 %_B%\net8.0\WixToolsetTest.Data.dll ^
 %_B%\net8.0\win-x86\WixToolsetTest.BootstrapperApplicationApi.dll ^
 %_B%\x86\BalUtilUnitTest.dll ^
 %_B%\x86\BextUtilUnitTest.dll ^
 --nologo -l "trx;LogFileName=%_L%\TestResults\api.trx" || exit /b

@goto :end

:clean
@rd /s/q "..\..\build\api" 2> nul
@del "..\..\build\artifacts\PanelSwWix4.BextUtil.*.nupkg" 2> nul
@del "..\..\build\artifacts\PanelSwWix4.Data.*.nupkg" 2> nul
@del "..\..\build\artifacts\PanelSwWix4.Extensibility.*.nupkg" 2> nul
@del "..\..\build\artifacts\PanelSwWix4.BootstrapperApplicationApi.*.nupkg" 2> nul
@del "..\..\build\artifacts\PanelSwWix4.BootstrapperExtensionApi.*.nupkg" 2> nul
@del "%_L%\TestResults\api.trx" 2> nul
@rd /s/q "%USERPROFILE%\.nuget\packages\panelswwix4.bextutil" 2> nul
@rd /s/q "%USERPROFILE%\.nuget\packages\panelswwix4.data" 2> nul
@rd /s/q "%USERPROFILE%\.nuget\packages\panelswwix4.extensibility" 2> nul
@rd /s/q "%USERPROFILE%\.nuget\packages\panelswwix4.bootstrapperapplicationapi" 2> nul
@rd /s/q "%USERPROFILE%\.nuget\packages\panelswwix4.bootstrapperextensionapi" 2> nul
@exit /b

:end
@popd
@endlocal
