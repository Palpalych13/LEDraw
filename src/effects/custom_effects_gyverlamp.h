#ifndef _custom_h
#define _custom_h
#include <stdint.h>
#include <pgmspace.h>
// ***** EFFECTS SETTINGS ***** last mod v1.7.07b

// ----- GRAPHIX LIBRARIES -----
#include "effects/LEDraw/LEDraw_2d.h"
#include "OSD.h"

// ----- EFFECTS LIBRARIES ----- last mod v1.7.13b
// Наборы эффектов. Для отключения достаточно закомментировать необходимую библиотеку.
// Списки эффектов при этом можно не редактировать. Эффекты отключаются автоматически!
// Любое изменение Наборов или Списков влечет переписывание их настроек на дефолтные!!!
#include "effects--System.h"
#include "effects--Classic.h"
#include "effects--ppMod.h"
#include "effects--SottNick.h"
#include "effects--Aurora.h"

// ----- EFFECTS ENUMERATION ----- last mod v1.7.13b
// Базовое имя эффекта - идентификатор. Максимум 255. Допускаются несуществующие имена/пересортица.
enum effects_id_e
{
#ifdef effectsClassic_h
	EFF_SPARKLES = (0U), // Конфетти
	EFF_FIRE,						 // Огонь
	EFF_WHITTE_FIRE,		 // Белый огонь
	EFF_RAINBOW_VER,		 // Радуга вертикальная
	EFF_RAINBOW_HOR,		 // Радуга горизонтальная
	EFF_RAINBOW_DIAG,		 // Радуга диагональная
	EFF_COLORS,					 // Смена цвета
	EFF_MADNESS,				 // Безумие 3D

	/*EFF_CLOUDS,          // Облака 3D
    EFF_LAVA,            // Лава 3D
    EFF_PLASMA,          // Плазма 3D
    EFF_RAINBOW,         // Радуга 3D
    EFF_SPECTRUM,  // Павлин 3D
    EFF_FOREST,          // Лес 3D
    EFF_OCEAN,           // Океан 3D*/
	EFF_NOISE3D, // Лава + Радуга + Облака...

	EFF_ZEBRA, // Зебра 3D
	EFF_COLOR, // Цвет
	EFF_SNOW,	 // Снегопад
	//EFF_SNOWSTORM,      // Метель
	EFF_STARFALL,				// Звездопад
	EFF_MATRIX,					// Матрица
	EFF_LIGHTERS,				// Светлячки
	EFF_LIGHTER_TRACES, // Светлячки со шлейфом
	EFF_PAINTBALL,			// Пейнтбол
	EFF_CUBE,						// Блуждающий кубик
#endif
#ifdef effects_ppMod_h
	EFF_PP_WATERFALL4IN1, // Водопад 4в1
	EFF_PP_RAINBOWCOMET,	// Радужная Комета
	EFF_PP_NOISESTREAMS,	// Шумные потоки 6в1
	EFF_PP_METABALLS,			// Меташарики
	EFF_PP_SINUSOID3,			// Синусоид 3
	EFF_PP_NEWFIRE,				// Новый огонь
	/*EFF_PP_MS1,
      EFF_PP_MS2,
      EFF_PP_MS3,
      EFF_PP_MS4,
      EFF_PP_MS5,
      EFF_PP_MS8,*/
	EFF_PP_PULSE, // Пульс
#endif
#ifdef effectsSottNick_h
	EFF_WATERPOOL,			 // Бассейн
	EFF_SHAMELESSMATRIX, // Нестыдная матрица
	EFF_RAIN,						 // Дождь
	EFF_COLOREDRAIN,
	EFF_SIMPLERAIN,
	EFF_STORMYRAIN,
	//EFF_COMBORAIN,
	EFF_FIRE2012,
	EFF_FIRE2018,
	EFF_SMOKE,
	EFF_CODELOCK,
	EFF_RUBIK,
	EFF_CLOCK,
#endif
#ifdef effectsAurora_h
	EFF_SPIRO,						// Спирали
	EFF_BOUNCINGBALLS,		// Прыгающие шарики
	EFF_PRISMATA,					// Призмата
	EFF_WAVES,						// Волны
	EFF_FLOCK_N_PREDATOR, // Стадо + Хищник
	EFF_SWIRL,						// Вихрь
	EFF_DRIFT,						// Дрифт
	EFF_DRIFT2,						// Дрифт 2
	EFF_RADAR,						// Радар
	EFF_SPIN,							// Спин
	EFF_FLOWFIELD,				// Каскад
	EFF_BOUNCE,						// Прыжки
	EFF_SIMPLEXNOISE,			// Одинокий шум
#endif
	EFFs_BLACKSCREEN,	 // Черный экран
	EFFs_SENDESCHLUSS, // Конец передачи
	EFFs_WHITE,				 // Белый свет
	EFFs_WHITE_STRIPE, // Светильник "Полоска"
	EFFs_CLOCK,				 // Часы
	EFFs_DEMO,				 // Демо
	EFFs_DEMO_RANDOM	 // Демо со случайным выбором
};

