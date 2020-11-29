#pragma once
#include <stdint.h>
#include "FastLED.h"


static void fillAll(CRGB, CRGB *);

class TimerManager
{
public:
	static bool TimerRunning;		// флаг "таймер взведён"
	static bool TimerHasFired;	// флаг "таймер отработал"
	static uint8_t TimerOption; // индекс элемента в списке List Picker'а
	static uint64_t TimeToFire; // время, в которое должен сработать таймер (millis)

	static void HandleTimer( // функция, обрабатывающая срабатывание таймера, гасит матрицу
			bool *ONflag,
			bool *forceToSave,
			uint32_t *flashTS32,
			void (*changePower)())
	{
		if (!TimerManager::TimerHasFired &&
				TimerManager::TimerRunning &&
				millis() >= TimerManager::TimeToFire)
		{
#if USE_DEBUG
			LOG.print(PSTR("Выключение по таймеру\n\n"));
#endif

			TimerManager::TimerRunning = false;
			TimerManager::TimerHasFired = true;
			fillAll(CRGB::Black, leds);
			delay(2);
			FastLED.show();
			*ONflag = !(*ONflag);
			changePower();
			*forceToSave = true;
			*flashTS32 = millis();
		}
	}
};

bool TimerManager::TimerRunning = false;
bool TimerManager::TimerHasFired = false;
uint8_t TimerManager::TimerOption = 1U;
uint64_t TimerManager::TimeToFire = 0ULL;
