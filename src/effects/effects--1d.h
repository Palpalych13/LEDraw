#ifndef effects1d_h
#define effects1d_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_Palettes.h"
#include "effects/LEDraw/LEDraw_1d.h"

// ============= Classical Effects for Gyver Lamp / Классические эффекты для лампы Гайвера ===============
// ***** SPARKLES / КОНФЕТТИ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 11.03.2020
// v1.1 - Add white pixels and spark divider by PalPalych 12.03.2020
// v1.2 - Add dither and speed correction by PalPalych 21.03.2020
// v1.3 - Universal code 1D & 2D by PalPalych 27.09.2020
// v1.4 - Universal constants by PalPalych 25.10.2020

#define e_spk_FADE_OUT_SPEED 32U	 // скорость затухания
#define e_spk_ADD_WHITE_PIXELS 16U // каждый N конфетка будет белая (0...1 - все белые)
#define e_spk_DIVIDER 8U					 // каждая N конфетка будет прорисовываться (>0!!!) Влияет на скорость!
#define e_spk_DITHER 2U						 // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново) Влияет на скорость!
static const uint8_t e_spk_amount = (NUM_LEDS * e_spk_FADE_OUT_SPEED) >> 8;

bool sparlkesRoutine(effect_t &e)
{
	if (e.hasChangedSpeed())
	{
		e.A.u8a = map8(e.getSpeedValue(), 5, 25); // get speed from speed regulator
	};

	EVERY_N_MILLIS(20) // 50 FPS
	{
		fadeAll(e.A.u8a);
		for (e.B.u8a = 0; e.B.u8a < scale8_video(e_spk_amount, e.scale); e.B.u8a++)
		{
			if (!random8(e_spk_DIVIDER))
			{
				e.i = wrap(random16(0U, NUM_LEDS));
				if (!getPixelValue(e.i))
					e.leds[e.i] = CHSV(random8(0U, 255U), random8(e_spk_ADD_WHITE_PIXELS) ? 255U : 0, 255U);
			}
		}
		return true;
	};
	return false;
};

	// ***** COLORS / ЦВЕТА *****
	// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
	// v1.1 - FastLogic by PalPalych 26.03.2020
	// v1.2 - SatCorrection PalPalych 19.04.2020
	// v1.3 - new command logic PalPalych 21.04.2020
	// v1.4 - universal variables PalPalych 27.10.2020

#define e_cols_DITHER 8U // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool colorsRoutine(effect_t &e)
{
#if (e_cols_DITHER)
	if (e.frame_counter % e_cols_DITHER)
		return false;
#endif

	if (e.hasChangedScale())
		e.A.u8d = e.getScaleValue(); // get speed from speed regulator

	if (e.A.u8c)
	{
		e.C.u8b = beatsin8((e.scale - 120U) >> 3U);
		EVERY_N_MILLIS(138U) { e.C.u8a++; }
	}
	else
	{
		EVERY_N_MILLIS((uint16_t)e.A.u8d * 2U + 11U)
		{
			if (e.scale < 4U)
				e.C.u8a += (5U - e.scale);
			else
				e.C.u8a++;
		}
	}
	e.A.u8b = e.speed ? brighten8_raw(beatsin8(e.speed >> 1U)) : 255U;
	fillAll(CHSV(e.C.u8a + e.C.u8b, e.A.u8b, 255U), e.leds);
	return true;
}

// ***** COLOR / ЦВЕТ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Dither & Saturation selection by PalPalych 20.03.2020
// v1.2 - universal variables by PalPalych 27.10.2020

bool colorRoutine(effect_t &e)
{
	if (e.hasChangedSpeed())
		e.A.u8a = e.getSpeedValue(); // get speed from speed regulator

	if (e.hasChangedScale())
		e.A.u8b = e.getScaleValue(); // get speed from speed regulator

	if (e.hasChangedParams() || effects.fader.active())
	{
		fillAll(CHSV(e.A.u8b, e.A.u8a, 255U), e.leds);
		return true;
	};
	return false;
}

// ***** RAINBOW / РАДУГА *****
// v1.0 - Updating for Stripe v1.7 by PalPalych 27.09.2020
#define e_rbw_DITHER 2U // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool rainbowRoutine(effect_t &e) // <- вхождение эффекта
{
#if e_rbw_DITHER
	if (e.frame_counter % e_rbw_DITHER)
		return false;
#endif
	uint32_t basehue = millis() / constrain(e.speed / 16, 1U, 255U);
	for (uint16_t x = 0U; x < NUM_LEDS; x++)
	{
		drawPixel(x, CHSV(basehue + (x * 3 * (e.scale / WIDTH + 1U)), 255, 255));
	}
	return true;
}

// ***** STARDUST / ЗВЕЗДНАЯ ПЫЛЬ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 27.09.2020
#define e_sd_DITHER 8U // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool starDustRoutine(effect_t &e)
{
	noiseAll(e.scale, 255);
	return true;
}

#endif
