#ifndef _LEDraw_Math_h
#define _LEDraw_Math_h
//#pragma ones
#include <stdint.h>
#include "effects/LEDraw/LEDraw_Palettes.h"
//#include <typeinfo>

#define LEDRAW_BLENDTYPE LINEARBLEND // NOBLEND / LINEARBLEND

// LEDraw! Fast Drawing Library for FastLED  by PalPalych v0.1b
// + family functions subPixel -- 23.05.2020

CRGBPalette16 setPaletteByType(paletteSet_t in, uint8_t offset)
{
	if (in.size & 0x80)
		return CRGBPalette16(gradientPalettes[in.index + offset]);
	return CRGBPalette16(*palettes_fastled[in.index + offset]);
};

// *** Rescale Index by Fader Parameters / Расчет координаты относительно положения Фэйдера
int8_t faderPosition(int8_t value, bool direction, int8_t scale)
{
	return scale8(scale, direction ? value >> 1U : ~value >> 1U);
};
int8_t faderPosition(int8_t value, bool direction, int8_t scale, uint8_t overscale)
{
	return scale8(scale + overscale * 2, direction ? value >> 1U : ~value >> 1U) - overscale;
};
int16_t faderPosition(int16_t value, bool direction, int16_t scale)
{
	return scale16by8(scale, direction ? value >> 1U : ~value >> 1U);
};
int16_t faderPosition(int16_t value, bool direction, int16_t scale, uint16_t overscale)
{
	return scale16by8(scale + overscale * 2, direction ? value >> 1U : ~value >> 1U) - overscale;
};

/*typedef union
{
	TProgmemRGBPalette16Ptr rgb16_palette;
	TProgmemRGBGradientPalettePtr g_palette;
} TProgmemPalettePtr;

typedef struct
{
	uint8_t _status;
#if NUM_LEDS < 128 //  7/8/15/16-bit index of leds array for quick calculation
	int8_t i;
#else
#if NUM_LEDS < 256
	uint8_t i;
#else
	uint16_t i;
#endif
#endif

	uint8_t i8u;									//  8-bit unsigned index
	uint16_t i16u;								// 16-bit unsigned index обычно это координаты светодиода на ленте
	uint8_t n1u;									//  8-bit unsigned number обычно это описание количества
	uint8_t d1u;									//  8-bit unsigned delta обычно это (положительное) отклонение
	uint8_t s1u;									//  8-bit unsigned sigma обычно это (отрицательное) отклонение
	uint8_t tAu, tBu;							//  8-bit unsigned theta обычно применяется как операнд тригонометрических функций
	int8_t tAi, tBi;							//  8-bit unsigned theta обычно применяется как операнд тригонометрических функций
	uint8_t w8Au, w8Bu, w8Cu;			//  8-bit unsigned wave обычно применяется как значение тригонометрических функций
	uint16_t w16Au, w16Bu, w16Cu; // 16-bit unsigned wave обычно применяется как значение тригонометрических функций
#if (LEDSARRAY_TYPE == MATRIX)
	int8_t x, x1;							 // переменные непосредственных координат (обычно используется в циклах)
	int8_t y, y1;							 //
	int8_t z, z1;							 //
	uint8_t i1u, i2u, i3u;		 // утилитарные переменные (относительные координаты)
	uint8_t j1u, j2u, j3u;		 //
	int8_t x16, y16;					 // переменные непосредственных 16-разрядных координат (для работы с массивом светодиодов)
	uint16_t i16u, j16u, k16u; // утилитарные 16-разрядные переменные (относительные координаты)
#endif

	CRGB colorA, colorB; // определения цвета
	uint8_t currentPaletteSet = 0;
	CRGBPalette16 palette = setPaletteByType(paletteSet[0], 0);
	//TProgmemPalettePtr u_palette;

	union
	{
		TProgmemRGBPalette16Ptr rgb16_palette;
		TProgmemRGBGradientPalettePtr g_palette;
	};
	//const PALETTE &pal = gradientPalettes[1];

	inline void setPalette(TProgmemRGBPalette16Ptr pal)
	{
		rgb16_palette = pal;
		_status &= ~0x01;
	};

	inline void setPalette(TProgmemRGBGradientPalettePtr pal)
	{
		g_palette = pal;
		_status |= 0x01;
	};

	CRGB ColorFromPalette(uint8_t index, uint8_t brightness, TBlendType blendType)
	{
		if (_status & 0x01)
			return ::ColorFromPalette(*g_palette, index, brightness, blendType);
		return ::ColorFromPalette(*rgb16_palette, index, brightness, blendType);
	};
	inline void setPalette(uint8_t mode) { palette = setPaletteByType(paletteSet[currentPaletteSet], mode); };
	inline void setPalette(uint8_t mode, uint8_t of_paletteSet) { palette = setPaletteByType(paletteSet[of_paletteSet], mode); };
	inline CRGB getPaletteColor(uint8_t index, uint8_t brightness = 255U) { return ::ColorFromPalette(palette, index, brightness, LEDRAW_BLENDTYPE); };

} ledraw_vars; */

inline void setBit(uint8_t &data, const uint8_t position)
{
	data |= ((uint8_t)1U << position);
}

inline void setBit(uint8_t &data, const uint8_t position, bool value)
{
	uint8_t mask = ((uint8_t)1U << position);
	value ? data |= mask : data &= mask;
}

inline void clearBit(uint8_t &data, const uint8_t position)
{
	data &= ~((uint8_t)1U << position);
}

inline bool checkBit(uint8_t &data, const uint8_t position)
{
	return data & ((uint8_t)1U << position);
}

inline bool checkBit(int8_t &data, const uint8_t position)
{
	return data & ((uint8_t)1U << position);
}

inline bool changeBit(uint8_t &data, const uint8_t position)
{
	uint8_t mask = ((uint8_t)1U << position);
	data ^= mask;
	return data & mask;
}

typedef uint8_t rint8_t; // reverse int value where 7 bit is the sign, 6...0 bits is the absolute value

inline int8_t rint8(uint8_t data)
{
	return data & 0x80 ? data & 0x7F : ~data;
}

#define qsubd(x, b) ((x > b) ? b : 0)			// Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b) ((x > b) ? x - b : 0) // Analog Unsigned subtraction macro. if result <0, then => 0

// функции масштабирования амплитуды тригонометрических функций
static uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255)
{
	return lowest + scale8(sin8(theta), highest - lowest);
}
static uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255)
{
	return lowest + scale8(cos8(theta), highest - lowest);
}
/* static uint8_t range8(uint8_t value, uint8_t scale = 255)
{
  return scale8(value, scale) + ((255 - scale) >> 1U);
}*/

static uint8_t rangesin8(uint8_t theta, uint8_t scale = 255)
{
	return scale8(sin8(theta), scale) + ((255 - scale) >> 1U);
}

static uint8_t rangecos8(uint8_t theta, uint8_t scale = 255)
{
	return scale8(cos8(theta), scale) + ((255 - scale) >> 1U);
}
/* static int16_t rangesin7(uint8_t theta, uint8_t scale = 255)
{
  return (int16_t)scale8(sin8(theta), scale) - 127 + ((~scale) >> 1U);
}
static int16_t rangecos7(uint8_t theta, uint8_t scale = 255)
{
  return (int16_t)scale8(cos8(theta), scale) - 127 + ((~scale) >> 1U);
}*/
#endif
