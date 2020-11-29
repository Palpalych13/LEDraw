#ifndef _custom_h
#define _custom_h
#include <stdint.h>
#include <pgmspace.h>
// ***** EFFECTS SETTINGS ***** last mod v1.7.21b

// ----- GRAPHIX LIBRARIES -----
#include "effects/LEDraw/LEDraw_2d.h"

// ----- EFFECTS ENUMERATION ----- last mod v1.7.13b
// Базовое имя эффекта - идентификатор. Максимум 255. Допускаются несуществующие имена/пересортица.
enum effects_id_e
{
	EFF_SPARKLES = (0U), // Конфетти
	EFF_COLORS,					 // Смена цвета
	EFF_COLOR,					 // Цвет
	EFF_RAINBOW,				 // Радуга горизонтальная
	EFF_STARDUST,				 // Пыль
	EFF_WAVEPAL,				 // 3 синусоиды
	EFF_PLASMA1D,				 // плазма 1d
	EFF_BLENDWAVE,			 // blendwave
	EFF_COLORWAVES,			 // colorwaves
	EFF_JUGGLEPAL,
	EFF_SERENDIPITOUSPAL,
	EFF_RAINBOWBEAT,
	EFF_BEATWAVEPAL,
	EFF_EASEPAL,
	EFF_NOISEFIREPAL,
	EFF_NOISEMOVERPAL,
	EFF_SINELONPAL,
	//EFF_SAWTOOTHPAL,

	EFFs_BLACKSCREEN,	 // Черный экран
	EFFs_SENDESCHLUSS, // Конец передачи
	EFFs_WHITE,				 // Белый свет
	EFFs_DEMO,				 // Демо
	EFFs_DEMO_RANDOM,	 // Демо со случайным выбором
	EFFs_DEMOGRADIENT,
};

// ***** EFFECT LISTS *****
// базовые списки эффектов с их названиями. Допускаются названия на национальных языках (при условии наличия шрифтов).
// Возможны повторы и пересортица. Максимально = 256 эффектов в каждом списке
// названия в фаворитном списке, а так же первоначальный порядок, будет формироваться уже из этого

// ----- ON-LIST EFFECTS ----- last mod v1.7.21b
eff_item_t const effects_list_on[] PROGMEM = {
		{EFF_SPARKLES, (char *)"SPARKLES"},
		{EFF_JUGGLEPAL, (char *)"JUGGLEPAL"},
		{EFF_RAINBOW, (char *)"RAINBOW"},
		{EFF_STARDUST, (char *)"STAR DUST"},
		{EFF_WAVEPAL, (char *)"WAVE PALETTE"},
		{EFF_PLASMA1D, (char *)"PLASMA 1D"},
		{EFF_BLENDWAVE, (char *)"BLEND WAVE"},
		{EFF_COLORWAVES, (char *)"COLOR WAVES"},
		{EFF_SERENDIPITOUSPAL, (char *)"SERENDIPITOUS"},
		{EFF_BEATWAVEPAL, (char *)"BEAT WAVE"},
		{EFF_RAINBOWBEAT, (char *)"RAINBOW BEAT"},
		{EFF_COLORS, (char *)"COLORS"},
		{EFF_COLOR, (char *)"COLOR"},
		{EFF_EASEPAL, (char *)"EASE"},
		{EFF_NOISEFIREPAL, (char *)"NOISE FIRE"},
		{EFF_NOISEMOVERPAL, (char *)"NOISE MOVER"},
		{EFF_SINELONPAL, (char *)"SINELON"},
		//{EFF_SAWTOOTHPAL, (char *)"SAW TOOTH"},
};

// ----- OFF-LIST EFFECTS ----- last mod v1.7.17b
static const eff_item_t effects_list_off[] PROGMEM = {
		{EFFs_SENDESCHLUSS, (char *)"SENDESCHLUSS"},
		{EFFs_BLACKSCREEN, (char *)"BLACK SCREEN"}};

// ----- LAMP-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effects_list_lamp[] PROGMEM = {
		{EFFs_WHITE, (char *)"WHITE LAMP"}};

// ----- DEMO-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effects_list_demo[] PROGMEM = {
		{EFFs_DEMO, (char *)"SERIAL DEMO"},
		{EFFs_DEMO_RANDOM, (char *)"RANDOM DEMO"},
		{EFFs_DEMOGRADIENT, (char *)"GRADIENT DEMO"},
};

