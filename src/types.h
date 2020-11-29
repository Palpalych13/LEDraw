#ifndef _types_h
#define _types_h
#include <stdint.h>

#include <FastLED.h>
#include <pixeltypes.h>
#include <effects/LEDraw/LEDraw_Math.h>
#include <effects/LEDraw/LEDraw_Palettes.h>
#include <EEPROM.h>

// ----- POWER SECTION -----
typedef enum pmode_t
{
	pwr_SLEEP,
	pwr_OFF,
	pwr_ON,
	pwr_LAMP,
	pwr_DEMO,
	pwr_PAINT,
	pwr_LOUNGE,
	pwr_last
} pmode_t;

#if USE_DEBUG
static const char *enum_pwr_mode[] PROGMEM = {"SLEEP", "OFF", "ON", "LAMP", "DEMO", "PAINT", "LOUNGE"}; //перечисление статусов режимов питания
#endif

static struct power_t // power_t структура менеджера питания
{
public:
	pmode_t mode;

	uint32_t powerTS32;									 // таймер перехода в режим SLEEP
	uint8_t bright;											 // полное значение регулятора яркости
	uint8_t bright_real;								 // действительное значение регулятора яркости
	uint8_t dimmer_leds, bright_reg;		 // действительное значение регулятора яркости
	uint8_t bright_def;									 // значение регулятора яркости по умолчанию
	uint8_t bright_fader, bright_global; // bright_real = 0,
	uint8_t bright_min, bright_max;

	uint8_t status; // статус питания \
                  0b = screen on \
                  1b = force to sleep\
                  2b = \
                  3b = \
									4b = режим минимальной яркости \
									5b = значение регулятора яркости изменено\
                  6b = \
                  7b = супердизеринг

	inline bool isScreenON() { return status & 0x01; };			 // флаг прорисовки и обновления матрицы
	inline bool isForcedToSleep() { return status & 0x02; }; // флаг начала перехода в режим SLEEP

	void forceToSleep();						 // <-- включить тайтер перехода в спящий режим
	void screenOFF(bool);						 // <-- процедура отключения экрана, питания матрицы и ее обновления
	void screenON(bool);						 // <-- процедура включения экрана, питания матрицы и ее обновления
	void change(pmode_t, uint8_t);	 // <-- изменить режим питания со своим набором эффектов, и указанным эффектом из списка режима (effect == 255 <-- текущий эффект)
	uint8_t Bright();								 // Яркость эффекта с учетом текущего состояния
	uint8_t setBrightness(uint8_t);	 // устанавливает полноразмерное значение яркости и подготавливает преобразованное
	void correctBrightness(uint8_t); // <-- корректировка диапазона яркости при смене режима
	void tick();
} power;

// ----- REGULATORS SECTION -----
enum reg_type_t
{
	reg_GlobalBrightness,
	reg_Brightness,
	reg_Speed,
	reg_Scale,
};

struct regulator_t
{
	uint8_t sectors;
	uint8_t range; // range = 256/sectors
	uint8_t subsectors;
	uint8_t effect;
	uint8_t effect_0;
	uint8_t effect_255;
	uint8_t status; // regulator status / статус регулятора \
                   0b = direction / направление
	uint8_t step(uint8_t value);
	inline bool direction() { return status & 0x01; };
	inline void changeDirection() { status ^= 0x01; };
	void setDirection(bool direction) { direction ? status |= 0x01 : status &= ~0x01; };

	CRGB color(uint8_t);
	uint8_t mode(uint8_t);
	uint8_t value(uint8_t);
	inline uint8_t maxValue();
	bool stops(uint8_t);
	bool ends(uint8_t);
	void set();
};

// ----- WIFI SECTION -----
#if USE_WIFI
struct chronometer_t // chronometer_t структура менеджера времени
{
	time_t currentTime;
	uint8_t thisDay;
	uint16_t thisTime;
	uint32_t thisTime_full;
	uint32_t secondsTS32;
	int8_t hours, minutes, seconds;
	bool isDayTime = true, lastDayTime;
	uint8_t status = B00000001; /* 	b0 - init sync
																	b1 - ready to sync */

	bool changingQuarters();
	bool changingMinutes();
	bool changingHours();
	void setCurrentTime(time_t time);
	void resolveNtpServerAddress(bool &ntpServerAddressResolved); // функция проверки подключения к интернету
	void getFormattedTime(char *buf);
	void tick();

} chronometer;
#endif
//----------------------------------------------------------------
// мини-класс таймера, версия 1.0
struct microtimer_t
{
	//microtimer_t(uint32_t);			// объявление таймера с указанием интервала
	void setInterval(uint32_t); // установка интервала работы таймера
	bool isReady();							// возвращает true, когда пришло время. Сбрасывается в false сам (AUTO) или вручную (MANUAL)
	bool isReadyManual();				// возвращает true, когда пришло время. Без сбороса
	void reset(uint32_t);				// ручной сброс таймера на установленный интервал

