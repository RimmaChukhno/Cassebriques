@echo off
setlocal enabledelayedexpansion

REM Build + run using MSYS2 MinGW-w64 SFML (recommended if SFML_DIR points to C:\msys64\mingw64).

set ROOT=%~dp0..
set MSYS2_MINGW64=C:\msys64\mingw64
set MSYS2_ROOT=C:\msys64
set TOOLCHAIN=%ROOT%\cmake\toolchains\msys2-mingw64.cmake
set BUILD_DIR=%ROOT%\build-mingw
set CONFIG=Release

if not exist "%MSYS2_MINGW64%" (
  echo ERROR: MSYS2 MinGW64 not found at "%MSYS2_MINGW64%"
  exit /b 1
)
if not exist "%TOOLCHAIN%" (
  echo ERROR: Toolchain file not found: "%TOOLCHAIN%"
  exit /b 1
)

if not exist "%MSYS2_MINGW64%\bin\gcc.exe" (
  echo ERROR: MinGW compiler not found at "%MSYS2_MINGW64%\bin\gcc.exe"
  echo Fix (MSYS2):
  echo   C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain"
  exit /b 1
)
if not exist "%MSYS2_MINGW64%\bin\g++.exe" (
  echo ERROR: MinGW compiler not found at "%MSYS2_MINGW64%\bin\g++.exe"
  echo Fix (MSYS2):
  echo   C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain"
  exit /b 1
)
set MAKE_EXE=
if exist "%MSYS2_MINGW64%\bin\mingw32-make.exe" set MAKE_EXE=%MSYS2_MINGW64%\bin\mingw32-make.exe
if not defined MAKE_EXE if exist "%MSYS2_MINGW64%\bin\make.exe" set MAKE_EXE=%MSYS2_MINGW64%\bin\make.exe
if not defined MAKE_EXE if exist "%MSYS2_ROOT%\usr\bin\make.exe" set MAKE_EXE=%MSYS2_ROOT%\usr\bin\make.exe

if not defined MAKE_EXE (
  echo ERROR: GNU make not found.
  echo.
  echo Fix (MSYS2):
  echo   1^) Open "MSYS2 MinGW x64" shell
  echo   2^) Run: pacman -S --needed mingw-w64-x86_64-make
  echo.
  echo Or from PowerShell (non-interactive) if you have MSYS2 installed:
  echo   C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm --needed mingw-w64-x86_64-make"
  exit /b 1
)

where cmake >nul 2>&1
if errorlevel 1 (
  echo ERROR: cmake not found in PATH
  exit /b 1
)

cd /d "%ROOT%"

echo Configuring (MinGW) into "%BUILD_DIR%" ...
cmake -S . -B "%BUILD_DIR%" -G "MinGW Makefiles" ^
  -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
  -DCMAKE_MAKE_PROGRAM="%MAKE_EXE%" ^
  -DSFML_DIR="%MSYS2_MINGW64%\lib\cmake\SFML" ^
  -DCMAKE_BUILD_TYPE=%CONFIG%
if errorlevel 1 exit /b 1

echo Building (%CONFIG%) ...
cmake --build "%BUILD_DIR%" -- -j
if errorlevel 1 exit /b 1

set EXE=%BUILD_DIR%\bin\CasseBriques.exe
if not exist "%EXE%" (
  echo ERROR: Built exe not found at "%EXE%"
  exit /b 1
)

echo Running: "%EXE%"
"%EXE%"

endlocal


