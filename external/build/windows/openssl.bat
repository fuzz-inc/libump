set MASM=%ProgramFiles%\NASM
echo %PATH%|find "%MASM%">NUL
if errorlevel 1 set PATH=%MASM%;%PATH%

set CURDIR=%~dp0
set OPTDIR=%CURDIR%..\..\opt\windows

pushd "%CURDIR%..\..\openssl"

perl Configure VC-WIN32 --prefix="%OPTDIR%\openssl" --openssldir="%OPTDIR%\ssl"
nmake clean
nmake 
rem nmake test
nmake install

popd
