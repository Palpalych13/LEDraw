#ifndef _flash_h
#define _flash_h
#pragma once
#include <stdint.h>

/*
   Схема использования EEPROM памяти:
   Используются адреса:
   Начало    Длина    Описание
                      массив alarm
   0         1        будильник пн вкл/выкл
   1-2       2        будильник пн время в минутах от начала суток (0 - 1440), 2 байта
   3         1        будильник вт вкл/выкл
   4-5       2        будильник вт время в минутах от начала суток (0 - 1440), 2 байта
   6         1        будильник ср вкл/выкл
   7-8       2        будильник ср время в минутах от начала суток (0 - 1440), 2 байта
   9         1        будильник чт вкл/выкл
   10-11     2        будильник чт время в минутах от начала суток (0 - 1440), 2 байта
   12        1        будильник пт вкл/выкл
   13-14     2        будильник пт время в минутах от начала суток (0 - 1440), 2 байта
   15        1        будильник сб вкл/выкл
   16-17     2        будильник сб время в минутах от начала суток (0 - 1440), 2 байта
   18        1        будильник вс вкл/выкл
   19-20     2        будильник вс время в минутах от начала суток (0 - 1440), 2 байта
                      массив modes

   111-116   6        настройки режима избранных эффектов (вкл/выкл - 1 байт; интервал - 2 байта; разброс - 2 байта; инициализировать вкл/выкл - 1 байт;
   116-159   43       вкл/выкл каждого эффекта - 43 (effs_count) байт;

   195       1        признак "кнопка разблокирована"
   196       1        рабочий режим лампы (ESP_MODE)
   197       1        состояние лампы (вкл/выкл)
   198       1        признак первого запуска (определяет необходимость первоначальной записи всех хранимых настроек)
   199       1        время до "рассвета" (dawnMode)
   200       1        текущий режим (currentMode)
*/

#define EEPROM_ESP_BUTTON_ENABLED 21U // адрес в EEPROM памяти для записи признака разблокированной кнопки
#define EEPROM_ESP_MODE 22U						// адрес в EEPROM памяти для записи режима работы модуля ESP (точка доступа/WiFi клиент)
#define EEPROM_POWER_MODE 23U					// адрес в EEPROM памяти для записи состояния лампы (вкл/выкл)
#define EEPROM_FIRST_RUN 24U					// адрес в EEPROM памяти для записи признака первого запуска (определяет необходимость первоначальной записи всех хранимых настроек)
#define EEPROM_TIME_BEFORE_DAWN 25U		// адрес в EEPROM памяти для записи времени до "рассвета"
#define EEPROM_TIME_AFTER_DAWN 26U		// адрес в EEPROM памяти для записи времени после "рассвета"

#define EEPROM_MODE_CURRENT 30U	 // адрес в EEPROM памяти для записи номера текущего эффекта режима SLEEP (+ количество режимов)
#define EEPROM_OFF_CURRENT 31U	 // адрес в EEPROM памяти для записи номера текущего эффекта режима OFF
#define EEPROM_ON_CURRENT 32U		 // адрес в EEPROM памяти для записи номера текущего эффекта режима ON
#define EEPROM_LAMP_CURRENT 33U	 // адрес в EEPROM памяти для записи номера текущего эффекта режима LAMP
#define EEPROM_DEMO_CURRENT 34U	 // адрес в EEPROM памяти для записи номера текущего эффекта режима DEMO
#define EEPROM_PAINT_CURRENT 35U // адрес в EEPROM памяти для записи номера текущего эффекта режима PAINT

#define EEPROM_ALARM_START 128U			// начальный адрес в EEPROM памяти для записи настроек будильников
#define EEPROM_ALARM_STRUCT_SIZE 3U // 1 байт - вкл/выкл; 2 байта - время от начала суток в минутах (0 - 1440)

