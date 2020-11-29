#ifndef _LEDraw_1d_h
#define _LEDraw_1d_h
//#pragma ones
#include <stdint.h>
#include <FastLED.h>
#include "effects/LEDraw/LEDraw_Palettes.h"

// LEDraw! Fast Drawing Library for FastLED (1d section) by PalPalych v0.1b
// + family functions subPixel -- 23.05.2020

//  7/8/15/16-bit index of leds array for quick calculation
#if NUM_LEDS <= 128
static const int8_t START_LED = 0;													//
static const int8_t NUM_LEDS_d = NUM_LEDS - 1U;							// number of leds in digital count
static const int8_t HALF_LEDS = NUM_LEDS / 2U;							// половина ширины (минорное значение)
static const int8_t HALF_LEDS_d = HALF_LEDS - 1U;						// половина ширины (минорное значение в цифровом представлении)
static const int8_t QUARTER_LEDS = NUM_LEDS / 4U;						// четверть ширины (минорное значение)
static const int8_t QUARTER_LEDS_d = QUARTER_LEDS - 1U;			// четверть ширины (минорное значение в цифровом представлении)
static const int8_t EIGHTH_LEDS = NUM_LEDS / 8U;						// восьмая часть ширины (минорное значение)
static const int8_t EIGHTH_LEDS_d = EIGHTH_LEDS - 1U;				// восьмая часть ширины (минорное значение в цифровом представлении)
static const int8_t SIXTEENTH_LEDS = NUM_LEDS / 16U;				// шестнадцатая часть ширины (минорное значение)
static const int8_t SIXTEENTH_LEDS_d = SIXTEENTH_LEDS - 1U; // шестнадцатая часть ширины (минорное значение в цифровом представлении)
#else
static const int16_t START_LED = 0;																			//
static const int16_t NUM_LEDS_d = NUM_LEDS - 1U;												// number of leds in digital count
static const int16_t NUM_LEDS_crop = 65535U - (65535U - NUM_LEDS) / 2U; // number of leds for uint8 cropped logic
static const int16_t HALF_LEDS = NUM_LEDS / 2U;													// половина ширины (минорное значение)
static const int16_t HALF_LEDS_d = HALF_LEDS - 1U;											// половина ширины (минорное значение в цифровом представлении)
static const int16_t QUARTER_LEDS = NUM_LEDS / 4U;											// четверть ширины (минорное значение)
static const int16_t QUARTER_LEDS_d = QUARTER_LEDS - 1U;								// четверть ширины (минорное значение в цифровом представлении)
static const int16_t EIGHTH_LEDS = NUM_LEDS / 8U;												// восьмая часть ширины (минорное значение)
static const int16_t EIGHTH_LEDS_d = EIGHTH_LEDS - 1U;									// восьмая часть ширины (минорное значение в цифровом представлении)
static const int16_t SIXTEENTH_LEDS = NUM_LEDS / 16U;										// шестнадцатая часть ширины (минорное значение)
static const int16_t SIXTEENTH_LEDS_d = SIXTEENTH_LEDS - 1U;						// шестнадцатая часть ширины (минорное значение в цифровом представлении)
#endif
static const uint8_t LED_RANGE8 = 256U / NUM_LEDS + 1U;		 // диапазон значений приходящихся на 1 пиксель длины ленты
static const int16_t LED_RANGE16 = 65536U / NUM_LEDS + 1U; // диапазон значений приходящихся на 1 пиксель длины ленты

// *** chech the index of pixel if it in legal range / проверка адреса пикселя на нахождения в своих границах
template <typename T>
inline static bool inRange(T index) //
{
	if ((index >= 0) && (index <= NUM_LEDS_d))
		return true;
	return false;
}

// *** wrap the index of pixel around array's borders / функции "оборачивания" координаты относительно границ
#if NUM_LEDS <= 128
inline static int8_t wrap(int8_t index) // not so quick in leds array number less than 128 but compatible for other logic
{
	return mod8(add8(index, NUM_LEDS), NUM_LEDS);
}
#else
inline static int16_t wrap(int16_t index)
{
	return (index % NUM_LEDS);
}
#endif

// *** crop the index of pixel over array's borders / функции "обрезания" координаты относительно границ
template <typename T>
inline static T crop(T index) // not so quick in leds array number less than 128 but compatible for other logic
{
	if (index < 0)
		return 0;
	if (index > NUM_LEDS_d)
		return NUM_LEDS_d;
	return index;
}