	uint32_t _interval;
	uint32_t _timer = 0;
};

/*microtimer_t::timerMinim(uint32_t interval)
{
	_interval = interval;
	_timer = millis();
}*/

void microtimer_t::setInterval(uint32_t interval)
{
	_interval = interval;
}

bool microtimer_t::isReady()
{
	if ((uint32_t)millis() - _timer < _interval)
		return false;
	_timer = millis();
	return true;
}

bool microtimer_t::isReadyManual()
{
	return ((uint32_t)millis() - _timer >= _interval);
}

void microtimer_t::reset(uint32_t interval = 0)
{
	if (interval)
		_interval = interval;
	_timer = millis();
}

//microtimer_t timeTimerSync(1000);

// ----- EFFECTS SECTION -----

typedef struct // Тип списка эффекта
{
	uint8_t id;
	const char *name;
	String Name();

} eff_item_t;

struct effects_list_t
{
	const eff_item_t *list;
	uint8_t size;
};

String eff_item_t::Name()
{
	return String(name);
}

typedef struct effect_t; // протопип

typedef struct // Тип описания эффекта
{
	uint8_t id; // идентификатор
#if GUYVER_SPEED_REGULATOR
	uint8_t speed_min; // минимальная скорость -> // если speed_min = speed_max то передается диапазоон 1...255,
	uint8_t speed_max; // максимальная скорость   // при этом если speed_min = 0, то передается 0...255
	uint8_t scale_min; // минимальный масштаб
	uint8_t scale_max; // максимальный масштаб
#endif
	bool (*action)(effect_t &); // вызываемая функция эффекта
	uint8_t regulators_type;		// тип индикатора/регулятора
	uint8_t bright_def;					// яркость эффекта по умолчанию (для мотора - эффект вращения) 0...255
	uint8_t speed_def;					// скорость эффекта по умолчанию
	uint8_t scale_def;					// масштаб эффекта по умолчанию
															// uint8_t motor_def;			 // масштаб эффекта по умолчанию
} eff_t;

typedef union univar_t // universal variable (just only 4 bytes in memory)
{
	CRGB color = CRGB::Black;
	uint32_t u32;
	uint32_t u32abcd;
	struct
	{
		uint8_t u8a;
		uint8_t u8b;
		uint8_t u8c;
		uint8_t u8d;
	};
	struct
	{
		int8_t i8a;
		int8_t i8b;
		int8_t i8c;
		int8_t i8d;
	};
	struct
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t alpha;
	};
	struct
	{
		uint8_t h;
		uint8_t s;
		uint8_t v;
		uint8_t a;
	};
	struct
	{
		uint16_t u16ab;
		uint16_t u16cd;
	};
	struct
	{
		int16_t i16ab;
		int16_t i16cd;
	};
};

#define LEDRAW_BACKUPS 6U // maximum count of backups variables per active effect

static struct effect_t // структура текущего эффекта
{
	//effect_t(){};
	eff_t const *current;
	uint8_t speed; // полное значение регулятора скорости
#if GUYVER_SPEED_REGULATOR
	uint16_t speed_delay; // значение регулятора скорости с обратной зависимостью
	uint8_t speed_min;		// минимальное значение регулятора скорости
	uint8_t speed_max;		// максимальное значение регулятора скорости
	uint8_t speed_real;		// действительное значение регулятора скорости
	uint8_t scale_real;		// действительное значение регулятора масштаба
#endif
	uint8_t scale;							// полное значение регулятора масштаба
	uint8_t status = B00000000; // статус эффекта \
					0b = эффект был сменён \
					1b = значение регулятора яркости изменено \
					2b = значение регулятора скорости изменено \
					3b = значение регулятора масштабв изменено \
					4b = регуляторы скорости или масштаба были изменены\
					5b = флаг типа палитры 0: rgb16   1: gp\
					6b = флаг постоянной скорости \
					7b = режим засыпания отключен

	uint16_t frame_counter; // глобальный счетчик кадров
	microtimer_t timer;			// effect timer
	CRGB *leds;

#if NUM_LEDS <= 128 //  7/8/15/16-bit index of leds array for quick calculation
	int8_t i;
	std::vector<int8_t> indexes;
#else
	uint16_t i;
	std::vector<uint16_t> indexes;
#endif
	std::vector<CRGB> colors;

