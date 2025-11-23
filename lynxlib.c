// Lynx functions for llvm-mos
// JH 2025

#include "lynxlib.h"

// Lynx init copied from Millfork atari_lyx_init function
// Also ref: https://www.monlynx.de/lynx/lynx6.html#_68  (Sprite engine init)
void InitLynx()
{
		// "Install" tgi driver
	// - Sets draw page to 0
	// - Installs VBL irq handler and enables it
	//   NB: VBL irq handler swaps TGI's buffer pointers!
	//tgi_install(tgi_static_stddrv);
	//tgi_init();			// Enables VBL, sets collision buffer

// NB: If you do NOT call tgi_install(), you will get linker error "Unresolved external '__LOWCODE_SIZE__' "!"

	//	vid memory to the furthest point from code 
	//	right at the top of ram 
	// DISPADR=$dc00					// word DISPADR		@$fd94
	// VIDBASE=$dc00					// word VIDBASE		@$fc08
	// COLLBASE=$bc00					// word COLLBASE		@$fc0a
	MIKEY.scrbase = BUFFER1;
	SUZY.sprbase = BUFFER1;
	SUZY.vidadr = BUFFER1;

		// Set collision buffer to $A058
//	SUZY.colbase = (unsigned char *)COLLBUF;
	// set offset to sprite collision depository to 23 (relative to SCB)
	// and set collision buffer address
	SUZY.colloff = COLLSCBOFFSET;

	//	reset Horizontal and Vertical offset regs
	// HOFF = 0
	// VOFF = 0
	SUZY.hoff = 0;
	SUZY.voff = 0;

	//	suzy and display 
	// SPRSYS = 0				// $fc92
	// SPRINT = $f3 		// $fc83 
	// DISPCTRL = DISPLAY_COLOR | DISPLAY_4bpp | DISPLAY_ENABLE    // $fd92 
	// 							const byte DISPLAY_COLOR=%1000
	// 							const byte DISPLAY_4bpp=%0100
	// 							const byte DISPLAY_FLIPPED=%0010
	// 							const byte DISPLAY_ENABLE=%0001
	// SUZYBUSEN = 1						// $fc90
	SUZY.sprsys = 0x20;	//0;		// Switch off collision
	SUZY.sprinit = 0xF3;
	SUZY.hsizeoff = 0;
	SUZY.vsizeoff = 0;
	SUZY.suzybusen = 1;

 	MIKEY.dispctl = 0x8 | 0x4 | 0x1;				// DISPLAY_COLOR | DISPLAY_4bpp | DISPLAY_ENABLE

	// Set 60hz display 
	//TIMER0_reload=$9e
	//TIMER0_ctrl=$18 
	MIKEY.timer0.reload = 0x9E;		// HTIMBKUP = timer 0 (HBL) "backup" reg
	MIKEY.timer0.control = 0x18;

	//TIMER2_reload=$68 
	//TIMER2_ctrl=$1f 
	MIKEY.timer2.reload = 0x68;
	MIKEY.timer2.control = 0x1f;

	//PBKUP=$29
	MIKEY.pkbkup = 0x29;

}

// Wait for the "raster" to hit 104 
void WaitVSync()
{
	// NB: RASTER counter counts from 0x68 (104) down to 0
	// RASTER = Mikdey timer 2 counter ($FD0A))
	while (MIKEY.timer2.count != 0)
		{
		//MIKEY.palette[0] = MIKEY.timer2.count;
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		}
}

// Replacement for tgi_busy()
void WaitSuzy()
{
//	while (tgi_busy()) ;


	while (SUZY.sprsys & 0x01)
		{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		}
}

/* DrawSprite from cc65 lynxlib draw_spr.m65:
_DrawSprite:    jsr popax       	; get pSCB
DrawSprite:
                sta $fc10			// write incoming PCB to SUZY.scbnext
                stx $fc11
		lda _RenderBuffer
		ldx _RenderBuffer+1
                sta $fc08			// write current render buffer address to SUZY.sprbase
                stx $fc09
                lda #1
                sta $fc91			// write 1 to SUZY.sprgo
                stz $fd90			// write 0 to MIKEY.sdoneack
_DrawSprite1:
                stz $fd91			// write 0 to MIKEY.cpusleep (cpu bus request disable)
                lda $fc92
                lsr A
                bcs _DrawSprite1	// check bit 0 of SUZY.sprsys  (1 = busy drawing sprite)
                stz $fd90			// write 0 to MIKEY.sdoneack
                rts
*/
void DrawSprites(struct SCB *firstSprite)
{
	//SUZY.sprbase = BUFFER1;
	SUZY.scbnext = (unsigned char *)firstSprite;			// Actual "start" of the SCB as needed by Suzy

  	// Tell Suzy to go
	SUZY.sprgo = 1;
	MIKEY.sdoneack = 0;   // 1;

	//	wait until finished
	// Note: you may not write to Suzy while she is busy
	//WaitSuzy();
	// This code from https://www.chibiakumas.com/6502/atarilynx.php
	while (1)
		{
		MIKEY.cpusleep = 0;
		if (!(SUZY.sprsys & 1))       // SUZY sprite draw busy
			break;
		}
	
	MIKEY.sdoneack = 0;
}

// NEW (relacement for old lib)
void SwapBuffers()
{
	WaitVSync();

	if (MIKEY.scrbase == BUFFER1)
		{
		MIKEY.scrbase = BUFFER2;
		//SUZY.vidadr = BUFFER1;
		SUZY.sprbase = BUFFER1;
		}
	else
		{
		MIKEY.scrbase = BUFFER1;
		//SUZY.vidadr = BUFFER2;
		SUZY.sprbase = BUFFER2;
		}	

/* WAS	
	WaitVSync();
	tgi_updatedisplay();
*/
/* RECENT
	// Set visible buffer (display buffer) to the one we have been drawing on 
	MIKEY.scrbase = drawBuffer;	

	// Swap double-buffer
	if (drawBuffer == (uchar *)BUFFER1)
		drawBuffer = (uchar *)BUFFER2;
	else
		drawBuffer = (uchar *)BUFFER1;

	//RenderBuffer = MIKEY.scrbase;
*/

/*
	// Wait for the next VBL, then
	// 1. Set view buffer to the draw buffer
	// 2. Swap draw buffer index (drawbuffer xor 1).
	VSYNC();

	// Set visible buffer (display buffer) to the one we have been drawing on
	// (According to Handy Specifications), MIKEY.DISPADDR is the backup or reload value
	// that gets transferred to the address counter at the start of the 3rd line of
	// vertical blanking)
	MIKEY.scrbase = RenderBuffer;

	// Swap double-buffer
	if (RenderBuffer == (uchar *)BUFFER1)
		RenderBuffer = (uchar *)BUFFER2;
	else
		RenderBuffer = (uchar *)BUFFER1;

	// Set Suzy draw buffer (base address of sprite video build buffer)
	//SUZY.sprbase = RenderBuffer;
	SUZY.vidadr = RenderBuffer;
*/
}

/// Set the Lynx palette
/// @param[in] pal			The palette to set to (NULL to set to black)
void SetPalette(unsigned char *pal)
{
	// Set palette to specified, or black
	for (unsigned char j = 0; j < 32; j++)
		MIKEY.palette[j] = pal[j];
}

/// @brief  Read the Lynx buttons (U/D/L/R/A/B)
/// @return     SUZY joystcik buttons register
unsigned char ReadButtons()
{
  return SUZY.joystick;
}