// ----- PAINT-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effects_list_paint[] PROGMEM = {
		{EFFs_BLACKSCREEN, (char *)"BLACK SCREEN"}};
#endif

// ----- EFFECTS LIBRARIES ----- last mod v1.7.21b
// Наборы эффектов. Для отключения достаточно закомментировать необходимую библиотеку.
// Списки эффектов при этом можно не редактировать. Эффекты отключаются автоматически!
// Любое изменение Наборов или Списков влечет переписывание их настроек на дефолтные!!!
#include "effects--variables.h"
#include "effects--System.h"
#include "effects--1d.h"
#include "effects--garland1d.h"

// ----- EFFECTS TABLE ----- last mod v1.7.21b
// Список ВСЕХ задействованных эффектов с их первоначальными параметрами. Неиспользуемые эффекты можно закомментировать.
// Порядок не важен. Повторы возможны, а их параметры уже задаются индивидуально. Идентификатор должен совпадать с вышеописанным. Максимально = 256 эффектов
// вызов эффекта осуществляется в цикле прорисовки эффектов 1 раз за кадр. Возвращаемое значение типа bool - был ли прорисован кадр (true), или был пропуск (false - для плавности отрисовки на больших fps)
// параметры эффекта задаются передачей параметров eff_scale и eff_speed или любыми другими необхожимыми (например с цветом/буз цвета для эффектов со схожим алгоритмом)
// такая система легко позволяет организовать масштабируемость эффектов (несколько эффектов в одном, временное переключение эффектов...)
// Передача выбранного эффекта: effects_array[n]--> effect (переменная текущего эффекта, тип)
// Скорость эффекта effect.speed_min, effect.speed_max <-- диапазоны скоростей 0...255, или параметры используемыми классическими эффектами
// (speed_min > speed_max) <-- можно! переварачивает индикатор
// если (speed_min == speed_max) <-- отключает временной регулятор скорости (классический), и каждый раз вызывается процедура прорисовки кадра (для эффектов которые зависят от глобального таймера)
//       ^^^^^^^^^^^^^^^^^^^^^^ - если 0 == 0, то на индикатор передается eff_speed = 0...255; если n == n, то eff_speed = 1...n (защита от передачи 0)
/*	id									- идентификатор
		speed_min						- минимальная скорость -> // если speed_min = speed_max то передается диапазоон 1...255,
		speed_max						- максимальная скорость   // при этом если speed_min = 0, то передается 0...255
		scale_min						- минимальный масштаб
		scale_max						- максимальный масштаб
		action()						- вызываемая функция эффекта
		regulators_type 		- тип индикатора/регулятора
		bright_def 					- яркость эффекта по умолчанию (для мотора - эффект вращения) 0...255
		speed_def						- скорость эффекта по умолчанию
		scale_def						- масштаб эффекта по умолчанию 
*/
#if GUYVER_SPEED_REGULATOR
static const eff_t effects_array[] = {
		{EFF_SPARKLES, 1, 100, 1, 255, sparlkesRoutine, 0, motor_Random, 180, 100},
		{EFF_COLORS, 0, 0, 0, 255, colorsRoutine, 1, motor_ON, 0, 180},
		{EFF_COLOR, 0, 0, 0, 255, colorRoutine, 1, motor_ON, 160, 9},
		{EFF_RAINBOW, 0, 0, 1, 255, rainbowRoutine, 0, motor_Sinus, 127, 63},
		{EFF_STARDUST, 1, 255, 0, 255, starDustRoutine, 0, motor_RandomSmooth, 192, 127},
		{EFF_WAVEPAL, 255, 255, 0, 255, wavePaletteRoutine, 3, motor_RandomSmooth, 192, 127},
		{EFF_PLASMA1D, 0, 0, 0, 255, plasmaPaletteRoutine, 3, motor_Random, 192, 127},
		{EFF_BLENDWAVE, 0, 0, 0, 255, blendWavePaletteRoutine, 3, motor_Sinus, 192, 127},
		{EFF_COLORWAVES, 1, 255, 0, 255, colorWavesRoutine, 3, motor_Triangle, 192, 127},
		{EFF_JUGGLEPAL, 0, 0, 0, 255, jugglePaletteRoutine, 2, motor_RandomSmooth, 192, 127},
		{EFF_SERENDIPITOUSPAL, 0, 0, 0, 255, serendipitousPaletteRoutine, 3, motor_RandomSmooth, 192, 127},
		{EFF_RAINBOWBEAT, 1, 255, 0, 255, rainbowBeatRoutine, 2, motor_Saw, 192, 127},
		{EFF_BEATWAVEPAL, 0, 0, 0, 255, beatWavePaletteRoutine, 3, motor_RandomSmooth, 192, 127},
		{EFF_EASEPAL, 0, 0, 0, 255, easePaletteRoutine, 4, motor_RandomSmooth, 192, 127},
		{EFF_NOISEFIREPAL, 0, 0, 0, 255, noiseFirePaletteRoutine, 4, motor_RandomSmooth, 192, 127},
		{EFF_NOISEMOVERPAL, 0, 0, 0, 255, noiseMoverPaletteRoutine, 4, motor_RandomSmooth, 192, 127},
		{EFF_SINELONPAL, 0, 0, 0, 255, sinelonPaletteRoutine, 4, motor_RandomSmooth, 192, 127},
		//{EFF_SAWTOOTHPAL, 0, 0, 0, 255, sawtoothPaletteRoutine, 4, motor_RandomSmooth, 192, 127},

		{EFFs_BLACKSCREEN, 0, 0, 0, 0, blackScreenRoutine, 0, motor_OFF, 0, 0},
		{EFFs_SENDESCHLUSS, 1, 255, 1, 255, sendeSchlussRoutine, 0, motor_OFF, 192, 127},
		{EFFs_WHITE, 1, 255, 1, 255, whiteRoutine, 0, motor_OFF, 127, 127},
		{EFFs_DEMO, 0, 0, 0, 255, demoRoutine, 0, motor_OFF, 127, 127},
		{EFFs_DEMOGRADIENT, 0, 0, 0, 255, gradientsRoutine, 0, motor_OFF, 127, 127},
};
#else
static const eff_t effects_array[] = {
		{EFF_SPARKLES, sparlkesRoutine, 0, motor_Random, 180, 100},
		{EFF_COLORS, colorsRoutine, 1, motor_ON, 0, 180},
		{EFF_COLOR, colorRoutine, 1, motor_ON, 160, 9},
		{EFF_RAINBOW, rainbowRoutine, 0, motor_Sinus, 86, 63},
		{EFF_STARDUST, starDustRoutine, 0, motor_RandomSmooth, 192, 127},
		{EFF_WAVEPAL, wavePaletteRoutine, 3, motor_RandomSmooth, 192, 223},
		{EFF_PLASMA1D, plasmaPaletteRoutine, 3, motor_Random, 192, 127},
		{EFF_BLENDWAVE, blendWavePaletteRoutine, 3, motor_Sinus, 192, 127},
		{EFF_COLORWAVES, colorWavesRoutine, 3, motor_Triangle, 192, 127},
		{EFF_JUGGLEPAL, jugglePaletteRoutine, 2, motor_RandomSmooth, 192, 255},
		{EFF_SERENDIPITOUSPAL, serendipitousPaletteRoutine, 3, motor_RandomSmooth, 192, 127},
		{EFF_RAINBOWBEAT, rainbowBeatRoutine, 2, motor_Saw, 192, 127},
		{EFF_BEATWAVEPAL, beatWavePaletteRoutine, 3, motor_RandomSmooth, 192, 127},
		{EFF_EASEPAL, easePaletteRoutine, 4, motor_RandomSmooth, 224, 192},
		{EFF_NOISEFIREPAL, noiseFirePaletteRoutine, 4, motor_RandomSmooth, 182, 191},
		{EFF_NOISEMOVERPAL, noiseMoverPaletteRoutine, 4, motor_RandomSmooth, 192, 127},
		{EFF_SINELONPAL, sinelonPaletteRoutine, 4, motor_RandomSmooth, 192, 31},
		//{EFF_SAWTOOTHPAL, sawtoothPaletteRoutine, 4, motor_RandomSmooth, 192, 127},

		{EFFs_BLACKSCREEN, blackScreenRoutine, 0, motor_OFF, 0, 0},
		{EFFs_SENDESCHLUSS, sendeSchlussRoutine, 0, motor_OFF, 192, 127},
		{EFFs_WHITE, whiteRoutine, 0, motor_OFF, 127, 127},
		{EFFs_DEMO, demoRoutine, 0, motor_OFF, 64, 127},
		{EFFs_DEMO_RANDOM, demoRoutine, 0, motor_OFF, 64, 127},
		{EFFs_DEMOGRADIENT, gradientsRoutine, 0, motor_OFF, 64, 127},
};
#endif

