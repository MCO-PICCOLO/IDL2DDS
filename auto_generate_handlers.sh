#!/bin/bash

# Script to automatically generate handlers for all IDL files that don't already have handlers
# This script scans all IDL files in src/topic and generates missing handlers

# Color escape sequences
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Scanning for IDL files without handlers...${NC}"

# Check if generate_handler.sh exists
if [ ! -f "generate_handler.sh" ]; then
    echo -e "${RED}Error: generate_handler.sh not found${NC}"
    exit 1
fi

# Make sure the script is executable
chmod +x generate_handler.sh

# Find all IDL files
IDL_FILES=$(find src/topic -name "*.idl" | sort)

# Counter for new handlers
NEW_HANDLERS=0

# Process each IDL file
for IDL_FILE in $IDL_FILES; do
    # Extract type name from the IDL file (first struct declaration)
    TYPE_NAME=$(grep -m 1 "struct" "$IDL_FILE" | awk '{print $2}')
    
    if [ -z "$TYPE_NAME" ]; then
        echo -e "${YELLOW}Warning: Could not extract type name from $IDL_FILE, skipping${NC}"
        continue
    fi
    
    # Check if handler already exists
    HANDLER_FILE="src/handlers/${TYPE_NAME}Handler.cpp"
    
    if [ -f "$HANDLER_FILE" ]; then
        echo -e "${GREEN}Handler already exists for $TYPE_NAME, skipping${NC}"
    else
        echo -e "${YELLOW}Generating new handler for $TYPE_NAME${NC}"
        # Call generate_handler.sh to create the handler
        ./generate_handler.sh --idl "$IDL_FILE"
        
        if [ $? -eq 0 ]; then
            NEW_HANDLERS=$((NEW_HANDLERS + 1))
        fi
    fi
done

# Run ensure_newlines.sh to fix any missing newlines in IDL files
if [ -f "ensure_newlines.sh" ]; then
    echo -e "${GREEN}Fixing newlines in IDL files...${NC}"
    chmod +x ensure_newlines.sh
    ./ensure_newlines.sh
fi

# Update HandlerGenerator.cpp to include all handlers
if [ $NEW_HANDLERS -gt 0 ]; then
    echo -e "${GREEN}Updating HandlerGenerator.cpp...${NC}"
    GENERATOR_FILE="src/HandlerGenerator.cpp"
    
    # Keep only the header and comment section
    sed -i '/^namespace/d; /^#include "handlers/d' $GENERATOR_FILE
    
    # Add includes for all handler files
    for IDL_FILE in $IDL_FILES; do
        TYPE_NAME=$(grep -m 1 "struct" "$IDL_FILE" | awk '{print $2}')
        if [ -n "$TYPE_NAME" ]; then
            HANDLER_FILE="src/handlers/${TYPE_NAME}Handler.cpp"
            if [ -f "$HANDLER_FILE" ]; then
                # Add include to HandlerGenerator.cpp
                sed -i "/\/\/ When adding a new IDL file:/i #include \"handlers/${TYPE_NAME}Handler.cpp\"" $GENERATOR_FILE
            fi
        fi
    done
fi

# Summary
echo -e "${GREEN}Process completed:${NC}"
echo -e "Found $(echo "$IDL_FILES" | wc -l) IDL files"
echo -e "Generated $NEW_HANDLERS new handlers"

if [ $NEW_HANDLERS -gt 0 ]; then
    echo -e "${YELLOW}Remember to review the generated handler files and update your YAML test file${NC}"
fi

exit 0
