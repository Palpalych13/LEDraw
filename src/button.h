#ifndef _button_h
#define _button_h
#include <stdint.h>
#include <GyverButton.h>
#include "types.h"

GButton touch(BTN_PIN, PULL_MODE, CONTACT_TYPE);

static struct // button_t структура менеджера питания
{
	uint32_t buttonTS32, stepTS32;
	uint8_t holdClicks_count, lastHoldClicks_count;
	uint8_t clicks_count;
	uint8_t status; /* статус кнопки
                    0b = is_hold
                    1b = 
                    2b = 
                    3b =
										4b =  
										5b =										
                    6b = 
                    7b = has_pressed
*/

	inline bool isHold() { return status & 0x1; };
	inline bool isNotHold() { return ~status & 0x1; };
	inline void pressed_set() { status |= 0x80; };
	inline void pressed_reset() { status &= ~0x80; };
	inline bool hasNotPressed() { return ~status & 0x80; };

	uint8_t direction;

	uint8_t step(uint8_t value, regulator_t *regulator) // <-- шаговое изменение величины нажатой кнопки
	{
		EVERY_N_MILLIS(qadd8(16U, qmul8(regulator->sectors, 8U)))
		{

			//if ((direction || (value > 0)) && (!direction || (value < 255)))

			uint8_t max_value = regulator->maxValue();
			if ((millis() - buttonTS32) > BUTTON_HOLD_TIMEOUT)
			{
				if (regulator->stops(value))
					direction = !direction;
			};

			if (regulator->ends(value))
			{
				if ((millis() - stepTS32) > BUTTON_REVERSE_TIMEOUT)
				{
					if (!value)
					{
						direction = true;
						value++;
					}
					if (value >= max_value)
					{
						value = max_value;
						direction = false;
						value--;
					}
				}
				indicators.generalBulb->set(bulb_Sinus, 75, regulator->color(value));
			}
			else if (regulator->stops(value))
			{
				if ((millis() - stepTS32) > BUTTON_RANGE_TIMEOUT)
				{
					direction ? value++ : value--;
					stepTS32 = millis();
					indicators.generalBulb->setON(regulator->color(value));
				}
				else
					indicators.generalBulb->setON(regulator->color(value) % 32U);
			}
			else
			{
				direction ? value++ : value--;
				stepTS32 = millis();
				indicators.generalBulb->set(bulb_Pulse4, value, regulator->color(value));
			}
		}
		return value;
	}

	uint8_t step(uint8_t value, CRGB color = CRGB::White, uint8_t holdStep = 32, uint8_t segmentRange = 0) // <-- шаговое изменение величины нажатой кнопки
	{
		EVERY_N_MILLIS(holdStep)
		{
			if (!(value % holdStep) && ((millis() - stepTS32) < BUTTON_RANGE_TIMEOUT))
				indicators.generalBulb->setON(color % 32U);
			else
			{
				if ((millis() - stepTS32) > BUTTON_REVERSE_TIMEOUT)
					direction = !direction;

				if ((direction || (value > 0)) && (!direction || (value < 255)))
				{
					direction ? value++ : value--;
					stepTS32 = millis();
#if INDICATORS
					indicators.generalBulb->set(bulb_Pulse4, value, color);
#endif
				}
				else
					indicators.generalBulb->set(bulb_Sinus, 75, CRGB::Silver);
			}
		}
		return value;
	}

