#ifndef effects_garland1d_h
#define effects_garland1d_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_Palettes.h"
#include "effects/LEDraw/LEDraw_1d.h"
#include "effects/LEDraw/LEDraw_Math.h"

// ============= Classical Garland / Классические эффекты для Гирлянды ===============

// ***** JUGGLE PALETTE / ЖОНГЛЁР ПОЛЛИТРА *****
/* This is adapted from a routine created by Mark Kriegsman */
// e.speed - 4 Sectors for number of dots (2...5), Range of it is speed
// e.scale - 8 Sectors for Palette selector, Range of it is smoothing
// v1.0 - Updating for pp-Disco Lamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants by PalPalych 13.10.2020
// v1.2 - correct for perfect smoothness 23.10.2020

#define e_numdots e.A.u8a //define a known names of universal variables / определение понятных имен для универсальных переменных
#define e_dotsrange e.A.u8d
#define e_bnumdots e.D.u8a
#define e_palette e.D.u8c
#define e_bpalette e.D.u8b
#define e_fade e.A.u8c
#define e_speed e.A.u8b
#define e_bspeed e.D.u8d
#define e_cycle e.B.u8c
#define e_index e.B.u8a
#define e_shift e.B.u8b
#define e_delta e.B.u8d
#define e_color e.C.color

static bool
jugglePaletteRoutine(effect_t &e) // Several colored dots, weaving in and out of sync with each other
{
	if (e.hasChangedEffect())
		{
			e.timer.setInterval(20); // 50 FPS
			e.initBackupVariables(6);
		}	

	if (e.hasChangedSpeed())
	{																		 // speed has changed / параметр "cкорость" был изменен
		e_numdots = e.getSpeedMode() + 2U; // e_numdots = number of dots (2...5) / количество светящихся точек
		if (e_bnumdots != e_numdots)			 // clear screen if num dots has changed / очистить экран если количество точек изменено
		{
			clearAll(e.leds);
			e_bnumdots = e_numdots;
		};
		e_dotsrange = 240U / e_numdots;
		e_bspeed = e_speed;
		e_speed = scale16by8(NUM_LEDS, map8(e.getSpeedValue(), 4U, 48U - e_numdots * 2)); // speed of dots / скорость сеетящихся точек
		e.correctPhase(e_bspeed, e_speed);																								// correct speed phase / корректировать фазу при изменении скорости
	};

	if (e.hasChangedScale()) // scale has changed / параметр "масштаб" был изменен
	{
		e_palette = e.getScaleMode(); // e_palette = number of palette (0...7) / номер поллитры
		if (e_bpalette != e_palette)	// clear screen if pallete has changed / очистить экран если палитра изменена
		{
			clearAll(e.leds);
			e_bpalette = e_palette;
		};
		e.setPalette(palettes_fastled[e_palette]);												 // выбор поллитры
		e_fade = map8(~e.getScaleValue(), 8U + e_numdots, 128U - e_speed); // величина затухания эффекта
	}

	if (e.timer.isReady()) 
	{
		blur1d(e.leds, NUM_LEDS, 3);
		fadeToBlackBy(e.leds, NUM_LEDS, e_fade);								 //Затухание к черному
		EVERY_N_MILLIS(beatsin16(2U, 13U, 666U)) { e_shift++; }; // Color index of the base point
		e_delta = beatsin16(1U, 8U, e_dotsrange);								 // Color difference of serial points
		e_index = e_shift;
		for (e_cycle = 0; e_cycle < e_numdots; e_cycle++)
		{
			e.i = wrap(beatsin16(e_speed + e_cycle + e_numdots, 0, NUM_LEDS_d, 0, e.speed_phase));
			e_color = e.ColorFromPalette(e_index, 255, LEDRAW_BLENDTYPE);
			fillSegment(e.i, e.indexes[e_cycle], e_color, e.colors[e_cycle], overlayMaximum, e.leds); // overlayAdditiveSoft - for more expressive, overlayMaximum - for smoothness
			e.indexes[e_cycle] = e.i;
			e.colors[e_cycle] = e_color;
			e_index += e_delta;
		}
		return true;
	}
	return false;
} // jugglePaletteRoutine()

