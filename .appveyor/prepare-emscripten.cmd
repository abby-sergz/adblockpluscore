@echo off

if "%Platform%"=="Win32" set bitness="32"

if "%Platform%"=="x64" set bitness="64"

cd third_party

appveyor DownloadFile https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz -FileName emsdk-portable.tar.gz

7z x emsdk-portable.tar.gz -so | 7z x -aoa -si -ttar -othird_party/emsdk-portable

emsdk update
emsdk install sdk-1.37.3-%bitness%bit
emsdk activate sdk-1.37.3-%bitness%bit

