set CURDIR=%~dp0
pushd %CURDIR%..\..\external\openssl
perl Configure VC-WIN32
nmake
nmake test
nmake install
popd
