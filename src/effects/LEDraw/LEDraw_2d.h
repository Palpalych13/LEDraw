#ifndef _LEDraw_2d_h
#define _LEDraw_2d_h
//#pragma ones
#include <stdint.h>
#include <FastLED.h>
#if (LEDSARRAY_TYPE == MATRIX)

// LEDraw! Fast Drawing Library for FastLED by PalPalych v0.1b
// Набор быстрых графических функций для среды FastLED для матриц размером не более 128x128 светодиодов
// Концепция заключается в использовании знаковых величин int8_t для адрессации координат X и Y для быстрой проверки на условие выхода за пределы экрана
// функции с суффиксом _fast требуют только инкрементное указание координа и не соделжат проверок выхода за края матрицы
// функции без суффикса _fast содержат проверки, и выводят на экран только видимую часть объекта

// + family functions subPixel -- 23.05.2020
// + functions clearAll -- 24.05.2020
// + functions mirrorX, mirrorY -- 27.05.2020
// + family functions gradientVRect2 -- 02.06.2020

// константы для быстрого пересчета графических функций
static const int8_t WIDTH_d = WIDTH - 1U;								// ширина в цифровом представлении
static const int8_t HEIGHT_d = HEIGHT - 1U;							// высота в цифровом представлении
static const int8_t WIDTH_odd = WIDTH % 2U;							// признак нечетной ширины
static const int8_t HEIGHT_odd = HEIGHT % 2U;						// признак нечетной высоты
static const int8_t SEMIWIDTH = WIDTH / 2U;							// половина ширины (минорное значение)
static const int8_t SEMIWIDTH_d = SEMIWIDTH - 1U;				// половина ширины (минорное значение в цифровом представлении)
static const int8_t SEMIHEIGHT = WIDTH / 2U;						// половина высоты (минорное значение)
static const int8_t SEMIHEIGHT_d = SEMIHEIGHT - 1U;			// половина ширины (минорное значение в цифровом представлении)
static const int8_t CENTERX = SEMIWIDTH + WIDTH_odd;		// середина по ширине
static const int8_t CENTERY = SEMIHEIGHT + HEIGHT_odd;	// середина по высоте
static const int8_t CENTERX_d = CENTERX - 1U;						// середина по ширине в цифровом представлении
static const int8_t CENTERY_d = CENTERY - 1U;						// середина по высоте в цифровом представлении
static const int8_t CENTER_max = max(CENTERX, CENTERY); // Наибольшее значение центра
static const int8_t DIMENSION_min = min(WIDTH, HEIGHT); // Наименьшее значение размера матрицы
static const int8_t DIMENSION_max = max(WIDTH, HEIGHT); // Наибольшее значение размера матрицы
//static const int8_t CENTERX_maj = CENTERX + 1U;        // середина по ширине (мажорное значение)
static const int8_t maxDim = max(WIDTH, HEIGHT);
static const int8_t WIDTH_steps = 256U / WIDTH;	 // диапазон значений приходящихся на 1 пиксель ширины матрицы
static const int8_t HEIGHT_steps = 256U / HEIGHT; // диапазон значений приходящихся на 1 пиксель высоты матрицы


// ----- MATRIX SECTION -----
// MATRIX_CONNECTION (угол подключения): 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
// MATRIX_DIRECTION (направление ленты из угла): 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
#if (MATRIX_CONNECTION == 0 && MATRIX_DIRECTION == 0) // функции координат для разных матриц подключения
#define _SEGLENGTH WIDTH

#if MATRIX_VMIRROR
#define THIS_X (WIDTH_d - x)
#define _THIS_X (x)
#else
#define THIS_X (x)
#define _THIS_X (WIDTH_d - x)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (HEIGHT_d - y)
#define _THIS_Y (y)
#else
#define THIS_Y (y)
#define _THIS_Y (HEIGHT_d - y)
#endif

#elif (MATRIX_CONNECTION == 0 && MATRIX_DIRECTION == 1)
#define _SEGLENGTH HEIGHT

#if MATRIX_VMIRROR
#define THIS_X (HEIGHT_d - y)
#define _THIS_X (y)
#else
#define THIS_X (y)
#define _THIS_X (HEIGHT_d - y)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (WIDTH_d - x)
#define _THIS_Y (x)
#else
#define THIS_Y (x)
#define _THIS_Y (WIDTH_d - x)
#endif

#elif (MATRIX_CONNECTION == 1 && MATRIX_DIRECTION == 0)
#define _SEGLENGTH WIDTH

#if MATRIX_VMIRROR
#define THIS_X (WIDTH_d - x)
#define _THIS_X (x)
#else
#define THIS_X (x)
#define _THIS_X (WIDTH_d - x)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (y)
#define _THIS_Y (HEIGHT_d - y)
#else
#define THIS_Y (HEIGHT_d - y)
#define _THIS_Y (y)
#endif

#elif (MATRIX_CONNECTION == 1 && MATRIX_DIRECTION == 3)
#define _SEGLENGTH HEIGHT