// ----- EFFECTS TABLE ----- last mod v1.7.16b
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
static eff_t effects_array[] = {
// const eff_t effects_array[] PROGMEM = {
#ifdef effectsClassic_h
		{EFF_SPARKLES, 1, 100, 1, 255, *[]() { return sparlkesRoutine(); }, 0, 160, 180, 100},
		{EFF_FIRE, 1, 75, 0, 255, *[]() { return fireRoutine(true); }, 1, 160, 180, 0},
		//{EFF_WHITTE_FIRE, 1, 100, 0, 0, *[]() { return fireRoutine(false, eff_scale); }, 0, BRIGHT, 75, 0},
		{EFF_RAINBOW_HOR, 0, 0, 1, 255, *[]() { return rainbowHorizontalRoutine(); }, 0, 92, 127, 63},
		{EFF_RAINBOW_VER, 0, 0, 1, 255, *[]() { return rainbowVerticalRoutine(); }, 0, 92, 127, 63},
		{EFF_RAINBOW_DIAG, 1, 100, 1, 100, *[]() { return rainbowDiagonalRoutine(); }, 0, 92, 50, 64},

		{EFF_MADNESS, 1, 100, 1, 100, *[]() { return madnessNoiseRoutine(); }, 0, BRIGHT, 10, 50}, //madnessNoiseRoutine

		//{EFF_CLOUDS, 1, 100, 1, 100, *[]() { return fillNoiseLED(CloudColors_p, 0); }, 0, BRIGHT, 1, 50},                 //cloudsNoiseRoutine
		//{EFF_LAVA, 1, 100, 1, 100, *[]() { return fillNoiseLED(LavaColors_p, 0); }, 0, 92, 1, 50},                        //lavaNoiseRoutine
		//{EFF_PLASMA, 1, 100, 1, 100, *[]() { return fillNoiseLED(PartyColors_p, 1); }, 0, 92, 1, 50},                     //plasmaNoiseRoutine
		//{EFF_RAINBOW, 1, 100, 1, 100, *[]() { return fillNoiseLED(RainbowColors_p, 1); }, 0, 92, 1, 50},                  //rainbowNoiseRoutine
		//{EFF_SPECTRUM, 1, 100, 1, 100, *[]() { return fillNoiseLED(RainbowStripeColors_p, 1); }, 0, BRIGHT, 1, 50}, //rainbowStripeNoiseRoutine
		//{EFF_FOREST, 1, 100, 1, 100, *[]() { return fillNoiseLED(ForestColors_p, 1); }, 0, 92, 5, 30},                    //forestNoiseRoutine
		//{EFF_OCEAN, 1, 100, 1, 100, *[]() { return fillNoiseLED(OceanColors_p, 0); }, 0, BRIGHT, 1, 85},                  //oceanNoiseRoutine

		//{EFF_NOISE3D, 1, 100, 0, 255, *[]() { return fillNoiseLED(); }, 0, BRIGHT, 1, 55},

		{EFF_ZEBRA, 50, 50, 0, 100, *[]() { return fillNoiseLED(&ZebraColors_p, true); }, 0, 50, 5, 18}, //zebraNoiseRoutine

		{EFF_COLORS, 0, 0, 0, 255, *[]() { return colorsRoutine(); }, 0, 96, 0, 180},
		{EFF_COLOR, 0, 0, 0, 255, *[]() { return colorRoutine(); }, 1, 92, 160, 9},
		{EFF_SNOW, 1, 100, 1, 100, *[]() { return snowRoutine(); }, 0, 192, 100, 100},
		{EFF_STARFALL, 1, 100, 0, 255, *[]() { return stormRoutine(); }, 0, 160, 100, 25},
		//{EFF_STARFALL, 1, 100, 1, 100, *[]() { return stormRoutine(255); }, 0, 160, 75, 100},
		{EFF_MATRIX, 1, 100, 1, 100, *[]() { return matrixRoutine(); }, 0, 160, 100, 25},
		{EFF_LIGHTERS, 1, 100, 1, 100, *[]() { return lightersRoutine(); }, 0, 192, 127, 100},
		{EFF_LIGHTER_TRACES, 1, 100, 1, 100, *[]() { return ballsRoutine(); }, 0, 192, 60, 100},
		{EFF_PAINTBALL, 0, 0, 1, 255, *[]() { return lightBallsRoutine(); }, 0, BRIGHT, 185, 1},
		{EFF_CUBE, 0, 75, 1, 255, *[]() { return ballRoutine(); }, 0, BRIGHT, 150, 50},
#endif
#ifdef effects_ppMod_h
		{EFF_PP_NEWFIRE, 1, 100, 0, 255, *[]() { return newFireRoutine(); }, 0, BRIGHT, 170, 0},
		{EFF_PP_RAINBOWCOMET, 0, 0, 0, 255, *[]() { return rainbowCometRoutine(); }, 2, BRIGHT, 255, 0},
		{EFF_PP_NOISESTREAMS, 1, 100, 1, 100, *[]() { return noiseStreamingRoutine(); }, 0, BRIGHT, 127, 233},
		//{EFF_PP_MS1, 1, 100, 1, 100, [](){return multipleStreamRoutine();}, 0, BRIGHT, 75, 8},
		//{EFF_PP_MS2, 1, 100, 1, 100, [](){return multipleStreamRoutine2();}, 0, BRIGHT, 75, 8},
		//{EFF_PP_MS3, 1, 100, 1, 100, [](){return multipleStreamRoutine3();}, 0, BRIGHT, 75, 8},
		//{EFF_PP_MS4, 1, 100, 1, 100, [](){return multipleStreamRoutine4();}, 0, BRIGHT, 75, 8},
		//{EFF_PP_MS5, 1, 100, 1, 100, [](){return multipleStreamRoutine5();}, 0, BRIGHT, 75, 8},
		//{EFF_PP_MS8, 1, 100, 1, 100, [](){return multipleStreamRoutine8();}, 0, BRIGHT, 75, 8},
		{EFF_PP_METABALLS, 25, 25, 0, palettes_range, *[]() { return metaBallsRoutine(); }, 0, 92, 90, 255},
		{EFF_PP_SINUSOID3, 0, 0, 0, 255, *[]() { return sinusoid3Routine(); }, 0, 96, 255, 28},
		{EFF_PP_PULSE, 1, 50, 1, 255, *[]() { return pulseRoutine(); }, 0, BRIGHT, 240, 255},
		{EFF_PP_WATERFALL4IN1, 1, 75, 1, 100, *[]() { return waterfall4in1Routine(); }, 0, 127, 45, 31},
#endif
#ifdef effectsSottNick_h
		{EFF_WATERPOOL, 1, 100, 0, 255, *[]() { return waterpoolRoutine(); }, 0, BRIGHT, 100, 160},
		{EFF_SHAMELESSMATRIX, 1, 100, 1, 255, *[]() { return shamelessMatrixRoutine(); }, 0, 160, 100, 255},
		{EFF_BOUNCINGBALLS, 1, 255, 1, 255, *[]() { return bouncingBallsRoutine(); }, 0, 192, 231, 239},
		{EFF_RAIN, 1, 100, 1, 100, *[]() { return rainRoutine(); }, 0, 192, 127, 127},
		{EFF_COLOREDRAIN, 1, 100, 0, 255, *[]() { return coloredRainRoutine(); }, 0, 192, 127, 160},
		{EFF_SIMPLERAIN, 1, 100, 0, 50, *[]() { return simpleRainRoutine(); }, 0, 192, 50, 25},
		{EFF_STORMYRAIN, 1, 100, 0, 50, *[]() { return stormyRainRoutine(); }, 0, 192, 50, 30},
		//{EFF_COMBORAIN, 1, 100, 0, 255, *[]() { return comboRainRoutine(); }, 0, 192, 127, 127},
		{EFF_FIRE2012, 1, 100, 0, 100, *[]() { return fire2012Routine(); }, 0, 140, 100, 96},
		{EFF_FIRE2018, 1, 100, 0, 255, *[]() { return fire2018Routine(); }, 0, 192, 186, 40},
		{EFF_SMOKE, 0, 255, 0, 255, *[]() { return smokeRoutine(); }, 0, 140, 176, 145},
		{EFF_CODELOCK, 1, 100, 0, 255, *[]() { return codelockRoutine(); }, 0, 140, 50, 7},
		{EFF_RUBIK, 1, 100, 0, 255, *[]() { return rubikRoutine(); }, 0, 140, 47, 241},
#endif
#ifdef effectsAurora_h
		{EFF_SPIRO, 0, 32, 0, palettes_range, *[]() { return spiroRoutine(); }, 0, BRIGHT, 180, 255},
		{EFF_PRISMATA, 0, 0, 0, palettes_range, *[]() { return prismataRoutine(); }, 0, 192, 63, 235},
		{EFF_WAVES, 0, 0, 0, palettes_range, *[]() { return wavesRoutine(); }, 0, 127, 127, 127},
		{EFF_FLOCK_N_PREDATOR, 0, 0, 0, palettes_range, *[]() { return flockRoutine(); }, 0, 192, 196, 237},
		{EFF_SWIRL, 0, 0, 0, palettes_range, *[]() { return swirlRoutine(); }, 0, 192, 160, 13},
		{EFF_DRIFT, 0, 0, 0, palettes_range, *[]() { return incrementalDriftRoutine(); }, 0, 192, 127, 127},
		{EFF_DRIFT2, 0, 0, 0, palettes_range, *[]() { return incrementalDrift2Routine(); }, 0, 192, 127, 68},
		{EFF_RADAR, 1, 100, 0, palettes_range, *[]() { return radarRoutine(); }, 0, 160, 80, 199},
		{EFF_SPIN, 1, 100, 0, palettes_range, *[]() { return spinRoutine(); }, 0, 192, 255, 135},
		{EFF_FLOWFIELD, 1, 100, 0, palettes_range, *[]() { return flowfieldRoutine(); }, 0, 192, 115, 176},
		{EFF_BOUNCE, 1, 100, 0, palettes_range, *[]() { return bounceRoutine(); }, 0, 160, 255, 73},
		{EFF_SIMPLEXNOISE, 0, 0, 0, palettes_range, *[]() { return simplexNoiseRoutine(); }, 0, 192, 163, 4},
#endif
		{EFFs_BLACKSCREEN, 0, 0, 0, 0, *[]() { return blackScreenRoutine(); }, 0, BRIGHT, 0, 0},
		{EFFs_SENDESCHLUSS, 1, 255, 1, 255, *[]() { return sendeSchlussRoutine(); }, 0, BRIGHT, 192, 127},
		{EFFs_WHITE_STRIPE, 1, 255, 1, 255, *[]() { return whiteColorStripeRoutine(); }, 0, BRIGHT, 127, 127},
		{EFFs_WHITE, 1, 255, 1, 255, *[]() { return whiteRoutine(); }, 0, BRIGHT, 127, 127},
		{EFFs_CLOCK, 0, 0, 0, 255, *[]() { return digiClockRoutine(); }, 0, 255, 0, 0},
		{EFFs_DEMO, 0, 0, 1, 255, *[]() { return demoRoutine(false); }, 0, 255, 0, 127},
		{EFFs_DEMO_RANDOM, 0, 0, 1, 255, *[]() { return demoRoutine(true); }, 0, 255, 0, 127}};

