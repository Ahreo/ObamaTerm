#include <cstdio>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

extern "C" {
    #include "libserialport.h"
}

#include "ObamaTerm.h"
using namespace OT;

int main() {
    ObamaTerm terminal;
	if (terminal.Construct(256, 240, 4, 4))
		terminal.Start();
	return 0;
}