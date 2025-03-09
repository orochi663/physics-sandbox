#!/bin/bash
echo "Creating static library for sandbox..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Detect platform and set generator
if [[ "$OSTYPE" == "darwin"* ]]; then
    GENERATOR="Xcode"
    echo "Using Xcode generator on macOS..."
else
    GENERATOR="Unix Makefiles"
    echo "Using Unix Makefiles generator on Linux..."
fi

# Build static library
cmake -G "$GENERATOR" -DBUILD_MODE=LIBRARY -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox_lib

echo "Static library created successfully!"
cd ..