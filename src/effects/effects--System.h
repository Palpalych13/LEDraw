#ifndef effects_System_h
#define effects_System_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_Palettes.h"
#include "effects/LEDraw/LEDraw_fader1d.h"
#include "effects/LEDraw/LEDraw_indicators1d.h"
#include "effects/LEDraw/LEDraw_1d.h"
#include "effects/LEDraw/LEDraw_Math.h"
#include "power.h"

// ============= System Effects for Gyver Lamp / Системные эффекты для лампы Гайвера ===============

// ***** SENDESCHLUSS / КОНЕЦ ПЕРЕДАЧИ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 11.03.2020
bool sendeSchlussRoutine(effect_t &e)
{
	noiseAll(16, 192);
	return true;
}

// ***** BLACK SCREEN / ЧЕРНЫЙ ЭКРАН *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 11.03.2020
// v1.1 - Adapting for Paintboard Effect by PalPalych 07.07.2020

#define e_black_DITHER (8U) // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
bool blackScreenRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
		clearAll();

#if (e_black_DITHER)
	if (e.frame_counter % e_black_DITHER)
		return false;
#endif
	dimAll(e.speed >> 2U);
	blurAll(e.scale >> 3U);
	return true;
}

// ***** WHITE LIGHT / БЕЛЫЙ СВЕТ *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020

#define e_whites_DITHER (4U) // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
bool whiteRoutine(effect_t &e)
{
	fillAll(CHSV(0U, 0U, 255U));
	return true;
}

// ***** WHITE STRIPE / БЕЛАЯ ПОЛОСА *****
// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.1 - Fasl Logic by PalPalych 31.03.2020
// (светится горизонтальная полоса по центру лампы; масштаб - высота центральной горизонтальной полосы;
// скорость - регулировка от холодного к тёплому; яркость - общая яркость)

#if (LEDSARRAY_TYPE == MATRIX)
#define e_whites_DITHER (4U) // разрешить дисперсию (0 - отключить; 2... - каждый n-кадр будет прорисовываться заново)
bool whiteColorStripeRoutine(effect_t &e)
{
#if e_whites_DITHER
	if (e.frame_counter % e_whites_DITHER)
		return false;
#endif
	fillAll(CRGB::Black);
	uint8_t delta = map8((constrain(e.scale, 128, 255) - 128) * 2, 0, SEMIHEIGHT - 1U);
	drawFillRect2_fast(0, WIDTH_d, SEMIHEIGHT_d - delta, SEMIHEIGHT + delta + HEIGHT_odd, CHSV(45U, map8(e.speed, 0U, 170U), constrain(e.scale, 0, 127) + 128));
	return true;
}
#endif

extern const effects_list_t effects_lists[]; // прототип
static uint8_t demo_index;
static effect_t demo_effect;

#define e_timer e.A.u8a
#define e_index e.A.u8b
#define e_id e.A.u8c
#define e_listindex e.A.u8d

// ***** SHOW GRADIENT ROUTINE / ПОКАЗ ГРАДИЕНТОВ *****
bool gradientsRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		e.timer.setInterval(10000);
		e.setPalette(palettes_fastled[2]);
		fillRange(START_LED, NUM_LEDS_d, e.rgb16_palette, 255, LINEARBLEND);
		return true;
	};

	if (e.hasChangedSpeed())
	{
		e_timer = map8(e.speed, 10, 120);
		e.timer.setInterval(e_timer * 1000);
		LOG.printf("DEMO: изменение интервала времени %u секунд\n", e_timer);
#if USE_OSD2
		effects.osd.set(0, e_timer, 0, false, 0, 8, effect.leds);
		effects.osd.fade_event = osd_tick_action;
#endif
	}

	if (e.timer.isReady())
	{
		static uint8_t pal;
		LOG.printf("DEMO: смена градиента %u из %u   \n", pal, sizeof(gradientPalettes) / sizeof(TProgmemRGBGradientPalettePtr));
		fillRange(START_LED, NUM_LEDS_d, e.g_palette, 255, LINEARBLEND);
		e.setPalette(gradientPalettes[pal]);
		fillRange(START_LED, NUM_LEDS_d, e.g_palette, 255, LINEARBLEND);
#if USE_OSD2
		effects.osd.set(8, pal, 0, false, 8, 12, effect.leds);
		effects.osd.fade_event = osd_tick_action;
#endif
		pal++;
		if (pal >= sizeof(gradientPalettes) / sizeof(TProgmemRGBGradientPalettePtr))
			pal = 0;
		return true;
	};
	return false;
};

