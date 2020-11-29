#ifndef _effects_h
#define _effects_h
#include <stdint.h>
#include <FastLED.h>
#include <effects/LEDraw/LEDraw_Math.h>
#include "effects/LEDraw/LEDraw_fader1d.h"
#include "effects/LEDraw/LEDraw_indicators1d.h"
#include <effects/LEDraw/LEDraw_1d.h>
#include <effects/LEDraw/LEDraw_Palettes.h>
#include <OSD.h>

// ----- EFFECT SECTION -----

inline void effect_t::setPalette(TProgmemRGBPalette16Ptr pal)
{
	rgb16_palette = pal;
	status &= ~0x20;
};

inline void effect_t::setPalette(TProgmemRGBGradientPalettePtr pal)
{
	g_palette = pal;
	status |= 0x20;
};

//inline void effect_t::setPalette(uint8_t mode) { palette = setPaletteByType(paletteSet[currentPaletteSet], mode); };
//inline void effect_t::setPalette(uint8_t mode, uint8_t of_paletteSet) { palette = setPaletteByType(paletteSet[of_paletteSet], mode); };
//inline CRGB effect_t::getPaletteColor(uint8_t index, uint8_t brightness = 255U) { return ::ColorFromPalette(palette, index, brightness, LEDRAW_BLENDTYPE); };
CRGB effect_t::ColorFromPalette(uint8_t index, uint8_t brightness = 255, TBlendType blendType = LEDRAW_BLENDTYPE)
{
	if (status & 0x20)
		return ::ColorFromPalette(*g_palette, index, brightness, blendType);
	return ::ColorFromPalette(*rgb16_palette, index, brightness, blendType);
};

void effect_t::fill_palette(uint16_t N, uint8_t startIndex, uint8_t incIndex, uint8_t brightness = 255, TBlendType blendType = LEDRAW_BLENDTYPE)
{
	if (status & 0x20)
		::fill_palette(leds, N, startIndex, incIndex, *g_palette, brightness, blendType);
	else
		::fill_palette(leds, N, startIndex, incIndex, *rgb16_palette, brightness, blendType);
};

void effect_t::initBackupVariables(uint8_t lenArray)
{
	indexes.resize(lenArray);
	colors.resize(lenArray);
};

bool effect_t::set(uint8_t to_effect)
{
	initBackupVariables(0);
	timer.setInterval(20); // 50 FPS
	leds = realleds;
	current = &effects_array[to_effect];
	rgb16_palette = palettes_fastled[0];
#if USE_MOTOR
	motor.setAction(current->bright_def);
	power.bright_def = 255U;
#else
	power.bright_def = current->bright_def;
#endif
	power.correctBrightness();
	power.bright_global = flash.readGlobalBrightness(power.mode);
	power.setBrightness(flash.readPref(pref_Brightness, to_effect));
	power.bright_real = power.Bright();
	power.bright_fader = 0;
#if GUYVER_SPEED_REGULATOR
	speed_min = current->speed_min;
	speed_max = current->speed_max;
	if (speed_min == speed_max)
	{
		status != 0x40;
		if (!speed_max)
			speed_max = 255U;
		else
			speed_min = 1U;
	}
#endif
	setSpeed(flash.readPref(pref_Speed, to_effect));
	setScale(flash.readPref(pref_Scale, to_effect));
	speedRegulator = effects.getSpeedRegulator(current->regulators_type);
	scaleRegulator = effects.getScaleRegulator(current->regulators_type);
	status |= 0x1F;
	return true;
};

bool effect_t::hasChangedParams()
{
	if (status & 0x10)
	{
		status &= (uint8_t)~0x10U;
		return true;
	}
	return false;
};

inline bool effect_t::Undelayed() { return status & 0x40U; };
inline bool effect_t::Unsleeped() { return status & 0x80U; };

bool effect_t::hasChangedSpeed()
{
	if (status & 0x4)
	{
		status &= (uint8_t)~0x4U;
		return true;
	}
	return false;
};

