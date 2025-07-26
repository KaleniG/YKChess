@echo off
pushd ..\
call Scripts\Premake\Windows\premake5.exe --file=premake5.lua vs2022
if %errorlevel% neq 0 ( pause )
popd