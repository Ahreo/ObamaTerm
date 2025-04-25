cmake_minimum_required(VERSION 3.22)
cmake_policy(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

project(ObamaTerm CXX)

find_library(gdiplus    gdiplus)
find_library(gdi32      gdi32)
find_library(opengl32   opengl32)
find_library(dwmapi     dwmapi)
find_library(user32     user32)
find_library(shlwapi    shlwapi)

add_subdirectory("src/lib")
add_subdirectory("src/core")
add_subdirectory("src/main")

set_target_properties(ObamaTermMain PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_BINARY_DIR}/debug)
set_target_properties(ObamaTermMain PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_BINARY_DIR}/release)
