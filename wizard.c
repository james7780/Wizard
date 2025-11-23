// Wizard of WASD
// llvm-mos C version
// JH 2025
#include <stdint.h>
#include <stddef.h>
//#include <stdio.h>
//#include <string.h>
//#include <lynx.h>
//#include <_mikey.h>
//#include <_suzy.h>
#include "lynxlib.h"
#include "drawtext.h"
#include "audio.h"
#include "instruments.h"

#include "wizard.pal"			// main game pal


// Sprite data
#include "gfx/playerSpr.h"
#include "gfx/cageSpr.h"
#include "gfx/crossSpr.h"
#include "gfx/entrance1Spr.h"
#include "gfx/entrance2Spr.h"
#include "gfx/entrance3Spr.h"
#include "gfx/falconSpr.h"
#include "gfx/fireSpr.h"
#include "gfx/flaskSpr.h"
#include "gfx/forestSpr.h"
#include "gfx/hammerSpr.h"
#include "gfx/landSpr.h"
#include "gfx/mountainSpr.h"
#include "gfx/portalSpr.h"
#include "gfx/shipSpr.h"
#include "gfx/skullSpr.h"
#include "gfx/snakeSpr.h"
#include "gfx/spadeSpr.h"
#include "gfx/stickSpr.h"
#include "gfx/wallSpr.h"
#include "gfx/waterSpr.h"
#include "gfx/webSpr.h"
#include "gfx/wizardSpr.h"
#include "gfx/floorSpr.h"
#include "gfx/stairSpr.h"
#include "gfx/bridgeSpr.h"
#include "gfx/daggerSpr.h"
#include "gfx/cagedFalconSpr.h"
#include "gfx/torchSpr.h"
#include "gfx/rampartSpr.h"

#include "gfx/logoSpr.h"

#include "worldtilemap.h"			// 32 x 96 tilemap

extern void *memset(void *ptr, int value, size_t num);

// This array must match the tilemap values/indices
const char *tileSprPtrs[] = { waterSpr, landSpr, forestSpr, mountainSpr, entrance1Spr, entrance2Spr, entrance3Spr, portalSpr, shipSpr, wallSpr, floorSpr, stairSpr, bridgeSpr, rampartSpr,
								waterSpr, waterSpr,			// filler till row 2 of tiles
								stickSpr, falconSpr, cageSpr, webSpr, crossSpr, fireSpr, hammerSpr, snakeSpr, skullSpr, flaskSpr, wizardSpr, daggerSpr, cagedFalconSpr,
								waterSpr, waterSpr, waterSpr,					// filler till row 3 of tiles
								playerSpr, spadeSpr, torchSpr };

#define MAPWIDTH		32
#define MAPHEIGHT		30
#define WINDOWWIDTH		11
#define WINDOWHEIGHT	11

// Functionality needed:
// 1. [DONE] Draw map:  Draw n rows of n tiles from tilemap
// 2. [DONE] Move player
// 3. [DONE] Match player inventory item with current tile type
// 	         - Replace current tile with another, if condition met
// 	         - "Tile data" system: Tile id, action object id, replacement tile id
// 4. [DONE] "Jump" tiles: Move to specific x/y in tilemap
// 5. AUDIO - "No" or "Wrong" sound (low tone)
// 6. [DONE] Intro screen + text

// Object ID's (tilemap value)
enum ObjectID { WATER = 0, LAND, FOREST, MOUNTAIN, ENT1, ENT2, BLOCKENT, PORTAL, SHIP, WALL, FLOOR, STAIR, BRIDGE, RAMPART, 
				STICK = 16, FALCON, CAGE, WEB, CROSS, FIRE, HAMMER, SNAKE, SKULL, URN, WIZARD, DAGGER, CAGEDFALCON,
				PLAYER = 32, SPADE, TORCH, MAX_OBJECTID, IMPASSABLE };

