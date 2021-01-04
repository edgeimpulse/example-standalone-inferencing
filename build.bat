@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
setlocal
REM go to the folder where this bat script is located
cd /d %~dp0

where /q mingw32-make
IF ERRORLEVEL 1 (
    GOTO MISSINGMINGW32
)

where /q gcc
IF ERRORLEVEL 1 (
    GOTO MISSINGMINGW32
)

echo "Building standalone classifier"

mingw32-make CC=gcc -j

echo "Building standalone classifier OK"

exit /b 0

:MISSINGMINGW32
echo Cannot find 'mingw32-make' or 'gcc' in your PATH. Install MinGW-W64 to continue.
echo Download link: https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download
echo Or see https://tls.mbed.org/kb/compiling-and-building/compiling-mbedtls-in-mingw for further instructions.
@pause
exit /b 1