	univar_t A; // universal variable A (just only 4 bytes in memory)
	univar_t B; // universal variable B (just only 4 bytes in memory)
	univar_t C; // universal variable C (just only 4 bytes in memory)
	univar_t D; // universal variable D (just only 4 bytes in memory)

	union
	{
		TProgmemRGBPalette16Ptr rgb16_palette;
		TProgmemRGBGradientPalettePtr g_palette;
	};

	inline void setPalette(TProgmemRGBPalette16Ptr);
	inline void setPalette(TProgmemRGBGradientPalettePtr);
	CRGB ColorFromPalette(uint8_t, uint8_t, TBlendType);
	void fill_palette(uint16_t, uint8_t, uint8_t, uint8_t, TBlendType);

	bool set(uint8_t);
	inline bool action() { return current->action(*this); };
	//inline bool action(effect_t &e) { return current->action(e); };

	inline bool Undelayed();
	inline bool Unsleeped();

	bool hasChangedParams();
	bool hasChangedSpeed();
	bool hasChangedScale();
	bool hasChangedEffect();

	void initBackupVariables(uint8_t);

#if GUYVER_SPEED_REGULATOR
	uint16_t Speed();							 // Скорость эффекта с учетом текущего состояния
	uint8_t setRealSpeed(uint8_t); // восстанавливает полноразмерное значение скорости из преобразованного
	uint8_t Scale();							 // Масштаб эффекта с учетом текущего состояния
	bool isNewFrame();
#endif
	uint16_t speed_phase;
	uint16_t correctPhase(accum88, accum88);
	uint8_t speedModes;
	uint8_t scaleModes;
	regulator_t *speedRegulator;
	regulator_t *scaleRegulator;

	inline uint8_t getSpeedMode();				 //
	inline uint8_t getSpeedValue();				 //
	inline uint8_t getScaleMode();				 //
	inline uint8_t getScaleValue();				 //
	inline uint8_t getSpeedMode(uint8_t);	 //
	inline uint8_t getSpeedValue(uint8_t); //
	inline uint8_t getScaleMode(uint8_t);	 //
	inline uint8_t getScaleValue(uint8_t); //

	uint8_t setSpeed(uint8_t); // устанавливает полноразмерное значение скорости и подготавливает преобразованное
	uint8_t setScale(uint8_t); // устанавливает полноразмерное значение масштаба и подготавливает преобразованное
} effect;

// ----- FADER SECTION -----
struct fader_t
{
	uint8_t current = 0;				// current position (value) of fade effect / текущее значение фэйдера
	uint32_t faderTS32;					// internal timer / внутренний таймер
	int8_t stamp;								// internal use / для внутреннего пользования
	uint8_t status = B00000000; // fader status / статус фейдера \
									 0b = direction / направление \
									 1b = reverse / реверс \
									 3b = auto unlock / автоматическое снятие с блокировки \
									 4b = 1 is finish event \
									 5b = 1 is start event \
									 6b = 1 is stop event \
									 7b = fader active  / замок
	uint8_t _duration;					// duration value (in count of 0.25s of fadiin/fadeout period) / значение времени полного открытия/закрытия с шагом 0,25с
	uint8_t _release = 0;				// release timer (in count of 0.25s) aka 0...254 = interval from 0s то 63s after as stop event has happend, 255 = handly release \
																 таймер высвобождения (с шагом 0,25с), или 0...254 = интервал от 0с до 63с после того, как произошло событие останова, 255 = ручное снятие
	uint8_t _hold = 0;
	uint8_t _value;			 // any value for internal use
	uint8_t _effect = 0; // effect value for internal use / вызываемая функция эффекта
	CRGB *leds;
	void (*fade_event)() = NULL;
	void (*start_event)() = NULL;
	void (*stop_event)() = NULL;
	void (*finish_event)() = NULL;
	inline bool done() { return !(current | status); };
	inline bool active() { return (current || status); };
	inline bool direction() { return status & 0x01; };
	inline bool reverse() { return status & 0x02; };
	inline void setReverse(bool value) { value ? status |= 0x02 : status &= ~0x02; };

	uint8_t ifRandom(uint8_t);

	bool started()
	{
		if (status & 0x20)
		{
			status &= ~0x20;
			return true;
		}
		return false;
	};

	bool finished()
	{
		if (status & 0x10)
		{
			status &= ~0x10;
			return true;
		}
		return false;
	};

	bool stopped()
	{
		if (status & 0x40)
			return false;
		faderTS32 = millis();
		stamp = 0;
		status |= 0x80;
		return true;
	};

