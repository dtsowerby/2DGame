project(Game)
cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Define compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11 /O2 /Zi /W4")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} /std:c11 /O2 /Zi /W4")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/lib/stb
    ${PROJECT_SOURCE_DIR}/lib/HandmadeMath
    ${PROJECT_SOURCE_DIR}/lib/glfw/include/GLFW
    ${PROJECT_SOURCE_DIR}/lib/glad/include
    ${PROJECT_SOURCE_DIR}/lib/glad/include/glad
    ${PROJECT_SOURCE_DIR}/lib/openal-soft/include
    ${PROJECT_SOURCE_DIR}/lib/openal-soft/common
    ${PROJECT_SOURCE_DIR}/lib/libsndfile/include
    ${PROJECT_SOURCE_DIR}/lib/steamworks_sdk_162/sdk/public
)

# Fix for cross platform?
link_directories(lib/steamworks_sdk_162/sdk/redistributable_bin/win64)

# Add subdirectories for external libraries
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glad EXCLUDE_FROM_ALL)
add_subdirectory(lib/openal-soft EXCLUDE_FROM_ALL)
add_subdirectory(lib/libsndfile EXCLUDE_FROM_ALL)

# Find all source files
file(GLOB_RECURSE SRC_FILES src/*.c src/networking/*.cpp)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw steam_api64 glad gdi32 alsoft.excommon alsoft.common alsoft.fmt sndfile)