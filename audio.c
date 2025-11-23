// Sky Raider game sound system
// Copyright JH 2019/2020
//#include <string.h>
#include <stddef.h>
#include "audio.h"
#include <lynx.h>

extern void *memset(void *ptr, int value, size_t num);

#define ENABLE_AUDIO

static struct INSTRUMENT *channelInstrument[NUM_CHANNELS];

// Reload values for each note in one octave
static uchar midiReloadMap[12] = {
    238,    // C,			C2 ? 	MIDI Note = 36		237.9   238 = 65.37 Hz 
    225,    // ^C, _D,									224.48  225 = 69.13 Hz
    212,    // D,										211.82  212 = 73.35 Hz
    200,    // ^D, _E,									199.88  200 = 77.74 Hz
    189,    // E,										188.6
    178,    // F,										177.96
    168,    // ^F, _G,									167.9
    158,    // G,										158.4
    149,    // ^G, _A,									149.4
    141,    // A,										141.0   141 = 110.0 Hz
    133,    // ^A, _B,									133.0
    126,    // B,										125.5
};

/* WAS:
// https://atariage.com/forums/topic/205973-sound-in-c-with-no-library/
// (Karri K)
// This table is used to cover the delays needed for 4 octaves
// These values work when the looplen is 2, 4, 8, 16...
// NB: MIDI note 60 is "C4 ("middle C)
// NB: Seems to be a problem with some notes, eg: A3 reload = 71 = 434Hz, should be 70 = 440Hz
static uchar midiPitchMap[] = {
	255,	// C 			C-1 ?	MN = 0
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,	// C 			C0 ?	MN = 12
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,	// C 			C1 ?	MN = 24	
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255,
    239,    // C,			C2 ? 	MN = 36			237.9   238 = 65.37 Hz 
    225,    // ^C, _D,									224.48  225 = 69.13 Hz
    213,    // D,												211.82  212 = 73.35 Hz
    201,    // ^D, _E,									199.88  200 = 77.74 Hz
    190,    // E,												188.6
    179,    // F,												177.96
    169,    // ^F, _G,									167.9
    159,    // G,												158.4
    151,    // ^G, _A,									149.4
    142,    // A,												141.0   141 = 110.0 Hz
    134,    // ^A, _B,									133.0
    127,    // B,												125.5
    119,    // C			C3 ? 	MN = 48			118.4   118 = 131.3 hz	E = 0.5 Hz
    113,    // ^C _D										111.7
    106,    // D												105.4
    100,    // ^D _E										99.4
    95,	 // E														93.8
    89,	 // F														88.485
    84,	 // ^F _G												83.45
    80,	 // G														78.7
    75,	 // ^G  _A											74.2
    71,	 // A														70.0    70 = 220.07 Hz
    67,	 // ^A _B												66.0
    63,	 // 														62.0
    60,	 // c				C4 ?	MN = 60				58.7    59 = 260.41 Hz E = 1.2 Hz
    56,	 // ^c _d												55.37
    53,	 // d														52.2
    50,	 // ^d 													49.2
    47,	 // e														46.4
    45,	 // f														43.7
    42,	 // ^f _g												41.2
    40,	 // g														38.8
    38,	 // ^g _a												36.6
    36,	 // a					A4, 440Hz					34.5    35 = 434.0 Hz, 34 = 446.4 Hz  E = 4 Hz
    34,	 // ^a 													32.5    33 = 459.56 Hz E = 6.5 Hz
    32,	 // b														30.6
    30,	 // c'				C5 ?	MN = 72			28.8    29 = 520.83 Hz  E = 1.5 Hz
    28,	 // ^c' _d'											27.2
    27,	 // d'													25.6
    25,	 // ^d' _e'											24.1
    24,	 // e'													22.7
    22,	 // f'													21.3
    21,	 // ^f' _g'											20.1
    20,	 // g'													18.9
    19,	 // ^g' _a'											17.8
    18,	 // a'				A5, 880 Hz				16.75   17 = 868.0 Hz E = 12 Hz
    17,	 // ^a' _b'											15.75
    16	  // b'													14.8
};
*/
// TODO - Fix frequency errors by using reload values from lower 2 octaves,
//        with a doubled clock
//        (UpdateTrackerSong() should feed adjusted reload and clock values to StartSound)

