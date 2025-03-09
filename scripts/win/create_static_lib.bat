@echo off
echo Creating static library for sandbox...

:: Create build directory if it doesn't exist
if not exist build mkdir build
cd build

:: Detect Visual Studio 2022
set GENERATOR="Visual Studio 17 2022"
echo Using %GENERATOR%...

:: Build static library
cmake -G %GENERATOR% -DBUILD_MODE=LIBRARY -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox_lib

echo Static library created successfully!
cd ..