#if MATRIX_VMIRROR
#define THIS_X (y)
#define _THIS_X (HEIGHT_d - y)
#else
#define THIS_X (HEIGHT_d - y)
#define _THIS_X (y)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (WIDTH_d - x)
#define _THIS_Y (x)
#else
#define THIS_Y (x)
#define _THIS_Y (WIDTH_d - x)
#endif

#elif (MATRIX_CONNECTION == 2 && MATRIX_DIRECTION == 2)
#define _SEGLENGTH WIDTH

#if MATRIX_VMIRROR
#define THIS_X (x)
#define _THIS_X (WIDTH_d - x)
#else
#define THIS_X (WIDTH_d - x)
#define _THIS_X (x)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (y)
#define _THIS_Y (HEIGHT_d - y)
#else
#define THIS_Y (HEIGHT_d - y)
#define _THIS_Y (y)
#endif

#elif (MATRIX_CONNECTION == 2 && MATRIX_DIRECTION == 3)
#define _SEGLENGTH HEIGHT

#if MATRIX_VMIRROR
#define THIS_X (y)
#define _THIS_X (HEIGHT_d - y)
#else
#define THIS_X (HEIGHT_d - y)
#define _THIS_X (y)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (x)
#define _THIS_Y (WIDTH_d - x)
#else
#define THIS_Y (WIDTH_d - x)
#define _THIS_Y (x)
#endif

#elif (MATRIX_CONNECTION == 3 && MATRIX_DIRECTION == 2)
#define _SEGLENGTH WIDTH

#if MATRIX_VMIRROR
#define THIS_X (x)
#define _THIS_X (WIDTH_d - x)
#else
#define THIS_X (WIDTH_d - x)
#define _THIS_X (x)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (HEIGHT_d - y)
#define _THIS_Y (y)
#else
#define THIS_Y (y)
#define _THIS_Y (HEIGHT_d - y)
#endif

#elif (MATRIX_CONNECTION == 3 && MATRIX_DIRECTION == 1)
#define _SEGLENGTH HEIGHT

#if MATRIX_VMIRROR
#define THIS_X (HEIGHT_d - y)
#define _THIS_X (y)
#else
#define THIS_X (y)
#define _THIS_X (HEIGHT_d - y)
#endif

#if MATRIX_HMIRROR
#define THIS_Y (x)
#define _THIS_Y (WIDTH_d - x)
#else
#define THIS_Y (WIDTH_d - x)
#define _THIS_Y (x)
#endif

#else
#define _SEGLENGTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters!"
#define _ERROR (0 / 0)
#endif

// функции "оборочивания" координаты относительно границ
static uint8_t wrapX(int8_t x)
{
	return mod8(x + WIDTH, WIDTH);
}

static uint8_t wrapY(int8_t y)
{
	return mod8(y + HEIGHT, HEIGHT);
}

// функции отзеркаливания координаты относительно границ
inline uint8_t mirrorX(int8_t x)
{
	return WIDTH_d - x;
}
inline uint8_t mirrorY(int8_t y)
{
	return HEIGHT_d - y;
}

// получить номер пикселя в ленте по координатам
static uint16_t getPixelNumber(int8_t x, int8_t y)
{
	if (!(THIS_Y & 0x01) || MATRIX_TYPE) // если чётная строка
		return (uint16_t)THIS_Y * _SEGLENGTH + THIS_X;
	else // если нечётная строка
		return (uint16_t)THIS_Y * _SEGLENGTH + _THIS_X;
}

// функция получения цвета пикселя по его номеру
/* static uint32_t getPixelValue(int16_t thisPixel, CRGB *LEDarray = leds)
{
	CRGB color = LEDarray[thisPixel];
	return (((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) | (uint32_t)color.b);
}*/

