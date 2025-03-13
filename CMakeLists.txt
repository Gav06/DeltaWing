cmake_minimum_required(VERSION 3.31.4)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")

cmake_policy(SET CMP0072 NEW)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

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

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION "${LIBRARY_DEPENDENCIES}/libglfw3.a")
# message(STATUS "GLFW Library: ${GLFW_LIBRARY}")


target_link_libraries(${PROJECT_NAME} 
    glfw3
    OpenGL::GL
    gdi32
    user32
    shell32
    advapi32
    ws2_32
)

# Enable optimizations
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