	bool tick()
	{
		if (!btn_Enabled)
			return false; // события кнопки не обрабатываются, если она заблокирована
		touch.tick();
		clicks_count = touch.hasClicks() ? touch.getClicks() : 0U;
		touch.isHold() ? status |= 0x1 : status &= ~0x1; // is_hold
		pressed_set();																	 //has_pressed
		if (isHold())
		{
#if USE_DEBUG
			if (touch.isHolded())
				LOG.printf_P(log_btn_holdClick, holdClicks_count, power.mode); // "Button: Удерживается кнопка %s из состояния %u\n"
#endif
			if ((millis() - buttonTS32) > BUTTON_HOLD_TIMEOUT)
			{
				holdClicks_count = touch.getHoldClicks();
				lastHoldClicks_count = holdClicks_count;
				direction = !direction;
			}
			else
			{
				if (clicks_count == 1U)
					direction = !direction;
				holdClicks_count = lastHoldClicks_count;
			}
			buttonTS32 = millis();
		}
		else
		{
#if USE_DEBUG
			if (clicks_count)
				LOG.printf_P(log_btn_click, clicks_count, power.mode); // "Button: Нажата кнопка %u раз(а) из состояния %u\n"
#endif
		}

#if INDICATORS
		if (touch.isPress())
			indicators.screw_SpareBulb(0);
		if (touch.isRelease())
			indicators.unscrew_SpareBulb(0);
#endif

		if ((clicks_count || isHold()) && power.mode == pwr_SLEEP)
			power.change(pwr_OFF);

		switch (power.mode)
		{
		case pwr_OFF:
			if (isNotHold()) // кнопка не удерживается, когда OFF
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда OFF
#if ONETOUCH_CLOCK
					if (!osd.printCurrentTime(CRGB::White)) // попытка напечатать время. Если не получается или текст уже на экране, то переходим ко включению
#endif
						power.change(pwr_ON);
					break;
				case 2: // ** : двоекратное нажатие, когда OFF
					power.change(pwr_LAMP);
					break;
				case 3: // *** : троекратное нажатие, когда OFF
					flash.writeCurrentEffect(effects.next(pwr_OFF), pwr_OFF);
#if USE_OSD
					osd.scrollText(PSTR("#>") + String(effects_list_off[eff_off_current].name), CRGB::LightGoldenrodYellow, 5);
#endif
					break;
				case 4: // **** : четырехкратное нажатие, когда OFF
					power.change(pwr_DEMO);
#if USE_OSD
					osd.scrollText(PSTR("*** DEMO ON ***"), CRGB::Orange, 5);
#endif
					break;
				case 5: // ***** : пятикратное нажатие, когда OFF
#if USE_OSD
					osd.scrollText(String(host_name) + PSTR(" IP:") + WiFi.localIP().toString(), CRGB::White, 10);
#endif
					break;
#if USE_OTA
				case 6: // ****** : шестикратное нажатие, когда OFF
					if (ota.RequestOtaUpdate())
						power.change(pwr_ON, EFF_MATRIX);
					break;
#endif
				case 7: // ******* : семикратное нажатие, когда OFF
#if USE_WIFI
					set_ESPmode(!ESP_mode);
					sprintf_P(textBuffer, msg_mode_wifi, ESP_mode ? PSTR("WiFi client") : PSTR("WiFi AP"));
#endif
#if USE_OSD
					osd.scrollText(textBuffer, CRGB::Yellow, 5, 0x80); //"ESP мode changed to %s. Reset...     ")
#endif
#if USE_DEBUG
					LOG.printf_P(PSTR("Рабочий режим лампы изменён и сохранён в энергонезависимую память\nНовый рабочий режим: ESP_MODE = %d, %s\nРестарт...\n"),
											 ESP_mode, ESP_mode == 0U ? PSTR("WiFi точка доступа") : PSTR("WiFi клиент (подключение к роутеру)"));
#endif
					break;
				case 8: // ******** : восьмикратное нажатие, когда OFF
					indicators.generalBulb->set(bulb_Blink25, 240U, CRGB::Yellow, CRGB::Black, 50);
					flash.defaultPrefs(true); // стирает все настройки эффектов
					effects.change(flash.getCurrentEffect(power.mode), 5, fade_OnlyDimming, false);

					break;
				default:
					pressed_reset();
				}
			}
			else // кнопка удерживается, когда OFF
			{
				switch (holdClicks_count)
				default:
					pressed_reset();
			}
			break;

