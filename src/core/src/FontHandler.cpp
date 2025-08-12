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

        // Feed the chars
        for(char c: text) {
            currLine += c;
            if(c == '\n') {
                logicalLines.push(currLine);
                currLine.clear();
            }
        }
        
        // Render visual lines from logical Lines
        int scrollStart = scrollOffset;
        for(const std::string& line: logicalLines) {
            if(line.size() - scrollStart < 0) {
                
            }
            for(int i=0;i<line.size();i += charsPerLine) {

            }
        }

        // if visual lines isn't full, append what's in currLine

        char tempChar;
        size_t visualLineOffset = 0;

        for(size_t i=0; i<currLine.size(); i++) {
            tempChar = currLine[i];
            if(currLine.size() >= charsPerLine) {
                visualLines.push(currLine.substr(0, charsPerLine));
                visualLineOffset += charsPerLine;
            }

            if(tempChar == '\n') {
                visualLines.push(currLine.substr(visualLineOffset));
                logicalLines.push(currLine);
                currLine = currLine.substr(i);
                visualLineOffset = 0;
            }
        }

            printf("chars per line: %d\n", charsPerLine);
            printf("lines per screen: %d\n", linesPerScreen);

        for(size_t i=0;i<linesPerScreen;i++) {
            size_t lineIndex = scrollOffset + i;

            if(lineIndex < visualLines.size()) {
                for(int vIndex=0; vIndex < charsPerLine; vIndex++) {
                    char c = visualLines[lineIndex][vIndex];
                    Glpyh& g = LoadGlpyh(c);
                    pge->DrawSprite(g.advanceX * vIndex, g.advanceY * lineIndex, g.sprite.get(), 1);
                }
            }
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

        // init the char width and height for future use
        Glpyh& g = LoadGlpyh('?');
        charWidth = g.advanceX;
        charHeight = g.advanceY;
        charsPerLine = SCREEN_WIDTH / charWidth;
        linesPerScreen = SCREEN_WIDTH / charHeight;
    
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
