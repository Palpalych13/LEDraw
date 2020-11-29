#ifndef effectsClassic_h
#define effectsClassic_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_2d.h"

const TProgmemRGBPalette16 ZebraColors_p PROGMEM = {
		CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
		CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
		CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
		CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black};

uint8_t ihue = 0;

// ***** FIRE / ОГОНЬ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Full range scale by PalPalych 23.03.2020

#define e_fir_SPARKLES 1U // вылетающие угольки вкл/выкл

uint8_t line[WIDTH];
uint8_t pcnt = 0U;
unsigned char matrixValue[8][16];

//these values are substracetd from the generated values to give a shape to the animation
static const uint8_t valueMask[8][16] PROGMEM = {
		{32, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 32},
		{64, 0, 0, 0, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 64},
		{96, 32, 0, 0, 0, 0, 32, 96, 96, 32, 0, 0, 0, 0, 32, 96},
		{128, 64, 32, 0, 0, 32, 64, 128, 128, 64, 32, 0, 0, 32, 64, 128},
		{160, 96, 64, 32, 32, 64, 96, 160, 160, 96, 64, 32, 32, 64, 96, 160},
		{192, 128, 96, 64, 64, 96, 128, 192, 192, 128, 96, 64, 64, 96, 128, 192},
		{255, 160, 128, 96, 96, 128, 160, 255, 255, 160, 128, 96, 96, 128, 160, 255},
		{255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
static const uint8_t hueMask[8][16] PROGMEM = {
		{1, 11, 19, 25, 25, 22, 11, 1, 1, 11, 19, 25, 25, 22, 11, 1},
		{1, 8, 13, 19, 25, 19, 8, 1, 1, 8, 13, 19, 25, 19, 8, 1},
		{1, 8, 13, 16, 19, 16, 8, 1, 1, 8, 13, 16, 19, 16, 8, 1},
		{1, 5, 11, 13, 13, 13, 5, 1, 1, 5, 11, 13, 13, 13, 5, 1},
		{1, 5, 11, 11, 11, 11, 5, 1, 1, 5, 11, 11, 11, 11, 5, 1},
		{0, 1, 5, 8, 8, 5, 1, 0, 0, 1, 5, 8, 8, 5, 1, 0},
		{0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}};

// Randomly generate the next line (matrix row)
void generateLine()
{
	for (uint8_t x = 0U; x < WIDTH; x++)
	{
		line[x] = random(64, 255);
	}
}

void shiftUp()
{
	for (uint8_t y = HEIGHT_d; y > 0U; y--)
	{
		for (uint8_t x = 0U; x < WIDTH; x++)
		{
			uint8_t newX = x;
			if (x > 15U)
				newX = x % 16U;
			if (y > 7U)
				continue;
			matrixValue[y][newX] = matrixValue[y - 1U][newX];
		}
	}

	for (uint8_t x = 0U; x < WIDTH; x++)
	{
		uint8_t newX = x;
		if (x > 15U)
			newX = x % 16U;
		matrixValue[0U][newX] = line[newX];
	}
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(uint8_t pcnt, bool isColored, uint8_t Scale)
{
	if (Scale == 255)
		isColored = false;
	int32_t nextv;
	//each row interpolates with the one before it
	for (uint8_t y = HEIGHT - 1U; y > 0U; y--)
	{
		for (uint8_t x = 0U; x < WIDTH; x++)
		{
			uint8_t newX = x;
			if (x > 15U)
				newX = x % 16U;
			if (y < 8U)
			{
				nextv =
						(((100.0 - pcnt) * matrixValue[y][newX] + pcnt * matrixValue[y - 1][newX]) / 100.0) - pgm_read_byte(&valueMask[y][newX]);

				CRGB color = CHSV(
						isColored ? Scale + pgm_read_byte(&hueMask[y][newX]) : 0U, // H
						isColored ? 255U : 0U,																		 // S
						(uint8_t)max(0, nextv)																		 // V
				);

				drawPixel(x, y, color);
			}
			else if (y == 8U && e_fir_SPARKLES)
			{
				if (random(0, 20) == 0 && getPixelValue(x, y - 1U) != 0U)
					drawPixel_fast(x, y, getPixelColor(x, y - 1U));
				else
					drawPixel_fast(x, y, 0U);
			}
			else if (e_fir_SPARKLES)
			{
				// старая версия для яркости
				if (getPixelValue(x, y - 1U) > 0U)
					drawPixel_fast(x, y, getPixelColor(x, y - 1U));
				else
					drawPixel_fast(x, y, 0U);
			}
		}
	}

	//first row interpolates with the "next" line
	for (uint8_t x = 0U; x < WIDTH; x++)
	{
		uint8_t newX = x;
		if (x > 15U)
			newX = x % 16U;
		CRGB color = CHSV(
				isColored ? Scale + pgm_read_byte(&(hueMask[0][newX])) : 0U,									 // H
				isColored ? 255U : 0U,																												 // S
				(uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
		);
		//leds[getPixelNumber(newX, 0)] = color;      // на форуме пишут что это ошибка - вместо newX должно быть x, иначе
		leds[getPixelNumber(x, 0)] = color; // на матрицах шире 16 столбцов нижний правый угол неработает
	}
}

bool fireRoutine(bool isColored, uint8_t Scale = effect.scale_real) // <- вхождение процедуры / true - цветной огонь, false - белый
{
	if (pcnt >= 100)
	{
		shiftUp();
		generateLine();
		pcnt = 0;
	}
	drawFrame(pcnt, isColored, Scale);
	pcnt += 30;
	return true;
}

// ***** VERTICAL RAINBOW / ВЕРТИКАЛЬНАЯ РАДУГА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Fast & Time-Independed Logic, Dither by PalPalych 26.03.2020
#define e_rbwv_DITHER 2U // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool rainbowVerticalRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{ // <- вхождение эффекта
#if e_rbwv_DITHER
	if (effect.frame_counter % e_rbwv_DITHER)
		return false;
#endif
	uint32_t basehue = millis() / constrain(Speed / 16, 1U, 255U);
	for (uint16_t y = 0U; y < HEIGHT; y++)
	{
		drawHLine2_fast(0U, WIDTH_d, y, CHSV(basehue + (y * 3 * (Scale / HEIGHT + 1U)), 255, 255));
	}
	return true;
}

// ***** HORIZONTAL RAINBOW / ГОРИЗОНТАЛЬНАЯ РАДУГА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Fast & Time-Independed Logic, Dither by PalPalych 26.03.2020
#define e_rbwh_DITHER 2U // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool rainbowHorizontalRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real) // <- вхождение эффекта
{
#if e_rbwh_DITHER
	if (effect.frame_counter % e_rbwh_DITHER)
		return false;
#endif
	uint32_t basehue = millis() / constrain(Speed / 16, 1U, 255U);
	for (uint16_t x = 0U; x < WIDTH; x++)
	{
		drawVLine2_fast(x, 0U, HEIGHT_d, CHSV(basehue + (x * 3 * (Scale / WIDTH + 1U)), 255, 255));
	}
	return true;
}

// ***** DIAGONAL RAINBOW / ДИАГОНАЛЬНАЯ РАДУГА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020

bool rainbowDiagonalRoutine(uint8_t Scale = effect.scale_real) // <- вхождение эффекта
{
	eff_offset += 8;
	for (uint8_t i = 0U; i < WIDTH; i++)
	{
		for (uint8_t j = 0U; j < HEIGHT; j++)
		{
			float twirlFactor = 3.0F * (Scale / 100.0F); // на сколько оборотов будет закручена матрица, [0..3]
			CRGB thisColor = CHSV((uint8_t)(eff_offset + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255);
			drawPixel_fast(i, j, thisColor);
		}
	}
	return true;
}

// ***** SNOW / СНЕГОПАД *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020

bool snowRoutine(uint8_t Scale = effect.scale_real)
{
	// сдвигаем всё вниз
	moveScreenDown_fast();

	for (uint8_t x = 0U; x < WIDTH; x++)
	{
		// заполняем случайно верхнюю строку
		// а также не даём двум блокам по вертикали вместе быть
		if (getPixelValue(x, HEIGHT - 2U) == 0U && (random8(0, 100 - Scale) == 0U))
			drawPixel_fast(x, HEIGHT_d, 0xE0FFFF - 0x101010 * random8(4));
		else
			drawPixel_fast(x, HEIGHT_d, 0x000000);
	}
	return true;
}

// ***** SNOWSTORM / МЕТЕЛЬ # STARFALL / ЗВЕЗДОПАД *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Fix wrong math & full screen drawing by PalPalych 14.03.2020
// v1.2 - Code optimisation + pseudo 3d by PalPalych 21.04.2020

#define e_sns_DENSE (32U) // плотность снега - меньше = плотнее

bool stormRoutine(uint8_t Scale = effect.scale_real) // Scale -- от 0 до 255
{
	// заполняем головами комет
	uint8_t Saturation = 0U;		// цвет хвостов
	uint8_t e_TAIL_STEP = 127U; // длина хвоста
	if (Scale > 127)
		Saturation = Scale;
	else
	{
		e_TAIL_STEP = 255U - Scale;
	}

	for (int8_t x = 0U; x < WIDTH_d; x++) // fix error i != 0U
	{
		if (!random8(e_sns_DENSE) &&
				!getPixelValue(wrapX(x), HEIGHT_d) &&
				!getPixelValue(wrapX(x + 1U), HEIGHT_d) &&
				!getPixelValue(wrapX(x - 1U), HEIGHT_d))
		{
			drawPixel_fast(x, HEIGHT_d, CHSV(random8(), Saturation, random8(64U, 255U)));
		}
	}
	// сдвигаем по диагонали
	for (int8_t y = 0U; y < HEIGHT_d; y++)
	{
		for (int8_t x = 0; x < WIDTH; x++)
		{
			drawPixel_fast(wrapX(x + 1U), y, getPixelColor(x, y + 1U));
		}
	}
	// уменьшаем яркость верхней линии, формируем "хвосты"
	for (int8_t i = 0U; i < WIDTH; i++)
	{
		fadePixel_fast(i, HEIGHT_d, e_TAIL_STEP);
	}
	return true;
}

// ***** THE MATRIX / МАТРИЦА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020

bool matrixRoutine(uint8_t Scale = effect.scale_real)
{
	moveScreenDown_fast();

	for (uint8_t x = 0U; x < WIDTH; x++) // заполняем случайно верхнюю строку
	{

		uint32_t thisColor = getPixelValue(x, HEIGHT_d);
		if (thisColor == 0U)
			drawPixel_fast(x, HEIGHT_d, 0x00FF00 * (random8(0, 100 - Scale) == 0U));
		else if (thisColor < 0x002000)
			drawPixel_fast(x, HEIGHT_d, 0U);
		else
			drawPixel_fast(x, HEIGHT_d, thisColor - 0x002000);
	}
	return true;
}

// ***** LIGHTERS / СВЕТЛЯЧКИ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - + Lighters tail by PalPalych 04.04.2020

#define LIGHTERS_AM (100U)
#define e_ligs_DIMMER (80)
int32_t lightersPos[2U][LIGHTERS_AM];
int8_t lightersSpeed[2U][LIGHTERS_AM];
CHSV lightersColor[LIGHTERS_AM];
uint8_t loopCounter;
int32_t angle[LIGHTERS_AM];
int32_t speedV[LIGHTERS_AM];
int8_t angleSpeed[LIGHTERS_AM];

bool lightersRoutine(uint8_t Scale = effect.scale_real)
{
	if (effect.hasChangedEffect())
	{
		randomSeed(millis());
		for (uint8_t i = 0U; i < LIGHTERS_AM; i++)
		{
			lightersPos[0U][i] = random(0, WIDTH * 10);
			lightersPos[1U][i] = random(0, HEIGHT * 10);
			lightersSpeed[0U][i] = random(-10, 10);
			lightersSpeed[1U][i] = random(-10, 10);
			lightersColor[i] = CHSV(random(0U, 255U), 255U, 255U);
		}
	}
	dimAll(e_ligs_DIMMER);
	if (++loopCounter > 20U)
		loopCounter = 0U;
	for (uint8_t i = 0U; i < Scale; i++)
	{
		if (loopCounter == 0U)
		{ // меняем скорость каждые 255 отрисовок
			lightersSpeed[0U][i] += random(-3, 4);
			lightersSpeed[1U][i] += random(-3, 4);
			lightersSpeed[0U][i] = constrain(lightersSpeed[0U][i], -20, 20);
			lightersSpeed[1U][i] = constrain(lightersSpeed[1U][i], -20, 20);
		}

		lightersPos[0U][i] += lightersSpeed[0U][i];
		lightersPos[1U][i] += lightersSpeed[1U][i];

		if (lightersPos[0U][i] < 0)
			lightersPos[0U][i] = (WIDTH_d)*10;
		if (lightersPos[0U][i] >= (int32_t)(WIDTH * 10))
			lightersPos[0U][i] = 0;

		if (lightersPos[1U][i] < 0)
		{
			lightersPos[1U][i] = 0;
			lightersSpeed[1U][i] = -lightersSpeed[1U][i];
		}
		if (lightersPos[1U][i] >= (int32_t)(HEIGHT_d)*10)
		{
			lightersPos[1U][i] = (HEIGHT_d)*10;
			lightersSpeed[1U][i] = -lightersSpeed[1U][i];
		}
		drawPixel(lightersPos[0U][i] / 10, lightersPos[1U][i] / 10, lightersColor[i]);
	}
	return true;
}

// ***** LIGHTER TRACES / СВЕТЛЯЧКИ СО ШЛЕЙФОМ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020

#define BALLS_AMOUNT (3U) // количество "шариков"
#define CLEAR_PATH (1U)		// очищать путь
#define BALL_TRACK (1U)		// (0 / 1) - вкл/выкл следы шариков
#define TRACK_STEP (70U)	// длина хвоста шарика (чем больше цифра, тем хвост короче)

int16_t coord[BALLS_AMOUNT][2U];
int8_t vector[BALLS_AMOUNT][2U];
CRGB ballColors[BALLS_AMOUNT];

bool ballsRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
	if (effect.hasChangedEffect())
	{
		for (uint8_t j = 0U; j < BALLS_AMOUNT; j++)
		{
			int8_t sign;
			// забиваем случайными данными
			coord[j][0U] = WIDTH / 2 * 10;
			random(0, 2) ? sign = 1 : sign = -1;
			vector[j][0U] = random(4, 15) * sign;
			coord[j][1U] = HEIGHT / 2 * 10;
			random(0, 2) ? sign = 1 : sign = -1;
			vector[j][1U] = random(4, 15) * sign;
			//ballColors[j] = CHSV(random(0, 9) * 28, 255U, 255U);
		}
	}

#if (BALL_TRACK) // режим со следами
	fadeAll(TRACK_STEP);
#else // режим без следов шариков
	clearAll();
#endif
	// движение шариков
	for (uint8_t j = 0U; j < BALLS_AMOUNT; j++)
	{
		// цвет зависит от масштаба
		ballColors[j] = CHSV((Scale * (j + 1)) % 256U, 255U, 255U);
		//ballColors[j] = CHSV((effects[EFF_LIGHTER_TRACES].Scale * (j + 1)) % 256U, 255U, 255U);
		// движение шариков
		for (uint8_t i = 0U; i < 2U; i++)
		{
			coord[j][i] += vector[j][i];
			if (coord[j][i] < 0)
			{
				coord[j][i] = 0;
				vector[j][i] = -vector[j][i];
			}
		}

		if (coord[j][0U] > (int16_t)(WIDTH_d * 10))
		{
			coord[j][0U] = (WIDTH_d)*10;
			vector[j][0U] = -vector[j][0U];
		}
		if (coord[j][1U] > (int16_t)(HEIGHT_d * 10))
		{
			coord[j][1U] = HEIGHT_d * 10;
			vector[j][1U] = -vector[j][1U];
		}
		leds[getPixelNumber(coord[j][0U] / 10, coord[j][1U] / 10)] = ballColors[j];
	}
	return true;
}

// ***** PAINTBALL / ПЕЙНТБОЛ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Speed Correction and Dither by PalPalych 19.03.2020

#define BORDERTHICKNESS (1U) // глубина бордюра для размытия яркой частицы: 0U - без границы (резкие края); 1U - 1 пиксель (среднее размытие) ; 2U - 2 пикселя (глубокое размытие)
#define e_lball_DITHER (2U)	 // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
const uint8_t paintWidth = WIDTH - BORDERTHICKNESS * 2;
const uint8_t paintHeight = HEIGHT - BORDERTHICKNESS * 2;

bool lightBallsRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_lball_DITHER
	if (effect.frame_counter % e_lball_DITHER)
		return false;
#endif
	// Apply some blurring to whatever's already on the matrix
	// Note that we never actually clear the matrix, we just constantly
	// blur it repeatedly.  Since the blurring is 'lossy', there's
	// an automatic trend toward black -- by design.
	uint8_t blurAmount = dim8_raw(beatsin8(3, 64, 100));
	blurAll(blurAmount);
	Speed = 64U - (Speed >> 2U);
	// Use two out-of-sync sine waves
	uint16_t i = beatsin8(79 - Speed); //91
	uint16_t j = beatsin8(67 - Speed); //109
	uint16_t k = beatsin8(71 - Speed); //73
	uint16_t m = beatsin8(97 - Speed); //123

	// The color of each point shifts over time, each at a different speed.
	uint32_t ms = millis() / (Scale / 4 + 1);
	leds[getPixelNumber(highByte(i * paintWidth) + BORDERTHICKNESS, highByte(j * paintHeight) + BORDERTHICKNESS)] += CHSV(ms / 29, 255U, 255U);
	leds[getPixelNumber(highByte(j * paintWidth) + BORDERTHICKNESS, highByte(k * paintHeight) + BORDERTHICKNESS)] += CHSV(ms / 41, 255U, 255U);
	leds[getPixelNumber(highByte(k * paintWidth) + BORDERTHICKNESS, highByte(m * paintHeight) + BORDERTHICKNESS)] += CHSV(ms / 37, 255U, 255U);
	leds[getPixelNumber(highByte(m * paintWidth) + BORDERTHICKNESS, highByte(i * paintHeight) + BORDERTHICKNESS)] += CHSV(ms / 53, 255U, 255U);
	return true;
}

// ***** WALKING CUBE / БЛУЖДАЮЩИЙ КУБИК *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Dither & box tail by PalPalych 04.04.2020
// v1.2 - New engine + WALKING BALL / БЛУЖДАЮЩИЙ ШАРИК by PalPalych 05.04.2020

#define e_box_RANDOM_COLOR (1U) // случайный цвет при отскоке
#define e_box_DITHER (2U)				// разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
#define e_box_DIMMER (192U)

CRGB e_bb_color;
uint8_t e_bb_size;

bool e_bb_dirX = true, e_bb_dirY = true;
int8_t e_bb_X = 0;
int8_t e_bb_Y = 0;
int8_t e_bb_stepsX = 5;
int8_t e_bb_stepsY = 3;
int8_t e_bb_countX = e_bb_stepsX;
int8_t e_bb_countY = e_bb_stepsY;

bool ballRoutine(uint8_t Scale = effect.scale_real)
{
#if e_box_DITHER
	if (effect.frame_counter % e_box_DITHER)
		return false;
#endif
	e_bb_size = map8(((Scale > 127U) ? ~Scale : Scale) << 1U, 2U, DIMENSION_min >> 1U);

	if (e_bb_X <= 0)
	{
		e_bb_dirX = true;
		if (e_box_RANDOM_COLOR)
		{
			e_bb_color = CHSV(random8(), 255U, 255U);
			e_bb_stepsX = random(5) + 1U;
		}
	}
	if (e_bb_X >= (int8_t)WIDTH - e_bb_size)
	{
		e_bb_X = WIDTH - e_bb_size;
		e_bb_dirX = false;
		if (e_box_RANDOM_COLOR)
			e_bb_color = CHSV(random8(), 255U, 255U);
		e_bb_stepsY = random(5) + 1U;
	}
	if (e_bb_Y <= 0)
	{
		e_bb_dirY = true;
		if (e_box_RANDOM_COLOR)
		{
			e_bb_color = CHSV(random8(), 255U, 255U);
			e_bb_stepsY = random(5) + 1U;
		}
	}
	if (e_bb_Y >= (int8_t)HEIGHT - e_bb_size)
	{
		e_bb_Y = HEIGHT - e_bb_size;
		e_bb_dirY = false;
		if (e_box_RANDOM_COLOR)
			e_bb_color = CHSV(random8(), 255U, 255U);
		e_bb_stepsX = random(5) + 1U;
	}

	if (!e_bb_countX)
	{
		e_bb_countX = e_bb_stepsX;
		e_bb_dirX ? e_bb_X++ : e_bb_X--;
	}
	else
	{
		e_bb_countX--;
	}

	if (!e_bb_countY)
	{
		e_bb_countY = e_bb_stepsY;
		e_bb_dirY ? e_bb_Y++ : e_bb_Y--;
	}
	else
	{
		e_bb_countY--;
	}

	dimAll(e_box_DIMMER);
	if (Scale > 127U)
	{
		drawFillRect(e_bb_X, e_bb_Y, e_bb_size, e_bb_size, e_bb_color);
	}
	else
	{
		drawOval2_fast(e_bb_X, e_bb_X + e_bb_size - 1U, e_bb_Y, e_bb_Y + e_bb_size - 1U, e_bb_color);
	}
	return true;
}

// ***** 3D NOISE EFFECTS / ЭФФЕКТЫ С 3D-ШУМОМ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 13.03.2020

#define e_n3d_DITHER (4U) // разрешить дисперсию эффектов DRIFT/DRIFT2 (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)

bool fillnoise8(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
	for (uint8_t i = 0; i < MAX_DIMENSION; i++)
	{
		int32_t i_offset = Scale * i;
		for (uint8_t j = 0; j < MAX_DIMENSION; j++)
		{
			int32_t j_offset = Scale * j;
			noise[i][j] = inoise8(x + i_offset, y + j_offset, z);
		}
	}
	z += Speed;
	return true;
}

bool madnessNoiseRoutine(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
	fillnoise8(Scale, Speed);
	for (uint8_t i = 0; i < WIDTH; i++)
	{
		for (uint8_t j = 0; j < HEIGHT; j++)
		{
			drawPixel(i, j, CHSV(noise[j][i], 255, noise[i][j]));
		}
	}
	ihue += 1;
	return true;
}

bool fillNoiseLED(const TProgmemRGBPalette16 *curPalette, bool ColorLoop, uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
#if e_n3d_DITHER
	if (effect.frame_counter % e_n3d_DITHER)
		return false;
#endif
	uint8_t dataSmoothing = 0;
	if (Speed <= 200U)
	{
		dataSmoothing = 200 - (Speed * 4);
	}
	for (uint8_t i = 0; i < MAX_DIMENSION; i++)
	{
		int16_t i_offset = Scale * i;
		for (uint8_t j = 0; j < MAX_DIMENSION; j++)
		{
			int16_t j_offset = Scale * j;
			uint8_t data = inoise8(x + i_offset, y + j_offset, z);
			data = qsub8(data, 16U);
			data = qadd8(data, scale8(data, 39U));

			if (dataSmoothing)
			{
				uint8_t olddata = noise[i][j];
				uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 255U - dataSmoothing);
				data = newdata;
			}
			noise[i][j] = data;
		}
	}
	z += Speed;
	// apply slow drift to X and Y, just for visual variation.
	x += Speed / 8;
	y -= Speed / 16;
	for (uint8_t i = 0; i < WIDTH; i++)
	{
		for (uint8_t j = 0; j < HEIGHT; j++)
		{
			uint8_t index = noise[j][i];
			uint8_t bri = noise[i][j];
			// if this palette is a 'loop', add a slowly-changing base value
			if (ColorLoop)
				index += ihue;
			// brighten up, as the color palette itself often contains the
			// light/dark dynamic range desired
			if (bri > 127)
			{
				bri = 255;
			}
			else
			{
				bri = dim8_raw(bri * 2);
			}
			drawPixel_fast(i, j, ColorFromPalette(*curPalette, index, bri));
		}
	}
	ihue += 1;
	return true;
}

bool fillNoiseLED(uint8_t Scale = effect.scale_real, uint8_t Speed = effect.speed_real)
{
	uint8_t pos = Scale / palettes_steps;
	bool loop = true;
	if (pos == 1 || pos == 4 || pos == 5)
		loop = false;
	const TProgmemRGBPalette16 *curPalette = palettes[pos];
	return fillNoiseLED(curPalette, loop, Scale % palettes_steps, Speed);
}

#endif
