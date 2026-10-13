@echo off


set "TARGET=SensorNode"


set "ROOT=%~dp0"
set "SKETCH=%ROOT%%TARGET%\main"
set "LIBRARIES=%ROOT%libraries"
set "BUILD=%ROOT%build"

echo Batch file location: %ROOT%
echo Sketch folder: %SKETCH%
echo Libraries folder: %LIBRARIES%
echo Build folder: %BUILD%

arduino-cli compile -v ^
  --jobs 8 ^
  --fqbn esp32:esp32:esp32 ^
  --libraries "%LIBRARIES%" ^
  --build-path "%BUILD%" ^
  "%SKETCH%"

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build succeeded.