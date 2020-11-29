#ifndef _indicators_h
#define _indicators_h
#include <stdint.h>
#include "effects/LEDraw/LEDraw_Math.h"

/* ===== Bulbs + Meters / Indicators Module for FastLED by PalPalych v1.1b =====
*/

enum bulb_action
{
	bulb_OFF,				// ( 0) - выключения Лампочки
	bulb_ON,				// ( 1) - включение (по умолчанию белым цветом)
	bulb_Sinus,			// ( 2) - плавное изменение между цветами (Sinuswave, по умолчанию белый/черный цвет)
	bulb_Sinus2,		// ( 3) - плавное изменение между цветами по квадрату синуса (Quadwave, по умолчанию белый/черный цвет)
	bulb_Sinus3,		// ( 4) - плавное изменение между цветами по кубу синуса (Cubicwave, по умолчанию белый/черный цвет)
	bulb_Saw,				// ( 5) - пилообразное изменение между цветами, "Пульсации" (по умолчанию белый/черный цвет)
	bulb_Saw2,			// ( 6) - пилообразное изменение между цветами по параболе, "Пульсации" (по умолчанию белый/черный цвет)
	bulb_Saw3,			// ( 7) - пилообразное изменение между цветами по гиперболе, "Пульсации" (по умолчанию белый/черный цвет)
	bulb_Triangle,	// ( 8) - линейное изменение между цветами, "Оппозит" (по умолчанию белый/черный цвет)
	bulb_Triangle2, // ( 9) - линейное изменение между цветами по параболе, "Оппозит" (по умолчанию белый/черный цвет)
	bulb_Triangle3, // (10) - линейное изменение между цветами по гиперболе, "Оппозит" (по умолчанию белый/черный цвет)
	bulb_Pulse1,		// (11)	-
	bulb_Pulse2,		// (12)
	bulb_Pulse4,		// (13)
	bulb_Pulse8,		// (14)
	bulb_Blink50,		// (15)
	bulb_Blink40,		// (16)
	bulb_Blink30,		// (17)
	bulb_Blink25,		// (18)
	bulb_Blink20,		// (19)
	bulb_Blink15,		// (20)
	bulb_Blink10,		// (21)
	bulb_Blink5,		// (22)
};

typedef uint8_t duration_t;

struct bulb_t // структура менеджера одиночного индикатора
{
	CRGB *led;								 // указатель на светодиод в массиве
	uint8_t position;					 // позиция светодиода в массиве индикаторов
	CRGB forecolor, backcolor; // основной цвет и цвет фона
	uint8_t a1;								 // параметры
	uint8_t status;						 /* статус индикаторов
																0b = screwed
																1b = 
																2b = 
																3b =
																4b =  
																5b =										
																6b = 
																7b =  */
	uint32_t durationTS = 0;	 // время действия системного (временного) индикатора
	void (*action)(bulb_t *);	 // подпрограмма действия
	void update();						 // подпрограмма обновления
	bool check_Update();			 // обновление и проверка на время действие системного индикатора
	void set(bulb_action, uint8_t, CRGB, CRGB, duration_t);
	void setOFF();
	void setON(CRGB);
	void screw(CRGB *);
};

void (*ind_actions[])(bulb_t *) =
		{
				*[](bulb_t *t) { *t->led = CRGB::Black; },	// 0:OFF
				*[](bulb_t *t) { *t->led = t->forecolor; }, // 1:ON

				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, beatsin8(t->a1)); },							// 2:Sinus
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, quadwave8(beatsin8(t->a1))); },	// 3:Sinus2
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, cubicwave8(beatsin8(t->a1))); }, // 4:Sinus3

				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, beat8(t->a1)); },						 // 5:Saw
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, quadwave8(beat8(t->a1))); },	 // 6:Saw2
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, cubicwave8(beat8(t->a1))); }, // 7:Saw3

				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, triwave8(beat8(t->a1))); },						 // 8:Triangle
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, quadwave8(triwave8(beat8(t->a1)))); },	 // 9:Triangle2
				*[](bulb_t *t) { *t->led = blend(t->forecolor, t->backcolor, cubicwave8(triwave8(beat8(t->a1)))); }, // 10:Triangle3

				*[](bulb_t *t) { *t->led = ((millis() & 0xFF) > t->a1) ? t->backcolor : t->forecolor; },				 // 11:Pulse1
				*[](bulb_t *t) { *t->led = ((millis() & 0x7F) > (t->a1 >> 1U)) ? t->backcolor : t->forecolor; }, // 12:Pulse2
				*[](bulb_t *t) { *t->led = ((millis() & 0x3F) > (t->a1 >> 2U)) ? t->backcolor : t->forecolor; }, // 13:Pulse4
				*[](bulb_t *t) { *t->led = ((millis() & 0x1F) > (t->a1 >> 3U)) ? t->backcolor : t->forecolor; }, // 14:Pulse8

				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x7F)) ? t->forecolor : t->backcolor; }, // 15:Blink50
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x64)) ? t->forecolor : t->backcolor; }, // 16:Blink40
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x4B)) ? t->forecolor : t->backcolor; }, // 17:Blink30
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x3F)) ? t->forecolor : t->backcolor; }, // 18:Blink25
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x32)) ? t->forecolor : t->backcolor; }, // 19:Blink20
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x1F)) ? t->forecolor : t->backcolor; }, // 20:Blink15
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x14)) ? t->forecolor : t->backcolor; }, // 21:Blink10
				*[](bulb_t *t) { *t->led = (squarewave8(beat8(t->a1), 0x0A)) ? t->forecolor : t->backcolor; }, // 22:Blink5
};

