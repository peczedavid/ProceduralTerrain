@echo off
pushd ..\
call vendor\bin\premake\premake5.exe vs2022
popd
if %ERRORLEVEL% NEQ 0 (
	pause
)