// ***** EFFECT LISTS *****
// базовые списки эффектов с их названиями. Допускаются названия на национальных языках (при условии наличия шрифтов).
// Возможны повторы и пересортица. Максимально = 256 эффектов в каждом списке
// названия в фаворитном списке, а так же первоначальный порядок, будет формироваться уже из этого

// ----- ON-LIST EFFECTS ----- last mod v1.7.14b
eff_item_t const effects_list_on[] PROGMEM = {
#ifdef effects_ppMod_h
		{EFF_PP_NEWFIRE, (char *)"NEW FIRE"},
#endif
#ifdef effectsSottNick_h
		{EFF_FIRE2012, (char *)"FIRE 2012"},
		{EFF_FIRE2018, (char *)"FIRE 2018"},
#endif
#ifdef effectsClassic_h
		//{EFF_FIRE, (char *)"FIRE"},
		{EFF_PP_WATERFALL4IN1, (char *)"WATERFALL 4in1"},
#endif
#ifdef effectsSottNick_h
		{EFF_CODELOCK, (char *)"CODELOCK"},
		{EFF_RUBIK, (char *)"RUBIK"},
		{EFF_SMOKE, (char *)"SMOKE"},
#endif
#ifdef effects_ppMod_h
		{EFF_PP_RAINBOWCOMET, (char *)"RAINBOW COMET"},
		{EFF_PP_METABALLS, (char *)"METABALLS"},
		{EFF_PP_SINUSOID3, (char *)"SINUSOID 3"},
		{EFF_PP_PULSE, (char *)"PULSE"},
		{EFF_PP_NOISESTREAMS, (char *)"NOISE STREAMS 6in1"},
//{EFF_PP_MS1,          (char *)"MULTIPLY STREAM 1"     },
//{EFF_PP_MS2,          (char *)"SPERMATOZOID 3D"       },
//{EFF_PP_MS3,          (char *)"MULTIPLY STREAM 3"     },
//{EFF_PP_MS4,          (char *)"MULTIPLY STREAM 4"     },
//{EFF_PP_MS5,          (char *)"MULTIPLY STREAM 5"     },
//{EFF_PP_MS8,          (char *)"MULTIPLY STREAM 8"     },
#endif
#ifdef effectsClassic_h
		{EFF_SPARKLES, (char *)"SPARKLES"},
		{EFF_LIGHTERS, (char *)"LIGHTERS"},
		{EFF_LIGHTER_TRACES, (char *)"LIGHTER TRACES"},
		{EFF_PAINTBALL, (char *)"PAINTBALL"},
#endif
#ifdef effectsAurora_h
		{EFF_SWIRL, (char *)"SWIRL"},
#endif
#ifdef effectsClassic_h
		{EFF_RAINBOW_HOR, (char *)"RAINBOW HORIZONTAL"},
		{EFF_RAINBOW_VER, (char *)"RAINBOW VERTICAL"},
		{EFF_RAINBOW_DIAG, (char *)"RAINBOW DIAGONAL"},
		{EFF_COLORS, (char *)"COLORS"},
		{EFF_COLOR, (char *)"COLOR"},
#endif
#ifdef effectsAurora_h
		{EFF_SIMPLEXNOISE, (char *)"SIMPLEX NOISE"},
#endif
#ifdef effectsClassic_h
		{EFF_MADNESS, (char *)"MADNESS"},
#endif
#ifdef effectsClassic_h
		{EFF_ZEBRA, (char *)"ZEBRA"},
#endif
#ifdef effectsAurora_h
		{EFF_FLOCK_N_PREDATOR, (char *)"FLOCK"},
		{EFF_FLOWFIELD, (char *)"FLOW FIELD"},
#endif
#ifdef effectsAurora_h
		{EFF_WAVES, (char *)"WAVES"},
		{EFF_RADAR, (char *)"RADAR"},
		{EFF_SPIRO, (char *)"SPIRO"},
		{EFF_DRIFT, (char *)"DRIFT"},
		{EFF_DRIFT2, (char *)"DRIFT 2"},
		{EFF_PRISMATA, (char *)"PRISMATA"},
		{EFF_SPIN, (char *)"SPIN"},
#endif
#ifdef effectsSottNick_h
		{EFF_COLOREDRAIN, (char *)"COLORED RAIN"},
		{EFF_SIMPLERAIN, (char *)"SIMPLE RAIN"},
		{EFF_STORMYRAIN, (char *)"STORMY RAIN"},
//{EFF_COMBORAIN, (char *)"RAIN 3D"},
#endif
#ifdef effectsClassic_h
		{EFF_SNOW, (char *)"SNOW"},
#endif
#ifdef effectsSottNick_h
		{EFF_RAIN, (char *)"RAIN"},
#endif
#ifdef effectsClassic_h
		//{EFF_SNOWSTORM, (char *)"SNOWSTORM"},
		{EFF_STARFALL, (char *)"STARFALL"},
		{EFF_MATRIX, (char *)"MATRIX"},
#endif
#ifdef effectsSottNick_h
		{EFF_SHAMELESSMATRIX, (char *)"MATRIX 2"},
#endif
#ifdef effectsClassic_h
		{EFF_CUBE, (char *)"CUBE"},
#endif
#ifdef effectsSottNick_h
//{EFF_WATERPOOL, (char *)"WATERPOOL"},
#endif
#ifdef effectsAurora_h
		{EFF_BOUNCE, (char *)"BOUNCE"},
#endif
#ifdef effectsSottNick_h
		{EFF_BOUNCINGBALLS, (char *)"BOUNCING BALLS"},
#endif
};

