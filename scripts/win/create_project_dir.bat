@echo off
echo Creating directory structure for sandbox...

:: Create directories
mkdir inc
mkdir inc\ui
mkdir inc\graphics
mkdir inc\physics
mkdir inc\utils

mkdir src
mkdir src\ui
mkdir src\graphics
mkdir src\physics
mkdir src\utils

mkdir thirdparty

mkdir assets
mkdir assets\meshes
mkdir assets\textures
mkdir assets\shaders

mkdir docs
mkdir docs\design
mkdir docs\api

mkdir tests
mkdir tests\test_ui
mkdir tests\test_physics
mkdir tests\test_graphics

mkdir scripts
mkdir config
mkdir examples

echo Directory structure created successfully!