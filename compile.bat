@echo off
echo Compilation du projet Casse-Briques...
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
echo.
echo Compilation terminee!
echo Executable: build\Release\CasseBriques.exe ou build\CasseBriques.exe
pause

