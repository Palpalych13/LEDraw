#ifndef _chronometer_h
#define _chronometer_h
#include <stdint.h>
#include "power.h"

#if USE_WIFI && USE_NTP
#include <Timezone.h>
#include <NTPClient.h>
WiFiUDP ntpUDP;
#ifdef SUMMER_WINTER_TIME
TimeChangeRule summerTime = {SUMMER_TIMEZONE_NAME, SUMMER_WEEK_NUM, SUMMER_WEEKDAY, SUMMER_MONTH, SUMMER_HOUR, SUMMER_OFFSET};
TimeChangeRule winterTime = {WINTER_TIMEZONE_NAME, WINTER_WEEK_NUM, WINTER_WEEKDAY, WINTER_MONTH, WINTER_HOUR, WINTER_OFFSET};
Timezone localTimeZone(summerTime, winterTime);
#else
TimeChangeRule localTime = {LOCAL_TIMEZONE_NAME, LOCAL_WEEK_NUM, LOCAL_WEEKDAY, LOCAL_MONTH, LOCAL_HOUR, LOCAL_OFFSET};
Timezone localTimeZone(localTime);
#endif
NTPClient timeClient(ntpUDP, NTP_ADDRESS, 0, NTP_INTERVAL); // объект, запрашивающий время с ntp сервера; в нём смещение часового пояса не используется (перенесено в объект localTimeZone); здесь всегда должно быть время UTC
#include "timerMinimManager.h"

struct AlarmType
{
	bool State = false;
	uint16_t Time = 0U;
};

AlarmType alarms[7];

bool ntpServerAddressResolved = false;
bool timeSynched = false;
uint32_t lastTimePrinted = 0U;

bool manualOff = false;
uint64_t lastResolveTryMoment = 0UL;
IPAddress ntpServerIp = {0, 0, 0, 0};
CHSV dawnColor = CHSV(0, 0, 0);				// цвет "рассвета"
CHSV dawnColorMinus1 = CHSV(0, 0, 0); // для большей плавности назначаем каждый новый цвет только 1/10 всех диодов; каждая следующая 1/10 часть будет "оставать" на 1 шаг
CHSV dawnColorMinus2 = CHSV(0, 0, 0);
CHSV dawnColorMinus3 = CHSV(0, 0, 0);
CHSV dawnColorMinus4 = CHSV(0, 0, 0);
CHSV dawnColorMinus5 = CHSV(0, 0, 0);
uint8_t dawnCounter = 0; // счётчик первых 10 шагов будильника

#include <Timezone.h>

bool chronometer_t::changingQuarters()
{
	return (!mod8(minutes, 15) && (!seconds));
}

bool chronometer_t::changingMinutes()
{
	return ((seconds == 59) && (millis() - secondsTS32 > 500));
}

bool chronometer_t::changingHours()
{
	return ((minutes == 59) && (seconds == 59) && (millis() - secondsTS32 > 500));
}

void chronometer_t::setCurrentTime(time_t time)
{
	if ((status & 0x01) || (seconds > 30 && (status & 0x02)))
	{
		setTime(time);
		currentTime = now();
		status &= ~0x03;
		power.correctBrightness();
	}
};

void chronometer_t::getFormattedTime(char *buf)
{
	//time_t currentTime = localTimeZone.toLocal(timeClient.getEpochTime());
	//sprintf_P(buf, PSTR("%02u:%02u:%02u"), hour(currentTime), minute(currentTime), second(currentTime));
	sprintf_P(buf, PSTR("%02u:%02u:%02u"), hours, minutes, seconds);
}

void chronometer_t::resolveNtpServerAddress(bool &ntpServerAddressResolved) // функция проверки подключения к интернету
{
	if (ntpServerAddressResolved)
		return;

	WiFi.hostByName(NTP_ADDRESS, ntpServerIp, RESOLVE_TIMEOUT);
	if (ntpServerIp[0] <= 0)
	{
#if USE_DEBUG
		if (ntpServerAddressResolved)
			LOG.println(PSTR("Подключение к интернету отсутствует"));
#endif

		ntpServerAddressResolved = false;
	}
	else
	{
#if USE_DEBUG
		if (!ntpServerAddressResolved)
			LOG.println(PSTR("Подключение к интернету установлено"));
#endif
		ntpServerAddressResolved = true;
	}
}

