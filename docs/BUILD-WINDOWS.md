# Windowsでビルドする

## OpenSSLをビルドする

### ActivePerlをインストールする

https://www.activestate.com/activeperl

### NASMをインストールする

http://www.nasm.us/

インストールしたあとnasm.exe(たぶんC:\Users\<USERNAME>\AppData\Local\bin\NASMあたり)にPATHを通しておく

### OpenSSLをビルド

開発者コマンドプロンプトforVS2017を開き

    > cd <libump>\external\build\windows
    > openssl.bat

でビルド

途中dmake.exeが見つからないとかほざくが無視して大丈夫

## libumpをビルドする

VisualStudio2017で

  <libump>\build\vs2017\libump.sln

を開いてビルドする
