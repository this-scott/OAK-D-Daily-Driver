cmake_minimum_required(VERSION 3.21)

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

#CREATING THE PROGRAM DISTRIBUTION

# generating the installation layout
install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
install(FILES dist/99-oakd-webcam.rules DESTINATION lib/udev/rules.d)
install(FILES dist/oakd-webcam.service DESTINATION lib/systemd/system)

# bundling dai items into the package, users don't need to install dai themselves
install(IMPORTED_RUNTIME_ARTIFACTS depthai::core DESTINATION lib/oakd-webcam)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/oakd-webcam")

# Cpack package details
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME oakd-webcam)
set(CPACK_PACKAGE_VERSION 0.6.0)
set(CPACK_PACKAGE_CONTACT "Scott Styslinger <this_scott@outlook.com>")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "v4l2loopback-dkms (>= 0.13.0)")
# calculates shared file dependencies
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/dist/postinst")
include(CPack)
