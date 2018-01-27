set VS2017DIR=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional
set ARC=x86
set SDK=10.0.15063.0
call "%VS2017DIR%\VC\Auxiliary\Build\vcvarsall.bat" %ARC% %SDK%

call %~dp0openssl.bat