		case pwr_ON: // кнопка не удерживается, когда ON
		{
			if (isNotHold())
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда ON
#if ONETOUCH_CLOCK
					if (!osd.printCurrentTime(CRGB::White)) // попытка напечатать время. Если не получается или текст уже на экране, то переходим ко включению
#endif
					{
#if USE_OSD
						osd.scrollText(FPSTR(msg_byebye), CRGB::Yellow, 5, 0x40);
#endif
						power.change(pwr_OFF);
					}
					break;
				case 2: // ** : двухкратное нажатие, когда ON
					flash.writeCurrentEffect(effects.next(pwr_ON, fade_Comet), pwr_ON);
#if TEXT_CHANGE_EFFECT
					osd.scrollText(PSTR(">>") + effect_getFromList(eff_on_current, pwr_ON).Name(), CRGB::LightGoldenrodYellow, 5);
#endif
					break;
				case 3: // *** : троекратное нажатие, когда ON
					flash.writeCurrentEffect(effects.previous(pwr_ON, fade_Comet), pwr_ON);
#if TEXT_CHANGE_EFFECT
					osd.scrollText(PSTR("<<") + effect_getFromList(eff_on_current, pwr_ON).Name(), CRGB::PowderBlue, 5);
#endif
					break;
				case 4: // **** : четырехкратное нажатие, когда ON
#if USE_MOTOR
					flash.writePref(pref_Brightness, pwr_ON, motor.stepAction());
#else
					power.change(pwr_PAINT);
#endif
#if USE_OSD
					osd.scrollText(PSTR("*** PAINT ON ***"), CRGB::Coral, 5);
#endif
					break;
				case 5: // ***** : пятикратное нажатие, когда ON
					effect_printInfo();
					break;
				case 8: // ******** : восьмикратное нажатие, когда ON
					indicators.generalBulb->set(bulb_Blink50, 120U, CRGB::Yellow, CRGB::Black, 30);
					flash.defaultPrefs(effect.current->id, true); // установки по умолчанию текущего эффекта
					effects.change(effect.current->id, 5, fade_OnlyDimming, false);

#if USE_OSD
					osd.scrollText(PSTR("Effects preferences cleared..."), CRGB::Yellow, 3, 0x15);
#endif
					break;
				default:
					pressed_reset();
				}
			}
			else // кнопка удерживается, когда ON
			{
				switch (holdClicks_count)
				{
				default:
					pressed_reset();
				}
			}
			break;
		}

		case pwr_LAMP:
		{
			if (isNotHold()) // кнопка не удерживается, когда LAMP
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда LAMP
					effects.fadeout(FADER_DURATION);
					power.change(pwr_SLEEP);
					break;
				case 2: // ** : двухкратное нажатие, когда LAMP
					power.change(pwr_ON);
					break;
				case 3: // *** : троекратное нажатие, когда LAMP
					flash.writeCurrentEffect(effects.next(pwr_LAMP), pwr_LAMP);
#if USE_OSD
					osd.scrollText((PSTR("#>") + String(effs_lamp[eff_lamp_current].name)), CRGB::LightGoldenrodYellow, 5);
#endif
					break;
				default:
					pressed_reset();
				}
			}
			else // кнопка удерживается, когда LAMP
			{
				switch (holdClicks_count)
				{
				default:
					pressed_reset();
				}
			}
			break;
		}

		case pwr_DEMO:
		{
			if (isNotHold()) // кнопка не удерживается, когда DEMO
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда DEMO
					power.change(pwr_OFF);
					break;
				case 2: // ** : двухкратное нажатие, когда DEMO
								//flash.writeCurrentEffect(eff_demo, pwr_ON);
#if USE_OSD
					osd.scrollText(PSTR("*** DEMO OFF ***"), CRGB::Orange, 5);
#endif
					power.change(pwr_ON);
					break;
				case 3: // *** : троекратное нажатие, когда DEMO
					effects.next(pwr_DEMO);
#if USE_OSD
					osd.scrollText(PSTR(">>") + effect_getFromList(eff_demo_current, DEMO).Name(), CRGB::Orange, 5);
#endif
					break;
				case 5: // ***** : пятикратное нажатие, когда DEMO
					//sprintf_P(textBuffer, PSTR("DEMO PERIOD:%us"), 15 * (flash.readPref(pref_Speed, effects.fader.toShow) / 19 + 1));
#if USE_OSD
					osd.scrollText(textBuffer, CRGB::Orange, 5);
#endif
					break;
				default:
					pressed_reset();
				}
			}
			else // кнопка удерживается, когда DEMO
			{
				switch (holdClicks_count)
				{
				default:
					pressed_reset();
				}
			}
			break;
		}

		case pwr_PAINT:
		{
			if (isNotHold()) // кнопка не удерживается, когда PAINT
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда PAINT
#if ONETOUCH_CLOCK
					if (!osd.printCurrentTime(CRGB::White)) // попытка напечатать время. Если не получается или текст уже на экране, то переходим ко включению
#endif
						power.change(pwr_ON);
					break;
				case 2: // ** : двухкратное нажатие, когда PAINT
					flash.writeCurrentEffect(effects.next(pwr_PAINT), pwr_PAINT);
#if USE_OSD
					osd.scrollText(PSTR(">>") + effect_getFromList(eff_paint_current, PAINT).Name(), CRGB::LightGoldenrodYellow, 5);
#endif
					break;
				case 3: // *** : троекратное нажатие, когда PAINT
					flash.writeCurrentEffect(effects.previous(pwr_PAINT), pwr_PAINT);
#if USE_OSD
					osd.scrollText(PSTR("<<") + effect_getFromList(eff_paint_current, PAINT).Name(), CRGB::PowderBlue, 5);
#endif
					break;
				case 4: // **** : четырехкратное нажатие, когда PAINT
					power.change(pwr_ON);
#if USE_OSD
					osd.scrollText(PSTR("*** PAINT OFF ***"), CRGB::Coral, 5);
#endif
					break;
				default:
					pressed_reset();
				}
			}
			else // кнопка удерживается, когда PAINT
			{
				switch (holdClicks_count)
				{
				default:
					pressed_reset();
				}
			}
			break;
		}
		/* шаблон
  case ANY:
  {
    if (isNotHold()) // кнопка не удерживается, когда ANY
    { 
      switch (clicks_count)
      {
      case 1: // * : однократное нажатие, когда ANY
        break;
      case 2: // ** : двухкратное нажатие, когда ANY
        break;
      case 3: // *** : троекратное нажатие, когда ANY
        break;
      case 4: // **** : четырехкратное нажатие, когда ANY
        break;
      case 5: // ***** : пятикратное нажатие, когда ANY
        break;
      case 6: // ****** : шестикратное нажатие, когда ANY
        break;
      case 7: // ******* : семикратное нажатие, когда ANY
        break;
      case 8: // ******** : восьмикратное нажатие, когда ANY
        break;
      }
    }
    else // кнопка удерживается, когда ANY
    { 
      switch (holdClicks_count)
      {
      case 0: // - : удержание без нажатий, когда ANY
        break;
      case 1: // *- : удержание с одним нажатием, когда ANY
        break;
      case 2: // **- : удержание с двумя нажатием, когда ANY
        break;
      case 3: // ***- : удержание с тремя нажатием, когда ANY
        break;
      }
    }
    break;
  }
  */

		break;
		}

		if (hasNotPressed()) // если кнопка нажата, но выполнение команды не произошло (обработка по умолчанию)
		{
			if (isNotHold()) // кнопка не удерживается, когда DEFAULT
			{
				switch (clicks_count)
				{
				case 1: // * : однократное нажатие, когда DEFAULT
					break;
				case 2: // ** : двухкратное нажатие, когда DEFAULT
					break;
				case 3: // *** : троекратное нажатие, когда DEFAULT
					break;
				case 4: // **** : четырехкратное нажатие, когда DEFAULT
					break;
				case 5: // ***** : пятикратное нажатие, когда DEFAULT
					break;
				case 6: // ****** : шестикратное нажатие, когда DEFAULT
					break;
				case 7: // ******* : семикратное нажатие, когда DEFAULT
					break;
				case 8: // ******** : восьмикратное нажатие, когда DEFAULT
					break;
				}
			}
			else // кнопка удерживается, когда DEFAULT
			{
				switch (holdClicks_count)
				{
				case 0: // - : удержание без нажатий, когда DEFAULT
					power.bright_global = step(flash.readGlobalBrightness(power.mode), effects.getRegulator(reg_GlobalBrightness));
					flash.writeGlobalBrightness(power.bright_global, power.mode);
					power.Bright();
#if USE_OSD
					osd.drawGauge(power.bright_global, 4);
#endif
					break;
				case 1: // *- : удержание с одним нажатием, когда DEFAULT
					effect.setSpeed(effect.speedRegulator->step(flash.readPref(pref_Speed, effect.current->id)));

					//effect.setSpeed(step(flash.readPref(pref_Speed, effect.current->id), effect.speedRegulator));
					flash.writePref(pref_Speed, effect.current->id, effect.speed);
#if USE_OSD
					osd.drawGauge(effect.speed, 1);
#endif
					break;
				case 2: // **- : удержание с двумя нажатием, когда DEFAULT
					effect.setScale(effect.scaleRegulator->step(flash.readPref(pref_Scale, effect.current->id)));

					//effect.setScale(step(flash.readPref(pref_Scale, effect.current->id), effect.scaleRegulator));
					flash.writePref(pref_Scale, effect.current->id, effect.scale);
#if USE_OSD
					osd.drawGauge(effect.scale, 3);
#endif
					break;
				case 3: // ***- : удержание с тремя нажатием, когда DEFAULT
#if USE_MOTOR
#if MOTOR_USE_REVERSE
					motor.speed = step(flash.readGlobalMotorSpeed(power.mode), effects.getRegulator(reg_Brightness));
					flash.writeGlobalMotorSpeed(power.mode, motor.speed);
#else
					motor.speed = step(flash.readGlobalMotorSpeed(power.mode), effect.getRegulator(reg_Brightness));
					flash.writeGlobalMotorSpeed(power.mode, motor.speed);
#endif
#else
					power.bright = step(flash.readPref(pref_Brightness, effect.id));
					flash.writePref(pref_Brightness, effect.id, power.bright);
					power.Bright();
#endif

#if USE_OSD
					osd.drawGauge(power.bright, 11);
#endif
					break;
				}
			}
		}
		return true;
	}
} button;
#endif