// ***** WAVE PALETTE / ВОЛНОВАЯ ПОЛЛИТРА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych (evolve from 3SINE_PALETTE) 28.09.2020
// v1.1 - 16-bit logic for smoothing by PalPalych 01.10.2020
// v1.2 - use ledraw constants by PalPalych 13.10.2020

static bool wavePaletteRoutine(effect_t &e)
{
	EVERY_N_MILLIS(20) // 50 FPS
	{
		if (e.hasChangedSpeed())
		{
			//e.n1u = e.getSpeedMode() + 2U;																	//количество светящихся точек
			//e.tAu = map8(e.getSpeedValue(), 8U - e.n1u, 30U - e.n1u); //скорость сеетящихся точек
		};
		if (e.hasChangedScale())
		{
			e.setPalette(palettes_fastled[e.getScaleMode()]); // выбор поллитры
																												//e.d1u = qsub8((e.getScaleValue() >> 3U) + 8U, e.n1u); //величина затухания эффекта
		};
		EVERY_N_MILLIS(e.speed)
		{
			e.A.u16ab += LED_RANGE16 / 53; // *thisdir;
			e.B.u16ab -= LED_RANGE16 / 21; // *thisdir;
			e.C.u16ab += LED_RANGE16 / 13; // *thisdir;
		}
		EVERY_N_MILLIS(1000)
		{
			e.C.u8d++;
		}
		for (e.i = 0; e.i < NUM_LEDS; e.i++)
		{
			e.D.u8a = ((sin16(LED_RANGE16 * e.i * (beatsin16(3, 4096, 28000)) / 24576 + beatsin16(4, 0, 32768, 0, e.A.u16ab) -
												beatsin16(5, 0, 21845, 0, e.B.u16ab) + beatsin16(7, 0, 16384, 0, e.C.u16ab)))>>8U);
			e.leds[wrap(e.i)] = e.ColorFromPalette(e.D.u8a + e.C.u8d, 255, LINEARBLEND);
			//e.leds[wrap(e.i)] = getColorFromPaletteSet(e.i8u, e.scale);
		}
		return true;
	}
	return false;
}

// ***** PLASMA PALETTE / ПЛАЗМА ПОЛЛИТРА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants by PalPalych 13.10.2020

static bool plasmaPaletteRoutine(effect_t &e)
{ // This is the heart of this program. Sure is short. . . and fast.

	EVERY_N_MILLIS(20) // 50 FPS
	{
		if (e.hasChangedEffect())
		{
		};
		if (e.hasChangedScale())
		{
			e.setPalette(palettes_fastled[e.getScaleMode()]); // Choose palette
			e.A.u8a = map8(e.getScaleValue(), 4, 12);					// u8Aa = Phase 1
			e.A.u8b = ~e.A.u8a;																// u8Ab = Phase 2
		};
		if (e.hasChangedSpeed())
		{
			e.A.u8c = e.speed / NUM_LEDS;		 // u8Ac = cubic freq
			e.A.u8d = (~e.speed) / NUM_LEDS; // u8Ad = sine freq
		};

		e.B.u8a = beatsin8(map8(e.A.u8a, 2, 10), 0, 192); // u8Ba = Setting phase change for a couple of waves.
		e_shift = beatsin8(map8(e.A.u8b, 3, 15), 0, 128); // u8Bb
		for (e.i = 0; e.i < NUM_LEDS; e.i++)
		{																																															// For each of the LED's in the strand, set a brightness based on a wave as follows:
			e.C.u8c = cubicwave8((e.i * e.A.u8c) + e.B.u8a) - cos8((e.i * e.A.u8d) + e.B.u8b);					// Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish, but don't change on the fly.
			e.leds[e.i] = e.ColorFromPalette(e.C.u8c, qsuba(e.C.u8c, beatsin8(7, 0, 96)), LINEARBLEND); // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..
																																																	// Let's now add the foreground colour.
																																																	//leds[k] = getColorFromPaletteSet(colorIndex, e.scale, 0, 0, thisBright);									// Let's now add the foreground colour.
		}
		return true;
	}
	return false;
} // plasmaPaletteRoutine()

