message(STATUS "Building imgui")
file(GLOB_RECURSE IMGUI_SRC "src/*")

add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler dwmapi)
