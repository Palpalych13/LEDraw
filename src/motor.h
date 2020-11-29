#ifndef _motor_h
#define _motor_h
#include <stdint.h>
#include "fastled_progmem.h"
#include "effects/LEDraw/LEDraw_Palettes.h"
#include "effects/LEDraw/LEDraw_Math.h"

// SIMPLE COLLECTOR MOTOR DRIVER by PalPalych / Simple Driver Library for collectors motors / Библиотека простого драйвера коллекторного моторчика
/* Особенностью этого драйвера является управлением мотором импульсами с постоянной скважностью. 
	 В отличие от ШИМ в данном драйвере реализованы 3 приемущества:
	  - отсутсвие провалов возникающее при ШИМ малой скважности 
		-	отсутствие обратного канеала для контроля частоты вращения вала 
			(работа моторчика наблюдается даже с самой маленькой скорости, для ШИМ требуется создания усровий для начала вращени вала, а так же отслеживание минимальных оборотов)	
		- фактическая работа двигателя не зависяшая от параметров внутренних PWM счетчиков
		хотя основная последовательность импульсов в цикле составляет 8, 
		драйвер позволяет регулировать скорость в пределах 256 шагов (0...255, 0 - выключено, 1...255 - скорость)
*/

#if USE_MOTOR

static struct motor_t //  структура менеджера экранной информации
{
	uint8_t speed = 0;	// полное значение регулятора скорости
	rint8_t vector = 0; //
#if MOTOR_USE_REVERSE
	bool reverse;
#else
	int8_t vector; //
#endif
	uint8_t patternTS8; // таймер ШИМ двигателя
	uint8_t actionTS8;	// таймер эффекта двигателя
	uint8_t step;
	uint8_t status; /* статус индикаторов
                    0b = полный останов мотора
                    1b = 
                    2b = 
                    3b =
										4b = 
										5b =										
                    6b = 
                    7b =  */
	uint8_t (*action)(uint8_t);
	uint8_t current_effect;
	uint8_t _random_speed;
	uint8_t _action_speed;
	bool tick();
	void init();
	void setSpeed(uint8_t);	 // устанавливает полноразмерное значение скорости и подготавливает преобразованное
	void setAction(uint8_t); // устанавливает эффект вращения двигателя
	uint8_t stepAction();		 // переключает на следующий эффект вращения двигателя
	inline void stop(bool value) { setBit(status, 0, value); };

private:
	uint8_t _step_real;
} motor;

static const uint8_t motorPattern[9] PROGMEM = { // цифры зеркально и на левом боку (так проще рисовать в циклах и экономнее для памяти)
		B00000000,																	 //Easy-reading font by PalPlalych
		B10000000,
		B10001000,
		B10010010,
		B10101010,
		B11010110,
		B11101110,
		B11111110,
		B11111111};

enum motor_effect
{
	motor_OFF,
	motor_ON,
	motor_Saw,
	motor_SawReverse,
	motor_Sinus,
	motor_Triangle,
	motor_Random,
	motor_RandomSmooth,
	motor_last
};

uint8_t (*motor_actions[])(uint8_t) = {*[](uint8_t value) { return uint8_0; },				 // 0:Direct
																			 *[](uint8_t value) { return uint8_255; },			 // 1:Direct
																			 *[](uint8_t value) { return value; },					 // 2:Saw
																			 *[](uint8_t value) { return (uint8_t)~value; }, // 3:Reverse Saw
																			 *[](uint8_t value) { return cos8(value); },		 // 4:Sinus
																			 *[](uint8_t value) { return triwave8(value); }, // 5:Triangle
																			 *[](uint8_t value) {														 // 6:Random
																				 EVERY_N_MILLIS(64)
																				 {
																					 if (!random8(15))
																						 motor._random_speed = random8();
																				 }
																				 return motor._random_speed;
																			 },
																			 *[](uint8_t value) { // 7:Random Smooth
																				 EVERY_N_MILLIS(64)
																				 {
																					 if (!random8(15))
																						 motor._random_speed = random8();
																					 else
																					 {
																						 if (motor._random_speed > motor._action_speed)
																							 motor._action_speed += ((motor._random_speed - motor._action_speed) >> 3U);
																						 if (motor._random_speed < motor._action_speed)
																							 motor._action_speed -= ((motor._action_speed - motor._random_speed) >> 3U);
																						 if (motor._action_speed == 0)
																							 motor._action_speed = 1U;
																					 }
																				 }
																				 return motor._action_speed;
																			 }};

uint8_t motor_t::stepAction() // восстанавливает полноразмерное значение скорости из преобразованного
{
	current_effect++;
	if (current_effect >= motor_last)
		current_effect = 0;
	action = motor_actions[current_effect];
	return current_effect;
}

void motor_t::setAction(uint8_t effect)
{
	current_effect = effect;
	if (effect >= motor_last)
		effect = 0;
	action = motor_actions[effect];
};

void motor_t::setSpeed(uint8_t value) // устанавливает полноразмерное значение скорости и подготавливает преобразованное
{
	speed = value;
}

void motor_t::init()
{
	pinMode(MOTOR_PIN, OUTPUT);
	digitalWrite(MOTOR_PIN, MOTOR_INVERSE);
#if MOTOR_USE_REVERSE
	pinMode(MOTOR_REVERSE_PIN, OUTPUT);
	digitalWrite(MOTOR_REVERSE_PIN, MOTOR_INVERSE);
#endif
	speed = flash.readGlobalMotorSpeed(power.mode), effects.getRegulator(reg_Brightness);
	//speed = 0;
	action = motor_actions[0];
}

bool motor_t::tick()
{
	{
		EVERY_N_MILLIS(MOTOR_PRESISTENCE)
/*#if MOTOR_USE_REVERSE
		//speed = rint8(vector) << 1U;
		speed = vector;
		//reverse = checkBit(vector, 7);
#else
		speed = vector;
#endif*/

		if (!checkBit(status, 0) || (speed && current_effect))
		{
			uint8_t act = action(actionTS8);
#if MOTOR_USE_REVERSE
			reverse = (checkBit(act, 7) != MOTOR_INVERSE);
			digitalWrite(MOTOR_REVERSE_PIN, reverse);
			_step_real = scale8(map8(rint8(act) << 1U, MOTOR_MINSPEED, MOTOR_MAXSPEED), speed);
#else
			_step_real = scale8(map8(act, MOTOR_MINSPEED, MOTOR_MAXSPEED), speed);
#endif
			uint8_t byte2 = pgm_read_byte(&motorPattern[((_step_real & 0x1C) >> 2U) + 1U]);
			uint8_t pos = (_step_real >> 5U) + ((checkBit(byte2, (patternTS8 & 0x38) >> 3U)) ? 1U : 0U);
			uint8_t byte = pgm_read_byte(&motorPattern[pos]);
			digitalWrite(MOTOR_PIN, checkBit(byte, patternTS8 & 0x7) != reverse);
			patternTS8++;

			EVERY_N_MILLIS(10)
			{
				// if (step++ > map8(getScaledRange(speed), MOTOR_EFFECTSTEP_MIN, MOTOR_EFFECTSTEP_MAX))
				if (step++ > map8(8, MOTOR_EFFECTSTEP_MIN, MOTOR_EFFECTSTEP_MAX))
				{
					actionTS8++;
					step = 0;
				};
			}
		}
		else
		{
			digitalWrite(MOTOR_PIN, MOTOR_INVERSE);
			digitalWrite(MOTOR_REVERSE_PIN, MOTOR_INVERSE);
		}
#if MOTOR_USE_REVERSE
#endif
	}
}
#endif
#endif