// Game structs
unsigned int playerMapPos;			// player position in map data array
unsigned int moveCount;				// game move counter
unsigned char gameOver;				// game over status

// Each object's requirements, replacements, inventory x/y
// Collected in a struct for visually easier mapping
typedef struct MapObject {
	unsigned char requirement;			// What the player need to "handle" this object type ("0" means not a pickable item, "1" means no requirement to pickup)
	unsigned char replacement;			// What this object type should be replaced with
	unsigned char invItemX;				// X pos of item in inventory display
	unsigned char invItemY;				// Y pos of item in inventory display
} T_MapObject;

const struct MapObject objects[MAX_OBJECTID] = {
	{ IMPASSABLE, 0, 0, 0 },		// Water tile
	{ 0, 0, 0, 0 },					// Land tile
	{ 0, 0, 0, 0 },					// Forest tile
	{ IMPASSABLE, 0, 0, 0 },		// Mountain tile
	{ 0, 0, 0, 0 },					// Entrance 1 tile
	{ 0, 0, 0, 0 },					// Entrance 2 tile
	{ SPADE, ENT2, 0, 0 },			// Blocked entrance tile / object
	{ 0, 0, 0, 0 },					// Portal tile
	{ 0, 0, 0, 0 },					// Ship tile / object
	{ IMPASSABLE, 0, 0, 0 },		// Wall tile
	{ 0, 0, 0, 0 },					// Floor tile
	{ 0, 0, 0, 0 },					// Stair tile
	{ 0, 0, 0, 0 },					// Bridge tile
	{ IMPASSABLE, 0, 0, 0 },		// Rampart tile
	{ 0, 0, 0, 0 },					// empty
	{ 0, 0, 0, 0 },					// empty
	{ 1, FOREST, 120, 80 },			// Stick tile / object
	{ CAGE, CAGEDFALCON, 0, 0 },	// Falcon tile / object
	{ 1, FLOOR, 130, 80 },			// Cage tile
	{ TORCH, FLOOR, 0, 0 },			// Web tile
	{ 1, WALL, 140, 80 },			// Cross tile
	{ STICK, TORCH, 0, 0 },			// Fire tile
	{ 1, FLOOR, 150, 80 },			// Hammer tile
	{ CAGEDFALCON, FLOOR, 0, 0 },	// Snake tile
	{ CROSS, FLOOR, 0, 0 },			// Skull tile
	{ HAMMER, DAGGER, 0, 0 },		// Urn tile
	{ DAGGER, FLOOR, 0, 0 },		// Wizard tile
	{ 1, FLOOR, 150, 80 },			// Dagger tile
	{ 1, FOREST, 130, 80 },			// Cage Falcon tile
	{ 0, 0, 0, 0 },					// empty
	{ 0, 0, 0, 0 },					// empty
	{ 0, 0, 0, 0 },					// empty
	{ 0, 0, 0, 0 },					// Player tile / object
	{ 1, FLOOR, 130, 90 },			// Spade tile / object
	{ 1, FLOOR, 120, 80 }			// Torch tile / object
};

// Our current inventory
unsigned char inventory[MAX_OBJECTID];

// Portal / entranceway jumps (in map tile index)
#define NUM_JUMPS 25
//                                       Dung1   Exit    Portal  Portal  D3E1    Exit    D3E2    Exit    Dung2   Exit    Tunnel          D2J1            D2J2            D2J3            D2J4            Tower           TowerL1         Wizard
unsigned int jumpStartPos[NUM_JUMPS] = { 0x00E6, 0x0631, 0x00AD, 0x02EE, 0x0110, 0x08C5, 0x0153, 0x09D9, 0x026B, 0x04D8, 0x0352, 0x0354, 0x0550, 0x04DA, 0x04D2, 0x0556, 0x04D4, 0x055E, 0x04DC, 0x0558, 0x0253, 0x0807, 0x0747, 0x06A7, 0x0607 };
unsigned int jumpEndPos[NUM_JUMPS] = {   0x0631, 0x00E6, 0x02EE, 0x00AD, 0x08C5, 0x0110, 0x09D9, 0x0153, 0x04D8, 0x026B, 0x0354, 0x0352, 0x04DA, 0x0550, 0x0556, 0x04D2, 0x055E, 0x04D4, 0x0558, 0x04DC, 0x0807, 0x0253, 0x06A7, 0x0747, 0x0567 };