void bulb_t::screw(CRGB *led)
{
	this->led = led;
	action = ind_actions[0];
	setBit(status, 0);
};

static struct indicators_t // структура менеджера одиночного индикатора
{
	bulb_t bulbs[INDICATORS]; // массив ламп
#if PREINDICATORS
	bulb_t *generalBulb = &bulbs[INDICATOR_GENERAL - 1];
#else
#if POSTINDICATORS
	bulb_t *generalBulb = &bulbs[PREINDICATORS + INDICATOR_GENERAL - 1];
#else
	bulb_t *generalBulb = &bulbs[0];
#endif
#endif
	bulb_t spareBulb;	 // резервная лампочка (для краткосрочной подмены эффекта)
	bulb_t systemBulb; // резервная системная лампочка (для краткосрочной подмены эффекта)
	uint8_t status;		 /* статус индикаторов
                    0b = systemBulb_busy
                    1b = 
                    2b = 
                    3b =
										4b =  
										5b =										
                    6b = 
                    7b = inDaLoop */

	bulb_t screw_Bulb(uint8_t id, CRGB *led)
	{
		bulbs[id].screw(led);
		return bulbs[id];
	};

	void screw_SpareBulb(uint8_t position) // заменить лампочку резервной
	{
		spareBulb = bulbs[position];
	};

	void unscrew_SpareBulb(uint8_t position) // извлечь резервную лампочку
	{
		bulbs[position] = spareBulb;
	};

	void screw_SystemBulb(uint8_t position) // заменить лампочку резервной
	{
		systemBulb = bulbs[position];
		setBit(status, 0);
	};

	void screw_SystemBulb(bulb_t bulb) // заменить лампочку резервной
	{
		systemBulb = bulb;
		setBit(status, 0);
	};

	void unscrew_SystemBulb() // извлечь резервную лампочку
	{
		bulbs[systemBulb.position] = systemBulb;
		clearBit(status, 0);
	};

	bool systemBulb_busy() // извлечь резервную лампочку
	{
		return (status & 0x01);
	};

	void tick()
	{
		for (uint8_t i = 0; i < INDICATORS; i++)
			if (bulbs[i].check_Update())
				unscrew_SystemBulb();
	}
} indicators;

void bulb_t::update()
{
	action(this);
};

bool bulb_t::check_Update()
{
	if (checkBit(status, 0))
	{
		action(this);
		if (durationTS)
			if (millis() > durationTS)
			{
				durationTS = 0;
				return true;
			}
	}
	return false;
}

void bulb_t::set(bulb_action action, uint8_t bpm, CRGB foreColor = CRGB::White, CRGB backColor = CRGB::Black, uint8_t duration = 0)
{
	if (!durationTS)
	{
		if (duration)
		{
			indicators.screw_SystemBulb(position);
			durationTS = millis() + duration * 100;
			//Serial.printf("durationTS %u\n", duration * 100);
		}
		else
			durationTS = 0;
		this->forecolor = foreColor;
		this->backcolor = backColor;
		this->a1 = bpm;
		this->action = ind_actions[action];
	}
}

void bulb_t::setOFF() // also works in non-loop routines
{
	*led = CRGB::Black;
	forecolor = CRGB::Black;
	backcolor = CRGB::Black;
	a1 = 0;
	action = ind_actions[0];
}

void bulb_t::setON(CRGB foreColor = CRGB::White) // also works in non-loop routines
{
	*led = foreColor;
	forecolor = foreColor;
	backcolor = CRGB::Black;
	a1 = 0;
	action = ind_actions[1];
}

// ----- REGULATORS SECTION -----
// *** Customization array for Color Effects ***
static CRGB (*regulator_actions[])(uint8_t) = {
		*[](uint8_t value) { return (CRGB)CRGB::Black; },																// 0:	typicaly use for
		*[](uint8_t value) { return (CRGB)CRGB::White; },																// 1:	typicaly use for Global Brightness
		*[](uint8_t value) { return (CRGB)CRGB::Cyan; },																// 2:	typicaly use for Speed Regulator
		*[](uint8_t value) { return (CRGB)CRGB::Magenta; },															// 3:	typicaly use for Scale Regulator
		*[](uint8_t value) { return (CRGB)CRGB::Yellow; },															// 4:	typicaly use for Brightness Regulator
		*[](uint8_t value) { return CRGB(CHSV(value, 255, 255)); },											// 5:
		*[](uint8_t value) { return CRGB(CHSV(~value, 255, 255)); },										// 6:
		*[](uint8_t value) { return CRGB(CHSV(random8(), 255, 255)); },									// 7:
		*[](uint8_t value) { return CRGB(CHSV(effect.scale, value, 255)); },						// 8:
		*[](uint8_t value) { return (CRGB)((value < 128U) ? CRGB::Blue : CRGB::Red); }, // 9:	typicaly use for Motor Regulator with reverse
};

