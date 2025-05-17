#include <cstdio>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ObamaTerm.h"
using namespace OT;

#define WIDTH   640
#define HEIGHT  480

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

void draw_bitmap(FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;

  /* for simplicity, we assume that `bitmap->pixel_mode' */
  /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

  for (i = x, p = 0; i < x_max; i++, p++) {
    for (j = y, q = 0; j < y_max; j++, q++) {
      if ( i < 0      || j < 0       ||
           i >= WIDTH || j >= HEIGHT )
        continue;

      image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}

void show_image(void) {
  int  i, j;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++)
      putchar( image[i][j] == 0 ? ' ': image[i][j] < 128 ? '+': '*' );
    putchar( '\n' );
  }
}

int main() {
    // ObamaTerm terminal;
	// if (terminal.Construct(256, 240, 2, 2))
	// 	terminal.Start();

	FT_Library 	  library;
	FT_Face 	  face;
	FT_GlyphSlot  slot;
	FT_Matrix     matrix;                 /* transformation matrix */
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error      error;

	char*         filename;
	std::string   text = "test text reo";

	double        angle;
	int           target_height;
	int           n, num_chars;
	
	error = FT_Init_FreeType(&library);
	if(error) {
		printf("An error occured during freetype lib init...");
	}
	std::string filePath = "../../src/main/EBGaramond-VariableFont_wght.ttf";
	error = FT_New_Face(library, filePath.c_str(), 0, &face);
	if(error == FT_Err_Unknown_File_Format) {
		printf("The font file could be opened and read, but format unsupported (?)");
	}
	else if(error) {
		printf("The font file couldn't be opened or read, or it's broken :(");
	}
	error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0);
	/* error handling omitted */

	/* cmap selection omitted;                                        */
	/* for simplicity we assume that the font contains a Unicode cmap */

	slot = face->glyph;

	/* set up matrix */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	/* the pen position in 26.6 cartesian space coordinates; */
	/* start at (300,200) relative to the upper left corner  */
	pen.x = 300 * 64;
	pen.y = ( target_height - 200 ) * 64;

	for ( n = 0; n < num_chars; n++ )
	{
		/* set transformation */
		FT_Set_Transform( face, &matrix, &pen );

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
		if (error)
		continue;                 /* ignore errors */

		/* now, draw to our target surface (convert position) */
		draw_bitmap(&slot->bitmap,
					slot->bitmap_left,
					target_height - slot->bitmap_top);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	show_image();

	FT_Done_Face    ( face );
	FT_Done_FreeType( library );
	return 0;
}