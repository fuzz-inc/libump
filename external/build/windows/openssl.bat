set CURDIR=%~dp0

pushd "%CURDIR%..\..\openssl"

call ppm install dmake
perl Configure VC-WIN32 --prefix="%CURDIR%..\..\opt\windows\openssl" --openssldir="%CURDIR%..\..\opt\ssl"
nmake
nmake test
nmake install

popd