/// Initialise sound output
void InitSound(void)
{
#ifdef ENABLE_AUDIO
	// Channel allocation

	//Procedure for setting up a channel to play a tone Starting from a reset:
	//Wait 100ms (for hardware to initialise)
	//Disable channel
	//Stop counter for channel
	//Write backup (reload) value to channel
	//Write counter value to channel
	//Write shift register bit pattern to channel (coutner must be disabled)
	//Write feedback register bit pattern to channel (must be >0 to get sound!)
	//Enable counter and set timer pre-selector
	//Enable channel

/* Do we need this (StartSound() sets this up anyway!)
	unsigned int feedback = 0x0019;			// JH - actually feedback bits
	unsigned int shift = 0x00B1;			// JH - actually shift bits
	unsigned int octave = 5;
	unsigned int integrate = 1;
	unsigned char pitch = 4;

	// stop counter for channel
	MIKEY.channel_a.control = 0x10;

	// Write feedback for channel
	MIKEY.channel_a.feedback = (feedback & 0x003f) + ((feedback >> 4) & 0xc0);

	// write backup (reload) value to channel
	MIKEY.channel_a.reload = pitch;
	MIKEY.channel_a.count = pitch;

	// Set up shift reg
	MIKEY.channel_a.shiftlo = shift & 0xFF;
	MIKEY.channel_a.other = (shift >> 4) & 0xf0;

	// Set feedback and enable counter
	//MIKEY.channel_a.control = 0x19; //(feedback & 0x0080) + 0x18 + octave + (integrate << 5);
	MIKEY.channel_a.control = (feedback & 0x0080) + 0x18 + octave + (integrate << 5);
*/
// test
	MIKEY.channel_a.volume = 63;
	MIKEY.mstereo = 0x00;
	//MIKEY.attena = 0x80;

	//for (pitch = 2; pitch < 80; pitch++)
	//	{
	//	MIKEY.channel_a.reload = pitch;
	//	VSYNC(); VSYNC(); VSYNC();
	//	}

	// stop counter for channel
	MIKEY.channel_a.control = 0x10;


	// Reset channel insturment assignments
	//channelInstrument[0] = NULL;
	//channelInstrument[1] = NULL;
	//channelInstrument[2] = NULL;
	//channelInstrument[3] = NULL;
	memset(channelInstrument, 0, 8);			// Uses less code space
#endif
}

/// Play sound using instr n on channel n
/// @param channel		Channel to play on (0 to 3)
/// @param instrument	Which intrument to assign to this channel
/// @param freq			Initial frequency (timer reload value) - Used for instrument sounds
void StartSound(uchar channel, struct INSTRUMENT *instrument, uchar freq)
{
#ifdef ENABLE_AUDIO
	volatile struct _mikey_audio *channelRegs;
	uint feedback;
	uchar reload;

	//if (channel < NUM_CHANNELS)			// commented out to save space
	//	{
		channelInstrument[channel] = instrument;
		channelRegs = &MIKEY.channel_a + channel;

		if (instrument)
			{
			instrument->envPos = 0;
			// Set up shift reg
			channelRegs->shiftlo = instrument->shifter & 0xFF;
			channelRegs->other = (instrument->shifter >> 4) & 0xf0;

			// Write feedback for channel
			feedback = instrument->feedback;
			channelRegs->feedback = (feedback & 0x003f) + ((feedback >> 4) & 0xc0);

			channelRegs->volume = instrument->volData[0];
			// NB: For music, set instrument pitchData to all 0's
			reload = instrument->pitchData[0];
			if (!reload)
				reload = freq;
			channelRegs->reload = reload;
			channelRegs->count = reload;

			//channelRegs->control = 0x1E;		// 64us (low octave)
			channelRegs->control = (feedback & 0x0080) + 0x18 + instrument->octave + (instrument->integrate << 5);
			}
		else
			{
			channelRegs->control = 0x10;		// stop counter for this channel
			}
//		}

#endif
}

/// Stop sound on channel n
void EndSound(uchar channel)
{
#ifdef ENABLE_AUDIO
	volatile struct _mikey_audio *channelRegs;
	struct INSTRUMENT *instrument;

	//if (channel < NUM_CHANNELS)					// commented out to save space
	//	{
		channelRegs = &MIKEY.channel_a + channel;
		// TODO! - Check pointer arithmetic!!!

		// stop counter for channel
		channelRegs->control = 0x10;

		// Stop instrument (so UpdateSound does not process it)
		instrument = channelInstrument[channel];
		if (instrument)
			instrument->envPos = 0xFF;
//		}
#endif
}

/// Stop sound on all channels
void EndAllSound(void)
{
	EndSound(0);
	EndSound(1);
	EndSound(2);
	EndSound(3);
}


