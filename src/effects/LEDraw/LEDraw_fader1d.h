#ifndef _LEDraw_fader1d_h
#define _LEDraw_fader1d_h
//#pragma ones
#include <stdint.h>
#include <FastLED.h>
#include "effects/LEDraw/LEDraw_1d.h"

// LEDraw! Fast Drawing Library for FastLED (1d section) by PalPalych v0.1b
// + family functions subPixel -- 23.05.2020

static void (*fader_actions[])(uint8_t, bool, bool, CRGB *) = {
		*[](uint8_t value, bool direction, bool reverse, CRGB *leds1) { dimAll(dim8_lin(value), leds1); }, // 0: Just fade

		*[](uint8_t value, bool direction, bool reverse, CRGB *leds1) {
			dimAll(dim8_lin(value), leds1);
			int16_t pos = faderPosition(value, direction, NUM_LEDS_d, EIGHTH_LEDS);
			fillRange(mirror(pos, reverse), (direction ^ reverse) ? START_LED : NUM_LEDS_d, CRGB::Black, leds1);
			drawSparkler(pos, EIGHTH_LEDS, EIGHTH_LEDS, (CRGB)0xF7DC6F, reverse, overlayAdditive, leds1);
		}, // 1: Sparkler

		*[](uint8_t value, bool direction, bool reverse, CRGB *leds1) {
			dimAll(dim8_lin(value), leds1);
			int16_t pos = faderPosition(value, direction, NUM_LEDS_d, QUARTER_LEDS) + QUARTER_LEDS;
			fillRange_safe(mirror(pos, reverse), (direction ^ reverse) ? START_LED : NUM_LEDS_d, CRGB::Black, leds1);
			drawComet(pos, QUARTER_LEDS, (CRGB)0xFFFFFF, reverse, overlayMaximum, leds1);
		}, // 2: Comet
};

static void(*fader_tick_action)() = []() { fader_actions[effects.fader._effect](effects.fader.current, effects.fader.direction(), effects.fader.reverse(), effects.fader.leds); };

#endif