// ***** BLEND WAVE PALETTE / СМЕШАННАЯ ПОЛЛИТРА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - palettes by PalPalych 23.10.2020

bool blendWavePaletteRoutine(effect_t &e)
{
	EVERY_N_MILLIS(20) // 50 FPS
	{
		if (e.hasChangedScale())
		{
			e.setPalette(palettes_fastled[e.getScaleMode()]); // Choose palette
			e.C.u8a = map8(e.getScaleValue(), 1U, 8U);				// u8Ca = speed of colors change
		};

		e.A.color = e.ColorFromPalette(beat8(2 + e.C.u8a), 255, LINEARBLEND);
		e.B.color = e.ColorFromPalette(beatsin8(5 + e.C.u8a), 255, LINEARBLEND);
		e.i = beatsin16(map8(e.speed, 4, 15), START_LED, NUM_LEDS_d); // u8Cc = amount of border change

		fillRange(START_LED, e.i, e.B.color, e.A.color, e.leds);
		fillRange(e.i, NUM_LEDS_d, e.B.color, e.A.color, e.leds);
		return true;
	}
	return false;
} // blendwave()

// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.

bool colorWavesRoutine(effect_t &e)
{
	static uint16_t sPseudotime = 0;
	static uint16_t sLastMillis = 0;
	static uint16_t sHue16 = 0;

	uint16_t i;

	uint8_t brightdepth = beatsin88(341, 96, 224);
	uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
	uint8_t msmultiplier = beatsin88(147, 23, 60);

	uint16_t hue16 = sHue16; //gHue * 256;
	uint16_t hueinc16 = beatsin88(113, 300, 1500);

	uint16_t ms = millis();
	uint16_t deltams = ms - sLastMillis;
	sLastMillis = ms;
	sPseudotime += deltams * msmultiplier;
	sHue16 += deltams * beatsin88(400, 5, 9);
	uint16_t brightnesstheta16 = sPseudotime;

	for (e.i = 0; e.i < NUM_LEDS; e.i++)
	{
		hue16 += hueinc16;
		uint8_t hue8 = hue16 / 256;
		uint16_t h16_128 = hue16 >> 7;
		if (h16_128 & 0x100)
			hue8 = 255 - (h16_128 >> 1);
		else
			hue8 = h16_128 >> 1;

		brightnesstheta16 += brightnessthetainc16;
		uint16_t b16 = sin16(brightnesstheta16) + 32768;

		uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
		uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
		bri8 += (255 - brightdepth);

		uint8_t index = hue8;
		//index = triwave8( index);
		index = scale8(index, 240);

		e.A.color = e.ColorFromPalette(index, 255, LINEARBLEND);
		//CRGB newcolor = getColorFromPaletteSet(index, e.scale, 0, 0, bri8);
		/*if (thisdir == 1) //направление
				nblend(leds[(NUM_LEDS - 1) - i], newcolor, 128);
			else*/
		nblend(e.leds[e.i], e.A.color, 128);
	}
	return true;
};

