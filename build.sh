#!/bin/bash

# Chess Game Build Script
echo "Building Chess-bluetooth game..."

# Clean previous build
echo "Cleaning previous build..."
make clean 2>/dev/null

# Generate Makefile
echo "Generating Makefile..."
qmake

# Build the project
echo "Building project..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "Run './Chess-bluetooth' to start the game"
else
    echo "❌ Build failed!"
    exit 1
fi