static uint32_t getPixelValue(int8_t x, int8_t y, CRGB *LEDarray = leds)
{
	CRGB color = LEDarray[getPixelNumber(x, y)];
	return (((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) | (uint32_t)color.b);
}

// функция получения цвета пикселя в матрице по его координатам
/*CRGB getPixelColor(int8_t x, int8_t y) {
  return leds[getPixelNumber(x, y)];
}*/
static CRGB getPixelColor(int8_t x, int8_t y, CRGB *LEDarray = leds)
{
	return LEDarray[getPixelNumber(x, y)];
}

/*static CRGB getPixelColor(int16_t thisPixel, CRGB *LEDarray = leds)
{
	return LEDarray[thisPixel];
}*/

// функция отрисовки точки по координатам X Y
static void drawPixel_fast(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // быстрая функция для просчета матриц
{
	LEDarray[getPixelNumber(x, y)] = color;
}

/* static void drawPixel(int16_t thisPixel, CRGB color, CRGB *LEDarray = leds) // быстрая функция для просчета матриц
{
	LEDarray[thisPixel] = color;
}*/

static void drawPixel(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // медленная функция с проверкой координат
{
	if (x < 0 || x > (int8_t)(WIDTH_d) || y < 0 || y > (int8_t)(HEIGHT_d))
		return;
	drawPixel_fast(x, y, color, LEDarray);
}

static void addPixel_fast(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // быстрая аддитивная функция для просчета матриц
{
	LEDarray[getPixelNumber(x, y)] += color;
}

static void addPixel(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // медленная аддитивная функция с проверкой координат
{
	if (x < 0 || x > (int8_t)(WIDTH_d) || y < 0 || y > (int8_t)(HEIGHT_d))
		return;
	addPixel_fast(x, y, color, LEDarray);
}

static void subPixel_fast(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // быстрая субтрактивная функция для просчета матриц
{
	LEDarray[getPixelNumber(x, y)] -= color;
}

static void subPixel(int8_t x, int8_t y, CRGB color, CRGB *LEDarray = leds) // медленная субтрактивная функция с проверкой координат
{
	if (x < 0 || x > (int8_t)(WIDTH_d) || y < 0 || y > (int8_t)(HEIGHT_d))
		return;
	subPixel_fast(x, y, color, LEDarray);
}

static void fadePixel_fast(int8_t x, int8_t y, uint8_t value, CRGB *LEDarray = leds) // быстрая функция угасания точки для просчета матриц
{
	LEDarray[getPixelNumber(x, y)].fadeToBlackBy(value);
}

static void fadePixel(int8_t x, int8_t y, uint8_t value, CRGB *LEDarray = leds) // медленная функция угасания точки с проверкой координат
{
	if (x < 0 || x > (int8_t)(WIDTH_d) || y < 0 || y > (int8_t)(HEIGHT_d))
		return;
	fadePixel_fast(x, y, value, LEDarray);
}

// ----- DRAW LINE by coordinates & range / Рисовать отрезок по координатам и длине
static void drawHLine2_fast(int8_t x1, int8_t x2, int8_t y, CRGB color, CRGB *LEDarray = leds) // Fine if: 0<x1<x2<WIDTH && 0<y<HEIGHT
{
	for (int8_t xP = x1; xP <= x2; xP++)
	{
		LEDarray[getPixelNumber(xP, y)] = color;
	}
}
static void drawHLine2(int8_t x1, int8_t x2, int8_t y, CRGB color, CRGB *LEDarray = leds)
{
	if (((x1 >= 0 && x1 < WIDTH) || (x2 >= 0 && x2 < WIDTH)) && (y >= 0 && y < HEIGHT))
	{
		x1 = constrain(x1, 0, WIDTH_d);
		x2 = constrain(x2, 0, WIDTH_d);
		drawHLine2_fast(x1, x2, y, color, LEDarray);
	}
}

static void drawVLine2_fast(int8_t x, int8_t y1, int8_t y2, CRGB color, CRGB *LEDarray = leds) // Fine if: 0<x1<x2<WIDTH && 0<y<HEIGHT
{
	for (int8_t yP = y1; yP <= y2; yP++)
	{
		LEDarray[getPixelNumber(x, yP)] = color;
	}
}

// ----- DRAW FILLed RECTangle by coordinates & dimensions / Рисовать заполненный прямоугольник по координатам и размерам
static void drawFillRect(int8_t x, int8_t y, int8_t width, int8_t height, CRGB color, uint8_t alpha = 255, CRGB *LEDarray = leds) // Fine if: 0<x<(x+width)<WIDTH && 0<y<(y+height)<HEIGHT
{
	int8_t xS = max(x, int8_0);
	int8_t xE = min(qadd7(x, width), (int8_t)WIDTH);
	int8_t yS = max(y, int8_0);
	int8_t yE = min(qadd7(y, height), (int8_t)HEIGHT);
	uint8_t nalpha = ~alpha;
	color = color.nscale8(alpha);
	for (int8_t xP = xS; xP < xE; xP++)
		for (int8_t yP = yS; yP < yE; yP++)
		{
			uint16_t i = getPixelNumber(xP, yP);
			LEDarray[i] = color + LEDarray[i].nscale8(nalpha);
		}
}

// ----- DRAW FILLed RECTangle by 2 points / Рисовать заполненный прямоугольник по 2 точкам
/*static void drawFillRect2(int8_t x1, int8_t x2, int8_t y1, int8_t y2, CRGB color, CRGB *LEDarray = leds)
{ // under construction
  //if (x2 < x1) swap(x1, x2); if (y2 < y1) swap(y1, y2);
  //drawFillRect2_fast (max(x1, int8_0), max(y1, int8_0), min(x2, cWIDTH), min(y2, cHEIGHT), color, LEDarray);
}*/

static void drawFillRect2_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2, CRGB color, CRGB *LEDarray = leds) // Fine if: 0<x1<x2<WIDTH && 0<y1<y2<HEIGHT
{
	for (int8_t xP = x1; xP <= x2; xP++)
		for (int8_t yP = y1; yP <= y2; yP++)
			LEDarray[getPixelNumber(xP, yP)] = color;
}

/*void drawFillRect2(int8_t x1, int8_t x2, int8_t y1, int8_t y2, CRGB color, uint8_t alpha, CRGB *LEDarray = leds)
{
  if (x2 < x1)
    swap(x1, x2);
  if (y2 < y1)
    swap(y1, y2);
  drawFillRect2_fast(max(x1, int8_0), max(y1, int8_0), min(x2, cWIDTH), min(y2, cHEIGHT), color, alpha, LEDarray);
}*/
static void drawFillRect2_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2,
															 CRGB color, uint8_t alpha, CRGB *LEDarray = leds) // Fine if: 0<x1<x2<WIDTH && 0<y1<y2<HEIGHT
{
	uint8_t nalpha = ~alpha;
	color = color.nscale8(alpha);
	for (int8_t xP = x1; xP <= x2; xP++)
		for (int8_t yP = y1; yP <= y2; yP++)
		{
			uint16_t i = getPixelNumber(xP, yP);
			LEDarray[i] = color + LEDarray[i].nscale8(nalpha);
		}
}

// ----- DRAW FILLed ELLIPSE by 2 points / Рисовать заполненный эллипс по 2 точкам
static void drawOval2(int8_t x1, int8_t x2, int8_t y1, int8_t y2, CRGB color, CRGB *LEDarray = leds)
{
	uint8_t w = x2 - x1 + 1U;
	uint8_t h = y2 - y1 + 1U;
	{
		uint8_t xD = constrain(256U / w, 0, 255);
		uint8_t yD = constrain(256U / h, 0, 255);
		for (uint8_t yP = 0U; yP <= (h >> 1U); yP++)
		{
			uint8_t yZ = 128U - (qmul8(yP, xD) + (xD >> 1U));
			for (uint8_t xP = 0U; xP <= (w >> 1U); xP++)
			{
				uint8_t xZ = 128U - (qmul8(xP, yD) + (yD >> 1U));
				if (sqrt16(((uint16_t)yZ * yZ) + ((uint16_t)xZ * xZ)) <= 128U)
				{
					drawHLine2(x1 + xP, x2 - xP, y1 + yP, color);
					drawHLine2(x1 + xP, x2 - xP, y2 - yP, color);
				}
			}
		}
		if (h & 1)
			drawHLine2(x1, x2, y1 + (h >> 1), color);
	}
}

static void drawOval2_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2, CRGB color, CRGB *LEDarray = leds) // Fine if: 0<x1<x2<WIDTH && 0<y1<y2<HEIGHT
{
	uint8_t w = x2 - x1 + 1U;
	uint8_t h = y2 - y1 + 1U;
	{
		uint8_t xD = constrain(256U / w, 0, 255);
		uint8_t yD = constrain(256U / h, 0, 255);
		for (uint8_t yP = 0U; yP <= (h >> 1U); yP++)
		{
			uint8_t yZ = 128U - (qmul8(yP, xD) + (xD >> 1U));

			for (uint8_t xP = 0U; xP <= (w >> 1U); xP++)
			{
				uint8_t xZ = 128U - (qmul8(xP, yD) + (yD >> 1U));
				if (sqrt16(((uint16_t)yZ * yZ) + ((uint16_t)xZ * xZ)) <= 128U)
				{
					drawHLine2_fast(x1 + xP, x2 - xP, y1 + yP, color);
					drawHLine2_fast(x1 + xP, x2 - xP, y2 - yP, color);
				}
			}
		}
		if (h & 1)
			drawHLine2_fast(x1, x2, y1 + (h >> 1), color);
	}
}

// ----- Vertical GRADIENT of RECTangle by 2 points and H, S, V ranges / Вертикальный прямоугольный градиент по 2 точкам с диапазонами по H, S, V
/*static void gradientVRect2HSV_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2,
                                   uint8_t hS, uint8_t hE, uint8_t sS, uint8_t sE, uint8_t vS, uint8_t vE, CRGB *LEDarray = leds)
{
  uint8_t yStep = y2 - y1;
  int16_t hStep = ((int16_t)hE - hS) / yStep;
  int16_t sStep = ((int16_t)sE - sS) / yStep;
  int16_t vStep = ((int16_t)vE - vS) / yStep;

  CHSV color = CHSV(hE, sS, vS);
  for (int8_t yP = y1; yP <= y2; yP++)
  {
    for (int8_t xP = x1; xP <= x2; xP++)
    {
      LEDarray[getPixelNumber(xP, yP)] = color;
    }
    color.h += hStep;
    color.s += sStep;
    color.v += vStep;
  }
}*/

static void gradientVRect2_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2,
																CRGB startColor, CRGB endColor, CRGB *LEDarray = leds)
{
	uint8_t yStep = y2 - y1;
	for (int8_t yP = y1; yP <= y2; yP++)
	{
		CRGB color = blend(startColor, endColor, 256 / yStep * yP);
		for (int8_t xP = x1; xP <= x2; xP++)
			LEDarray[getPixelNumber(xP, yP)] = color;
	}
}

static void gradientVRect2_fast(int8_t x1, int8_t x2, int8_t y1, int8_t y2,
																CHSV startColor, CHSV endColor, CRGB *LEDarray = leds)
{
	uint8_t yStep = y2 - y1;
	for (int8_t yP = y1; yP <= y2; yP++)
	{
		CHSV color = blend(startColor, endColor, 256 / yStep * (yP - y1), FORWARD_HUES);
		for (int8_t xP = x1; xP <= x2; xP++)
			LEDarray[getPixelNumber(xP, yP)] = color;
	}
}

// -----  fill SCREEN by S, V NOISE amplitudes / Заполнить экран амплитудным шумом по каналам S, V
/* static void noiseAll(uint8_t satAmpl, uint8_t valAmpl, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
	{
		LEDarray[i] = CHSV(random8(255), random8(satAmpl), random8(valAmpl));
	}
}*/

static void moveScreenDown_fast(int8_t steps = 1, CRGB *LEDarray = leds)
{
	int8_t height = HEIGHT - steps;
	for (int8_t y = 0U; y < height; y++)
	{
		int8_t ys = y + steps;
		for (int8_t x = 0U; x < WIDTH; x++)
			drawPixel_fast(x, y, getPixelColor(x, ys), LEDarray);
	}
}

/* // -----  FILL SCREEN by color / Заполнить экран цветом
static void fillAll(CRGB color, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i] = color;
}

inline void fillAll(CRGB color, CRGBSet LEDset) { LEDset = color; }

static void dimAll(uint8_t value, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i].nscale8(value);
}

inline void dimAll(uint8_t value, CRGBSet LEDset) { LEDset.nscale8(value); };

static void fadeAll(uint8_t value, CRGB *LEDarray = leds)
{
	for (uint16_t i = 0; i < NUM_LEDS; i++)
		LEDarray[i].fadeToBlackBy(value);
}

inline void fadeAll(uint8_t value, CRGBSet LEDset) { LEDset.fadeToBlackBy(value); };

static void clearAll(CRGB *LEDarray = leds)
{
	fillAll(CRGB::Black, LEDarray);
}

inline void clearAll(CRGBSet LEDset) { LEDset = 0; } 
*/



/*
  void MoveX(int8_t delta) {
  for (int8_t y = 0; y < HEIGHT; y++) {
    for (int8_t x = 0; x < WIDTH - delta; x++) {
      ledsbuff[XY(x, y)] = ledstripe[XY(x + delta, y)];
    }
    for (int8_t x = WIDTH - delta; x < WIDTH; x++) {
      ledsbuff[XY(x, y)] = ledstripe[XY(x + delta - WIDTH, y)];
    }
  }
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      ledstripe[XY(x, y)] = ledsbuff[XY(x, y)];
    }
  }
  }

  void MoveY(int8_t delta) {
  for (int8_t x = 0; x < WIDTH; x++) {
    for (int8_t y = 0; y < HEIGHT - delta; y++) {
      ledsbuff[XY(x, y)] = ledstripe[XY(x, y + delta)];
    }
    for (int8_t y = HEIGHT - delta; y < HEIGHT; y++) {
      ledsbuff[XY(x, y)] = ledstripe[XY(x, y + delta - HEIGHT)];
    }
  }
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      ledstripe[XY(x, y)] = ledsbuff[XY(x, y)];
    }
  }
  }*/

// *** Функция быстрого расчета координат точки на матрице.
// Нужна для совместимости с некотороми внутренними функциями FastLED
uint16_t XY(uint8_t x, uint8_t y)
{
#if (MATRIX_DIRECTION & 0x01)
	if (!(THIS_X & 0x01) || MATRIX_TYPE) // если чётная строка
		return (uint16_t)THIS_X * _SEGLENGTH + THIS_Y;
	else // если нечётная строка
		return (uint16_t)THIS_X * _SEGLENGTH + _THIS_Y;
#else
	if (!(THIS_Y & 0x01) || MATRIX_TYPE) // если чётная строка
		return (uint16_t)THIS_Y * _SEGLENGTH + THIS_X;
	else // если нечётная строка
		return (uint16_t)THIS_Y * _SEGLENGTH + _THIS_X;
#endif
}

// ----- 2D/3D NOISE FUNCTIONS
// The coordinates for 3 16-bit noise spaces.
#define NUM_LAYERS 2

// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

static uint32_t noise32_x[NUM_LAYERS];
static uint32_t noise32_y[NUM_LAYERS];
static uint32_t noise32_z[NUM_LAYERS];
static uint32_t scale32_x[NUM_LAYERS];
static uint32_t scale32_y[NUM_LAYERS];

// This is the array that we keep our computed noise values in
#define MAX_DIMENSION (max(WIDTH, HEIGHT))
#if (WIDTH > HEIGHT)
uint8_t noise[WIDTH][WIDTH];
#else
uint8_t noise[HEIGHT][HEIGHT];
#endif

uint8_t noise3d[NUM_LAYERS][WIDTH][HEIGHT];

uint8_t noisesmooth;
bool eNs_isSetupped;

void noise_setup()
{
	noisesmooth = 200;
	for (int i = 0; i < NUM_LAYERS; i++)
	{
		noise32_x[i] = random16();
		noise32_y[i] = random16();
		noise32_z[i] = random16();
		scale32_x[i] = 6000;
		scale32_y[i] = 6000;
	}
	eNs_isSetupped = true;
}

void fillNoise2d(int8_t layer)
{
	for (uint8_t i = 0; i < WIDTH; i++)
	{
		uint32_t ioffset = scale32_x[layer] * (i - CENTERX_d);
		for (uint8_t j = 0; j < HEIGHT; j++)
		{
			uint32_t joffset = scale32_y[layer] * (j - CENTERY_d);
			byte data = inoise16(noise32_x[layer] + ioffset, noise32_y[layer] + joffset, noise32_z[layer]) >> 8;
			uint8_t olddata = noise[i][j];
			uint8_t newdata = scale8(olddata, noisesmooth) + scale8(data, 256 - noisesmooth);
			data = newdata;
			noise[i][j] = data;
		}
	}
}

void fillNoise3d(int8_t layer)
{
	for (int8_t i = 0; i < WIDTH; i++)
	{
		int32_t ioffset = scale32_x[layer] * (i - CENTERX_d);
		for (int8_t j = 0; j < HEIGHT; j++)
		{
			int32_t joffset = scale32_y[layer] * (j - CENTERY_d);
			int8_t data = inoise16(noise32_x[layer] + ioffset, noise32_y[layer] + joffset, noise32_z[layer]) >> 8;
			int8_t olddata = noise3d[layer][i][j];
			int8_t newdata = scale8(olddata, noisesmooth) + scale8(data, 255 - noisesmooth);
			data = newdata;
			noise3d[layer][i][j] = data;
		}
	}
}

static int8_t zD;
static int8_t zF;

void MoveFractionalNoiseX(int8_t amplitude = 1, float shift = 0)
{
	for (int8_t y = 0; y < HEIGHT; y++)
	{
		int16_t amount = ((int16_t)noise3d[0][0][y] - 128) * 2 * amplitude + shift * 256;
		int8_t delta = abs(amount) >> 8;
		int8_t fraction = abs(amount) & 255;
		for (int8_t x = 0; x < WIDTH; x++)
		{
			if (amount < 0)
			{
				zD = x - delta;
				zF = zD - 1;
			}
			else
			{
				zD = x + delta;
				zF = zD + 1;
			}
			CRGB PixelA = CRGB::Black;
			if ((zD >= 0) && (zD < WIDTH))
				PixelA = getPixelColor(zD, y, leds);
			CRGB PixelB = CRGB::Black;
			if ((zF >= 0) && (zF < WIDTH))
				PixelB = getPixelColor(zF, y, leds);
			drawPixel_fast(x, y, (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction))), ledsbuff); // lerp8by8(PixelA, PixelB, fraction );
		}
	}
	memcpy(leds, ledsbuff, BUFFER_SIZE);
}

