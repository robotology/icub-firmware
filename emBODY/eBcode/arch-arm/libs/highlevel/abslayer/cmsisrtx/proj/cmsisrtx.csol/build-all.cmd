@echo off
rem Builds all the contexts of cmsisrtx.csolution.yml with cbuild (CMSIS-Toolbox). The project copies each library to ..\..\lib
rem (i.e. cmsisrtx\lib) by itself (node executes: of the cproject), as the build button of VS Code does.
rem
rem Usage (from any folder):
rem   build-all.cmd            incremental build of all contexts
rem   build-all.cmd rebuild    clean and build all contexts
rem   build-all.cmd sizes      as the first, then the sizes of each object of every library in ..\..\lib (armar --sizes: AC6 only)
rem
rem It needs cbuild, cmake, ninja, armclang (AC6) and clang (ATfE) in the PATH: run it from a new terminal of VS Code with this solution
rem open (Keil Studio activates the tools of vcpkg-configuration.json), or run "vcpkg activate" in this folder before.

setlocal
cd /d "%~dp0"

where cbuild >nul 2>nul
if errorlevel 1 (
    echo cbuild not found: open a new terminal of VS Code on this solution, or run "vcpkg activate" in %~dp0
    exit /b 1
)

set OPTS=--packs
if /i "%~1"=="rebuild" set OPTS=--packs --rebuild

rem no --context: the executes: steps (copy to ..\..\lib) run only in a build of the whole solution
cbuild cmsisrtx.csolution.yml %OPTS%
if errorlevel 1 (
    echo BUILD FAILED
    exit /b 1
)

echo.
echo Libraries in ..\..\lib:
for %%f in (..\..\lib\cmsisrtx.csol.*.lib ..\..\lib\cmsisrtx.csol.atfe.*.a) do echo   %%~nxf  %%~tf

if /i "%~1"=="sizes" (
    for %%f in (..\..\lib\cmsisrtx.csol.*.lib) do (
        echo.
        echo ==== %%~nxf
        armar --sizes "%%f"
    )
)

endlocal
exit /b 0
