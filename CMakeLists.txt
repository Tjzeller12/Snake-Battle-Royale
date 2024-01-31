cmake_minimum_required(VERSION 3.0)
project(SnakeBR)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Add executable
file(GLOB SOURCES "src/cpp/*.cpp")
add_executable(SnakeBR ${SOURCES})

# If you're using the static library
target_link_libraries(SnakeBR "${PROJECT_SOURCE_DIR}/lib/libraylib.a")

# Link against macOS frameworks
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(SnakeBR "${COCOA_LIBRARY}" "${IOKIT_LIBRARY}" "${COREVIDEO_LIBRARY}")
endif()
