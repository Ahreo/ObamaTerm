add_executable(ObamaTermMain main.cpp)
target_link_libraries(ObamaTermMain olcPixelGameEngine serialport-static core freetype etl::etl user32 gdi32 opengl32 gdiplus shlwapi dwmapi stdc++fs)