// ***** DEMO / ДЕМО *****
// v1.0 - Demo effect for GuverLamp v1.7 by PalPalych 31.03.2020
// v2.0 - Demo effect for GuverLamp v1.7 by PalPalych 18.11.2020

bool demoRoutine(effect_t &e)
{
	if (e.hasChangedEffect())
	{
		LOG.printf("DEMO: открытие эффекта Демо %u\n", 0);
		demo_effect.set(0);
		e_listindex = 0;
	}

	if (e.hasChangedSpeed())
	{
		e_timer = map8(e.speed, 10, 120);
		e.timer.setInterval(e_timer * 1000);
		LOG.printf("DEMO: изменение интервала времени %u секунд\n", e_timer);
#if USE_OSD2
		effects.fader.start_event = []() { effects.osd.reset(); };
		effects.osd.set(0, e_timer, 0, false, 0, 8, effect.leds);
		effects.osd.fade_event = osd_tick_action;
#endif
	}

	if (e.timer.isReady())
	{
		effects_list_t const &item = effects_lists[pwr_ON];
		eff_item_t const &list = *item.list;
		if (e.current->id == EFFs_DEMO_RANDOM)
		{
			e_listindex = random8(item.size);
		}
		else
		{
			e_listindex++;
			if (e_listindex >= item.size)
				e_listindex = 0;
		}
		//LOG.printf("DEMO: &item.size %u, item.size %u секунд\n", pgm_read_byte(&item.size), item.size);

		e_id = pgm_read_byte(&item.list[e_listindex].id);
		demo_index = effects.getIndexByID(e_id);

		LOG.printf("DEMO: смена эффекта %s(%u/%u), прошло %u секунд\n", effects.getListItem(e_listindex, pwr_ON).name, e_listindex, demo_index, e_timer);

		effects.fader.set(FADER_DEMO_DURATION, 0, effects.fader.ifRandom(fade_Random), random8(2), 0, 0, demo_effect.leds);
		effects.fader.fade_event = fader_tick_action;
		effects.fader.stop_event = []() {
						demo_effect.set(demo_index);
						clearAll(leds);
						clearAll(ledsbuff); };
#if USE_OSD2
		//effects.fader.start_event = []() { effects.osd.reset(); };
		effects.osd.set(8, e_listindex, 0, false, 12, 8, effect.leds);
		effects.osd.fade_event = osd_tick_action;
#endif
	};
	return demo_effect.action();
};

	// ***** WARNING / МИГАЮЩИЙ СВЕТ *****
	// v1.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
	// мигающий цвет (не эффект! используется для отображения краткосрочного предупреждения; блокирующий код!)

#define WARNING_BRIGHT (10U) // яркость вспышки

void showWarning(
		CRGB color,								/* цвет вспышки                                                 */
		uint32_t duration,				/* продолжительность отображения предупреждения (общее время)   */
		uint16_t blinkHalfPeriod) /* продолжительность одной вспышки в миллисекундах (полупериод) */
{
	uint32_t blinkTimer = millis();
	enum BlinkState
	{
		OFF = 0,
		ON = 1
	} blinkState = BlinkState::OFF;
	FastLED.setBrightness(WARNING_BRIGHT); // установка яркости для предупреждения
	fillAll(CRGB::Black);
	delay(2);
	FastLED.show();

	fillAll(color); // установка цвета всех диодов в WARNING_COLOR
#if INDICATORS
	indicators.generalBulb->setON(color);
#endif
	uint32_t startTime = millis();
	while (millis() - startTime <= (duration + 5)) // блокировка дальнейшего выполнения циклом на время отображения предупреждения
	{
		if (millis() - blinkTimer >= blinkHalfPeriod) // переключение вспышка/темнота
		{
			blinkTimer = millis();
			blinkState = (BlinkState)!blinkState;
			FastLED.setBrightness(blinkState == BlinkState::OFF ? 0 : WARNING_BRIGHT);
			delay(1);
			FastLED.show();
		}
		delay(50);
	}

	fillAll(CRGB::Black);
	//FastLED.setBrightness(effects[eff_on_current].Bright());  // установка яркости, которая была выставлена до вызова предупреждения
	delay(1);
	FastLED.show();
}
#endif
