#!/bin/bash
echo "Setting up environment for sandbox..."

# Initialize Git submodules
git submodule update --init --recursive

echo "Environment setup completed!"