/// Update sound output from envelopes
/// Call once a frame
void UpdateSound(void)
{
#ifdef ENABLE_AUDIO
	uchar channel;
	uchar envPos;
	volatile struct _mikey_audio *channelRegs;
	struct INSTRUMENT *instrument;
	uchar reload;

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		{
		// Warning: Pointer arithmetic!!!
		channelRegs = &MIKEY.channel_a + channel;

		instrument = channelInstrument[channel];
		if (instrument)
			{
			envPos = instrument->envPos + 1;
			if (0 == envPos)	// Instrument envPos = 0xFF, meaning not active
				continue;

			if (envPos < ENVELOPE_SIZE)
				{
				// Update sound hardware from the envelope data
				channelRegs->volume = instrument->volData[envPos];
				// NB: For musical instruments, set pitchData to all 0's
				reload = instrument->pitchData[envPos];
				if (reload)
					channelRegs->reload = reload;
				// increment envelope position
				instrument->envPos = envPos;

				// loop instrument if neccessary
				if (instrument->loopLength != 0 && envPos == instrument->loopLength)
					instrument->envPos = 0;
				}
			else
				{
				// We have reached the end of the envelope
				instrument->envPos = 0xFF;		// deactivate this instrument
				EndSound(channel);					// switch off sound on this channel
				}
			}
		}
#endif
}

/*
/// Play a sample on channel 3  (***BLOCKING***)
void PlaySample(uchar *sampleData)
{
#ifdef ENABLE_AUDIO
	uchar *dataPos;
	unsigned int count;
	uchar j;
	uchar divider;
	dataPos = sampleData;
	dataPos++;
	count = *dataPos << 8;
	dataPos++;
	count |= *dataPos;
	dataPos++;
	divider = *dataPos;
	dataPos = sampleData + 5;

	MIKEY.channel_d.reload = 0;
	MIKEY.channel_d.control = 0x10;
	MIKEY.channel_d.volume = 127;
	while (--count)
		{
		MIKEY.channel_d.dac = *dataPos;
		dataPos++;
		// wait 200 cycles (for 5000 Hz)
		//j = 12;
		j = divider >> 4;			// tune for pitch
		while (j--)
			{
			asm("nop");			// yeah yeah i know this will be much more than 200 cycles!
			}
		}
	MIKEY.channel_d.volume = 0;
#endif
}

*/


///////////////////////////////////////////////////////////////////////
// Tracker API
///////////////////////////////////////////////////////////////////////

/// Initialise a song
void InitTrackerSong(struct TrackerSong *song)
{
	song->speed = SPEED_100BPM;
	song->tick = 0;
	song->numPatterns = 0;
	song->patternData = NULL;
	song->numBars = 0;
	song->barData = NULL;
	song->numInstruments = 0;
	memset(song->instruments, 0, MAX_INSTR * 2);			// Uses less code space

	song->currentBar = 0;
	song->currentPatternPos = 0;
	memset(song->currentPattern, 0, NUM_TRACKS);				// Uses less code space
	memset(song->currentInstr, 0, NUM_TRACKS);
}

/// Called every frame
void UpdateTrackerSong(struct TrackerSong *song)
{
	uchar track;
	struct TrackerBar *barData;
	struct TrackerPattern *patternData;
	struct TrackerNote *noteData;
	uchar patternIndex;
	struct INSTRUMENT *instrument;
	uchar reload;	
		
	if (0 == song->numBars)
		return;
		
	// Time to trigger instrument in current pattern position?
	if (0 == song->tick)
		{
		// Note on current pattern pos?
		barData = &song->barData[song->currentBar];
		for (track = 0; track < NUM_TRACKS; track++)
			{
			patternIndex = barData->patternIndex[track];
			if (patternIndex)			// Pattern index 0 = "empty pattern"
				{ 
				patternData = &song->patternData[patternIndex];
				if (patternData)
					{
					noteData = &patternData->notes[song->currentPatternPos];
					if (noteData->instrIndex)			// Instrument index 0 = "no instrument" (do nothing)
						{
						// Trigger instrument on this channel
						instrument = song->instruments[noteData->instrIndex];
						if (instrument)
							{
							//freq = midiPitchMap[noteData->noteNumber];
							//StartSound(track, instrument, noteData->volume, freq);
							// 2020-01 - Use 12-note MIDI not to reload map, + octave modifier
							reload = midiReloadMap[noteData->noteNumber % 12];
							instrument->octave = 6 - (noteData->noteNumber / 12);
							// 2020-04-30 - Removed volume parameter (unused bt StartSound)
							//StartSound(track, instrument, noteData->volume, reload);
							StartSound(track, instrument, reload);
							}
						}
					}
				}
			}
		
		// Increment pattern position?
		song->currentPatternPos++;
		if (PATTERN_SIZE == song->currentPatternPos)
			{
			// go to next bar in the song, or loop to start
			song->currentBar++;
			if (song->currentBar == song->numBars)
				song->currentBar = 0;
				
			song->currentPatternPos = 0;
			}

		// Reset tick
		song->tick = song->speed;  // (speed - 1 ?)
		}
	else
		{
		song->tick--;
		}
}
