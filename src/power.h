#include <stdint.h>
#ifndef _power_h
#define _power_h

//static uint8_t power.mode;

//extern uint8_t effects.getIndexByID(uint8_t, pmode_t);
//extern bool effect_dimmerDone();

// ----- POWER SECTION -----

template <typename T>
void readAnything_P(const T *sce, T &dest)
{
	memcpy_P(&dest, sce, sizeof(T));
}

template <typename T>
T getAnything_P(const T *sce)
{
	static T temp;
	memcpy_P(&temp, sce, sizeof(T));
	return temp;
}

pwr_bright_t power_getBright(uint8_t mode = power.mode)
{
	pwr_bright_t bri;
	readAnything_P(&pwr_bright[mode], bri);
	return bri;
}

void power_t::forceToSleep() // <-- включить тайтер перехода в спящий режим
{
	status |= 0x02;
	powerTS32 = millis();
}

static void fillAll(CRGB, CRGB *);

void power_t::screenOFF(bool withMOSFET) // <-- процедура отключения экрана, питания матрицы и ее обновления
{
	power.mode = pwr_SLEEP;
	bright_real = 0;
	bright_fader = 0;
	fillAll(CRGB::Black, leds);
	fillAll(CRGB::Black, ledsbuff);
	FastLED.setBrightness(0);
	delay(10);
	FastLED.show();
#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL) // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
	digitalWrite(MOSFET_PIN, !MOSFET_LEVEL);
#endif
#if USE_DEBUG
	LOG.printf_P(log_power_screenoff);
#endif
	status &= ~0x01;
	//isScreenON = false;
}

void power_t::screenON(bool withMOSFET) // <-- процедура включения экрана, питания матрицы и ее обновления
{
	status |= 0x01;
	//isScreenON = true;
#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL) // <-- установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
	digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
#endif
#if USE_DEBUG
	LOG.printf_P(log_power_screenon);
#endif
}

void power_t::change(pmode_t to_mode, uint8_t eff_id = 255) // <-- изменить режим питания со своим набором эффектов, и указанным эффектом из списка режима (effect == 255 <-- текущий эффект)
{
	if (mode == to_mode)
		return;
	mode = to_mode;
	status &= ~0x02;
	//isForcedToSleep = false;
#if USE_MOTOR
	motor.speed = flash.readGlobalMotorSpeed(power.mode);
#endif
	EEPROM.commit();
#if USE_DEBUG
	LOG.printf_P(log_power_change, enum_pwr_mode[to_mode]);
#endif

	switch (to_mode)
	{
	case pwr_SLEEP:
		forceToSleep();
		effects.fadeout(FADER_DURATION);
#if INDICATORS
		indicators.generalBulb->setOFF();
#endif
#if USE_MOTOR
		motor.speed = 0;
#endif
		break;

	case pwr_OFF:
		screenON(true);
		forceToSleep();
		effects.change(flash.getCurrentEffect(pwr_OFF), FADER_DURATION, fade_OnlyDimming, false);
#if INDICATORS
		indicators.generalBulb->set(bulb_Sinus, 30);
#endif
		break;

	case pwr_ON:
		screenON(true);
		effects.fadein((eff_id == 255U) ? flash.getCurrentEffect(pwr_ON) : eff_id, FADER_DURATION);
		break;

	case pwr_LAMP:
		screenON(true);
		effects.fadein((eff_id == 255U) ? flash.getCurrentEffect(pwr_LAMP) : eff_id, FADER_DURATION);
#if INDICATORS
		indicators.generalBulb->set(bulb_Triangle, 10, CRGB::Cyan, CRGB::White);
#endif
		break;

	case pwr_DEMO:
		screenON(true);
		effects.fadein(flash.getCurrentEffect(pwr_DEMO), FADER_DURATION);
		//effects.toDemoShow = flash.getCurrentEffect(pwr_ON);
#if INDICATORS
		indicators.generalBulb->set(bulb_Sinus2, 240, CRGB::Gray, CRGB::White);
#endif
		break;
	case pwr_PAINT:
		screenON(true);
		effects.change(flash.getCurrentEffect(pwr_PAINT), FADER_DURATION, fade_OnlyDimming, false);
#if INDICATORS
		indicators.generalBulb->set(bulb_Sinus3, 60, CRGB::Yellow, CRGB::White);
#endif
		break;
	default:
		break;
	}
}

uint8_t power_t::Bright() // Яркость эффекта с учетом текущего состояния
{
#if USE_MOTOR
	bright_real = map8(bright_global, bright_min, bright_max);
#else
	bright_real = map8(scale8(bright, bright_global), bright_min, bright_max);
#endif

	if (bright_real == 1)
	{
		FastLED.setDither(DISABLE_DITHER);
		status |= 0x10;
		bright_real = 2;
	}
	else
	{
		FastLED.setDither(BINARY_DITHER);
		status &= ~0x10;
	}

	if (bright_real >= BRIGHT_INDICATORS_MIN)
	{
		dimmer_leds = 0;
		bright_reg = bright_real;
	}
	else
	{
		dimmer_leds = ~qmul8(bright_real, 256 / BRIGHT_INDICATORS_MIN);
		bright_reg = BRIGHT_INDICATORS_MIN;
	}
	return bright_real;
}

uint8_t power_t::setBrightness(uint8_t value) // устанавливает полноразмерное значение яркости и подготавливает преобразованное
{
	bright = value;
	status |= 0x20;
	return Bright();
}

void power_t::correctBrightness(uint8_t of_mode = power.mode) // <-- корректировка диапазона яркости при смене режима
{
	pwr_bright_t bri = power_getBright(of_mode);
#if USE_WIFI
	if (chronometer.isDayTime)
	{
		bright_min = bri.day_min;
		bright_max = bri.day_max;
	}
	else
	{
		bright_min = bri.night_min;
		bright_max = bri.night_max;
	}
#else
	bright_min = bri.day_min;
	bright_max = bri.day_max;
#endif
	Bright();
}

void power_t::tick()
{
	if (!(~status & 0x03) && !effect.Unsleeped())
	{
		if (power.mode == pwr_OFF)
#if USE_OSD
			if (osd.scrollDone())
#endif
			{
				if ((millis() - powerTS32) >= 2000)
					change(pwr_SLEEP);
			}
			else
				powerTS32 = millis();
		//if (power.mode == pwr_SLEEP)
			//if (effects.fader.stopped())
				//{screenOFF(true);}
	}

	// Day & Night Brightness Correction
#if USE_WIFI
	chronometer.isDayTime = ((chronometer.thisTime > NIGHT_HOURS_STOP) && (chronometer.thisTime < NIGHT_HOURS_START));
	if (chronometer.lastDayTime != chronometer.isDayTime)
	{
		chronometer.lastDayTime = chronometer.isDayTime;
		correctBrightness(power.mode);
	}
#endif
}
#endif
