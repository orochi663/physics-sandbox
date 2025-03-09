@echo off
echo Compiling shaders...

:: Create shaders output directory in build
if not exist "%~dp0..\build\shaders" mkdir "%~dp0..\build\shaders"

:: Find glslc (assumes Vulkan SDK is installed)
for %%F in (glslc.exe) do set GLSLC=%%~$PATH:F
if "%GLSLC%"=="" (
    echo Error: glslc not found. Ensure Vulkan SDK is installed and glslc is in PATH.
    exit /b 1
)

:: Compile all .vert and .frag files
for %%F in (..\assets\shaders\*.vert ..\assets\shaders\*.frag) do (
    echo Compiling %%F...
    %GLSLC% "%%F" -o "..\build\shaders\%%~nF.spv"
    if errorlevel 1 (
        echo Failed to compile %%F
        exit /b 1
    )
)

echo Shader compilation completed!