#include <cstdio>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ObamaTerm.h"
using namespace OT;

#include "Constants.h"

int main() {
    ObamaTerm terminal;
	if (terminal.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, PIXEL_WIDTH, PIXEL_HEIGHT)) {
		terminal.Start();
	}

	return 0;
}