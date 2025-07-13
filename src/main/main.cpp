#include <cstdio>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ObamaTerm.h"
using namespace OT;

int main() {
    ObamaTerm terminal;
	if (terminal.Construct(256, 240, 2, 2)) {
		terminal.Start();
	}

	return 0;
}