#define EEPROM_EFFS_COUNT 40U // адрес в EEPROM памяти для записи номера текущего эффекта лампы

#define EEPROM_GLOBAL_DAYBRIGHT 50U		// адрес в EEPROM памяти для записи глобальной яркости (SLEEP+)  (+ количество режимов)
#define EEPROM_GLOBAL_NIGHTBRIGHT 60U // адрес в EEPROM памяти для записи глобальной яркости (SLEEP+)  (+ количество режимов)

#define EEPROM_GLOBAL_MOTORSPEED 70U // адрес в EEPROM памяти для записи глобальной скорости мотора (SLEEP+)  (+ количество режимов)

// начальный адрес в EEPROM памяти для записи настроек эффектов (яркость, скорость, масштаб)
#define EEPROM_PREF_SIZE 5U							// 1 байт - яркость; 1 байт - скорость; 1 байт - масштаб ; 1 байт - фавориты
#define EEPROM_PREFS_COUNT 256U					// 1 байт - яркость; 1 байт - скорость; 1 байт - масштаб ; 1 байт - фавориты
#define EEPROM_PREFS_START_ADDRESS 256U // начальный адрес для настроек эффектов
static const uint16_t EEPROM_TOTAL_BYTES_USED(EEPROM_PREFS_START_ADDRESS + EEPROM_PREF_SIZE * EEPROM_PREFS_COUNT);

#define EEPROM_FIRST_RUN_MARK 0xA5 // число-метка, если ещё не записно в EEPROM_FIRST_RUN, значит нужно проинициализировать EEPROM и записать все первоначальные настройки
#define EEPROM_WRITE_DELAY 30U		 // отсрочка в секундах записи в EEPROM после последнего изменения хранимых настроек, позволяет уменьшить количество операций записи в EEPROM

// ------ FUNCTIONS -----
/*
static uint16_t ReadUint16(uint16_t address)
{
	uint16_t val;
	uint8_t *p = (uint8_t *)&val;
	*p = EEPROM.read(address);
	*(p + 1) = EEPROM.read(address + 1);
	return val;
}

static void WriteUint16(uint16_t address, uint16_t val)
{
	uint8_t *p = (uint8_t *)&val;
	EEPROM.write(address, *p);
	EEPROM.write(address + 1, *(p + 1));
	EEPROM.commit();
}

static int16_t ReadInt16(uint16_t address)
{
	int16_t val;
	uint8_t *p = (uint8_t *)&val;
	*p = EEPROM.read(address);
	*(p + 1) = EEPROM.read(address + 1);
	return val;
}

static void WriteInt16(uint16_t address, int16_t val)    {
  uint8_t* p = (uint8_t*)&val;
  EEPROM.write(address, *p);
  EEPROM.write(address + 1, *(p + 1));
  EEPROM.commit();
}

static uint32_t ReadUint32(uint16_t address)    {
  uint32_t val;
  uint8_t* p = (uint8_t*)&val;
  *p        = EEPROM.read(address);
  *(p + 1)  = EEPROM.read(address + 1);
  *(p + 2)  = EEPROM.read(address + 2);
  *(p + 3)  = EEPROM.read(address + 3);
  return val;
}

static void WriteUint32(uint16_t address, uint32_t val)    {
  uint8_t* p = (uint8_t*)&val;
  EEPROM.write(address, *p);
  EEPROM.write(address + 1, *(p + 1));
  EEPROM.write(address + 2, *(p + 2));
  EEPROM.write(address + 3, *(p + 3));
  EEPROM.commit();
}

static int32_t ReadInt32(uint16_t address)    {
  int32_t val;
  uint8_t* p = (uint8_t*)&val;
  *p        = EEPROM.read(address);
  *(p + 1)  = EEPROM.read(address + 1);
  *(p + 2)  = EEPROM.read(address + 2);
  *(p + 3)  = EEPROM.read(address + 3);
  return val;
}

static void WriteInt32(uint16_t address, int32_t val)    {
  uint8_t* p = (uint8_t*)&val;
  EEPROM.write(address, *p);
  EEPROM.write(address + 1, *(p + 1));
  EEPROM.write(address + 2, *(p + 2));
  EEPROM.write(address + 3, *(p + 3));
  EEPROM.commit();
} */

