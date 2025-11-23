// Lynx functions for llvm-mos
// JH 2025
#include <lynx.h>
#include <_mikey.h>
#include <_suzy.h>

// Lynx Sprite Control Block structure
typedef struct SCB {
	unsigned char sprctl0;
	unsigned char sprctl1;
	unsigned char sprcoll;			// spr collision number
	struct SCB *next;
	char *data;
	signed int hpos;
	signed int vpos; 
	unsigned int hscale;
	unsigned int vscale;
	unsigned char palmap[8];
	unsigned char collResult;		// collision result data (offset = 23)
} TSCB;

// offset of collision result from beginning of SCB
#define COLLSCBOFFSET 23

// These MUST be defined in your program
extern unsigned char *BUFFER1;              // suggest 0x9000
extern unsigned char *BUFFER2;              // suggset 0xB000
extern unsigned char *COLLBUF;              // suggest 0xD000
// TODO - How to disable/enable collision "globally"?

// Lib functions
extern void InitLynx();
extern void WaitVSync();
extern void WaitSuzy();
extern void DrawSprites(struct SCB *firstSprite);
extern void SwapBuffers();
extern void SetPalette(unsigned char *pal);
extern unsigned char ReadButtons();
