# IDL2DDS: Modular DDS Publisher

A modular and extensible DDS (Data Distribution Service) publisher application using Eclipse CycloneDDS. This project implements an extensible DDS publisher that can automatically handle different topic types defined in IDL files.

## Project Structure

```
IDL2DDS/
├── src/
│   ├── main.cpp                 # Entry point of the application
│   ├── publisher.cpp            # Generic DDS publisher implementation
│   ├── publisher.h              # Header file for publisher functions
│   ├── HandlerGenerator.cpp     # Includes all topic handler implementations
│   ├── handlers/                # Topic-specific handler implementations
│   │   ├── HandlerTemplate.txt  # Template for generating new handlers
│   │   ├── ADASObstacleDetectionIsWarningHandler.cpp
│   │   ├── VehicleStatusHandler.cpp
│   │   └── BodyLightsHeadLampStatusHandler.cpp
│   └── topic/                   # IDL files defining topic structures
│       ├── ADASObstacleDetection.idl
│       ├── BodyLightsHeadLamp.idl
│       └── VehicleStatus.idl
├── include/
│   ├── TopicHandler.h           # Interface for topic handlers
│   ├── TopicHandlerRegistry.h   # Registry for automatic handler registration
│   └── common_types.h           # Common data types and constants
├── test-cases/
│   └── test-message.yaml        # Test data for different topics
├── CMakeLists.txt               # CMake configuration file
├── generate_handler.sh          # Script to generate handlers from IDL files
├── build.sh                     # Build script
├── run_test.sh                  # Script to run tests
└── README.md                    # Project documentation
```

## Dependencies

- CMake 3.10 or higher
- C++17 compiler (GCC or Clang)
- Eclipse CycloneDDS library
- yaml-cpp library

## Features

- **Modular Design**: Each topic type has its own handler class
- **Automatic Registration**: Topic handlers are automatically registered with a central registry
- **Easy Extension**: Add new IDL files and generate handlers without changing core code
- **Generic Publisher**: The main publisher code is topic-independent
- **YAML Configuration**: Configure test messages using YAML files

## Adding a New Topic Type

### 자동 생성 (Automatic Generation)

새로운 DDS 토픽을 추가할 때는 IDL 파일만 추가하면 빌드 시 자동으로 핸들러가 생성됩니다:

1. **Create an IDL file** in `src/topic/` directory (e.g., `NewTopic.idl`)
2. **Run the build** script or Docker compose - handlers will be generated automatically
3. **Edit the generated handler** to properly map YAML fields to your type's fields
4. **Add test messages** to your YAML test file

### 수동 생성 (Manual Generation)

특정 IDL 파일에 대한 핸들러를 수동으로 생성할 수도 있습니다:

1. **Create an IDL file** in `src/topic/` directory (e.g., `NewTopic.idl`)

2. **Generate a handler** using the provided script:
   ```bash
   # Generate handler from existing IDL file
   ./generate_handler.sh --idl src/topic/NewTopic.idl
   
   # Or create both IDL and handler at once
   ./generate_handler.sh --new NewTopic
   ```

3. **Edit the generated handler** to properly map YAML fields to your type's fields

4. **Add test messages** to your YAML test file:
   ```yaml
   NewTopic:
     start_time: 0
     end_time: 60
     frequency: 1
     data:
       - field1: value1
         field2: value2
   ```

5. **Build and run** the application with your new topic

## Building the Project

```bash
# Build the project
./build.sh

# Clean and rebuild
./rebuild.sh
```

## Running Tests

```bash
# Run with the default test case
./run_test.sh

# Run with a specific test file
./run_test.sh test-cases/my-test.yaml
```

## Running with Docker Compose

You can also run the application using Docker Compose:

```bash
# Build and run with the default test case
./run_docker.sh

# Run with a specific test file
./run_docker.sh test-message.yaml
```

The Docker Compose configuration:
- Builds the project from source
- Uses the CycloneDDS configuration from cyclonedds.xml
- Mounts the test-cases directory for easy testing
- Uses host networking for DDS discovery

## Monitoring Topics

To list all available DDS topics on your network:
```bash
./list_dds_topics.sh
```

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