void chronometer_t::tick()
{
	if (ESP_mode == 1U)
	{
		EVERY_N_SECONDS(1)
		{
			currentTime++;
			seconds = second(currentTime);
			minutes = minute(currentTime);
			hours = hour(currentTime);
			thisTime = hours * 60 + minutes;
			thisTime_full = hours * 3600 + minutes * 60 + seconds;
			thisDay = dayOfWeek(currentTime);
			if (thisDay == 1)
				thisDay = 8; // в библиотеке Time воскресенье - это 1; приводим к диапазону [0..6], где воскресенье - это 6
			thisDay -= 2;
			if ((seconds < 10) && !(status & 0x02))
				status |= 0x02;
			secondsTS32 = millis();
		}

		if (timeTimerSync.isReady())
		{
			if (!timeSynched)
			{
				if (millis() - lastResolveTryMoment >= RESOLVE_INTERVAL || lastResolveTryMoment == 0)
				{
					resolveNtpServerAddress(ntpServerAddressResolved); // пытаемся получить IP адрес сервера времени (тест интернет подключения) до тех пор, пока время не будет успешно синхронизировано
					lastResolveTryMoment = millis();
#if USE_DEBUG
					if (!ntpServerAddressResolved)
						LOG.println(PSTR("Функции будильника отключены до восстановления подключения к интернету"));
#endif
				}
				if (!ntpServerAddressResolved)
					return; // если нет интернет подключения, отключаем будильник до тех пор, пока оно не будет восстановлено
			}

			timeSynched = timeClient.update() || timeSynched; // если время хотя бы один раз было синхронизировано, продолжаем
			if (!timeSynched)																	// если время не было синхронизиировано ни разу, отключаем будильник до тех пор, пока оно не будет синхронизировано
				return;

			setCurrentTime(localTimeZone.toLocal(timeClient.getEpochTime()));

			//printTime(thisTime, false, ONflag);                                 // проверка текущего времени и его вывод (если заказан и если текущее время соответстует заказанному расписанию вывода)
			// проверка рассвета
			/*if (alarms[thisDay].State &&																																											 // день будильника
					thisTime >= (uint16_t)constrain(alarms[thisDay].Time - pgm_read_byte(&dawnOffsets[dawnMode]), 0, (24 * 60)) && // позже начала
					thisTime < (alarms[thisDay].Time + DAWN_TIMEOUT))																															 // раньше конца + минута
			{
				if (!manualOff) // будильник не был выключен вручную (из приложения или кнопкой)
				{
					// величина рассвета 0-255
					int32_t dawnPosition = 255 * ((float)(thisTime_full - (alarms[thisDay].Time - pgm_read_byte(&dawnOffsets[dawnMode])) * 60) / (pgm_read_byte(&dawnOffsets[dawnMode]) * 60));
					dawnPosition = constrain(dawnPosition, 0, 255);
					dawnColorMinus5 = dawnCounter > 4 ? dawnColorMinus4 : dawnColorMinus5;
					dawnColorMinus4 = dawnCounter > 3 ? dawnColorMinus3 : dawnColorMinus4;
					dawnColorMinus3 = dawnCounter > 2 ? dawnColorMinus2 : dawnColorMinus3;
					dawnColorMinus2 = dawnCounter > 1 ? dawnColorMinus1 : dawnColorMinus2;
					dawnColorMinus1 = dawnCounter > 0 ? dawnColor : dawnColorMinus1;
					dawnColor = CHSV(map(dawnPosition, 0, 255, 10, 35),
													 map(dawnPosition, 0, 255, 255, 170),
													 map(dawnPosition, 0, 255, 10, DAWN_BRIGHT));
					dawnCounter++;
					// fill_solid(ledstripe, NUM_LEDS, dawnColor);
					for (uint16_t i = 0U; i < NUM_LEDS; i++)
					{
						if (i % 6 == 0)
							leds[i] = dawnColor; // 1я 1/10 диодов: цвет текущего шага
						if (i % 6 == 1)
							leds[i] = dawnColorMinus1; // 2я 1/10 диодов: -1 шаг
						if (i % 6 == 2)
							leds[i] = dawnColorMinus2; // 3я 1/10 диодов: -2 шага
						if (i % 6 == 3)
							leds[i] = dawnColorMinus3; // 3я 1/10 диодов: -3 шага
						if (i % 6 == 4)
							leds[i] = dawnColorMinus4; // 3я 1/10 диодов: -4 шага
						if (i % 6 == 5)
							leds[i] = dawnColorMinus5; // 3я 1/10 диодов: -5 шагов
					}
					FastLED.setBrightness(255);
					delay(1);
					FastLED.show();
					dawnFlag = true;
				} 

#if defined(ALARM_PIN) && defined(ALARM_LEVEL) // установка сигнала в пин, управляющий будильником
			if (thisTime == alarms[thisDay].Time)		 // установка, только в минуту, на которую заведён будильник
			{
				digitalWrite(ALARM_PIN, manualOff ? !ALARM_LEVEL : ALARM_LEVEL); // установка сигнала в зависимости от того, был ли отключен будильник вручную
			}
#endif

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL) // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время работы будильника
			digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
#endif
		}
		else
		{
			// не время будильника (ещё не начался или закончился по времени)
			if (dawnFlag)
			{
				dawnFlag = false;
				//fillAll(CRGB::Black);
				delay(2);
				FastLED.show();
				//power.change();                                                  // выключение матрицы или установка яркости текущего эффекта в засисимости от того, была ли включена лампа до срабатывания будильника
			}
			manualOff = false;
			dawnColorMinus1 = CHSV(0, 0, 0);
			dawnColorMinus2 = CHSV(0, 0, 0);
			dawnColorMinus3 = CHSV(0, 0, 0);
			dawnColorMinus4 = CHSV(0, 0, 0);
			dawnColorMinus5 = CHSV(0, 0, 0);
			dawnCounter = 0;

#if defined(ALARM_PIN) && defined(ALARM_LEVEL) // установка сигнала в пин, управляющий будильником
			digitalWrite(ALARM_PIN, !ALARM_LEVEL);
#endif

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL) // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
			digitalWrite(MOSFET_PIN, ONflag ? MOSFET_LEVEL : !MOSFET_LEVEL);
#endif
		}*/
		}
	}
}

#endif
#endif
