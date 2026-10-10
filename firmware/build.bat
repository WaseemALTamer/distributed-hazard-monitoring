@echo off
set "ROOT=%~dp0"
set "LIBRARIES=%ROOT%libraries"

echo Batch file location: %ROOT%
echo Libraries folder: %LIBRARIES%



arduino-cli compile -v `
  --fqbn esp32:esp32:esp32 `
  --libraries %LIBRARIES% `
  --build-path ".\build" `
  "."