bool effect_t::hasChangedScale()
{
	if (status & 0x8)
	{
		status &= (uint8_t)~0x8U;
		return true;
	}
	return false;
}

bool effect_t::hasChangedEffect()
{
	if (status & 0x1)
	{
		status &= ~0x1;
		return true;
	}
	return false;
};
#if GUYVER_SPEED_REGULATOR

bool effect_t::isNewFrame()
{
	if (~status & 0x40)
	{
		EVERY_N_MILLIS(speed_delay)
		{
			effects.setUpdate(false);
			return false;
		};
	}
	frame_counter++;
	effects.setUpdate(true);
	return true;
};

uint16_t effect_t::Speed() // Скорость эффекта с учетом текущего состояния
{
	return (speed_min > speed_max) ? map8(brighten8_raw(speed), speed_max, speed_min) : map8(brighten8_raw(speed), speed_min, speed_max);
};

uint8_t effect_t::Scale() // Масштаб эффекта с учетом текущего состояния
{
	return (current->scale_min > current->scale_max) ? map8(scale, current->scale_max, current->scale_min) : map8(scale, current->scale_min, current->scale_max);
};
#endif

uint8_t effect_t::setScale(uint8_t value) // устанавливает полноразмерное значение масштаба и подготавливает преобразованное
{
	scale = value;
	status |= 0x18;
#if GUYVER_SPEED_REGULATOR
	scale_real = Scale();
	return scale_real;
#endif
	return scale;
};

uint8_t effect_t::setSpeed(uint8_t value) // устанавливает полноразмерное значение скорости и подготавливает преобразованное
{
	speed = value;
	status |= 0x14;
#if GUYVER_SPEED_REGULATOR
	speed_real = Speed();
	speed_delay = ((speed_min > speed_max) ? speed_min - speed_real : speed_max - speed_real) + FASTLED_DITHER_TIME;
	//LOG.printf_P(PSTR("speed: %u <- speed: %u\n"), speed, speed);
	return speed_real;
#endif
	return speed;
};

#if GUYVER_SPEED_REGULATOR
uint8_t effect_t::setRealSpeed(uint8_t value) // восстанавливает полноразмерное значение скорости из преобразованного
{
	if (speed_min > speed_max)
		speed = 255U - sqrtf(((float)(~value) - speed_max) / ((float)speed_min - speed_max)) * 255.0F;
	else
		speed = 255U - sqrtf(((float)(~value) - speed_min) / ((float)speed_max - speed_min)) * 255.0F;
	//LOG.printf_P(PSTR("speed: %u -> speed: %u\n"), speed, speed);
	//LOG.printf_P(PSTR("speed_min: %u # speed_max: %u\n"), speed_min, speed_max);
	speed_real = value;
	speed_delay = ((speed_min > speed_max) ? speed_min - value : speed_max - value) + FASTLED_DITHER_TIME;
	status |= 0x14;
	return speed;
};
#endif
uint16_t effect_t::correctPhase(accum88 in, accum88 out)
{
	speed_phase += beat16(in) - beat16(out);
	return speed_phase;
};

inline uint8_t effect_t::getSpeedMode() { return speedRegulator->mode(speed); };								//
inline uint8_t effect_t::getSpeedValue() { return speedRegulator->value(speed); };							//
inline uint8_t effect_t::getScaleMode() { return scaleRegulator->mode(scale); };								//
inline uint8_t effect_t::getScaleValue() { return scaleRegulator->value(scale); };							//
inline uint8_t effect_t::getSpeedMode(uint8_t value) { return speedRegulator->mode(value); };		//
inline uint8_t effect_t::getSpeedValue(uint8_t value) { return speedRegulator->value(value); }; //
inline uint8_t effect_t::getScaleMode(uint8_t value) { return scaleRegulator->mode(value); };		//
inline uint8_t effect_t::getScaleValue(uint8_t value) { return scaleRegulator->value(value); }; //

