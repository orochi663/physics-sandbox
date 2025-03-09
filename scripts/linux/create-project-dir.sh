#!/bin/bash
echo "Creating directory structure for sandbox..."

# Create directories
mkdir -p inc/ui
mkdir -p inc/graphics
mkdir -p inc/physics
mkdir -p inc/utils

mkdir -p src/ui
mkdir -p src/graphics
mkdir -p src/physics
mkdir -p src/utils


mkdir -p assets/meshes
mkdir -p assets/textures
mkdir -p assets/shaders

mkdir -p docs/design
mkdir -p docs/api

mkdir -p tests/test_ui
mkdir -p tests/test_physics
mkdir -p tests/test_graphics

mkdir -p scripts
mkdir -p config
mkdir -p examples

echo "Directory structure created successfully!"