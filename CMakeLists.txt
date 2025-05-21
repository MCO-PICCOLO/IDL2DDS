cmake_minimum_required(VERSION 3.10)

project(cyclonedds_sender VERSION 1.0)

set(CMAKE_C_STANDARD 11)

# Add option to specify CycloneDDS installation directory
set(CYCLONEDDS_HOME "" CACHE PATH "Path to CycloneDDS installation")

# If CYCLONEDDS_HOME is provided, use it to find CycloneDDS
if(CYCLONEDDS_HOME)
    list(APPEND CMAKE_PREFIX_PATH ${CYCLONEDDS_HOME})
endif()

# Find CycloneDDS package
find_package(CycloneDDS REQUIRED)

# Generate code from IDL
find_program(IDLC_EXECUTABLE idlc PATHS 
    ${CYCLONEDDS_HOME}/bin
    /usr/bin
    /usr/local/bin
)

if(NOT IDLC_EXECUTABLE)
    message(FATAL_ERROR "Cannot find 'idlc' executable for CycloneDDS code generation")
endif()

# Set the output directory for generated files
set(IDL_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${IDL_GEN_DIR})

# Process IDL file
set(IDL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/topic/ADASObstacleDetection.idl)
set(GENERATED_FILES ${IDL_GEN_DIR}/ADASObstacleDetection.c ${IDL_GEN_DIR}/ADASObstacleDetection.h)

add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND ${IDLC_EXECUTABLE} -o ${IDL_GEN_DIR} ${IDL_FILE}
    DEPENDS ${IDL_FILE}
    COMMENT "Generating code from ADASObstacleDetection.idl")

include_directories(
    include
    ${IDL_GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB SOURCES
    src/*.c
    src/topic/*.c
)

add_executable(cyclonedds_sender
    ${SOURCES}
    ${GENERATED_FILES}
)

target_link_libraries(cyclonedds_sender CycloneDDS::ddsc)