@echo off

git submodule update --init --recursive

set build_dir=./build

if not exist %build_dir% (mkdir "%build_dir%")

cmake -B %build_dir% -G "Visual Studio 17 2022" -A "x64" -T "v143"

pause
@echo on
