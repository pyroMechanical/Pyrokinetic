@echo off
pushd ..\
call ext\bin\premake\premake5.exe vs2019
popd
PAUSE
