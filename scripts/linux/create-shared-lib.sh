#!/bin/bash
echo "Creating shared library for sandbox..."

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

# Build shared library
cmake -G "$GENERATOR" -DBUILD_MODE=LIBRARY -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox_lib

echo "Shared library created successfully!"
cd ..