// ----- EFFECTS SECTION -----
void effects_t::tick() // процедура тикера эффектов
{
	if (power.isScreenON())
	{
		setUpdate(false);
#if GUYVER_SPEED_REGULATOR
		if (effect.isNewFrame()) // основной цикл прорисовки эффекта (времянезависимый)
#endif
			setUpdate(effect.action());
		fadeBrightness();
		if (isUpdated()) // сохранение сформированной картинки эффекта в буффер (для мелденных или зависящих от предыдущей)
			memcpy(ledsbuff, leds, BUFFER_SIZE);

		if (fader.tick())
		{
#if USE_MOTOR
			motor.stop(fader.current);
#endif
			setUpdate(true);
		};

#if USE_OSD2
		if (osd.tick())
			setUpdate(true);
#endif

#if (LEDSARRAY_TYPE == MATRIX)
		if (power.dimmer_leds)
		{
			isUpdated = true;
			leds.nscale8_video(~power.dimmer_leds);
			if (power.status & 0x10)
			{
				for (x = 0; x < WIDTH; x++)
				{
					for (y = 0; y < HEIGHT; y++)
					{
						if (power.status & 0x80)
							drawPixel(x, y, CRGB::Black);
						power.status ^= 0x80;
					}
#if (!HEIGHT_odd)
					power.status ^= 0x80;
#endif
				}
#if (!WIDTH_odd)
				power.status ^= 0x80;
#endif
			}
		}
		isUpdated |= osd.tick(); // прорисовка текста поверх экрана (для каждого выводимого кадра)
#endif
#if !FASTLED_DITHERING_OVERSCAN
		if (isUpdated())
#endif
			FastLED.show();
		if (isUpdated())
			memcpy(leds, ledsbuff, BUFFER_SIZE); // восстановление кадра с прорисованным эффектом из буфера (без текста и индикаторов)
	};
};

// ----- EFFECTS SECTION -----

regulator_t *effects_t::getRegulator(reg_type_t reg) { return &regulators[regulators_set[effect.current->regulators_type][reg]]; };
regulator_t *effects_t::getSpeedRegulator(uint8_t reg) { return &regulators[regulators_set[reg][reg_Speed]]; };
regulator_t *effects_t::getScaleRegulator(uint8_t reg) { return &regulators[regulators_set[reg][reg_Scale]]; };

eff_t effects_t::get(uint8_t index)
{
	eff_t e;
	readAnything_P(&effects_array[index], e);
	return e;
};

uint8_t effects_t::getIndexByID(uint8_t id) // <-- получить id эффекта из eff_arr по его номеру в списке заданного режима
{
	for (uint8_t i = 0; i < effs_count; i++)
		if (get(i).id == id)
			return i;
	return 0;
};

eff_item_t effects_t::getListItem(uint8_t index, uint8_t of_mode)
{
	eff_item_t record;
	readAnything_P(&effects_lists[of_mode].list[index], record);
	return record;
};

uint8_t effects_t::getListSize(uint8_t of_mode)
{
	return effects_lists[of_mode].size;
};

eff_item_t effects_t::getListItemByID(uint8_t id, uint8_t of_mode)
{
	eff_item_t record;
	uint8_t size = getListSize(of_mode);
	for (uint8_t i = 0; i < size; i++)
	{
		readAnything_P(&effects_lists[of_mode].list[i], record);
		if (record.id == id)
			return record;
	}
	return record;
};

uint8_t effects_t::getIndexByList(uint8_t index, uint8_t of_mode) // <-- получить id эффекта из eff_arr по его номеру в списке заданного режима
{
	return getIndexByID(getListItem(index, of_mode).id);
};

void effects_t::change(uint8_t to_effect, uint8_t duration05s = FADER_DURATION, uint8_t fader_effect = fade_OnlyDimming, bool reverse = false) // плавное выключение экрана (по номеру эффекта в eff_arr)
{
	this->to_effect = to_effect;
	fader.set(duration05s, 0, fader_effect, reverse, 0, 0, leds);
	fader.fade_event = fader_tick_action;
	fader.stop_event = []() { effect.set(effects.to_effect);
														clearAll(leds);
														clearAll(ledsbuff); };

#if USE_DEBUG
	//LOG.printf("current %u   fstatus %X   status %X\n", fader.current, fader.status, status);
	LOG.printf_P(log_eff_fadeChange, effects.getListItemByID(effects.get(to_effect).id, power.mode).name, to_effect); //"Effect: Выбран эффект %u\n"
#endif
};