// *** mirror the index of pixel in an array / функции "отражения" координаты относительно границ
template <typename T>
inline static T mirror(T index) // of permanent action
{
	return NUM_LEDS_d - index;
}

template <typename T>
inline static T mirror(T index, bool reverse) // of selectable action
{
	return reverse ? NUM_LEDS_d - index : index;
}

// *** get color value of pixel by index / функция получения цвета пикселя по его номеру
template <typename T>
static uint32_t getPixelValue(T index, CRGB *LEDarray = leds)
{
	CRGB color = LEDarray[index];
	return (((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) | (uint32_t)color.b);
}

// blurAll: two-dimensional blur filter. Spreads light to 8 XY neighbors.
//           0 = no spread at all
//          64 = moderate spreading
//         172 = maximum smooth, even spreading
//         173..255 = wider spreading, but increasing flicker
//
//         Total light is NOT entirely conserved, so many repeated
//         calls to 'blur' will also result in the light fading,
//         eventually all the way to black; this is by design so that
//         it can be used to (slowly) clear the LEDs to black.
static void blurAll(fract8 blur_amount, CRGB *LEDarray = leds)
{
#if (LEDSARRAY_TYPE == MATRIX)
#if (MATRIX_DIRECTION & 0x01)
	blur2d(LEDarray, HEIGHT, WIDTH, blur_amount);
#else
	blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
#endif
#else
	blur1d(LEDarray, NUM_LEDS, blur_amount);
#endif
}

CRGB blend_fast(CRGB color1, CRGB color2, uint8_t amount)
{
	uint8_t namount = ~amount; // <- more faster and more resource-intensive function than 255-f2 (a tiny cent saves a dollar)
	uint8_t red1 = color1.r;
	uint8_t red2 = color2.r;
	red1 = scale8_LEAVING_R1_DIRTY(red1, namount);
	red2 = scale8_LEAVING_R1_DIRTY(red2, amount);
	red1 += red2;

	uint8_t green1 = color1.g;
	uint8_t green2 = color2.g;
	green1 = scale8_LEAVING_R1_DIRTY(green1, namount);
	green2 = scale8_LEAVING_R1_DIRTY(green2, amount);
	green1 += green2;

	uint8_t blue1 = color1.b;
	uint8_t blue2 = color2.b;
	blue1 = scale8_LEAVING_R1_DIRTY(blue1, namount);
	blue2 = scale8_LEAVING_R1_DIRTY(blue2, amount);
	blue1 += blue2;

	cleanup_R1();
	return CRGB(red1, green1, blue1);
}

enum ledraw_overlay
{
	overlayMaximum,
	overlayAdditive,
	overlayMinimum,
	overlaySubstractive,
	overlaySolid,
	_overlay_last,
	overlaySoftSingles = 0x40,	 // soft overlay segments with single length
	overlayExceptSingles = 0x80, // prohibit to overlay segments with single length
	overlayAdditiveSoft = 0x41,
	overlaySubstractiveSoft = 0x43,
	overlayAdditiveExcept = 0x81,
	overlaySubstractiveExcept = 0x83,
	_overlay_mask = 0x3F,
};

static void (*ledrawOverlayMethods[_overlay_last])(CRGB &, CRGB) = {
		[](CRGB &led, CRGB color) { led |= color; },
		[](CRGB &led, CRGB color) { led += color; },
		[](CRGB &led, CRGB color) { led &= color; },
		[](CRGB &led, CRGB color) { led -= color; },
		[](CRGB &led, CRGB color) { led = color; },
};

// *** draw color pixel
template <typename T, typename C>
static void drawPixel(T index, C color, CRGB *LEDarray = leds)
{
	LEDarray[index] = color;
};

template <typename T, typename C>
static void drawPixel_safe(T index, C color, CRGB *LEDarray = leds)
{
	if (inRange(index))
		LEDarray[index] = color;
};

template <typename T, typename C>
static void drawPixel(T index, C color, ledraw_overlay overlay, CRGB *LEDarray = leds)
{
	ledrawOverlayMethods[overlay](LEDarray[index], color);
};

template <typename T, typename C>
static void drawPixel_safe(T index, C color, ledraw_overlay overlay, CRGB *LEDarray = leds)
{
	if (inRange(index))
		ledrawOverlayMethods[overlay](LEDarray[index], color);
};

template <typename T>
static void dimRange_safe(T i1, T i2, uint8_t amount, CRGB *LEDarray = leds) // with solid color
{
	if (i1 == i2)
	{
		if (inRange(i1))
			LEDarray[i1].fadeLightBy(amount);
	}
	else if (i1 < i2)
	{
		for (i1; i1 <= i2; i1++)
			if (inRange(i1))
				LEDarray[i1].fadeLightBy(amount);
	}
	else
		for (i2; i2 <= i1; i2++)
			if (inRange(i2))
				LEDarray[i2].fadeLightBy(amount);
};

// *** draw color line
template <typename T, typename T1, typename C>
static void fillRange(T i1, T1 i2, C color, CRGB *LEDarray = leds) // with solid color
{
	if (i1 == i2)
		LEDarray[i1] = color;
	else if (i1 < i2)
		for (i1; i1 <= i2; i1++)
			LEDarray[i1] = color;
	else
		for (i2; i2 <= i1; i2++)
			LEDarray[i2] = color;
};

template <typename T, typename T1, typename P>
static void fillRange(T i1, T1 i2, P palette, uint8_t brightness, TBlendType blendType, CRGB *LEDarray = leds) // with solid color
{
	if (i1 == i2)
		LEDarray[i1] = ColorFromPalette(*palette, 0, brightness, blendType);
	else if (i1 < i2)
	{
		T range = i2 - i1;
		uint16_t steps = 65535 / range; //slow operation, but it cause one times per filled range
		for (T i = 0; i <= range; i++)
			LEDarray[i1 + i] = ColorFromPalette(*palette, (steps * i) >> 8U, brightness, blendType);
	}
	else
	{
		T range = i1 - i2;
		uint16_t steps = 65535 / range; //slow operation, but it cause one times per filled range
		for (T i = 0; i <= range; i++)
			LEDarray[i2 + i] = ColorFromPalette(*palette, ~((steps * i) >> 8U), brightness, blendType);
	}
};

template <typename T, typename T1, typename C>
static void fillRange_safe(T i1, T1 i2, C color, CRGB *LEDarray = leds) // safe with solid color
{
	if (i1 == i2)
	{
		if (inRange(i1))
			LEDarray[i1] = color;
	}
	else if (i1 < i2)
	{
		for (i1; i1 <= i2; i1++)
			if (inRange(i1))
				LEDarray[i1] = color;
	}
	else
		for (i2; i2 <= i1; i2++)
			if (inRange(i2))
				LEDarray[i2] = color;
};

template <typename T, typename T1, typename C>
static void fillRange_safe(T i1, T1 i2, C color, ledraw_overlay overlay, CRGB *LEDarray = leds) // safe with solid color
{
	if (i1 == i2)
	{
		if (inRange(i1))
			ledrawOverlayMethods[overlay](LEDarray[i1], color); // may overlay single dots on overlayAdditive
		return;
	}
	else if (i1 < i2)
	{
		uint8_t range = i2 - i1;
		for (T i = (i1 + 1); i <= i2; i++)
			if (inRange(i))
				ledrawOverlayMethods[overlay](LEDarray[i], color);
	}
	else
	{
		uint8_t range = i1 - i2;
		for (T i = i2; i < i1; i++)
			if (inRange(i))
				ledrawOverlayMethods[overlay](LEDarray[i], color);
	}
};

template <typename T, typename T1, typename C>
static void fillRange(T i1, T1 i2, C color1, C color2, CRGB *LEDarray = leds) // with blend gradient color
{
	LEDarray[i1] = color1;
	if (i1 < i2)
	{
		uint8_t range = i2 - i1;
		for (T i = (i1 + 1U); i <= i2; i++)
			LEDarray[i] = blend_fast(color1, color2, (uint16_t)255U * (i - i1) / range);
	}
	else if (i1 > i2)
	{
		uint8_t range = i1 - i2;
		for (T i = i2; i < i1; i++)
			LEDarray[i] = blend_fast(color2, color1, (uint16_t)255U * (i - i2) / range);
	}
};

template <typename T, typename T1, typename C>
static void fillRange(T i1, T1 i2, C color1, C color2, ledraw_overlay overlay, CRGB *LEDarray = leds) // with blend gradient color
{
	if (i1 == i2)
	{
		ledrawOverlayMethods[overlay](LEDarray[i1], color1); // may overlay single dots on overlayAdditive
		return;
	}
	else if (i1 < i2)
	{
		uint8_t range = i2 - i1;
		for (T i = (i1 + 1); i <= i2; i++)
			ledrawOverlayMethods[overlay](LEDarray[i], blend_fast(color1, color2, (uint16_t)255U * (i - i1) / range));
	}
	else
	{
		uint8_t range = i1 - i2;
		for (T i = i2; i < i1; i++)
			ledrawOverlayMethods[overlay](LEDarray[i], blend_fast(color2, color1, (uint16_t)255U * (i - i2) / range));
	}
};

typedef uint8_t ledraw_overlay_t;

template <typename T, typename T1>
static void fillSegment(T i1, T1 i2, CRGB color1, CRGB color2, ledraw_overlay_t overlay, CRGB *LEDarray = leds) // variant of overlayed function drawLine
{																																																							 // if you draw a continues line of polyline (or chain) \
																																																							no need to overdraw a second point of the new clip(aka the first of previous)
	if (i1 == i2)
	{
		if (overlay & overlaySoftSingles)
			ledrawOverlayMethods[overlay & 0x3E](LEDarray[i1], color1); // may overlay single dots on overlayAdditive
		else if (!(overlay & overlayExceptSingles))
			ledrawOverlayMethods[overlay & 0x3F](LEDarray[i1], color1); // may overlay single dots on overlayAdditive
		return;
	}
	else if (i1 < i2)
	{
		overlay &= _overlay_mask;
		uint8_t range = i2 - i1;
		for (T i = i1; i < i2; i++)
			ledrawOverlayMethods[overlay](LEDarray[i], blend_fast(color1, color2, (uint16_t)255U * (i - i1) / range));
	}
	else
	{
		overlay &= _overlay_mask;
		uint8_t range = i1 - i2;
		for (T i = (i2 + 1); i <= i1; i++)
			ledrawOverlayMethods[overlay](LEDarray[i], blend_fast(color2, color1, (uint16_t)255U * (i - i2) / range));
	}
};

// ----- LEDraw figures / фигуры LEDraw -----
template <typename T>
static void drawSparkler(T center, T amplitude, T quantity, CRGB color, bool reverse, ledraw_overlay_t overlay = overlaySolid, CRGB *LEDarray = leds)
{
	for (T i = 0; i < quantity; i++)
	{
		T pos = center + random8(amplitude) - random8(amplitude);
		if (inRange(pos))
			ledrawOverlayMethods[overlay](LEDarray[mirror(pos, reverse)], color % random8());
	}
};

template <typename T, typename T1>
static void drawComet(T head, T1 length, CRGB color, bool reverse, ledraw_overlay_t overlay = overlaySolid, CRGB *LEDarray = leds)
{
	if (inRange(head))
		ledrawOverlayMethods[overlay](LEDarray[mirror(head, reverse)], color);
	T range = 255 / length;

	for (T i = 1; i < length; i++)
	{
		T pos = head - i;
		if (inRange(pos))
			ledrawOverlayMethods[overlay](LEDarray[mirror(pos, reverse)], color % qsub8(~(i * range), random8(127)));
	}
};

// -----  fill SCREEN by S, V NOISE amplitudes / Заполнить экран амплитудным шумом по каналам S, V
static void noiseAll(uint8_t satAmpl, uint8_t valAmpl, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i] = CHSV(random8(255), random8(satAmpl), random8(valAmpl));
};
// -----  FILL SCREEN by color / Заполнить экран цветом
static void fillAll(CRGB color, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i] = color;
};

inline void fillAll(CRGB color, CRGBSet LEDset) { LEDset = color; }

inline void dimAll(uint8_t value, CRGBSet LEDset) { LEDset.fadeLightBy(value); };
static void dimAll(uint8_t value, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i].fadeLightBy(value);
};

inline void fadeAll(uint8_t value, CRGBSet LEDset) { LEDset.fadeToBlackBy(value); };
static void fadeAll(uint8_t value, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i].fadeToBlackBy(value);
};

static void clearAll(CRGB *LEDarray = leds)
{
	fillAll(CRGB::Black, LEDarray);
};

inline void clearAll(CRGBSet LEDset) { LEDset = 0; };

#endif