	void set(uint8_t, uint8_t);
	void set(uint8_t, uint8_t, uint8_t, bool, uint8_t, uint8_t, CRGB *);
	void reset();

	void release(uint8_t duration025s = 0U, uint8_t release025s = 0U)
	{
		_duration = duration025s ? duration025s : _duration;
		_release = release025s;
		faderTS32 = millis();
		stamp = 0;
		status &= ~0x01;
	};

	bool tick();
};

enum faders_t
{
	fade_OnlyDimming,
	fade_Sparkler,
	fade_Comet,
	fade_Random
};

static struct effects_t // структура текущего эффекта
{
	uint8_t status = B00000000; // 	effect status / статус эффекта \
																	0b = frame has updated / кадр был сменён \
																	1b = \
																	2b = \
																	3b = \
																	4b = \
																	5b = \
																	6b = fader reverse\
																	7b = fader lock / освобождение

	fader_t fader; // fader object
#if USE_OSD2
	fader_t osd;
#endif
	uint8_t to_effect; // effect which to be changed after fade

	void fadeBrightness();
	uint8_t next(uint8_t);
	uint8_t previous(uint8_t);
	uint8_t next(uint8_t, uint8_t);
	uint8_t previous(uint8_t, uint8_t);

	eff_t get(uint8_t);
	uint8_t getIndexByID(uint8_t);
	eff_item_t getListItem(uint8_t, uint8_t);
	uint8_t getListSize(uint8_t);
	uint8_t getIndexByList(uint8_t, uint8_t);
	eff_item_t getListItemByID(uint8_t, uint8_t);

	void change(uint8_t, uint8_t, uint8_t, bool);
	void fadeout(uint8_t);
	void fadein(uint8_t, uint8_t);
	inline bool fader_reverse() { return status & 0x40; };
	inline bool isUpdated() { return status & 0x01; };
	void setUpdate(bool update) { update ? status |= 0x01 : status &= ~0x01; };
	void tick();

	regulator_t *getRegulator(reg_type_t);
	regulator_t *getSpeedRegulator(uint8_t);
	regulator_t *getScaleRegulator(uint8_t);

} effects;

enum prefs_t
{
	pref_Brightness,
	pref_Speed,
	pref_Scale,
	pref_Favorits,
	pref_Motor
};

static struct flash_t
{
	bool forceToSave;
	void readSettings();
	void defaultPrefs(bool);
	void defaultPrefs(uint16_t, bool);
	void defaultGlobalBrights(bool);
	bool checkReset();
	uint8_t readGlobalBrightness(uint8_t);
	void writeGlobalBrightness(uint8_t, uint8_t);
	uint8_t readGlobalMotorSpeed(uint8_t);
	void writeGlobalMotorSpeed(uint8_t, uint8_t);
	uint8_t readPref(prefs_t, uint8_t);
	void writePref(prefs_t, uint8_t, uint8_t);
	void writeCurrentEffect(uint8_t, uint8_t);
	uint8_t readCurrentEffect(uint8_t);
	uint8_t getCurrentEffect(uint8_t);
	void init();
	void tick();
} flash;

#if USE_OSD
static struct osd_t //  структура менеджера экранной информации
{
	uint32_t osd_scrollTimer = 0LL, osd_gaugeTimer = 0LL, osd_flashTimer = 0LL; // таймер бегущей строки
	uint16_t osd_scrollCounter = 0, osd_faderDuration = 0;
	uint32_t osd_flashPeriod;
	uint8_t osd_scrollSpeed = 100, osd_scrollStage = 0; // переменная текущeй скорости
	uint8_t osd_gaugeValue, osd_faderValue;
	uint8_t osd_gaugeStyle, osd_mode, osd_textShine = 0;
	bool osd_textFlash, osd_flash, osd_shine;

	bool drawGauge(uint8_t, uint8_t);
	bool scrollDone();
	bool scrollText(String, CRGB, // печать бегущей строки с заданным цветом
									uint8_t,			// скорость прокрутки - 10x мс для смещения на 1 пиксель
									uint8_t,			/* режим вывода (флаги) 0...3:эффект вывода, 6: переписать текст, 7:сброс после вывода
                                // мерцание - 0x01...0x07 - глубина эффекта
                                // мигание - 0x08...0x15 - период от 1/4 до 2 сек */
									uint8_t,			// стиль фейдера
									uint8_t);
	bool printCurrentTime(CRGB);
	void fillLetter(uint16_t, int16_t);
	bool fillString();
	bool tick(); // тикер вывода текста
} osd;
#endif

#endif