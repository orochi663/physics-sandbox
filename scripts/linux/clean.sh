#!/bin/bash
echo "Cleaning build directory..."

if [ -d "build" ]; then
    rm -rf build
    echo "Build directory cleaned!"
else
    echo "No build directory found."
fi