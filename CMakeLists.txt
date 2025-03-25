cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

if (WIN32)
	message(STATUS "Using default Windows MinGW compiler paths")
	set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
	set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")
elseif(UNIX)
	message("Using default Linux gcc compiler path")
	set(CMAKE_C_COMPILER "/usr/bin/gcc")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
	message(STATUS "Building on an unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

cmake_policy(SET CMP0072 NEW)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

project(DeltaWing)


# Add our source code
file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

set(INCLUDE_DEPENDENCIES "${CMAKE_SOURCE_DIR}/include")
set(LIBRARY_DEPENDENCIES "${CMAKE_SOURCE_DIR}/lib")

message(STATUS "Library Path: ${LIBRARY_DEPENDENCIES}")
message(STATUS "Include Path: ${INCLUDE_DEPENDENCIES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DEPENDENCIES})

find_package(OpenGL REQUIRED)

# Find the package - this works if GLFW provides a config file
find_package(glfw3 QUIET)

# If the above doesn't work, use a more manual approach
if (NOT glfw3_FOUND)
    find_library(GLFW_LIB 
                 NAMES glfw3 glfw
                 PATHS ${LIBRARY_DEPENDENCIES}
                 NO_DEFAULT_PATH)
                 
    if (GLFW_LIB)
        add_library(glfw3 STATIC IMPORTED)
        set_target_properties(glfw3 PROPERTIES
            IMPORTED_LOCATION "${GLFW_LIB}")
    else()
        message(FATAL_ERROR "Could not find GLFW library")
    endif()
endif()


target_link_libraries(${PROJECT_NAME} 
    glfw3 m
)

# Enable optimizations
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
