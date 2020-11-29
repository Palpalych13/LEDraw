#ifndef _LEDraw_indicators1d_h
#define _LEDraw_indicators1d_h
//#pragma ones
#include <stdint.h>
#include <FastLED.h>
#include "effects/LEDraw/LEDraw_1d.h"

// LEDraw! Fast Drawing Library for FastLED (1d section) by PalPalych v0.1b

static const CRGB resistor_colors[] = {
		CRGB::Black,
		0x400800,
		CRGB::Red,
		CRGB::Orange,
		CRGB::Yellow,
		CRGB::Green,
		CRGB::Blue,
		CRGB::Magenta,
		0x404040,
		CRGB::White};

struct decimals_t
{
	uint8_t digits[5];
	void calc(uint16_t value)
	{
		for (int8_t i = 0; i < 5; i++)
		{
			digits[i] = value % 10;
			value /= 10;
		}
	};

} decimals;

template <typename T, typename C>
void drawResistor(uint16_t value, T position, C backcolor, CRGB *LEDarray = effect.leds)
{
	drawPixel_safe(position++, backcolor);

	decimals.calc(value);
	for (int8_t i = 4; i >= 0; i--)
	{
		drawPixel_safe(position++, resistor_colors[decimals.digits[i]]);
		drawPixel_safe(position++, resistor_colors[decimals.digits[i]]);
		drawPixel_safe(position++, backcolor);
		if (i == 4)
			drawPixel_safe(position++, backcolor);
	}
};

static void (*osd_tick_action)() = []() {
	dimRange_safe(7, 29, effects.osd.current);
	if (effects.osd.current == 255U)
		drawResistor(effects.osd._value, 10, CRGB::SeaGreen);
};
#endif
