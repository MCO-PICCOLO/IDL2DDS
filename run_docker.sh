#!/bin/bash

# Script to run IDL2DDS in Docker Compose

# Color escape sequences
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building and running IDL2DDS with Docker Compose...${NC}"

# Check if a test case file was specified
if [ -n "$1" ]; then
    # Override the command in docker-compose.yml
    echo -e "${YELLOW}Using test file: $1${NC}"
    docker compose run dds-sender /app/IDL2DDS/build/IDL2DDS /app/IDL2DDS/test-cases/$1
else
    # Use the default command in docker-compose.yml
    docker compose up --build
fi
