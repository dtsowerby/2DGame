project(Game)
cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    message(STATUS "Configuring for Emscripten (WebAssembly)")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/web)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=134217728 --emrun -s FULL_ES3=1"
    )

    # Include dirs (minimal needed)
    include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/lib/stb
        ${PROJECT_SOURCE_DIR}/lib/HandmadeMath
        ${PROJECT_SOURCE_DIR}/lib/glad/include
        ${PROJECT_SOURCE_DIR}/lib/glad/include/glad
        ${PROJECT_SOURCE_DIR}/lib/ALHelpers
        #${PROJECT_SOURCE_DIR}/lib/openal-soft/include
        ${PROJECT_SOURCE_DIR}/lib/openal-soft/common
        ${PROJECT_SOURCE_DIR}/lib/libsndfile/include
    )

    #add_subdirectory(lib/openal-soft)
    #add_subdirectory(lib/libsndfile)

    file(GLOB_RECURSE SRC_FILES
        src/*.c
        lib/glad/src/*.c
        #src/networking/*.cpp
        lib/ALHelpers/*.c
    )

    add_executable(${PROJECT_NAME} ${SRC_FILES})

    # For Emscripten, don't link native Windows/OpenAL/etc.
    # target_link_libraries(${PROJECT_NAME} PRIVATE glfw steam_api64 glad gdi32 alsoft.excommon alsoft.common alsoft.fmt sndfile)
    target_link_libraries(${PROJECT_NAME} PRIVATE)
else()

# Define compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c17 /O2 /Zi /W4")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} /std:c17 /O2 /Zi /W4")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996")

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/lib/stb
    ${PROJECT_SOURCE_DIR}/lib/HandmadeMath
    ${PROJECT_SOURCE_DIR}/lib/glfw/include
    ${PROJECT_SOURCE_DIR}/lib/glad/include
    ${PROJECT_SOURCE_DIR}/lib/glad/include/glad
    ${PROJECT_SOURCE_DIR}/lib/ALHelpers
    ${PROJECT_SOURCE_DIR}/lib/openal-soft/include
    ${PROJECT_SOURCE_DIR}/lib/openal-soft/common
    ${PROJECT_SOURCE_DIR}/lib/libsndfile/include
    ${PROJECT_SOURCE_DIR}/lib/steamworks_sdk_162/sdk/public
    #${PROJECT_SOURCE_DIR}/lib/GameNetworkingSockets/include
)

# Fix for cross platform?
link_directories(lib/steamworks_sdk_162/sdk/redistributable_bin/win64)

# Add subdirectories for external libraries
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glad EXCLUDE_FROM_ALL)
add_subdirectory(lib/openal-soft EXCLUDE_FROM_ALL)
add_subdirectory(lib/libsndfile EXCLUDE_FROM_ALL)
#add_subdirectory(lib/GameNetworkingSockets EXCLUDE_FROM_ALL)

# Find all source files
file(GLOB_RECURSE SRC_FILES 
    src/*.c 
    src/networking/*.cpp 
    lib/ALHelpers/*.c
)

#set(CMAKE_EXE_LINKER_FLAGS "/ENTRY:wmainCRTStartup")

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw steam_api64 glad gdi32 alsoft.excommon alsoft.common alsoft.fmt sndfile)
endif()