#!/bin/bash
echo "Running tests..."

cd build
ctest --build-config Debug --output-on-failure
if [ $? -ne 0 ]; then
    echo "Tests failed!"
    cd ..
    exit 1
fi

echo "Tests passed!"
cd ..