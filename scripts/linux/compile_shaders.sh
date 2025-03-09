#!/bin/bash
echo "Compiling shaders..."

# Create shaders output directory in build
mkdir -p ../build/shaders

# Find glslc (assumes Vulkan SDK is installed)
GLSLC=$(which glslc)
if [ -z "$GLSLC" ]; then
    echo "Error: glslc not found. Ensure Vulkan SDK is installed and glslc is in PATH."
    exit 1
fi

# Compile all .vert and .frag files
for SHADER in ../assets/shaders/*.vert ../assets/shaders/*.frag; do
    if [ -f "$SHADER" ]; then
        echo "Compiling $SHADER..."
        $GLSLC "$SHADER" -o "../build/shaders/$(basename $SHADER).spv"
        if [ $? -ne 0 ]; then
            echo "Failed to compile $SHADER"
            exit 1
        fi
    fi
done

echo "Shader compilation completed!"