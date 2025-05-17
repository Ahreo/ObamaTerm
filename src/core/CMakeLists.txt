# grab files from src
FILE(GLOB_RECURSE SRC "src/*")

message(STATUS "Building core files")

add_library(core ${SRC})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(core PUBLIC serialport-static olcPixelGameEngine freetype)
