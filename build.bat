@echo off
REM Windows double-click / cmd build. Needs g++ (MSYS2 MinGW) on PATH.
if exist omar.exe del omar.exe
g++ -std=c++17 -O1 -o omar.exe omarpp_all.cpp
if errorlevel 1 (
    echo Build FAILED
    exit /b 1
)
echo Build OK
omar.exe --version
