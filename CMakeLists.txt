cmake_minimum_required(VERSION 3.20)

# Create a project with name 'myapp'
set(TARGET_NAME OakDWebcam)
project(${TARGET_NAME})

# Generate 'compile_commands.json' for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Depthai dependency
find_package(depthai REQUIRED)
message(STATUS "Found depthai: ${depthai_DIR}")

# Dependencies (optional, only used for example)
# find_package(OpenCV REQUIRED)

# Add source files
add_executable("${TARGET_NAME}"
    src/main.cpp
)

# Add src as subdir since it has it's own package
add_subdirectory(src)

# Link with libraries
target_link_libraries(${TARGET_NAME}
    PUBLIC
        depthai::core
    PRIVATE
        oakd_webcam
)

# Set some errors
if(NOT MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Werror=return-type>)
endif()

# Set compiler features (c++17)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)
