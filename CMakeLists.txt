cmake_minimum_required(VERSION 3.28)

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
set( SOURCES 
	"src/engine.c" 
	"src/engine.h"
	"src/font.c"
	"src/font.h"
	"src/glad.c"
	"src/input.c"
	"src/input.h"
	"src/main.c"
	"src/renderer.c"
	"src/renderer.h"
	"src/scenes.h"
	"src/stb_image.h"
	"src/util.h"
	"src/util.c"
	"src/scenes/mainmenu.h"
)
add_executable(${PROJECT_NAME} ${SOURCES})

set(INCLUDE_DEPENDENCIES "${CMAKE_SOURCE_DIR}/include")
set(LIBRARY_DEPENDENCIES "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

message(STATUS "Library Path: ${LIBRARY_DEPENDENCIES}")
message(STATUS "Include Path: ${INCLUDE_DEPENDENCIES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DEPENDENCIES})

find_package(OpenGL REQUIRED)

# Resolve glfw from static lib depending on os
if (WIN32)
	set(GLFW_LIB "${LIBRARY_DEPENDENCIES}/libglfw3.lib")
elseif(UNIX)
	set(GLFW_LIB "${LIBRARY_DEPENDENCIES}/libglfw3.a")
else()
	message(FATAL "Building on an unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

target_link_libraries(${PROJECT_NAME} 
    ${GLFW_LIB} m
)

# Enable optimizations
target_compile_options(${PROJECT_NAME} PRIVATE -g)
