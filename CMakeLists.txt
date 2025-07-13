cmake_minimum_required(VERSION 3.22)
cmake_policy(VERSION 3.22)

project(ObamaTerm CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release")

# --- System Libraries ---
find_library(gdiplus    gdiplus)
find_library(gdi32      gdi32)
find_library(opengl32   opengl32)
find_library(dwmapi     dwmapi)
find_library(user32     user32)
find_library(shlwapi    shlwapi)

# Path to fonts asset folder (source)
set(FONT_ASSET_DIR "${CMAKE_SOURCE_DIR}/src/assets/fonts" CACHE PATH "Path to font assets")

# Expose FONT_ASSET_DIR as a preprocessor macro to all targets
add_compile_definitions(FONT_DIR="${FONT_ASSET_DIR}")

# Set output asset folder (destination)
set(ASSETS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/assets")

# --- Asset copying setup using custom command ---
# Dummy file to track copy timestamp
set(ASSETS_STAMP "${ASSETS_OUTPUT_DIR}/.copied")

add_custom_command(
    OUTPUT "${ASSETS_STAMP}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/assets" "${ASSETS_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${ASSETS_STAMP}"
    COMMENT "Copying updated assets to ${ASSETS_OUTPUT_DIR}"
)

add_custom_target(copy_assets ALL
    DEPENDS "${ASSETS_STAMP}"
)

add_subdirectory("src/lib")
add_subdirectory("src/core")
add_subdirectory("src/main")

add_dependencies(ObamaTermMain copy_assets)

set_target_properties(ObamaTermMain PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_BINARY_DIR}/bin/debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_BINARY_DIR}/bin/release
)
