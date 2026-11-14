#!/bin/bash
# Builds Omar++ from the single file omarpp_all.cpp and tells you exactly what happened.
cd "$(dirname "$0")"
rm -f omar omar.exe

echo "Building..."
g++ -std=c++17 -O1 -o omar omarpp_all.cpp
code=$?
echo "g++ exit code: $code"

# MinGW writes omar.exe when you ask for 'omar'
if [ -f omar.exe ]; then EXE=./omar.exe; elif [ -f omar ]; then EXE=./omar; else EXE=""; fi

if [ $code -eq 0 ] && [ -n "$EXE" ]; then
    echo "Build OK -> $EXE"
    $EXE --version
    echo "Now run:  $EXE examples/hello.omar"
    exit 0
fi

echo
echo "Build did not produce a program in this folder."
echo "Trying again in a folder outside OneDrive (to test if OneDrive/antivirus is the cause)..."
TMPDIR_OUT="${TMPDIR:-/tmp}/omarpp_build"
mkdir -p "$TMPDIR_OUT"
g++ -std=c++17 -O1 -o "$TMPDIR_OUT/omar" omarpp_all.cpp
code2=$?
echo "g++ exit code: $code2"
if [ -f "$TMPDIR_OUT/omar.exe" ] || [ -f "$TMPDIR_OUT/omar" ]; then
    echo "It works outside this folder. Run it with:"
    ls "$TMPDIR_OUT"/omar* 
    echo "Example:  $TMPDIR_OUT/omar.exe examples/hello.omar"
else
    echo "g++ itself is failing. Open the 'MSYS2 MinGW 64-bit' terminal and try again."
fi
exit 1
