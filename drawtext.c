// LLVM-MOS Lynx text drawing functions
// JH 2025
#include "lynxlib.h"
#include "fontData.h"

// Font data (64 sprites)
unsigned char *charData[64];

struct SCB charSCB = {	BPP_2 | TYPE_BACKNONCOLL,
						LITERAL | REHV, 		// literal, scale XY
						0,
						0, (char *)fontData,
						0x0, 0x0, 0x100, 0x100,	
						{ 0x0D, 0xEF, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },	    // NB: pen 1, 2, 3 mapped!
						0 };
// Note: LLVM-MOS tries to put charSCB on zero page if we try to use it directly inside init_font etc !

// Initialise text drawing
void TextInit()
{
	unsigned char *p = (unsigned char *)fontData;

	for (unsigned int i = 0; i < 1216; i++)
		{
		charData[i] = p;
		p = p + 19;
		}
		
	// Init "character" sprite	
	// (2-bit background noncollideable sprite using packed sprite data)
	charSCB.data = (char *)fontData;								// init sprite data to a valid pointer
	charSCB.next = 0;			//spriteSCBs[0].addr + 1					// +1 to skip byte 0 of the SCB (collresult) 
 	// TODO - Also need to map 2-bit colour indices to 16-colour palette 
//	fontSCB.penpal[0] = 0x0A;
//	fontSCB.penpal[1] = 0xBF	;
}

// Draw a single character
void TextDrawChar(unsigned char charnum, int x, int y)
{
    charSCB.hpos = x;
    charSCB.vpos = y;
    charSCB.data = (char *)charData[charnum];

    //	lynx_draw_sprites(charSCB.pointer, true)	;
    DrawSprites(&charSCB);
}

// Draw a text string
void TextDrawString(char *text, int x, int y)
{
	unsigned char i = 0;
	unsigned char c = text[0];
	while (c != 0)
		{
		TextDrawChar(c - 32, x, y);
		x = x + 6;
		i = i + 1;
		c = text[i];
		}
}

// Convert a UNIT to a string
// Note: buffer must be 6 chars or more
static void uitoa(unsigned int value, char *buffer)
{
	char tmp[8];
	char *p = tmp;
    do {
        unsigned int m = value % 10;
        value /= 10;
        *p++ = '0' + m;
		}
	while (value);

	// Have to reverse the digits
	p--;
	char *outp = buffer;
	while (p >= &tmp[0])
		{
		*outp++ = *p--;
		}
	*outp = 0;		// string terminator
}

// Draw am unisgned integer value at the specified x, y
static char textBuffer[20] = { 0 };
void TextDrawInt(unsigned int value, int x, int y)
{
	uitoa(value, textBuffer);
	TextDrawString(textBuffer, x, y);
}

// Draw a 4-digit hex value at the specified x, y
void TextDrawHex(unsigned int value, int x, int y)
{
	unsigned char c = (value & 0xF000) >> 12;
	if (c < 10)
		c += 48;
	else
		c += 55;
	TextDrawChar(c - 32, x, y);
	x = x + 6;

	c = (value & 0x0F00) >> 8;
	if (c < 10)
		c += 48;
	else
		c += 55;
	TextDrawChar(c - 32, x, y);
	x = x + 6;

	c = (value & 0x00F0) >> 4;
	if (c < 10)
		c += 48;
	else
		c += 55;
	TextDrawChar(c - 32, x, y);
	x = x + 6;

	c = (value & 0x000F);
	if (c < 10)
		c += 48;
	else
		c += 55;
	TextDrawChar(c - 32, x, y);
	x = x + 6;
}
