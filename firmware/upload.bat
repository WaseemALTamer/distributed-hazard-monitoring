@echo off
set "ROOT=%~dp0"
set "BUILD=%ROOT%build"
set "PORT=COM5"
set "FQBN=esp32:esp32:esp32"

echo Batch file location: %ROOT%
echo Build folder: %BUILD%
echo Upload port: %PORT%

if not exist "%BUILD%\main.ino.bootloader.bin" (
    echo Error: Compiled firmware files not found.
    echo Run build.bat first.
    exit /b 1
)

arduino-cli upload -v ^
  --port "%PORT%" ^
  --fqbn "%FQBN%" ^
  --input-dir "%BUILD%" ^
  "%ROOT%firmware\SensorNode"

if errorlevel 1 (
    echo Upload failed.
    exit /b 1
)

echo Upload successful.