// These MUST be defined for LynxLib to work
unsigned char *BUFFER1 = (unsigned char *)0x9000;			// C038;
unsigned char *BUFFER2 = (unsigned char *)0xB000;			// E018;
unsigned char *COLLBUF = (unsigned char *)0xD000;


#define CLI asm("\tcli")	/* enable Mikeys interrupt response */
#define SEI asm("\tsei")	/* disable it */

#define EnableIRQ(n)\
  asm(" lda #$80\n"\
      " tsb $fd01+"#n"*4\n")	/* enable interrupt of timer n */

#define DisableIRQ(n)\
  asm(" lda #$80\n"\
      " trb $fd01+"#n"*4\n") /* disable it */

struct SCB playerSCB =	{	BPP_4 | TYPE_NORMAL,
							REHV | LITERAL,			// literal, scale XY
							0x00,			// collision number
							0, playerSpr,
							50, 50,
							0x100, 0x100,
							{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
							0 };

// Horizontal "blank" interrupt routine
// note: get cycle count!
void HBL() //interrupt
{
// #asm

// ch_clr	pha
// 		phx
// 		ldx	_lines
// 		inc _lines
// 		inc _lines
// 		lda _bgcol,x
// 		sta	$FDA0
// 		lda _bgcol+1,x
// 		sta $FDB0
// 		plx
// 		pla
// #endasm
asm("pha");
asm("pla");
}

// Wait for a button press (NOT dpad)
static void WaitKey()
{
	// Wait for key up
	while (SUZY.joystick) ;

	// Wait for key down
	while (!(SUZY.joystick & 0x0F)) ;
}

// Wait for a DPad press
static void WaitDPad()
{
	// Wait for key up
	while (SUZY.joystick) ;

	// Wait for key down
	while (!(SUZY.joystick & 0xF0)) ;
}


// Play and instrument sound and wait till it finishes
// NB: Uses global waitFrameCount
static void PlaySoundBlock(uchar channel, struct INSTRUMENT *instrument)
{
	volatile struct _mikey_audio *channelRegs = &MIKEY.channel_a + channel; // Warning: Pointer arithmetic!!!
	StartSound(channel, instrument, 0);		// plays first data in envelope
	while (channelRegs->volume)
		{
		WaitVSync();
		UpdateSound();
		}
}


// SCBS for rendering playfield
struct SCB pfSCBTemplate = { BPP_4 | TYPE_BACKNONCOLL,
							LITERAL | REHV, 		// literal, scale XY
							0,
							0, waterSpr,
							0x0, 0x0, 0x100, 0x100,	
							{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },	
							0 };
struct SCB pfSCBs[WINDOWWIDTH];

struct SCB inventorySCB = { BPP_4 | TYPE_BACKNONCOLL,
							LITERAL | REHV, 		// literal, scale XY
							0,
							0, spadeSpr,
							0x0, 0x0, 0x100, 0x100,	
							{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },	
							0 };


void InitPlayfieldSCBs()
{
	// Copy playfield template SCB to the playfield SCB array
	for (unsigned char i = 0; i < WINDOWWIDTH; i++)
		{
		struct SCB *pSCB = &pfSCBs[i];
		*pSCB = pfSCBTemplate;
		//pSCB->sprctl0 = BPP_4 | TYPE_BACKNONCOLL;
		//pSCB->sprctl1 = LITERAL | REHV; 		// literal, scale XY
		pSCB->hpos = (i * 10);
		//pSCB->vpos = 0;
		//pSCB->hscale = 0x100;
		//pSCB->vscale = 0x100;
		pSCB->next = &pfSCBs[i+1];
		if (i == (WINDOWWIDTH - 1))
			pSCB->next = 0;	// end of chain
		//memcpy(playerSCB.palmap, pSCB->palmap, 8);
		}
}

// Draw the playfield 11x11 area, centred on the player
void DrawPlayfield()
{
	unsigned int mapPos = playerMapPos - (5 * MAPWIDTH) - 5;
	for (unsigned char y = 0; y < WINDOWHEIGHT; y++)
		{
		for (unsigned char x = 0; x < WINDOWWIDTH; x++)
			{
			pfSCBs[x].data = (char *)tileSprPtrs[worldtilemap[mapPos]]; //forestSpr;
			pfSCBs[x].vpos = (y * 10);
			mapPos++;
			}
		DrawSprites(&pfSCBs[0]);
		mapPos += (MAPWIDTH - WINDOWWIDTH);
		}

}

// Draw the inventory
void DrawInventory()
{
	struct SCB *pSCB = &inventorySCB;
	for (unsigned char i = 0; i < MAX_OBJECTID; i++)
		{
		if (inventory[i])
			{
			unsigned char x = objects[i].invItemX;				// invItemX[i];
			unsigned char y = objects[i].invItemY;				//invItemY[i];
			pSCB->hpos = x;
			pSCB->vpos = y;
			pSCB->data = (char *)tileSprPtrs[i];
			DrawSprites(pSCB);
			}	
		}
}

// Clear screen sprite data and SCB
char sprclr[4] = { 0x03, 0x00, 0x00, 0x00 };

struct SCB clearSCB =	{	BPP_4 | TYPE_BACKGROUND, //TYPE_BACKGROUND,
							REHV | LITERAL,			// scale XY
							0x00,			// 0 to clear coll buffer 
							0, sprclr,
							0x0, 0x0, 0xA000, 0x6600,
							{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
							0 };

struct SCB logoSCB =	{	BPP_4 | TYPE_BACKGROUND, //TYPE_BACKGROUND,
							REHV | LITERAL,			// scale XY
							0x00,			// 0 to clear coll buffer 
							0, logoSpr,
							116, 2, 0x100, 0x100,
							{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
							0 };

/// Set up for a new game							
void InitGame()
{
	playerMapPos = 0x0226;
	moveCount = 999;
	memset((void *)inventory, 0, sizeof(inventory));
}							

// Move the ship as necessary
// Returns - new ship pos
unsigned int DoShipMove()
{
	unsigned int destPos;

	// AUDIO TODO - Ship jingle (what should we do with the drunken sailor) if targetObjectId is a ship

	// Play "whoosh" sound
	PlaySoundBlock(0, &instrShip);

	// 0x0316, 0x0276
	if (worldtilemap[0x0316] == SHIP)
		{
		destPos = 0x0256;
		worldtilemap[0x0256] = SHIP;
		worldtilemap[0x0316] = WATER;
		}
	else
		{
		destPos = 0x0316;
		worldtilemap[0x0256] = WATER;
		worldtilemap[0x0316] = SHIP;
		}

	return destPos;
}

// PLayer movement routine
void MovePlayer()
{
	// Wait for a DPad direction to be pressed
	WaitDPad();

	// Get which map tile we will be moving onto
	unsigned char j = SUZY.joystick;
	unsigned int targetPos = playerMapPos;
	if (j & JOYPAD_LEFT)
		targetPos--;
	else if (j & JOYPAD_RIGHT)
		targetPos++;

	if (j & JOYPAD_UP)
		targetPos -= MAPWIDTH;
	else if (j & JOYPAD_DOWN)
		targetPos += MAPWIDTH;

	// Can player move into target pos?
	unsigned char targetObjectId = worldtilemap[targetPos];
	unsigned char required = objects[targetObjectId].requirement; //objectRequiremments[targetObjectId];

	if (IMPASSABLE == required)
	 	{
	 	// TODO - Play "no" sound 
	 	return;		// Do not move onto the target tile
	 	}

	// Process target object with required?
	if (required != 0 && IMPASSABLE != required)
		{
		// Free pickup (no requirement)
		if (1 == required)
			{
			// Play "pickup" sound 
			PlaySoundBlock(0, &instrDing);
			inventory[targetObjectId] = 1;
			worldtilemap[targetPos] = objects[targetObjectId].replacement;		//objectReplacements[targetObjectId];
			}
		// Do we have the required item in inventory?
		else if (inventory[required] > 0)
			{
			// Play "replace item / handle item" sound 
			PlaySoundBlock(0, &instrSolved);
			inventory[required] = 0;
			worldtilemap[targetPos] = objects[targetObjectId].replacement;		//objectReplacements[targetObjectId];
			}
		else
			{
			// Play no-no sound
			PlaySoundBlock(0, &instrNo);
			}

		return;			// Do not move onto the target tile
		}

	// Is this an entrance / portal tile?  ("jump" tile)
	for (unsigned char i = 0 ; i < NUM_JUMPS; i++)
		{
		if (targetPos == jumpStartPos[i])
			{
			// AUDIO - "portal jump" sound if targetObjectIOd is a portal
			// AUDIO - "dor/step sound" sound if targetObjectIOd is a door or stairs
			if (PORTAL == targetObjectId)
				PlaySoundBlock(0, &instrZapper);
			else
				PlaySoundBlock(0, &instrBeep);
			targetPos = jumpEndPos[i];
			break;
			}	
		}

	// Ship voyage?
	if (SHIP == targetObjectId)
		{
		targetPos = DoShipMove();
		}

  	playerMapPos = targetPos;
	moveCount--;
}

// Redraw everything
void Redraw()
{
	DrawSprites(&clearSCB);
	DrawSprites(&logoSCB);
	DrawPlayfield();
	DrawInventory();
	DrawSprites(&playerSCB);

	// TextDrawString("WIZARD\0", 120, 2);
	// TextDrawString("  OF  \0", 120, 12);
	// TextDrawString(" WAZD\0", 120, 22);

	// DEBUG
	TextDrawHex(playerMapPos, 120, 50);

	TextDrawString("T:\0", 120, 60);	
	TextDrawInt(moveCount, 132, 60);

	SwapBuffers();
}

// Show the intro text
void DoIntro()
{
	SwapBuffers();

	DrawSprites(&clearSCB);
	TextDrawString("THE EVIL WIZARD 'BALKOR", 2, 2);
	TextDrawString("THE BLUE' HAS RETURNED,", 2, 12); 
	TextDrawString("TO AGAIN THREATEN THE", 2, 22); 
	TextDrawString("PEACEFUL LAND OF WASD!", 2, 32);
	//TextDrawString(" ", 2, 42);
	TextDrawString("HE HAS SEALED HIMSELF OFF", 2, 52);
	TextDrawString("AT THE TOP OF THE TOWER", 2, 62); 
	TextDrawString("ON EASTERN ISLAND,", 2, 72); 
	TextDrawString("PROTECTED BY ONE OF HIS", 2, 82);
	TextDrawString("UNDEAD MINIONS.", 2, 92);
	SwapBuffers();
	WaitKey();

	DrawSprites(&clearSCB);
	TextDrawString("THERE THE SORCERER IS", 2, 2);
	TextDrawString("PREPARING A POWERFUL", 2, 12);
	TextDrawString("SPELL, WHICH WHEN", 2, 22);
	TextDrawString("COMPLETED WILL RAISE", 2, 32);
	TextDrawString("HORDES OF UNDEAD", 2, 42);
	TextDrawString("CREATURES, AND SURELY", 2, 52);
	TextDrawString("SPELL DOOM FOR ALL LIFE", 2, 62);
	TextDrawString("IN THE LAND!", 2, 72);
	SwapBuffers();
	WaitKey();

	DrawSprites(&clearSCB);
	TextDrawString("THERE IS ONLY ONE HOPE", 2, 2);
	TextDrawString("FOR DEFEATING BALKOR THE", 2, 12);
	TextDrawString("BLUE. CENTURIES AGO THE", 2, 22);
	TextDrawString("MAGICAL DAGGER OF VENN", 2, 32);
	TextDrawString("WAS FORGED USING THE", 2, 42);
	TextDrawString("COMBINED POWERS OF THE", 2, 52);
	TextDrawString("NINE MOST POWERFUL GOOD", 2, 62);
	TextDrawString("WIZARDS IN THE REALM, IN", 2, 72);
	TextDrawString("CASE THE EVIL WIZARD", 2, 82);
	TextDrawString("SHOULD EVER RETURN.", 2, 92);
	SwapBuffers();
	WaitKey();

	DrawSprites(&clearSCB);
	TextDrawString("ALAS, THE DAGGER WAS", 2, 2);
	TextDrawString("HIDDEN AWAY IN A LONG", 2, 12);
	TextDrawString("SINCE FORGOTTEN LOCATION.", 2, 22);
	//TextDrawString(" ", 2, 32);
	TextDrawString("YOUR QUEST IS TO FIND", 2, 42);
	TextDrawString("THIS MAGIC DAGGER, AND", 2, 52);
	TextDrawString("USE IT TO DESTROY THE", 2, 62);
	TextDrawString("BLUE WIZARD ONCE AND", 2, 72);
	TextDrawString("FOR ALL, BEFORE HE", 2, 82);
	TextDrawString("DEPLOYS HIS DEADLY SPELL!", 2, 92);
	SwapBuffers();
	WaitKey();

	DrawSprites(&clearSCB);
	TextDrawString("TIME IS RUNNING OUT!", 2, 2);
	//TextDrawString(" ", 2, 12); 
	TextDrawString("THE PEACEFUL LAND OF", 2, 22); 
	TextDrawString("WASD IS COUNTING ON", 2, 32);
	TextDrawString("YOU!", 2, 42);
	SwapBuffers();
	WaitKey();
}

int main()
{
	InitLynx();
	InitSound();
	SetPalette(pal);

	PlaySoundBlock(0, &instrZapper);

	// JH - Disable VBL and HBL irq
//	DisableIRQ(2);
//	DisableIRQ(0);

//	SEI;

	//	InstallIRQ(2,VBL);
	//	InstallIRQ(0,HBL);
	//EnableIRQ(2); /* 42BS */					// TODO - reimplment!
	//EnableIRQ(0);

	InitPlayfieldSCBs();						// Set up playfield SCBs
	TextInit();									// Set up text renderer

	// Show story text
	DoIntro();

	// Setup for a new game
	InitGame();

// DEBUG
//inventory[SPADE] = 1;
//inventory[HAMMER] = 1;
//inventory[CAGE] = 1;

	Redraw();

	const unsigned int blueWizPos = 0x04E7;
	while(moveCount > 0)
		{
		Redraw();
		MovePlayer();

		// Is Blue Wizard dead?
		if (worldtilemap[blueWizPos] != WIZARD)
			break;
		}

	// Did we win the game?
	if (moveCount > 0)
		{
		TextDrawString("CONGRATULATIONS!\0", 6, 20);
		TextDrawString("THE BLUE WIZARD\0", 12, 40);
		TextDrawString("IS DEAD!\0", 34, 50);
		SwapBuffers();
		}
	else
		{
		TextDrawString("YOU RAN OUT OF TIME!\0", 2, 20);
		TextDrawString("THE LAND OF WASD IS\0", 5, 40);
		TextDrawString("DOOMED!\0", 40, 50);
		SwapBuffers();
		}
}
