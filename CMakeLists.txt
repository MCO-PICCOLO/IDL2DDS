cmake_minimum_required(VERSION 3.10)

project(IDL2DDS VERSION 1.0)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add option to specify CycloneDDS installation directory
set(CYCLONEDDS_HOME "" CACHE PATH "Path to CycloneDDS installation")

# If CYCLONEDDS_HOME is provided, use it to find CycloneDDS
if(CYCLONEDDS_HOME)
    list(APPEND CMAKE_PREFIX_PATH ${CYCLONEDDS_HOME})
endif()

# Find CycloneDDS package
find_package(CycloneDDS REQUIRED)
find_package(yaml-cpp REQUIRED)  # YAML-CPP 라이브러리 추가

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

# 모든 IDL 파일 처리
file(GLOB IDL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/topic/*.idl)
set(GENERATED_FILES)

# 각 IDL 파일마다 코드 생성 규칙 추가
foreach(IDL_FILE ${IDL_FILES})
    get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)
    list(APPEND GENERATED_FILES 
         ${IDL_GEN_DIR}/${IDL_NAME}.c 
         ${IDL_GEN_DIR}/${IDL_NAME}.h)
    
    add_custom_command(
        OUTPUT ${IDL_GEN_DIR}/${IDL_NAME}.c ${IDL_GEN_DIR}/${IDL_NAME}.h
        COMMAND ${IDLC_EXECUTABLE} -o ${IDL_GEN_DIR} ${IDL_FILE}
        DEPENDS ${IDL_FILE}
        COMMENT "Generating code from ${IDL_NAME}.idl")
endforeach()

include_directories(
    include
    ${IDL_GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# C++와 C 파일 모두 포함
file(GLOB SOURCES
    src/*.cpp
    src/*.c
    src/topic/*.c
)

add_executable(IDL2DDS
    ${SOURCES}
    ${GENERATED_FILES}
)

target_link_libraries(IDL2DDS 
    CycloneDDS::ddsc
    yaml-cpp
)