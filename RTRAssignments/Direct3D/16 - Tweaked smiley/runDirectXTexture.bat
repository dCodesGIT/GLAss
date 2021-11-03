:: Clear cmd
CLS
:: Clean the project (delete .obj and .exe)
@echo off
	del %1.obj
	del %1.exe
echo on
:: Build the project
cl.exe /c /EHsc %1.cpp
@echo off
	if not %ERRORLEVEL% == 0 goto finish
echo on
link.exe %1.obj ../Include/Icon/WinIcon.res /NODEFAULTLIB:msvcrt.lib
@echo off
	if not %ERRORLEVEL% == 0 goto finish
echo on
%1.exe
:finish
