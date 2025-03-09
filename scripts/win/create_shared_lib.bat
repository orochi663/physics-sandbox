@echo off
echo Creating shared library for sandbox...

:: Create build directory if it doesn't exist
if not exist build mkdir build
cd build

:: Detect Visual Studio 2022
set GENERATOR="Visual Studio 17 2022"
echo Using %GENERATOR%...

:: Build shared library
cmake -G %GENERATOR% -DBUILD_MODE=LIBRARY -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox_lib

echo Shared library created successfully!
cd ..