// ----- OFF-LIST EFFECTS ----- last mod v1.7.17b
static const eff_item_t effects_list_off[] PROGMEM = {
		{EFFs_SENDESCHLUSS, (char *)"SENDESCHLUSS"},
#if USE_NTP
		{EFFs_CLOCK, (char *)"CLOCK"},
#endif
		{EFFs_BLACKSCREEN, (char *)"BLACK SCREEN"}};

// ----- LAMP-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effects_list_lamp[] PROGMEM = {
		{EFFs_WHITE_STRIPE, (char *)"BED LAMP"},
		{EFFs_WHITE, (char *)"WHITE LAMP"},
		{EFF_WHITTE_FIRE, (char *)"WHITE FIRE"}};

// ----- DEMO-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effs_demo[] PROGMEM = {
		{EFFs_DEMO, (char *)"SERIAL DEMO"},
		{EFFs_DEMO_RANDOM, (char *)"RANDOM DEMO"}};

// ----- PAINT-LIST EFFECTS ----- last mod v1.7.14b
static const eff_item_t effects_list_paint[] PROGMEM = {
		{EFFs_BLACKSCREEN, (char *)"BLACK SCREEN"}};

// ----- ON-LIST EFFECTS CLASSIC ----- last mod v1.7.13b
/* 
eff_item_t const effects_list_on[] PROGMEM = {
#ifdef effectsClassic_h
    {EFF_SPARKLES, "SPARKLES"},
    {EFF_FIRE, "FIRE"},
    {EFF_PP_WATERFALL4IN1, "WATERFALL 4in1"},
    //{EFF_WHITTE_FIRE, "WHITE FIRE"},
    {EFF_RAINBOW_HOR, "RAINBOW HORIZONTAL"},
    {EFF_RAINBOW_VER, "RAINBOW VERTICAL"},
    {EFF_RAINBOW_DIAG, "RAINBOW DIAGONAL"},
    {EFF_COLORS, "COLORS"},
    {EFF_MADNESS, "MADNESS"},
    {EFF_CLOUDS, "CLOUDS"},
    {EFF_LAVA, "LAVA"},
    {EFF_PLASMA, "PLASMA"},
    {EFF_RAINBOW, "RAINBOW"},
    {EFF_SPECTRUM, "SPECTRUM"},
    {EFF_FOREST, "FOREST"},
    {EFF_OCEAN, "OCEAN"},
    {EFF_ZEBRA, "ZEBRA"},
    {EFF_COLOR, "COLOR"},
    {EFF_SNOW, "SNOW"},
    {EFF_SNOWSTORM, "SNOWSTORM"},
    {EFF_STARFALL, "STARFALL"},
    {EFF_MATRIX, "MATRIX"},
    {EFF_LIGHTERS, "LIGHTERS"},
    {EFF_LIGHTER_TRACES, "LIGHTER TRACES"},
    {EFF_PAINTBALL, "PAINTBALL"},
    {EFF_CUBE, "CUBE"},
#endif
#ifdef effectsSottNick_h
    {EFF_WATERPOOL, "WATERPOOL"},
#endif
#ifdef effects_ppMod_h
    {EFF_PP_PULSE, "PULSE"},
    {EFF_PP_RAINBOWCOMET, "RAINBOW COMET"},
    {EFF_PP_METABALLS, "METABALLS"},
#endif
#ifdef effectsSottNick_h
    {EFF_BOUNCINGBALLS, "BOUNCING BALLS"},
#endif
}; */

#endif