void MoveFractionalNoiseY(int8_t amplitude = 1, float shift = 0)
{
	for (int8_t x = 0; x < WIDTH; x++)
	{
		int16_t amount = ((int16_t)noise3d[0][x][0] - 128) * 2 * amplitude + shift * 256;
		int8_t delta = abs(amount) >> 8;
		int8_t fraction = abs(amount) & 255;
		for (int8_t y = 0; y < HEIGHT; y++)
		{
			if (amount < 0)
			{
				zD = y - delta;
				zF = zD - 1;
			}
			else
			{
				zD = y + delta;
				zF = zD + 1;
			}
			CRGB PixelA = CRGB::Black;
			if ((zD >= 0) && (zD < HEIGHT))
				PixelA = getPixelColor(x, zD, leds);
			CRGB PixelB = CRGB::Black;
			if ((zF >= 0) && (zF < HEIGHT))
				PixelB = getPixelColor(x, zF, leds);
			drawPixel_fast(x, y, (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction))), ledsbuff);
		}
	}
	memcpy(leds, ledsbuff, BUFFER_SIZE);
}

void moveScreenY(float shift = 0)
{
	for (int8_t x = 0; x < WIDTH; x++)
	{
		int16_t amount = shift * 256;
		int8_t delta = abs(amount) >> 8;
		int8_t fraction = abs(amount) & 255;
		for (int8_t y = 0; y < HEIGHT; y++)
		{
			if (amount < 0)
			{
				zD = y - delta;
				zF = zD - 1;
			}
			else
			{
				zD = y + delta;
				zF = zD + 1;
			}
			CRGB PixelA = CRGB::Black;
			if ((zD >= 0) && (zD < HEIGHT))
				PixelA = getPixelColor(x, zD, leds);
			CRGB PixelB = CRGB::Black;
			if ((zF >= 0) && (zF < HEIGHT))
				PixelB = getPixelColor(x, zF, leds);
			drawPixel_fast(x, y, (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction))), ledsbuff);
		}
	}
	memcpy(leds, ledsbuff, BUFFER_SIZE);
}

