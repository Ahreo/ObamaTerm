#include "FontHandler.h"
#include <iostream>

namespace OT {
    FontHandler::FontHandler(const std::string& font, int size) {
        if(FT_Init_FreeType(&ft)) {
            fprintf(stderr, "Error initing FreeType library\n");
            ft = nullptr;
            return;
        }
    
        if(!LoadFont(font, size)) {
            fprintf(stderr, "Failed to load font in ctor!\n");
        }
    }

    FontHandler::FontHandler(const std::string& font, int size, const std::string& logName) : logFile(logName) {
        if(FT_Init_FreeType(&ft)) {
            fprintf(stderr, "Error initing FreeType library\n");
            ft = nullptr;
            return;
        }
    
        if(!LoadFont(font, size)) {
            fprintf(stderr, "Failed to load font in ctor!\n");
        }
    }
    
    FontHandler::~FontHandler() {
        if(face) {
            FT_Done_Face(face);
            face = nullptr;
        }
        if(ft) {
            FT_Done_FreeType(ft);
            ft = nullptr;
        }
    }
    
    void FontHandler::RenderTextAt(olc::PixelGameEngine* pge, const std::string& text, olc::Pixel color, int x, int y) {
        int penX = x;
    
        for(char c: text) {
            Glpyh& g = LoadGlpyh(c);
            pge->DrawSprite(penX + g.bearingX, y - g.bearingY, g.sprite.get(), 1);
            penX += g.advanceX;
        }
    }

    void FontHandler::RenderText(olc::PixelGameEngine* pge, const std::string& text, olc::Pixel color) {
        int x = 0;
        int y = charHeight;
        int penX = x;
    
        for(char c: text) {
            Glpyh& g = LoadGlpyh(c);
            pge->DrawSprite(penX + g.bearingX, y - g.bearingY, g.sprite.get(), 1);
            penX += g.advanceX;
        }
    }
    
    bool FontHandler::LoadFont(const std::string& font, int size) {
        if(!ft) {
            return false;
        }
    
        // prepare the face for a new font
        if(face) {
            FT_Done_Face(face);
            face = nullptr;
        }
    
        if(FT_New_Face(ft, font.c_str(), 0, &face)) {
            fprintf(stderr, "Could not load new font face\n");
            return false;
        }
    
        FT_Set_Pixel_Sizes(face, 0, size);
        glpyhCache.clear(); // remove old glpyhs!

        Glpyh& g = LoadGlpyh('?');
        charWidth = g.advanceX;
        charHeight = g.advanceY;
        charsPerLine = SCREEN_WIDTH / charWidth;
    
        return true;
    }
    
    Glpyh& FontHandler::LoadGlpyh(char c) {
        if(glpyhCache.count(c)) {
            return glpyhCache.at(c);
        }
    
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "Failed to load glpyh %c \n", c);
            return glpyhCache['?'];
        }
    
        FT_GlyphSlot g = face->glyph;
    
        std::unique_ptr<olc::Sprite> s = std::make_unique<olc::Sprite>(g->bitmap.width, g->bitmap.rows);
        for(int y = 0; y < g->bitmap.rows; y++) {
            for(int x = 0; x < g->bitmap.width; x++) {
                uint8_t val = g->bitmap.buffer[y * g->bitmap.pitch + x];
                s->SetPixel(x, y, olc::Pixel(val, val, val, val));
            }
        }
    
        glpyhCache[c] = {
            std::move(s),
            g->advance.x >> 6,
            face->size->metrics.height >> 6,
            g->bitmap_left,
            g->bitmap_top,
            (int) g->bitmap.width,
            (int) g->bitmap.rows
        };
    
        return glpyhCache[c];
    }
}
