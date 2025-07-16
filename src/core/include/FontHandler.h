#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include <string>
#include <fstream>
#include "etl/circular_buffer.h"

#include "Constants.h"

namespace OT {
typedef struct {
    std::unique_ptr<olc::Sprite> sprite; // cached sprite
    int advanceX;       // how far to advance the X Cursor
    int advanceY;       // how far to advance Y / Height of char
    int bearingX;       // left bearing
    int bearingY;       // right bearing
    int width;
    int height;
} Glpyh;

class FontHandler {
    public:
        FontHandler(const std::string& font, int size);
        FontHandler(const std::string& font, int size, const std::string& logName);
        ~FontHandler();

        void RenderTextAt(olc::PixelGameEngine* pge, const std::string& text, olc::Pixel color, int x, int y);
        void RenderText(olc::PixelGameEngine* pge, const std::string& text, olc::Pixel color);
        bool LoadFont(const std::string& font, int size);

    private:
        Glpyh& LoadGlpyh(char c);

        FT_Library ft = nullptr;
        FT_Face face = nullptr;
        
        std::unordered_map<char, Glpyh> glpyhCache;
        etl::circular_buffer<std::string, 1000> logicalLines;
        etl::circular_buffer<std::string, 1000> visualLines;

        std::string currLine = "";
        std::ofstream logFile;

        size_t charWidth;
        size_t charHeight;
        size_t charsPerLine;

        float scrollOffset = 0.0f;
        
};
}
#endif // FONT_HANDLER_H
