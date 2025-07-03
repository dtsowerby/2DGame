project(Game)
cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_STANDARD 11)

# Define compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11 /O2 /Zi /W4")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996")

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/lib/stb
    ${PROJECT_SOURCE_DIR}/lib/HandmadeMath
    ${PROJECT_SOURCE_DIR}/lib/glfw/include/GLFW
    ${PROJECT_SOURCE_DIR}/lib/glad/include
    ${PROJECT_SOURCE_DIR}/lib/glad/include/glad
)

# Add subdirectories for external libraries
add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)

# Find all source files
file(GLOB_RECURSE SRC_FILES src/*.c)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad gdi32)