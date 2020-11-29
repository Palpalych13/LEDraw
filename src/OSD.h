#ifndef _osd_h
#define _osd_h

#if USE_OSD2
#include "fonts.h"
#include <stdint.h>
//fader_t osd;
#endif

#if USE_OSD
/*
//#include "chronometer.h"
// OSD v1.0b / On Screen Display -- работа с бегущим текстом и графическими представлениями для матриц на ws821х для библиотеки FastLED
// TODO: переместить функции прорисовки индикаторов

#define TEXT_HEIGHT (1U) // высота, на которой бежит текст (от низа матрицы)
#define LET_WIDTH (5)		 // ширина буквы шрифта
#define LET_HEIGHT (8)	 // высота буквы шрифта
#define SPACE (1U)			 // пробел
//#define LETTER_COLOR          (CRGB::White)                 // цвет букв по умолчанию

char osd_string[64]; // буфер бегущей строки макс. 64 символа [0...31], [32] = 00 конец строки

CRGB osd_fontColor = CRGB::White; // переменная цвета текста

const int16_t let_width = LET_WIDTH - 1;		 // константы
const int16_t let_space = LET_WIDTH + SPACE; //

// ----- OSD Fader Routines -----

bool osd_faderSimpleRoutine(uint8_t value)
{
	drawFillRect2_fast(0, WIDTH_d, 1, 9, CRGB::Black, ease8InOutApprox(scale8(208, value)));
	return true;
}

// ----- OSD Fader Styles Array -----
bool((*osd_faderStyles[]))(uint8_t) = {[](uint8_t value) { return osd_faderSimpleRoutine(value); }};

uint8_t osd_faderStageStyles[][3] = {{0, 0, 0}};

// ----- OSD Gauge Routines -----
bool osd_gaugeBigVerticalRoutine(uint8_t value, CRGB color)
{
	drawFillRect2_fast(5, 10, 0, HEIGHT_d, CRGB::Black, 192);
	drawFillRect2_fast(7, 8, 0, constrain(value / HEIGHT, 0, 255), color);
	return true;
}

bool osd_gaugeHueGradientVerticalRoutine(uint8_t value)
{
	drawFillRect2_fast(5, 10, 0, HEIGHT_d, CRGB::Black);
	gradientVRect2_fast(7, 8, 0, HEIGHT_d, CHSV(0, 255, 255), CHSV(255, 255, 255));
	drawHLine2_fast(5, 10, value / HEIGHT, CRGB::White);
	return true;
}

bool osd_gaugeValueGradientVerticalRoutine(uint8_t value, CRGB startColor = CRGB::Black, CRGB endColor = CRGB::White)
{
	drawFillRect2_fast(5, 10, 0, HEIGHT_d, CRGB::Black);
	gradientVRect2_fast(7, 8, 0, HEIGHT_d, startColor, endColor);
	drawHLine2_fast(5, 10, value / HEIGHT, CRGB::White);
	return true;
}

bool osd_gaugeShowHorizontalRoutine(uint8_t value, CRGB color, uint8_t yCol) //by kDn
{
	byte yStep = HEIGHT / yCol;
	for (byte y = 0; y <= yCol * (yStep - 1); y += yStep)
		for (byte x = 0; x < WIDTH; x++)
			if (value / WIDTH > x)
				drawPixel_fast(wrapX(x + y), y, color);
			else
				drawPixel_fast(wrapX(x + y), y, 0);
	return true;
}

bool osd_gaugeShowVerticalRoutine(uint8_t value, CRGB color, uint8_t xCol) //by kDn
{
	byte xStep = WIDTH / xCol;
	for (byte x = 0; x <= xCol * (xStep - 1); x += xStep)
		for (byte y = 0; y < HEIGHT; y++)
			if (value / HEIGHT > y)
				drawPixel_fast(x, y, color);
			else
				drawPixel_fast(x, y, 0);
	return true;
}

// ----- OSD Gauge Styles Array -----
bool (*osd_gaugeStyles[])(uint8_t) = {
		[](uint8_t value) {
			return osd_gaugeBigVerticalRoutine(value, CRGB::Red); // 0 - Big Vertical -- Red
		},
		[](uint8_t value) {
			return osd_gaugeBigVerticalRoutine(value, CRGB::Blue); // 1 - Big Vertical -- Blue
		},
		[](uint8_t value) {
			return osd_gaugeBigVerticalRoutine(value, CRGB::Lime); // 2 - Big Vertical -- Green
		},
		[](uint8_t value) {
			return osd_gaugeHueGradientVerticalRoutine(value); // 3 - Hue Gradient Vertical
		},
		[](uint8_t value) {
			return osd_gaugeValueGradientVerticalRoutine(value); // 4 - Value Gradient Vertical
		},
		[](uint8_t value) {
			return osd_gaugeShowHorizontalRoutine(value, CHSV(10, 255, 255), 3); // 5 - Show Horizontal -- Flame 3x3
		},
		[](uint8_t value) {
			return osd_gaugeShowHorizontalRoutine(value, CHSV(100, 255, 255), 2); // ... - Show Horizontal -- Green 2x2
		},
		[](uint8_t value) {
			return osd_gaugeShowHorizontalRoutine(value, CHSV(150, 255, 255), 1); // ... - Show Horizontal -- Blue 1x1
		},
		[](uint8_t value) {
			return osd_gaugeShowVerticalRoutine(value, CHSV(10, 255, 255), 4); // ... - Show Vertical -- Flame 4x4
		},
		[](uint8_t value) {
			return osd_gaugeShowVerticalRoutine(value, CHSV(100, 255, 255), 2); // ... - Show Vertical -- Green 2x2
		},
		[](uint8_t value) {
			return osd_gaugeShowVerticalRoutine(value, CHSV(150, 255, 255), 1); // ... - Show Vertical -- Blue 1x1
		},
		[](uint8_t value) {
			return osd_gaugeValueGradientVerticalRoutine(value, CRGB::Black, CRGB::Yellow); // 11 - Big Vertical -- Yellow
		}};

// ----- OSD Ticker Routine -----

bool osd_t::drawGauge(uint8_t value, uint8_t style = 0)
{ // вывод индикатора
	osd_gaugeValue = value;
	osd_gaugeStyle = style;
	osd_gaugeTimer = millis();
	return true;
}

bool osd_t::scrollDone()
{
	//return osd_scrollStage;    osd_string                                               // возвращает признак завершения показа строки
	return !osd_string[0];
}

bool osd_t::scrollText(String text, CRGB color = CRGB::White, // печать бегущей строки с заданным цветом
											 uint8_t Speed = 10,										// скорость прокрутки - 10x мс для смещения на 1 пиксель
											 uint8_t Mode = 0,											/* режим вывода (флаги) 0...3:эффект вывода, 6: переписать текст, 7:сброс после вывода
                                                           // мерцание - 0x01...0x07 - глубина эффекта
                                                           // мигание - 0x08...0x15 - период от 1/4 до 2 сек */
											 uint8_t style = 0,											// стиль фейдера
											 uint8_t times = 1)
											 { // TODO количество повторов

												 osd_mode = Mode;
												 if (!(osd_mode & 0x40))
													 if (!text[0] || osd_scrollStage) // процедура не приняла значение или находится на стадии печати строки (возвращает false)
														 return false;
												 osd_scrollSpeed = Speed * 10U;
												 if (osd_mode & 0x07)
												 {
													 if (!(osd_mode & 0x08))
													 {
														 osd_shine = true;
														 osd_textShine = (((uint8_t)(osd_mode & 0x07) + 1) * 31);
														 LOG.print(PSTR("Text: Параметр мерцания = "));
														 LOG.println(osd_textShine);
													 }
													 else
													 {
														 osd_flash = true;
														 osd_flashPeriod = ((uint8_t)(osd_mode & 0x07) + 1) * 63;
														 LOG.print(PSTR("Text: Параметр мигания = "));
														 LOG.println(osd_flashPeriod);
													 }
												 }
												 osd_fontColor = color; // сохранение текущего цвета
												 uint8_t pointer = 0;
												 uint8_t len = text.length() + 1;
												 for (uint8_t i = 0; i < len; i++)
												 {
													 if (text[i] > 191U)
														 i++;
													 osd_string[pointer] = text[i];
													 pointer++;
												 }
												 osd_scrollStage = 1U;
												 osd_scrollCounter = 0;
												 osd_scrollTimer = millis();
												 osd_faderDuration = 2000;
												 return true; // показ строки начался (возвращает true)
											 }

											 bool osd_t::printCurrentTime(CRGB color)
											 { // печать текущего времени // TODO: переписать поцедуру для вывода правильного формата "->00:00"
												 if (osd_scrollStage || !ESP_mode)
													 return false; // процедура находится на стадии печати строки
#if USE_NTP															 // вывод, только если используется синхронизация времени и если заказан его вывод бегущей строкой
												 if (isWifiOffMode)
													 return false;																// при отключенном WiFi - сразу на выход
												 if (!ntpServerAddressResolved || !timeSynched) // вывод только в режиме WiFi клиента и только, если имя сервера времени разрезолвлено
													 return scrollText("WiFi OFF", CRGB::Red, 5);
												 char stringTime[5];
												 sprintf_P(stringTime, PSTR("%u:%02u"), chronometer.hours, chronometer.minutes);
												 return scrollText(String(LOCAL_TIMEZONE_NAME) + stringTime, color, 5);
#endif
												 return false; //без интернета возвращаем
											 }

											 void osd_t::fillLetter(uint16_t letter, int16_t offset)
											 { // прорисовка заданного символа относительно его положения на экране
												 //if ((offset < 0) || ((offset - let_space) > WIDTH)) return;                  // если символ целиком находится вне матрицы - выход их процедуры
												 for (int16_t i = 0; i < LET_WIDTH; i++)
												 { // прорисовка столбцов единичного символа
													 int16_t realoffset = offset - i;
													 if ((realoffset >= 0) && (realoffset < WIDTH))
													 {																									// исполняем, если столбец находится в матрице
														 uint8_t thisByte = getFont(letter, (int8_t)(i)); // вычисление столбца
														 for (int16_t j = 0; j < LET_HEIGHT; j++)
														 { // цикл побитового вывода
															 bool thisBit = thisByte & (1 << (LET_HEIGHT - 1 - j));
															 if (thisBit)
															 {
																 CRGB color = osd_fontColor;
																 if (osd_shine)
																	 color.nscale8(random8(~osd_textShine, 255));
																 if (osd_flash)
																	 color.nscale8(osd_textFlash ? 255 : 63);
																 leds[getPixelNumber((WIDTH_d - realoffset), (TEXT_HEIGHT + j))] += color; // печатаем пиксель заданным цветом, если необходимо
															 }
														 }
													 }
												 }
											 }

											 bool osd_t::fillString()
											 { // прорисовка части строки относительно заданного смещения / TODO: проверить работу с русскими буквами
												 int16_t len = strlen(osd_string);
												 if (osd_scrollCounter > (let_space * len + WIDTH_d))
													 return true; // если вся строка напечатана возвращаем признак
												 for (int16_t i = 0; i < len; i++)
												 { // перечисление строки по символам
													 fillLetter(osd_string[i], osd_scrollCounter - i * let_space);
												 }
												 return false; // строка еще печатается
											 }

											 bool osd_t::tick() // тикер вывода текста
											 {
												 bool osd_isUpdate = false;
												 if (millis() - osd_gaugeTimer < BUTTON_GAUGE_TIMEOUT)
												 {
													 osd_gaugeStyles[osd_gaugeStyle](osd_gaugeValue);
													 osd_isUpdate = true;
												 }
												 if (!osd_scrollStage)
													 return osd_isUpdate; // текст полностью был показан - выход из функции
												 switch (osd_scrollStage)
												 {
												 case 1:
												 {
													 uint32_t osd_faderTime = millis() - osd_scrollTimer;
													 if (osd_faderTime >= osd_faderDuration)
													 { // наступил момент смещения текста на 1 пиксель
														 osd_scrollTimer = millis();
														 osd_scrollStage++;
														 osd_scrollCounter = 0;
														 osd_flashTimer = millis();
													 }
													 else
														 osd_faderStyles[osd_faderStageStyles[0][0]](osd_faderTime * 256 / osd_faderDuration);
													 break;
												 }
												 case 2:
													 if (millis() - osd_scrollTimer >= osd_scrollSpeed) // наступил момент смещения текста на 1 пиксель
													 {
														 osd_scrollTimer = millis();
														 osd_scrollCounter++;
													 }
													 if (millis() - osd_flashTimer >= osd_flashPeriod) // наступил момент смещения текста на 1 пиксель
													 {
														 osd_flashTimer = millis();
														 osd_textFlash = !osd_textFlash;
													 }
													 osd_faderStyles[osd_faderStageStyles[0][1]](255);
													 if (fillString())
													 {
														 osd_scrollStage++;
														 osd_scrollCounter = 0;
														 osd_scrollTimer = millis();
													 }
													 break;
												 case 3:
												 {
													 uint32_t osd_faderTime = millis() - osd_scrollTimer;
													 if (osd_faderTime >= osd_faderDuration)
													 { // наступил момент смещения текста на 1 пиксель
														 if (osd_mode & 0x80)
															 ESP.restart();
														 osd_scrollTimer = millis();
														 osd_scrollStage = 0;
														 osd_scrollCounter = 0;
														 osd_textShine = 0;
														 osd_string[0] = 0;
														 osd_mode = 0;
														 osd_flash = false;
														 osd_shine = false;
														 return false;
													 }
													 else
														 osd_faderStyles[osd_faderStageStyles[0][2]]((osd_faderDuration - osd_faderTime) * 256 / osd_faderDuration);
													 break;
												 }
												 }
												 return true;
											 }
											 * /
#endif
#endif