void effects_t::fadeout(uint8_t duration025s = FADER_DURATION) // плавное выключение экрана
{
	fader.set(duration025s, 0);
	fader.fade_event = fader_tick_action;
	fader.stop_event = []() { effects.fader.reset();
#if USE_OSD2
														effects.osd.reset();
#endif
														power.screenOFF(true); };
#if USE_DEBUG
	//LOG.printf("current %u   fstatus %X   status %X\n", fader.current, fader.status, status);
	LOG.print(FPSTR(log_eff_fadeout)); // "Effect: Плавное закрытие эффекта\n"
#endif
};

void effects_t::fadein(uint8_t to_effect, uint8_t duration025s = FADER_DURATION) // плавное включение экрана (по номеру эффекта в eff_arr)
{
	this->to_effect = to_effect;
	effect.set(to_effect);
	fader.set(duration025s, 255);
	fader.fade_event = fader_tick_action;
#if USE_DEBUG
	//LOG.printf("current %u   fstatus %X   status %X\n", fader.current, fader.status, status);
	LOG.printf_P(log_eff_fadein, to_effect); // "Effect: Плавное открытие эффекта %u\n"
#endif
};

uint8_t effects_t::next(uint8_t of_mode, uint8_t fader_effect)
{
	uint8_t currEffect = flash.readCurrentEffect(of_mode);

	//	if (fader.active())
	//		return currEffect;
	currEffect++;
	if (currEffect >= effects.getListSize(of_mode))
		currEffect = 0U;

#if USE_OSD2
	osd.set(12, currEffect, 0, false, 0, 20, effect.leds); //
	osd.fade_event = osd_tick_action;
#endif

#if USE_DEBUG
	LOG.printf_P(log_eff_next, effects.getListItem(currEffect, of_mode).name); // "Effect:\tВыбран следующий эффект: %s\n"
#endif
	flash.writeCurrentEffect(currEffect, of_mode);
	change(flash.getCurrentEffect(of_mode), 8, fader_effect, false);
	return currEffect;
};

uint8_t effects_t::next(uint8_t of_mode)
{
	return next(of_mode, fade_OnlyDimming);
};

uint8_t effects_t::previous(uint8_t of_mode, uint8_t fader_effect)
{
	uint8_t currEffect = flash.readCurrentEffect(of_mode);
	//	if (fader.active())
	//		return currEffect;
	uint8_t cnt = effects.getListSize(of_mode) - 1U;
	currEffect--;
	if (currEffect > cnt)
		currEffect = cnt;

#if USE_OSD2
	osd.set(12, currEffect, 0, false, 0, 20, effect.leds);
	osd.fade_event = osd_tick_action;
#endif
#if USE_DEBUG
	LOG.printf_P(log_eff_prev, effects.getListItem(currEffect, of_mode).name); // "Effect:\tВыбран предыдущий эффект: %s\n"
#endif
	flash.writeCurrentEffect(currEffect, of_mode);
	change(flash.getCurrentEffect(of_mode), 8, fader_effect, true);
	return currEffect;
};

uint8_t effects_t::previous(uint8_t of_mode)
{
	return previous(of_mode, fade_OnlyDimming);
};

void effects_t::fadeBrightness()
{
	if (power.bright_reg != power.bright_fader) // Brightness fader
	{
		EVERY_N_MILLIS(BRIGHT_STEPTIME) // if (millis() - brightTS32 >= BRIGHT_STEPTIME)
		{
			(power.bright_fader < power.bright_reg) ? power.bright_fader++ : power.bright_fader--;
			//brightTS32 = millis();
			FastLED.setBrightness(power.bright_fader);
		}
	}
};

