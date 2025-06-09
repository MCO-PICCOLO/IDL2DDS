#!/bin/bash
# filepath: /home/edo/2025/projects/dds-sender/list_dds_topics.sh

# Set directories to search
DIRECTORIES=("src/topic/idl" "src/topic")

echo "=== DDS Topics Defined in IDL Files ==="
echo ""

for DIR in "${DIRECTORIES[@]}"; do
    if [ -d "$DIR" ]; then
        echo "Searching in $DIR:"
        echo "-----------------------------"
        
        # Find all .idl files in the directory
        find "$DIR" -name "*.idl" | while read -r FILE; do
            echo "File: $(basename "$FILE")"
            
            # Extract struct definitions (each struct is a topic)
            grep -n "^struct" "$FILE" | while read -r line; do
                line_num=$(echo "$line" | cut -d':' -f1)
                struct_def=$(echo "$line" | cut -d':' -f2-)
                
                # Extract struct name
                struct_name=$(echo "$struct_def" | sed 's/struct \([a-zA-Z0-9_]*\).*/\1/')
                echo "  - Topic: $struct_name"
                
                # Get the 5 lines before the struct definition to look for description
                prev_lines=$(head -n "$line_num" "$FILE" | tail -n 5)
                description=$(echo "$prev_lines" | grep -o "description:.*" | head -n 1)
                
                if [ -n "$description" ]; then
                    # Clean up the description
                    clean_desc=$(echo "$description" | sed 's/description: //' | sed 's/\/\///g' | sed 's/^\s*//')
                    echo "    Description: $clean_desc"
                fi
            done
            echo ""
        done
    fi
done

echo "=== End of Topic List ==="