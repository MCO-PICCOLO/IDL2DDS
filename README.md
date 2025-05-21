# CycloneDDS Sender

A DDS message publisher application using Eclipse CycloneDDS. This project implements a DDS (Data Distribution Service) sender that publishes messages on a specific topic.

## Project Structure

```
cyclonedds-sender
├── src
│   ├── main.c                 # Entry point of the application
│   ├── publisher.c            # Implementation of the DDS publisher
│   ├── publisher.h            # Header file for publisher functions
│   └── topic
│       ├── topic_definition.idl # IDL file defining the topic structure
│       ├── topic_publisher.c   # Topic-specific publisher logic
│       └── topic_publisher.h   # Header file for topic publisher functions
├── include
│   └── common_types.h         # Common data types and constants
├── build
│   └── CMakeLists.txt         # Build configuration for the project
├── CMakeLists.txt             # Main CMake configuration file
├── Dockerfile                 # Docker container configuration
├── docker-compose.yml         # Docker Compose configuration
├── build.sh                   # Build script for local and Docker builds
└── README.md                  # Project documentation
```

## Dependencies

- CMake 3.10 or higher
- C compiler (GCC or Clang)
- Eclipse CycloneDDS library
- Docker (optional, for containerized builds)

## Building the Project

### Using the Build Script

The project includes a build script that makes building easier:

```bash
# Build locally
./build.sh

# Build and run
./build.sh --run

# Build with Docker
./build.sh --docker

# Build with Docker and run
./build.sh --docker --run

# Clean build
./build.sh --clean
```

### Manual Build

To build the project manually:

```bash
mkdir -p build
cd build
cmake .. -DCYCLONEDDS_HOME=/path/to/cyclonedds  # Optional: specify CycloneDDS path
cmake --build .
```

## Running the DDS Sender

After building the project:

```bash
# If built locally
./build/cyclonedds_sender

# If using Docker
docker run --rm cyclonedds-sender

# If using Docker Compose
docker-compose up
```

## Docker Support

The project includes Docker support for easy deployment:

```bash
# Build the Docker image
docker build -t cyclonedds-sender .

# Run the container
docker run --rm cyclonedds-sender

# Using Docker Compose
docker-compose up --build
```

## Topic Definition

The DDS topic is defined in `src/topic/topic_definition.idl`:

```idl
module SenderData {
    struct Message {
        long id;
        string content;
        float value;
        long timestamp;
    };
};
```

## Configuration

You can customize the DDS configuration by setting the `CYCLONEDDS_URI` environment variable:

```bash
export CYCLONEDDS_URI='<CycloneDDS><Domain><Id>0</Id></Domain></CycloneDDS>'
```

## Contributing

Feel free to contribute to this project by submitting issues or pull requests. Your contributions are welcome!

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

## Acknowledgments

- Eclipse CycloneDDS: https://github.com/eclipse-cyclonedds/cyclonedds