void fader_t::reset()
{
	current = 0;
	status = 0;
	fade_event = NULL;
	start_event = NULL;
	stop_event = NULL;
	finish_event = NULL;
};

void fader_t::set(uint8_t duration025s, uint8_t init = 0)
{
	//LOG.printf("SET INIT current %u   fstatus %X   init %u\n", current, status, init);
	_value = 0;
	_effect = 0;
	if (status & 0x40)
	{
		stamp = 0;
		faderTS32 = millis();
	};
	if (done())
	{
		reset();
		setReverse(false);
		_release = 0;
		_duration = duration025s;
		_hold = 0;
		current = init;
		status |= 0x21;
		leds = realleds;
	};
};

void fader_t::set(uint8_t duration025s, uint8_t value, uint8_t effect, bool reverse, uint8_t hold025s = 0, uint8_t release025s = 0, CRGB *LEDarray = realleds)
{
	//LOG.printf("SET current %u   fstatus %X\n", current, status);
	_value = value;
	_effect = effect;
	if (status & 0x40)
	{
		stamp = 0;
		faderTS32 = millis() + (uint16_t)release025s * 256;
	};
	if (done())
	{
		faderTS32 = millis();
		reset();
		setReverse(reverse);
		_release = release025s;
		_duration = duration025s;
		_hold = hold025s;
		if (hold025s)
		{
			faderTS32 += (uint16_t)hold025s * 256;
			status |= 0x20;
		}
		status |= 0x01;
		leds = LEDarray;
	};
};

uint8_t fader_t::ifRandom(uint8_t effect)
{
	return (effect == fade_Random) ? random8(fade_Random) : effect;
};

bool fader_t::tick()
{
	if (done())
		return false;

	if (status & 0x20) // start event is active
	{
		if (_hold != 255) // if timer has activated
		{
			if (millis() > faderTS32)
			{
				//LOG.printf("DEBUG FADER:\t(%u) start event is active current %u, status %X, _hold %u, event=%u\n", _value, current, status, _hold, (start_event != NULL));
				if (start_event != NULL)
					start_event();
				faderTS32 = millis();
				stamp = 0;
				status &= ~0x20;
			}
		}
	}
	else if (status & 0x40) // stop event is active
	{
		if (_release != 255) // if timer has activated
		{
			if (millis() > faderTS32)
			{
				//LOG.printf("DEBUG FADER:\t(%u) stop event is active current %u, status %X, _release %u, event=%u\n", _value, current, status, _release, (stop_event != NULL));
				if (stop_event != NULL)
					stop_event();
				faderTS32 = millis();
				stamp = 0;
				status &= ~0x41; // reset stop flag and change direction
			}
		}
	}
	else if (_duration)
	{
		stamp += (millis() - faderTS32);
		faderTS32 = millis();
		while (stamp >= _duration)
		{
			if (direction())
			{
				if (current == 255U)
				{
					status |= 0x40;
					faderTS32 += (uint16_t)_release * 256;
				}
				else
					current++;
			}
			else
			{
				current--;
				if (!(current)) // finish event is active
				{
					//LOG.printf("DEBUG FADER:\t(%u) goto finish, _duration %u, stamp %u\n", _value, _duration, stamp);
					status |= 0x10;
				}
			}
			stamp -= _duration;
			//LOG.printf("DEBUG FADER:\t(%u) fuck!!!!, current %u, status %X, _duration %u, stamp %u\n", _value, current, status, _duration, stamp);
		}
	}
	else
	{
		if (direction())
		{
			current = 255U;
			status |= 0x40;
			faderTS32 = millis() + (uint16_t)_release * 256;
		}
		else // finish event is active
		{
			current = 0;
			status |= 0x10;
		}
	}

	if (fade_event != NULL)
		fade_event();

	if (status & 0x10)
	{
		//LOG.printf("DEBUG FADER:\t(%u) finish event is active current %u, status %X, event=%u\n", _value, current, status, (finish_event != NULL));
		if (finish_event != NULL)
			finish_event();
		reset();
	}

	return true;
};

#endif