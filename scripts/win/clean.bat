@echo off
echo Cleaning build directory...

if exist build (
    rmdir /s /q build
    echo Build directory cleaned!
) else (
    echo No build directory found.
)

exit /b 0