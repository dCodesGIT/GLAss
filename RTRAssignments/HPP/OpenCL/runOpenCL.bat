:: Clear cmd
CLS

:: Changing directory to %1 (specified folder)
cd %1

:: Clean directory
@echo off
IF EXIST %2.obj ( del %2.obj )
IF EXIST %2.exe ( del %2.exe )
IF EXIST *.txt  ( del *.txt )
@echo on

:: Compile the OpenCL application
cl.exe /c /EHsc %2.%3
	@echo off
	if NOT %errorlevel% == 0 ( goto :end )
	@echo on

:: Link the OpenCL application only if it compiles successfully
link.exe %2.obj /libpath "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\OpenCL.lib"
	@echo off
	if NOT %errorlevel% == 0 ( goto :end )
	@echo on

:: Run the OpenCL application only if it builds successfully
%2.exe

:: Delete object file since not required further
@echo off
del %2.obj
@echo on

:: Returning back to parent directory
:end
cd ..