// ***** SERENDIPITOUS / СЕРЕНДИПИТНЫЙ *****
/*	By : Andrew Tuline *Date : August,	2016 
		This is from Serendipitous Circles from the August 1977 and April 1978 issues of Byte Magazine. 
		I didn't do a very good job of it, but am at least getting some animation and the routine is very short. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020 (maybe harmonic effect)
// v1.1 - ledraw logic + palettes + regulators by PalPalych 24.10.2020

bool serendipitousPaletteRoutine(effect_t &e)
{
	EVERY_N_MILLIS(20) // 50 FPS
	{
		if (e.hasChangedSpeed())
		{ // параметр Скорость был изменен
		};
		if (e.hasChangedScale())
		{																										// параметр Масштаб был изменен
			e.setPalette(gradientPalettes[e.getScaleMode()]); // выбор поллитры
			e.C.u8a = map8(e.getScaleValue(), 1, 8);					//скорость сеетящихся точек
		}
		EVERY_N_SECONDS(5)
		{
			e.A.u16ab = 0x013;
			e.A.u16cd = 0x021;
		}

		fadeToBlackBy(e.leds, NUM_LEDS, 10); // 8 bit, 1 = slow, 255 = fast

		e.B.u16ab = e.A.u16ab - ((float)e.A.u16cd * 2 / e.C.u8a);
		e.B.u16cd = e.A.u16cd + ((float)e.B.u16ab / (4 - e.C.u8a / 3));
		e.A.u16ab = e.B.u16ab;
		e.A.u16cd = e.B.u16cd;
		e.A.u8c = (sin8(e.A.u16ab) + cos8(e.A.u16cd)) >> 1U;
		nblend(e.leds[wrap(e.A.u16ab)], e.ColorFromPalette(e.A.u8c, 255, LINEARBLEND), 224);
		return true;
	}
	return false;
} // serendipitous_pal()

bool rainbowBeatRoutine(effect_t &e)
{																										// The fill_rainbow call doesn't support brightness levels
	e.A.u8a = beatsin8(map8(e.scale, 7, 23), 0, 255); // Starting hue
	e.A.u8b = beatsin8(map8(e.scale, 5, 37), 0, 255);
	fill_rainbow(e.leds, NUM_LEDS, (e.A.u8a + e.A.u8b) / 2, 8); // Use FastLED's fill_rainbow routine.
	return true;
} // rainbow_beat()

// ***** BEATWAVE PALETTE / БИТВЭЙВ ПОЛЛИТРА *****
/* By : Andrew Tuline   Date : October,	2015
	 This animation combines coloured wave values from several beatsin8() functions. See what you can do to mix this one up. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 24.10.2020

static bool beatWavePaletteRoutine(effect_t &e)
{
	if (e.hasChangedSpeed())
	{
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // выбор поллитры
	};
	e.A.u8a = (beatsin16(9) + beatsin16(8) + beatsin16(7) + beatsin16(6)) >> 8U; // That's the same as beatsin8(9);
	for (e.i = 0; e.i < NUM_LEDS; e.i++)
	{
		e.leds[wrap(e.i)] = e.ColorFromPalette(e.i + e.A.u8a, 255, LINEARBLEND);
	}
	return true;
}

// ***** EASE PALETTE / ПРОСТОТА ПОЛЛИТРА *****
/* By : Andrew Tuline   Date : October,	2015
	 This boring display demonstrates the easing capability of FastLED. 
	 The Red LED starts out slow, speeds up and then slows down when it gets to the end. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 24.10.2020

static bool easePaletteRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
		e.initBackupVariables(1);
	if (e.hasChangedSpeed())
	{
		e.A.u8d = e.getSpeedMode() & 0x01;																			 // get reverse from speed regulator
		e.C.u8a = map8(e.A.u8d ? ~e.getSpeedValue() : e.getSpeedValue(), 5, 40); // get speed from speed regulator
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // выбор поллитры
		e.C.u8b = map8(e.getScaleValue(), 2, 25);					// select blur from scale regulator
	};

	EVERY_N_MILLIS(e.C.u8a) // 50 FPS
	{
		e.indexes[0] = e.i;
		e.D.color = e.leds[e.indexes[0]];
		e.A.u8c = ease8InOutQuad(e.A.u8b); // Start with easeInVal at 0 and then go to 255 for the full easing.
		e.A.u8b++;
		e.A.u8a = scale16by8(NUM_LEDS_d, e.A.u8c); // Map it to the number of LED's you have.

		e.i = mirror(wrap(e.A.u8a), e.A.u8d); // Map that to the length of the strand.
		if (!e.A.u8a)
			e.indexes[0] = e.i;
		fadeToBlackBy(e.leds, NUM_LEDS, e.C.u8b); // 8 bit, 1 = slow fade, 255 = fast fade
		fillSegment(e.i, e.indexes[0], e.ColorFromPalette(e.A.u8c, 255, LINEARBLEND), e.D.color, overlayMaximum, e.leds);
		return true;
	}
	return false;
}

// ***** NOISEFIRE PALETTE / ШУМОВОЙ ОГОНЬ ПОЛЛИТРА *****
/* By : Andrew Tuline   Date : January,	2017
	 This super short sketch (just two lines to the algorithm) displays fire thanks to FastLED's Perlin Noise function and Palettes.
   It needs some tweaking in order to work across a wide range of NUM_LED values, but looks pretty good at 60. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 24.10.2020

static bool noiseFirePaletteRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		e.B.u16ab = 20 * 256; // xscale How far apart they are
		e.B.u16cd = 3 * 256;	// yscale How fast they move
	};
	if (e.hasChangedSpeed())
	{
		e.A.u8d = e.getSpeedMode() & 0x01;																			 // get reverse from speed regulator
		e.A.u8c = map8(e.A.u8d ? ~e.getSpeedValue() : e.getSpeedValue(), 5, 25); // get speed from speed regulator
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // выбор поллитры
	};
	for (e.D.u16cd = 0; e.D.u16cd < NUM_LEDS; e.D.u16cd++)
	{
		e.A.u8a = inoise16(e.D.u16cd * e.B.u16ab, millis() * e.B.u16cd * NUM_LEDS / 255) >> 8U;																												// X location is constant, but we move along the Y at the rate of millis()
		e.leds[mirror(wrap(e.D.u16cd), e.A.u8d)] = e.ColorFromPalette(min(e.D.u16cd * (e.A.u8a) >> 6, 255), e.D.u16cd * 255 / NUM_LEDS, LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
	}
	return true;
};

// ***** NOISEMOVER PALETTE / ШУМОВОЙ ДВИЖИТЕЛЬ ПОЛЛИТРА *****
/* By : Andrew Tuline   Date : February,	2017
	 We've used sine waves and counting to move pixels around a strand. In this case, I'm using Perlin Noise to move a pixel up and down the strand.
   The advantage here is that it provides random natural movement without requiring lots of fancy math by joe programmer. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 24.10.2020

static bool noiseMoverPaletteRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		e.initBackupVariables(1);
		// i16Bcd = dist A moving location for our noise generator.
		e.C.u16ab = 30; // xscale Wouldn't recommend changing this on the fly, or the animation will be really blocky.
		e.C.u16cd = 30; // yscale Wouldn't recommend changing this on the fly, or the animation will be really blocky.
		e.A.u8c = 24;		// maxChanges Value for blending between palettes.
	};
	if (e.hasChangedSpeed())
	{
		e.A.u8d = e.getSpeedMode() & 0x01;																			 // get reverse from speed regulator
		e.A.u8a = map8(e.A.u8d ? ~e.getSpeedValue() : e.getSpeedValue(), 5, 25); // get speed from speed regulator
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // select palette from scale regulator
		e.A.u8b = map8(e.getScaleValue(), 5, 25);					// select blur from scale regulator
	};
	e.indexes[0] = e.i;
	e.colors[0] = e.leds[e.indexes[0]];
	EVERY_N_MILLIS(20) // 50 FPS
	{
		fadeToBlackBy(e.leds, NUM_LEDS, e.A.u8b); // 8 bit, 1 = slow fade, 255 = fast fade
		e.C.u16cd += e.A.u8a;
	}
	e.B.i16cd = beatsin8(5, 0, 215);																// Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
	e.B.u16ab = inoise8(e.C.u16cd, e.B.i16cd);											// Get a new pixel location from moving noise.
	e.i = mirror(wrap(scale16by8(NUM_LEDS_d, e.B.u16ab)), e.A.u8d); // Map that to the length of the strand.
	fillSegment(e.i, e.indexes[0], e.ColorFromPalette(e.B.u16ab, 255, LINEARBLEND), e.colors[0], overlayMaximum, e.leds);
	return true;
};

// ***** SINELON PALETTE / САЙНЛОН ПОЛЛИТРА *****
/* By: Mark Kriegsman    Modified by: Andrew Tuline    Date : February,	2015
	 his uses the built in beat in FastLED to move a dot back and forth. In this case, it uses two beats added together for more randomness. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 25.10.2020

static bool sinelonPaletteRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		e.initBackupVariables(1);
		e.C.u8a = 23; // e.C.u8a = thisbeat Beats per minute for first part of dot.
		e.C.u8b = 28; // e.C.u8b = thatbeat Combined the above with this one.
									// How quickly does it fade? Lower = slower fade rate.
									// e.C.u8d = myhue
	};
	if (e.hasChangedSpeed())
	{
		e.A.u8c = e.getSpeedMode() & 0x01;																			 // get reverse from speed regulator
		e.A.u8a = map8(e.A.u8d ? ~e.getSpeedValue() : e.getSpeedValue(), 5, 25); // get speed from speed regulator
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // select palette from scale regulator
		e.A.u8b = map8(e.getScaleValue(), 5, 25);					// select blur from scale regulator
	};

	EVERY_N_MILLIS(20) // 50 FPS
	{
		e.indexes[0] = e.i;
		e.D.color = e.leds[e.indexes[0]];
		fadeToBlackBy(leds, NUM_LEDS, e.A.u8b);
		e.C.u8d++;
		e.B.u16ab = (beatsin16(e.C.u8a, 0, NUM_LEDS) + beatsin16(e.C.u8b, 0, NUM_LEDS)) >> 1U;
		e.i = mirror(crop(e.B.u16ab), e.A.u8c);
		e.leds[e.i] += e.ColorFromPalette(e.C.u8d, 255, LINEARBLEND);
		fillSegment(e.i, e.indexes[0], e.leds[e.i], e.D.color, overlayMaximum, e.leds);
		return true;
	};
	return false;
};

// ***** SAWTOOTH PALETTE / ЧЕЛЮСТИ ПОЛЛИТРА *****
/* By : Andrew Tuline   Date : February,	2018
	 Creating a sawtooth pattern using millis and bpm. It runs at about 440 fps with the configuration below on an Arduino Nano. */
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 28.09.2020
// v1.1 - use ledraw constants + fast smooth logic by PalPalych 25.10.2020

