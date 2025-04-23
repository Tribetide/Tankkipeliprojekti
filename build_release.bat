@echo off
setlocal

REM ------------------- asetukset --------------------
REM Jos käytät vcpkg:tä, viittaa siihen:
set "VCPKG_ROOT=%USERPROFILE%\vcpkg"
set "TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

REM Build‑kansio:
set "BUILD_DIR=build"
REM Asennus/stage‑kansio:
set "STAGE_DIR=stage"

REM ---------------------------------------------------
echo == 1 == Konfiguroi CMake (Release) ...
cmake -B %BUILD_DIR% -G "Visual Studio 17 2022" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%"

if errorlevel 1 goto :error

echo == 2 == Rakenna ...
cmake --build %BUILD_DIR% --config Release
if errorlevel 1 goto :error

echo == 3 == Asenna stageen ...
cmake --install %BUILD_DIR% --config Release --prefix %STAGE_DIR%
if errorlevel 1 goto :error

echo == 4 == Tee ZIP‑paketti ...
cmake --build %BUILD_DIR% --config Release --target package
if errorlevel 1 goto :error

echo == 5 == Valmis!


exit /b 0

:error
echo **** VIRHE rakennuksessa ****
exit /b 1