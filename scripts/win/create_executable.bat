@echo off
echo Creating executable for sandbox...

:: Create build directory if it doesn't exist
if not exist build mkdir build
cd build

:: Detect Visual Studio 2022
set GENERATOR="Visual Studio 17 2022"
echo Using %GENERATOR%...

:: Build executable
cmake -G %GENERATOR% -DBUILD_MODE=EXECUTABLE -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox

echo Executable created successfully!
cd ..