uint32_t regulatorTS32;

uint8_t regulator_t::step(uint8_t value) // <-- шаговое изменение величины нажатой кнопки
{
	EVERY_N_MILLIS(qadd8(16U, qmul8(sectors, 8U)))
	{
		uint16_t time = millis() - regulatorTS32;
		if (time > BUTTON_HOLD_TIMEOUT && stops(value))
				changeDirection();

		if (ends(value))
		{
			if (time > BUTTON_REVERSE_TIMEOUT)
			{
				uint8_t max_value = maxValue();
				if (!value)
				{
					setDirection(true);
					value++;
				}
				if (value >= max_value)
				{
					value = max_value;
					setDirection(false);
					value--;
				}
			}
			indicators.generalBulb->set(bulb_Sinus, 75, color(value));
		}
		else if (stops(value))
		{
			if (time > BUTTON_RANGE_TIMEOUT)
			{
				direction() ? value++ : value--;
				indicators.generalBulb->setON(color(value));
				regulatorTS32 = millis();
			}
			else
				indicators.generalBulb->setON(color(value) % 32U);
		}
		else
		{
			direction() ? value++ : value--;
			regulatorTS32 = millis();
			indicators.generalBulb->set(bulb_Pulse4, value, color(value));
		}
	}
	return value;
}

uint8_t regulator_t::mode(uint8_t of_value)
{
	return (!range) ? 1U : of_value / range;
};

uint8_t regulator_t::value(uint8_t of_value)
{
	return (!range) ? of_value : qmul8(mod8(of_value, range), sectors);
};

inline uint8_t regulator_t::maxValue()
{
	return (sub8(mul8(range, sectors), 1U));
};

bool regulator_t::stops(uint8_t of_value)
{

	return (!range) ? false : (addmod8(of_value, 1U, range) < 2U);

	//uint8_t mod = mod8(of_value, range);
	//return ((!mod) || (mod == sub8(range, 1U)));
};

bool regulator_t::ends(uint8_t of_value)
{
	return (!of_value || of_value == maxValue());
};

void regulator_t::set(){};

CRGB regulator_t::color(uint8_t value)
{
	if ((effect_0) && (!value))
		return regulator_actions[effect_0](value);
	if ((effect_255) && (value == 255))
		return regulator_actions[effect_255](value);
	return regulator_actions[effect](value);
};

// *** Customization array for types of Regulators ***
//	params of single regulator -> {1: # of Sectors, \
																	 2: Range оf each Sector, \
												  				 3: Subsectors (or Steps of single Sector), \
																	 4: code of Color Effect, 5: code of Color Effect if value is 0, 5: code of Color Effect if value is max (=255)}
static regulator_t regulators[] = {
		(regulator_t){1, 0, 0, 0, 0, 0},	 // 0:	OFF
		(regulator_t){1, 0, 0, 1, 0, 0},	 // 1:	Global Brightness (default)
		(regulator_t){1, 0, 0, 4, 0, 0},	 // 2:	Brightness/Motor Speed (default)
		(regulator_t){1, 0, 0, 2, 0, 0},	 // 3:	Speed (default)
		(regulator_t){1, 0, 0, 3, 0, 0},	 // 4:	Scale (default)
		(regulator_t){1, 0, 0, 5, 0, 0},	 // 5:	Rainbow
		(regulator_t){1, 0, 0, 8, 0, 0},	 // 6:	DesaturateScale
		(regulator_t){2, 128, 0, 2, 0, 0}, // 7:	2 Sectors / Speed
		(regulator_t){4, 64, 0, 2, 0, 0},	 // 8:	4 Sectors / Speed
		(regulator_t){8, 32, 0, 2, 0, 0},	 // 9:	8 Sectors / Speed
		(regulator_t){2, 128, 0, 3, 0, 0}, // 10:	2 Sectors / Scale
		(regulator_t){4, 64, 0, 3, 0, 0},	 // 11:	4 Sectors / Scale
		(regulator_t){8, 32, 0, 3, 0, 0},	 // 12:	8 Sectors / Scale
};

// *** Customization array for Sets of Regulator types ***
//	type of -> {1: Global Bright, 2: Brightness, 3: Speed, 4: Scale}
static const uint8_t regulators_set[][4] = {
		{1, 2, 3, 4},	 // 0: Base
		{1, 2, 6, 5},	 // 1: Speed = DesaturateScale, Scale = Rainbow
		{1, 2, 8, 12}, // 2: Speed = 4 Sectors, Scale = 8 Sectors
		{1, 2, 3, 12}, // 3: Speed = 1 Sectors, Scale = 8 Sectors
		{1, 2, 7, 12}, // 4: Speed = 2 Sectors, Scale = 8 Sectors
};

#endif
