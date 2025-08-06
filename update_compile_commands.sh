#!/bin/bash
# Auto-regenerate compile_commands.json for better LSP support

# Change to project directory
cd "$(dirname "$0")"

# Clean and regenerate compile commands
echo "Updating compile_commands.json for better LSP support..."
make clean > /dev/null 2>&1
bear -- make > /dev/null 2>&1

if [ -f "compile_commands.json" ]; then
    echo "✅ compile_commands.json updated successfully"
else
    echo "⚠️ Failed to generate compile_commands.json"
fi