static bool sawtoothPaletteRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		e.A.u8b = 60;													// bpm
		e.C.u8a = 60000 / e.A.u8b;						// ms_per_beat 500ms per beat, where 60,000 = 60 seconds * 1000 ms
		e.C.u8b = 60000 / e.A.u8b / NUM_LEDS; // ms_per_led
	};
	if (e.hasChangedSpeed())
	{
		e.A.u8c = e.getSpeedMode() & 0x01;																			 // get reverse from speed regulator
		e.A.u8a = map8(e.A.u8d ? ~e.getSpeedValue() : e.getSpeedValue(), 5, 25); // get speed from speed regulator
	};
	if (e.hasChangedScale())
	{
		e.setPalette(palettes_fastled[e.getScaleMode()]); // select palette from scale regulator
		e.A.u8b = map8(e.getScaleValue(), 5, 25);					// select blur from scale regulator
	};

	EVERY_N_MILLIS(20) // 50 FPS
	{
		e.i = ((millis() % e.C.u8a) / e.C.u8b) % (NUM_LEDS); // Using millis to count up the strand, with %NUM_LEDS at the end as a safety factor.

		if (e.i == 0)
			fill_solid(e.leds, NUM_LEDS, CRGB::Black);
		else
			e.leds[e.i] = e.ColorFromPalette(0, 255, LINEARBLEND); // I prefer to use palettes instead of CHSV or CRGB assignments.
		return true;
	};
	return false;
};

#endif
