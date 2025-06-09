#!/bin/bash

# Script to generate C++ handler files for DDS topics from IDL files

# Color escape sequences
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to show usage
show_usage() {
    echo -e "Usage: $0 <options>"
    echo -e "Options:"
    echo -e "  --idl <path-to-idl-file> [topic-name]   Generate handler from existing IDL file"
    echo -e "  --new <type-name> [topic-name]          Generate both IDL and handler files"
    echo -e "  --help                                 Show this help message"
    echo -e "If topic-name is not provided, it will be derived from the type name"
}

# Check if no arguments provided
if [ $# -lt 1 ]; then
    show_usage
    exit 1
fi

TEMPLATE_FILE="src/handlers/HandlerTemplate.txt"
IDL_TEMPLATE_FILE="src/topic/IDLTemplate.txt"

# Process command line arguments
case "$1" in
    --idl)
        if [ $# -lt 2 ]; then
            echo -e "${RED}Error: Missing IDL file path${NC}"
            show_usage
            exit 1
        fi
        
        IDL_FILE="$2"
        OPERATION="from_idl"
        
        # Check if the IDL file exists
        if [ ! -f "$IDL_FILE" ]; then
            echo -e "${RED}Error: IDL file '$IDL_FILE' not found${NC}"
            exit 1
        fi
        
        # Extract type name from the IDL file (first struct declaration)
        TYPE_NAME=$(grep -m 1 "struct" "$IDL_FILE" | awk '{print $2}')
        
        if [ -z "$TYPE_NAME" ]; then
            echo -e "${RED}Error: Could not extract type name from IDL file${NC}"
            exit 1
        fi
        
        # Determine topic name - use the third argument if provided, otherwise use the type name
        if [ -n "$3" ]; then
            TOPIC_NAME="$3"
        else
            TOPIC_NAME="$TYPE_NAME"
        fi
        ;;
        
    --new)
        if [ $# -lt 2 ]; then
            echo -e "${RED}Error: Missing type name${NC}"
            show_usage
            exit 1
        fi
        
        TYPE_NAME="$2"
        OPERATION="create_new"
        
        # Determine topic name - use the third argument if provided, otherwise use the type name
        if [ -n "$3" ]; then
            TOPIC_NAME="$3"
        else
            TOPIC_NAME="$TYPE_NAME"
        fi
        
        # Create IDL file path
        IDL_FILE="src/topic/${TYPE_NAME}.idl"
        
        # Check if IDL file already exists
        if [ -f "$IDL_FILE" ]; then
            echo -e "${YELLOW}Warning: IDL file '$IDL_FILE' already exists. Overwrite? (y/n)${NC}"
            read -r OVERWRITE
            if [[ ! "$OVERWRITE" =~ ^[Yy]$ ]]; then
                echo "Operation cancelled."
                exit 0
            fi
        fi
        ;;
        
    --help)
        show_usage
        exit 0
        ;;
        
    *)
        # For backward compatibility, assume the first argument is an IDL file
        IDL_FILE="$1"
        OPERATION="from_idl"
        
        # Check if the IDL file exists
        if [ ! -f "$IDL_FILE" ]; then
            echo -e "${RED}Error: IDL file '$IDL_FILE' not found${NC}"
            exit 1
        fi
        
        # Extract type name from the IDL file (first struct declaration)
        TYPE_NAME=$(grep -m 1 "struct" "$IDL_FILE" | awk '{print $2}')
        
        if [ -z "$TYPE_NAME" ]; then
            echo -e "${RED}Error: Could not extract type name from IDL file${NC}"
            exit 1
        fi
        
        # Determine topic name - use the second argument if provided, otherwise use the type name
        if [ -n "$2" ]; then
            TOPIC_NAME="$2"
        else
            TOPIC_NAME="$TYPE_NAME"
        fi
        ;;
esac

# Check if template exists
if [ ! -f "$TEMPLATE_FILE" ]; then
    echo -e "${RED}Error: Handler template file '$TEMPLATE_FILE' not found${NC}"
    exit 1
fi

if [ "$OPERATION" = "create_new" ]; then
    if [ ! -f "$IDL_TEMPLATE_FILE" ]; then
        echo -e "${RED}Error: IDL template file '$IDL_TEMPLATE_FILE' not found${NC}"
        exit 1
    fi
fi

HANDLER_FILE="src/handlers/${TYPE_NAME}Handler.cpp"

# Check if handler file already exists
if [ -f "$HANDLER_FILE" ]; then
    echo -e "${YELLOW}Warning: Handler file '$HANDLER_FILE' already exists. Overwrite? (y/n)${NC}"
    read -r OVERWRITE
    if [[ ! "$OVERWRITE" =~ ^[Yy]$ ]]; then
        echo "Operation cancelled."
        exit 0
    fi
fi

# Generate IDL file if needed
if [ "$OPERATION" = "create_new" ]; then
    echo -e "${GREEN}Generating IDL file for type '$TYPE_NAME'...${NC}"
    sed -e "s/{{TYPE_NAME}}/$TYPE_NAME/g" "$IDL_TEMPLATE_FILE" > "$IDL_FILE"
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Failed to generate IDL file${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}Successfully generated IDL file: $IDL_FILE${NC}"
    echo -e "${YELLOW}Remember to edit the IDL file to define your data fields.${NC}"
fi

# Generate the handler file from the template
echo -e "${GREEN}Generating handler for type '$TYPE_NAME' with topic name '$TOPIC_NAME'...${NC}"

sed -e "s/{{TYPE_NAME}}/$TYPE_NAME/g" -e "s/{{TOPIC_NAME}}/$TOPIC_NAME/g" "$TEMPLATE_FILE" > "$HANDLER_FILE"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Successfully generated handler file: $HANDLER_FILE${NC}"

    # Now update the HandlerGenerator.cpp file to include the new handler
    GENERATOR_FILE="src/HandlerGenerator.cpp"
    
    # Check if HandlerGenerator.cpp exists
    if [ ! -f "$GENERATOR_FILE" ]; then
        echo -e "${RED}Error: HandlerGenerator.cpp not found${NC}"
        exit 1
    fi

    # Check if handler is already included
    if grep -q "#include \"handlers/${TYPE_NAME}Handler.cpp\"" "$GENERATOR_FILE"; then
        echo -e "${YELLOW}Handler already included in HandlerGenerator.cpp${NC}"
    else
        # Add include before the comment marker
        sed -i "/\/\/ When adding a new IDL file:/i #include \"handlers/${TYPE_NAME}Handler.cpp\"" "$GENERATOR_FILE"
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}Updated HandlerGenerator.cpp with new handler${NC}"
        else
            echo -e "${RED}Failed to update HandlerGenerator.cpp${NC}"
        fi
    fi

else
    echo -e "${RED}Failed to generate handler file${NC}"
    exit 1
fi

echo -e "${GREEN}Generation complete!${NC}"
if [ "$OPERATION" = "create_new" ]; then
    echo -e "Next steps:"
    echo -e "1. Edit ${YELLOW}$IDL_FILE${NC} to define your data structure"
    echo -e "2. Run ${YELLOW}./ensure_newlines.sh${NC} to fix any missing newlines"
    echo -e "3. Edit ${YELLOW}$HANDLER_FILE${NC} to implement field mapping from YAML to your data type"
    echo -e "4. Add test data to your YAML test file"
else
    echo -e "Remember to review and edit the generated handler to ensure proper field mapping."
fi
