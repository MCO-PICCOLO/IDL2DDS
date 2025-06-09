#!/bin/bash

# Script to ensure all IDL files end with a newline

# Find all IDL files
IDL_FILES=$(find src/topic -name "*.idl")

for file in $IDL_FILES; do
    # Check if file ends with newline
    if [ -n "$(tail -c1 "$file")" ]; then
        echo "Adding newline to end of $file"
        echo "" >> "$file"
    else
        echo "$file already has a newline at the end"
    fi
done

echo "All IDL files now have proper newlines"
