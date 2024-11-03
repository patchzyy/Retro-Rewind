SETLOCAL EnableDelayedExpansion
@echo off
cls

:: Ensure the build directory exists
if not exist "build" mkdir build
del build\*.o

SET "debug="
SET "cwDWARF="
if "%1" equ "-d" SET "debug=-debug -map=^"%appdata%\Dolphin Emulator\Maps\RMCP01.map^" -readelf=^"C:\MinGW\bin\readelf.exe^""
if "%1" equ "-d" SET "cwDWARF=-g"

:: Destination (change as necessary)
SET "SOURCE=RetroRewind6"
SET "RIIVO=%appdata%\Dolphin Emulator\Load\Riivolution\RetroRewind6"
SET "ENGINE=G:\Coding\MarioKart\RR\Pulsar\KamekInclude"
echo %ENGINE%

:: CPP compilation settings
SET CC="C:/CodeWarriors/Command_Line_Tools/mwcceppc.exe"
SET CFLAGS=-I- -i "G:\Coding\MarioKart\RR\Pulsar\KamekInclude" -i "G:\Coding\MarioKart\RR\Pulsar\GameSource" -i "G:\Coding\MarioKart\RR\Pulsar\GameSource\include" -i "G:\Coding\MarioKart\RR\Pulsar\GameSource\game" -i "G:\Coding\MarioKart\RR\Pulsar\GameSource\core" -i "G:\Coding\MarioKart\RR\Pulsar\PulsarEngine" -i "G:\Coding\MarioKart\RR\Pulsar" -i code ^
  -opt all -inline auto -enum int -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 %cwDWARF%


SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R "../../Pulsar/PulsarEngine" %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "%ENGINE%\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking... %time%
"../../Pulsar/KamekLinker/kamek" "build/kamek.o" %OBJECTS% %debug% -dynamic -externals="../../Pulsar/GameSource/symbols.txt" -versions="../../Pulsar/GameSource/versions.txt" -output-combined=build\Code.pul

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.pul "%RIIVO%\Binaries" >nul
    echo Binaries copied
)

:end
ENDLOCAL
