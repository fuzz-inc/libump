set CURDIR=%~dp0
set OPTDIR=%CURDIR%..\..\opt\windows

pushd "%CURDIR%..\..\openssl"

perl Configure VC-WIN32 --prefix="%OPTDIR%\openssl" --openssldir="%OPTDIR%\ssl"
nmake clean
nmake 
rem nmake test
nmake install

popd