// ----- VECTOR SECTION -----
template <class T>
class Vector2
{
public:
	T x, y;

	Vector2() : x(0), y(0) {}
	Vector2(T x, T y) : x(x), y(y) {}
	Vector2(const Vector2 &v) : x(v.x), y(v.y) {}

	Vector2 &operator=(const Vector2 &v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	bool isEmpty()
	{
		return x == 0 && y == 0;
	}

	bool operator==(Vector2 &v)
	{
		return x == v.x && y == v.y;
	}

	bool operator!=(Vector2 &v)
	{
		return !(x == y);
	}

	Vector2 operator+(Vector2 &v)
	{
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 operator-(Vector2 &v)
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 &operator+=(Vector2 &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2 &operator-=(Vector2 &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2 operator+(double s)
	{
		return Vector2(x + s, y + s);
	}
	Vector2 operator-(double s)
	{
		return Vector2(x - s, y - s);
	}
	Vector2 operator*(double s)
	{
		return Vector2(x * s, y * s);
	}
	Vector2 operator/(double s)
	{
		return Vector2(x / s, y / s);
	}

	Vector2 &operator+=(double s)
	{
		x += s;
		y += s;
		return *this;
	}
	Vector2 &operator-=(double s)
	{
		x -= s;
		y -= s;
		return *this;
	}
	Vector2 &operator*=(double s)
	{
		x *= s;
		y *= s;
		return *this;
	}
	Vector2 &operator/=(double s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	void set(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	void rotate(double deg)
	{
		double theta = deg / 180.0 * M_PI;
		double c = cos(theta);
		double s = sin(theta);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		x = tx;
		y = ty;
	}

	Vector2 &normalize()
	{
		if (length() == 0)
			return *this;
		*this *= (1.0 / length());
		return *this;
	}

	float dist(Vector2 v) const
	{
		Vector2 d(v.x - x, v.y - y);
		return d.length();
	}
	float length() const
	{
		return sqrt(x * x + y * y);
	}

	float mag() const
	{
		return length();
	}

	float magSq()
	{
		return (x * x + y * y);
	}

	void truncate(double length)
	{
		double angle = atan2f(y, x);
		x = length * cos(angle);
		y = length * sin(angle);
	}

	Vector2 ortho() const
	{
		return Vector2(y, -x);
	}

	static float dot(Vector2 v1, Vector2 v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
	static float cross(Vector2 v1, Vector2 v2)
	{
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

	void limit(float max)
	{
		if (magSq() > max * max)
		{
			normalize();
			*this *= max;
		}
	}
};

typedef Vector2<float> PVector;

// Boid class
// Methods for Separation, Cohesion, Alignment added
class Boid
{
public:
	PVector location;
	PVector velocity;
	PVector acceleration;
	float maxforce; // Maximum steering force
	float maxspeed; // Maximum speed

	float desiredseparation = 4;
	float neighbordist = 8;
	byte colorIndex = 0;
	float mass;

	boolean enabled = true;

	Boid() {}

	Boid(float x, float y)
	{
		acceleration = PVector(0, 0);
		velocity = PVector(randomf(), randomf());
		location = PVector(x, y);
		maxspeed = 1.5;
		maxforce = 0.05;
	}

	static float randomf()
	{
		return mapfloat(random8(), 0, 255, -.5, .5);
	}

	static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	void run(Boid boids[], uint8_t boidCount)
	{
		flock(boids, boidCount);
		update();
		// wrapAroundBorders();
		// render();
	}

	// Method to update location
	void update()
	{
		// Update velocity
		velocity += acceleration;
		// Limit speed
		velocity.limit(maxspeed);
		location += velocity;
		// Reset acceleration to 0 each cycle
		acceleration *= 0;
	}

	void applyForce(PVector force)
	{
		// We could add mass here if we want A = F / M
		acceleration += force;
	}

	void repelForce(PVector obstacle, float radius)
	{
		//Force that drives boid away from obstacle.

		PVector futPos = location + velocity; //Calculate future position for more effective behavior.
		PVector dist = obstacle - futPos;
		float d = dist.mag();

		if (d <= radius)
		{
			PVector repelVec = location - obstacle;
			repelVec.normalize();
			if (d != 0)
			{ //Don't divide by zero.
				// float scale = 1.0 / d; //The closer to the obstacle, the stronger the force.
				repelVec.normalize();
				repelVec *= (maxforce * 7);
				if (repelVec.mag() < 0)
				{ //Don't let the boids turn around to avoid the obstacle.
					repelVec.y = 0;
				}
			}
			applyForce(repelVec);
		}
	}

	// We accumulate a new acceleration each time based on three rules
	void flock(Boid boids[], uint8_t boidCount)
	{
		PVector sep = separate(boids, boidCount); // Separation
		PVector ali = align(boids, boidCount);		// Alignment
		PVector coh = cohesion(boids, boidCount); // Cohesion
		// Arbitrarily weight these forces
		sep *= 1.5;
		ali *= 1.0;
		coh *= 1.0;
		// Add the force vectors to acceleration
		applyForce(sep);
		applyForce(ali);
		applyForce(coh);
	}

	// Separation
	// Method checks for nearby boids and steers away
	PVector separate(Boid boids[], uint8_t boidCount)
	{
		PVector steer = PVector(0, 0);
		int count = 0;
		// For every boid in the system, check if it's too close
		for (int i = 0; i < boidCount; i++)
		{
			Boid other = boids[i];
			if (!other.enabled)
				continue;
			float d = location.dist(other.location);
			// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
			if ((d > 0) && (d < desiredseparation))
			{
				// Calculate vector pointing away from neighbor
				PVector diff = location - other.location;
				diff.normalize();
				diff /= d; // Weight by distance
				steer += diff;
				count++; // Keep track of how many
			}
		}
		// Average -- divide by how many
		if (count > 0)
		{
			steer /= (float)count;
		}

		// As long as the vector is greater than 0
		if (steer.mag() > 0)
		{
			// Implement Reynolds: Steering = Desired - Velocity
			steer.normalize();
			steer *= maxspeed;
			steer -= velocity;
			steer.limit(maxforce);
		}
		return steer;
	}

	// Alignment
	// For every nearby boid in the system, calculate the average velocity
	PVector align(Boid boids[], uint8_t boidCount)
	{
		PVector sum = PVector(0, 0);
		int count = 0;
		for (int i = 0; i < boidCount; i++)
		{
			Boid other = boids[i];
			if (!other.enabled)
				continue;
			float d = location.dist(other.location);
			if ((d > 0) && (d < neighbordist))
			{
				sum += other.velocity;
				count++;
			}
		}
		if (count > 0)
		{
			sum /= (float)count;
			sum.normalize();
			sum *= maxspeed;
			PVector steer = sum - velocity;
			steer.limit(maxforce);
			return steer;
		}
		else
		{
			return PVector(0, 0);
		}
	}

	// Cohesion
	// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
	PVector cohesion(Boid boids[], uint8_t boidCount)
	{
		PVector sum = PVector(0, 0); // Start with empty vector to accumulate all locations
		int count = 0;
		for (int i = 0; i < boidCount; i++)
		{
			Boid other = boids[i];
			if (!other.enabled)
				continue;
			float d = location.dist(other.location);
			if ((d > 0) && (d < neighbordist))
			{
				sum += other.location; // Add location
				count++;
			}
		}
		if (count > 0)
		{
			sum /= count;
			return seek(sum); // Steer towards the location
		}
		else
		{
			return PVector(0, 0);
		}
	}

	// A method that calculates and applies a steering force towards a target
	// STEER = DESIRED MINUS VELOCITY
	PVector seek(PVector target)
	{
		PVector desired = target - location; // A vector pointing from the location to the target
		// Normalize desired and scale to maximum speed
		desired.normalize();
		desired *= maxspeed;
		// Steering = Desired minus Velocity
		PVector steer = desired - velocity;
		steer.limit(maxforce); // Limit to maximum steering force
		return steer;
	}

	// A method that calculates a steering force towards a target
	// STEER = DESIRED MINUS VELOCITY
	void arrive(PVector target)
	{
		PVector desired = target - location; // A vector pointing from the location to the target
		float d = desired.mag();
		// Normalize desired and scale with arbitrary damping within 100 pixels
		desired.normalize();
		if (d < 4)
		{
			float m = map(d, 0, 100, 0, maxspeed);
			desired *= m;
		}
		else
		{
			desired *= maxspeed;
		}

		// Steering = Desired minus Velocity
		PVector steer = desired - velocity;
		steer.limit(maxforce); // Limit to maximum steering force
		applyForce(steer);
		//Serial.println(d);
	}

	void wrapAroundBorders()
	{
		if (location.x < 0)
			location.x = WIDTH_d;
		if (location.y < 0)
			location.y = HEIGHT_d;
		if (location.x >= WIDTH)
			location.x = 0;
		if (location.y >= HEIGHT)
			location.y = 0;
	}

	void avoidBorders()
	{
		PVector desired = velocity;

		if (location.x < 8)
			desired = PVector(maxspeed, velocity.y);
		if (location.x >= WIDTH - 8)
			desired = PVector(-maxspeed, velocity.y);
		if (location.y < 8)
			desired = PVector(velocity.x, maxspeed);
		if (location.y >= HEIGHT - 8)
			desired = PVector(velocity.x, -maxspeed);

		if (desired != velocity)
		{
			PVector steer = desired - velocity;
			steer.limit(maxforce);
			applyForce(steer);
		}

		if (location.x < 0)
			location.x = 0;
		if (location.y < 0)
			location.y = 0;
		if (location.x >= WIDTH)
			location.x = WIDTH - 1;
		if (location.y >= HEIGHT)
			location.y = HEIGHT - 1;
	}

	bool bounceOffBorders(float bounce)
	{
		bool bounced = false;

		if (location.x >= WIDTH)
		{
			location.x = WIDTH_d;
			velocity.x *= -bounce;
			bounced = true;
		}
		else if (location.x < 0)
		{
			location.x = 0;
			velocity.x *= -bounce;
			bounced = true;
		}

		if (location.y >= HEIGHT)
		{
			location.y = HEIGHT_d;
			velocity.y *= -bounce;
			bounced = true;
		}
		else if (location.y < 0)
		{
			location.y = 0;
			velocity.y *= -bounce;
			bounced = true;
		}

		return bounced;
	}

	void render()
	{
		// // Draw a triangle rotated in the direction of velocity
		// float theta = velocity.heading2D() + radians(90);
		// fill(175);
		// stroke(0);
		// pushMatrix();
		// translate(location.x,location.y);
		// rotate(theta);
		// beginShape(TRIANGLES);
		// vertex(0, -r*2);
		// vertex(-r, r*2);
		// vertex(r, r*2);
		// endShape();
		// popMatrix();
		// backgroundLayer.drawPixel(location.x, location.y, CRGB::Blue);
	}
};

#endif
#endif
