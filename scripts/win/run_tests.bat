@echo off
echo Running tests...

cd build
ctest --build-config Debug --output-on-failure
if errorlevel 1 (
    echo Tests failed!
    cd ..
    exit /b 1
)

echo Tests passed!
cd ..