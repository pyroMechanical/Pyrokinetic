@echo off
mkdir ..\build
pushd ..\build
call cmake ..
popd
PAUSE