// ----- INITIALIZATION -----

//eff_t effect_get(uint8_t);

void flash_t::readSettings()
{
	//ESP_mode = EEPROM.read(EEPROM_ESP_MODE);
	power.mode = (pmode_t)EEPROM.read(EEPROM_POWER_MODE);

	btn_Enabled = true;
}

void flash_t::defaultPrefs(uint16_t of_effect, bool commit)
{
	eff_t e = effects.get(of_effect);
	writePref(pref_Brightness, of_effect, e.bright_def);
	writePref(pref_Speed, of_effect, e.speed_def);
	writePref(pref_Scale, of_effect, e.scale_def);
	writePref(pref_Favorits, of_effect, 0xFF);
#if USE_MOTOR
	//writePref(pref_Motor, of_effect, e.motor_def);
#endif
	if (commit)
	{
		EEPROM.commit();
#if USE_DEBUG
		LOG.printf_P(log_flash_effect_reset, effects.getListItem(of_effect, power.mode).name, of_effect);
#endif
	}
}

void flash_t::defaultPrefs(bool commit)
{
	defaultGlobalBrights(false);

	for (uint16_t i = 0; i < EEPROM_PREFS_COUNT; i++)
		defaultPrefs(i, false);

#if USE_WIFI
	for (uint8_t i = 0; i < 7; i++)
	{
		alarms[i].State = EEPROM.read(EEPROM_ALARM_START + EEPROM_ALARM_STRUCT_SIZE * i);
		alarms[i].Time = ReadInt16(EEPROM_ALARM_START + EEPROM_ALARM_STRUCT_SIZE * i + 1);
	}
#endif

#if USE_DEBUG
	LOG.println(FPSTR(log_eeprom_reset));
#endif

	if (commit)
		EEPROM.commit();
}

void flash_t::defaultGlobalBrights(bool commit)
{
	for (uint8_t i = 0; i < pwr_last; i++) //pwr_mode_size
	{
		EEPROM.write(EEPROM_GLOBAL_DAYBRIGHT + i, BRIGHT);
		EEPROM.write(EEPROM_GLOBAL_NIGHTBRIGHT + i, BRIGHT);
	}
	if (commit)
		EEPROM.commit();
}

bool flash_t::checkReset()
{
	return ((EEPROM.read(EEPROM_FIRST_RUN) != EEPROM_FIRST_RUN_MARK) || (EEPROM.read(EEPROM_EFFS_COUNT) != effs_count));
}

uint8_t flash_t::readGlobalBrightness(uint8_t of_mode)
{
#if USE_WIFI
	if (chronometer.isDayTime)
		return EEPROM.read(EEPROM_GLOBAL_DAYBRIGHT + of_mode);
	else
		return EEPROM.read(EEPROM_GLOBAL_NIGHTBRIGHT + of_mode);
#else
	return EEPROM.read(EEPROM_GLOBAL_DAYBRIGHT + of_mode);
#endif
}

void flash_t::writeGlobalBrightness(uint8_t value, uint8_t of_mode)
{
#if USE_WIFI
	if (chronometer.isDayTime)
		EEPROM.write(EEPROM_GLOBAL_DAYBRIGHT + of_mode, value);
	else
		EEPROM.write(EEPROM_GLOBAL_NIGHTBRIGHT + of_mode, value);
#else
	return EEPROM.write(EEPROM_GLOBAL_DAYBRIGHT + of_mode, value);
#endif

	forceToSave = true;
}

