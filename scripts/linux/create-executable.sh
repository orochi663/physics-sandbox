#!/bin/bash
echo "Creating executable for sandbox..."

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

# Build executable
cmake -G "$GENERATOR" -DBUILD_MODE=EXECUTABLE -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target sandbox

echo "Executable created successfully!"
cd ..