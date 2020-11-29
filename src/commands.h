#ifndef _commands_h
#define _commands_h
#include <stdint.h>
#include "power.h"
// ----- EFFECTS SECTION -----

void effect_printInfo() // вывод информации об эффекте
{
	sprintf_P(textBuffer, PSTR("FPS:%u BRI:%u SPD:%u SCA:%u"), FastLED.getFPS(), power.bright_real, effect.speed, effect.scale);
#if USE_OSD
	osd.scrollText(textBuffer, CRGB::White, 4);
#endif
#if USE_DEBUG
  LOG.println(textBuffer);
#endif
}

// ----- BRIGHTNESS SECTION -----

uint8_t incGlobalBright(uint8_t mode, uint8_t cmd_step = 1U) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readGlobalBrightness(mode);
  value = qadd8(value, cmd_step);
  flash.writeGlobalBrightness(value, mode);
  return value;
}

uint8_t decGlobalBright(uint8_t mode, uint8_t cmd_step = 1U) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readGlobalBrightness(mode);
  value = qsub8(value, cmd_step);
  flash.writeGlobalBrightness(value, mode);
  return value;
}

uint8_t incBright(uint8_t eff_id, uint8_t cmd_step = 1U) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Brightness,eff_id);
  value = qadd8(value, cmd_step);
  flash.writePref(pref_Brightness, eff_id, value);
  return value;
}

uint8_t decBright(uint8_t eff_id, uint8_t cmd_step = 1U) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Brightness,eff_id);
  value = qsub8(value, cmd_step);
  flash.writePref(pref_Brightness, eff_id, value);
  return value;
}

// ----- SPEED SECTION -----

uint8_t incSpeed(uint8_t eff_id, uint8_t cmd_step = 1) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Speed,eff_id);
  value = qadd8(value, cmd_step);
  flash.writePref(pref_Speed, eff_id, value);
  return value;
}

uint8_t decSpeed(uint8_t eff_id, uint8_t cmd_step = 1) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Speed,eff_id);
  value = qsub8(value, cmd_step);
  flash.writePref(pref_Speed, eff_id, value);
  return value;
}

// ----- SCALE SECTION -----

uint8_t incScale(uint8_t eff_id, uint8_t cmd_step = 1) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Scale,eff_id);
  value = qadd8(value, cmd_step);
  flash.writePref(pref_Scale, eff_id, value);
  return value;
}

uint8_t decScale(uint8_t eff_id, uint8_t cmd_step = 1) // <-- шаговое изменение яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
  uint8_t value = flash.readPref(pref_Scale,eff_id);
  value = qsub8(value, cmd_step);
  flash.writePref(pref_Scale, eff_id, value);
  return value;
}

#endif