#if USE_MOTOR
uint8_t flash_t::readGlobalMotorSpeed(uint8_t of_mode)
{
	return EEPROM.read(EEPROM_GLOBAL_MOTORSPEED + of_mode);
}

void flash_t::writeGlobalMotorSpeed(uint8_t of_mode, uint8_t value)
{
	return EEPROM.write(EEPROM_GLOBAL_MOTORSPEED + of_mode, value);
	forceToSave = true;
}
#endif

uint8_t flash_t::readPref(prefs_t pref, uint8_t of_effect)
{
	return EEPROM.read(EEPROM_PREFS_START_ADDRESS + of_effect * EEPROM_PREF_SIZE + pref);
}

void flash_t::writePref(prefs_t pref, uint8_t of_effect, uint8_t value)
{
	EEPROM.write(EEPROM_PREFS_START_ADDRESS + of_effect * EEPROM_PREF_SIZE + pref, value);
	forceToSave = true;
}

void flash_t::writeCurrentEffect(uint8_t value, uint8_t of_effect)
{
	EEPROM.write(EEPROM_MODE_CURRENT + of_effect, value);
	forceToSave = true;
}

uint8_t flash_t::readCurrentEffect(uint8_t of_mode)
{
	LOG.println(FPSTR(log_eeprom_eff_read));
	return EEPROM.read(EEPROM_MODE_CURRENT + of_mode);
}

uint8_t flash_t::getCurrentEffect(uint8_t of_mode)
{
	return effects.getIndexByList(readCurrentEffect(of_mode), of_mode);
}

// ----- TICKER -----
void flash_t::tick()
{
	if (forceToSave)
	{
		EVERY_N_SECONDS(EEPROM_WRITE_DELAY)
		{
			forceToSave = false;
			EEPROM.commit();
			Serial.println(PSTR("EEPROM: Сохранение накопившшихся настроек..."));
		}
	}
}

#if USE_WIFI
static void SaveAlarmsSettings(uint8_t *alarmNumber, AlarmType alarms[])
{
	EEPROM.write(EEPROM_ALARM_START + EEPROM_ALARM_STRUCT_SIZE * (*alarmNumber), alarms[*alarmNumber].State);
	WriteUint16(EEPROM_ALARM_START + EEPROM_ALARM_STRUCT_SIZE * (*alarmNumber) + 1, alarms[*alarmNumber].Time);
	EEPROM.commit();
}
#endif

void flash_t::init()
{
	EEPROM.begin(EEPROM_TOTAL_BYTES_USED); // инициализация работы с флэш-памятью
	if (checkReset())											 // проверка на наличие измененний в списке эффектов
	{
		EEPROM.write(EEPROM_FIRST_RUN, EEPROM_FIRST_RUN_MARK);
		EEPROM.write(EEPROM_EFFS_COUNT, effs_count);
		EEPROM.write(EEPROM_ESP_MODE, 1U);
		EEPROM.write(EEPROM_POWER_MODE, pwr_OFF);
		EEPROM.write(EEPROM_ON_CURRENT, 0U);
		EEPROM.write(EEPROM_OFF_CURRENT, 0U);
		EEPROM.write(EEPROM_LAMP_CURRENT, 0U);
		EEPROM.write(EEPROM_DEMO_CURRENT, 0U);
		EEPROM.write(EEPROM_PAINT_CURRENT, 0U);
		EEPROM.write(EEPROM_ESP_BUTTON_ENABLED, 1U);

		defaultPrefs(true);

#if INDICATORS
		indicators.generalBulb->setON(CRGB::Yellow);
		delay(1000);
		indicators.generalBulb->setON(CRGB::Black);
#else
		showWarning(CRGB(16, 16, 0), 150U, 50U);
		FastLED.setBrightness(BRIGHT);
		fillAll(CRGB::Black); // очистка матрицы
		delay(10);
		FastLED.show();
#endif
	}
	flash.readSettings(); // чтение сохраненных рабочих переменных
}

#endif
