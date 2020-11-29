#ifndef _remote_h
#define _remote_h
#if USE_IR
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

static IRrecv irrecv(IR_PIN);

static struct remote_t // структура менеджера питания
{
	decode_results results;
	uint64_t lastcode; // Decoded value
	uint64_t repeatcode;
	uint32_t remoteTS32;
	uint8_t step = 1U;
	bool isGlobal = true;

	void setup()
	{
		irrecv.enableIRIn(); //
	}

	bool cmds_separate(uint64_t cmd) //блок комманд для разово нажатых кнопок
	{
		switch (cmd)
		{

#ifdef IR_ON
		case IR_ON:
			if (power.mode == pwr_ON)
			{
				power.change(pwr_OFF);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress OFF\n"));
#endif
			}
			else
			{
				power.change(pwr_ON);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress ON\n"));
#endif
			}
			return true;
#endif

#ifdef IR_LAMP
		case IR_LAMP:
			if (power.mode == pwr_LAMP)
			{
				power.change(pwr_ON);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress ON\n"));
#endif
			}
			else
			{
				power.change(pwr_LAMP);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress LAMP\n"));
#endif
			}
			return true;
#endif

#ifdef IR_DEMO
		case IR_DEMO:
			if (power.mode == pwr_DEMO)
			{
				power.change(pwr_ON);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress ON\n"));
#endif
			}
			else
			{
				power.change(pwr_DEMO);
#if USE_DEBUG && DEBUG_IR
				LOG.printf(PSTR("IR Cmd:\tPress DEMO\n"));
#endif
			}
			return true;
#endif

#ifdef IR_NEXT
		case IR_NEXT:
			power.change(pwr_ON);
			//eff_on_current = effect.next(pwr_ON);
#if TEXT_IR_CHANGE_EFFECT
			osd.scrollText(String(eff_on_current + 1) + ">" + effect_getFromList(eff_on_current, ON).Name(), CRGB::LightGoldenrodYellow, 5);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\tNext effect\n"));
#endif
			return true;
#endif

#ifdef IR_PREVIOUS
		case IR_PREVIOUS:
			power.change(pwr_ON);
			//eff_on_current = effects.previous(pwr_ON);
#if TEXT_IR_CHANGE_EFFECT
			osd.scrollText(String(eff_on_current + 1) + "<" + effect_getFromList(eff_on_current, ON).Name(), CRGB::PowderBlue, 5);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\tPrevious effect\n"));
#endif
			return true;
#endif

#ifdef IR_BRIGHT_DEF
		case IR_BRIGHT_DEF:
			isGlobal = !isGlobal;
#if USE_OSD
			if (isGlobal)
				osd.drawGauge(power.bright_global, 4);
			else
				osd.drawGauge(power.bright, 11);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\tReset effect brightness to default\n"));
#endif
			return true;
#endif

#if IR_PARAMS_DEF
		case IR_PARAMS_DEF:
			effect.setSpeed(effect.current->speed_def);
			flash.writePref(pref_Speed, effect.current->id, effect.speed);
			effect.setScale(effect.current->scale_def);
			flash.writePref(pref_Scale, effect.current->id, effect.scale);
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\tReset effect params to default\n"));
#endif
			return true;
#endif

#ifdef IR_INFO
		case IR_INFO:
			effect_printInfo();
			return true;
#endif

#ifdef IR_CLOCK
		case IR_CLOCK:
#if USE_OSD
			osd.printCurrentTime(CRGB::White);
#endif
			return true;
#endif

		default:
			break;
		}
		return false;
	}

	bool cmds_repeat(uint64_t cmd, uint8_t cmd_step = 1U) //блок комманд для нажатых кнопок с удерживанием
	{
		switch (cmd)
		{

#ifdef IR_BRIGHT_DOWN
		case IR_BRIGHT_DOWN:
			if (isGlobal)
			{
				power.bright_global = decGlobalBright(power.mode, cmd_step);
				power.bright_real = power.Bright();
#if USE_OSD
				osd.drawGauge(power.bright_global, 4);
#endif
			}
			else
			{
				power.bright = decBright(effect.current->id, cmd_step);
				power.bright_real = power.Bright();
#if USE_OSD
				osd.drawGauge(power.bright, 11);
#endif
			}
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Brightness Down %u\n"), cmd, cmd_step);
#endif
			return true;
#endif

#ifdef IR_BRIGHT_UP
		case IR_BRIGHT_UP:
			if (isGlobal)
			{
				power.bright_global = incGlobalBright(power.mode, cmd_step);
				power.bright_real = power.Bright();
#if USE_OSD
				osd.drawGauge(power.bright_global, 4);
#endif
			}
			else
			{
				power.bright = incBright(effect.current->id, cmd_step);
				power.bright_real = power.Bright();
#if USE_OSD
				osd.drawGauge(power.bright, 11);
#endif
			}
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Brightness Up %u\n"), cmd, cmd_step);
#endif
			return true;
#endif

#ifdef IR_SPEED_DOWN
		case IR_SPEED_DOWN:
			effect.setSpeed(decSpeed(effect.current->id, cmd_step));
#if USE_OSD
			osd.drawGauge(effect.speed, 1);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Speed Down %u\n"), cmd, cmd_step);
#endif
			return true;
		case IR_SPEED_UP:
			effect.setSpeed(incSpeed(effect.current->id, cmd_step));
#if USE_OSD
			osd.drawGauge(effect.speed, 1);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Speed Up %u\n"), cmd, cmd_step);
#endif
			return true;
#endif

#ifdef IR_SCALE_DOWN
		case IR_SCALE_DOWN:
			effect.setScale(decScale(effect.current->id, cmd_step));
#if USE_OSD
			osd.drawGauge(effect.scale, 3);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Scale Down %u\n"), cmd, cmd_step);
#endif
			return true;
#endif

#ifdef IR_SCALE_UP
		case IR_SCALE_UP:
			effect.setScale(incScale(effect.current->id, cmd_step));
#if USE_OSD
			osd.drawGauge(effect.scale, 3);
#endif
#if USE_DEBUG && DEBUG_IR
			LOG.printf(PSTR("IR Cmd:\t(%u) Scale Up %u\n"), cmd, cmd_step);
#endif
			return true;
#endif
		default:
			break;
		}
		return false;
	}

	void tick()
	{
		if (irrecv.decode(&results))
		{
			decode_type_t protocol = results.decode_type;
			uint64_t code = results.value & IR_CODEMASK;
			bool isHappen = false;
			if (protocol != UNKNOWN)
			{
				if (results.repeat)
					code = repeatcode;
				if ((millis() - remoteTS32) < IR_REPEATE_TIMEOUT)
				{
					if (step < IR_START_STEPS)
						step++;
				}
				else
				{
					lastcode = 0UL;
					step = 1U;
				}
				if (lastcode != code)
				{
					isHappen = cmds_repeat(code, (step < IR_START_STEPS ? 1U : IR_REPEATE_STEP));
					if (!isHappen)
					{
						isHappen = cmds_separate(code);
						lastcode = code;
					}
				}
				remoteTS32 = millis();
				repeatcode = code;
			}

#if USE_DEBUG && DEBUG_IR
			if ((!isHappen) && (protocol != decode_type_t::UNKNOWN))
			{
				LOG.printf(PSTR("IR:\t%s\tCommand: %u\tRepeat: %u\tCode: 0x"), typeToString(protocol).c_str(), results.command, results.repeat);
				serialPrintUint64(results.value, HEX);
				LOG.println();
			}
#endif

			irrecv.resume(); // Receive the next value
		};
	};
} remote;

//BTN.tick(); //Опрос датчика Вставить в void loop() при использовании

/*if (BTN.pressed_num)
    {
        uint32_t IRBTN = BTN.NumisClick();
        if (IRBTN < MODE_AMOUNT)
            currentMode = IRBTN;
        if (IRBTN > 1000 && IRBTN <= 1255)
        {
            modes[currentMode].Brightness = IRBTN - 1000;
            FastLED.setBrightness(modes[currentMode].Brightness);
        }
        if (IRBTN > 2000 && IRBTN <= 2255)
            modes[currentMode].Speed = IRBTN - 2000;
        if (IRBTN > 3000 && IRBTN <= 3100)
            modes[currentMode].Scale = IRBTN - 3000;
        for (uint32_t i = 1; i < 8; i++) // Будильник
            if (IRBTN > i * 10000 && IRBTN <= 2359 + i * 10000)
                alarms[i - 1].Time = IRtoTime(IRBTN - i * 10000);
        if (IRBTN > 80000 && IRBTN <= 2359 + 80000) // Будильники пн-пт
            for (uint32_t i = 1; i < 6; i++)
                alarms[i - 1].Time = IRtoTime(IRBTN - 80000);
        if (IRBTN > 90000 && IRBTN <= 2359 + 90000) // Будильники сб-вс
            for (uint32_t i = 6; i < 7; i++)
                alarms[i - 1].Time = IRtoTime(IRBTN - 90000);
        if (IRBTN > 90 && IRBTN <= 97) // Включить Будильник 91-Пн, 97-Вс
        {
            alarms[IRBTN - 91].State = true;
            while (!fillString(String(IRBTN - 90).c_str(), CRGB::Green))
            {
                delay(1);
                ESP.wdtFeed();
            }
        }

        if (IRBTN > 80 && IRBTN <= 87) // Отключить Будильник 81-Пн, 87-Вс
        {
            alarms[IRBTN - 81].State = false;
            while (!fillString(String(IRBTN - 80).c_str(), CRGB::Red))
            {
                delay(1);
                ESP.wdtFeed();
            }
        }
        if (IRBTN == 90)
            for (uint32_t i = 0; i < 7; i++) // Включить все будильники
            {
                alarms[i].State = true;
                while (!fillString("All", CRGB::Green))
                {
                    delay(1);
                    ESP.wdtFeed();
                }
            }
        if (IRBTN == 80)
            for (uint32_t i = 0; i < 7; i++) // Отключить все будильники
            {
                alarms[i].State = false;
                while (!fillString("All", CRGB::Red))
                {
                    delay(1);
                    ESP.wdtFeed();
                }
            }
        if (IRBTN == 55)
            mp3_play_time = 1; // Сказать время
        if (IRBTN >= 100 && IRBTN < 131)
            modes[EFF_VOICE].Speed = (IRBTN - 100) * 8.2; //Громкость
        if (IRBTN >= 200 && IRBTN < 231)
        {
            modes[EFF_VOICE].Scale = (IRBTN - 200) * 8.2;
            mp3_play_time = 1;
            set_adv_volume = IRBTN - 200;
        } //Громкость для голоса
        if (IRBTN == 89)
        { // Включить текущий эффект в избранное
            FavoritesManager::FavoriteModes[currentMode] = 0;
            showWarning(CRGB::Red, 1000U, 500U); // мигание цветом 0,5 секунды (1 раза)
        }
        if (IRBTN == 88)
        { // 88 - Включить режим избранное
            FavoritesManager::FavoritesRunning = true;
            FavoritesManager::UseSavedFavoritesRunning = true;
        }
        if (IRBTN == 98)
        { //  Исключить текущий эффект из избранного
            FavoritesManager::FavoriteModes[currentMode] = 1;
            showWarning(CRGB::Green, 1000U, 500U); // мигание цветом 0,5 секунды (1 раза)
        }
        if (IRBTN == 99)
        { // 99 - Отключить режим избранное
            FavoritesManager::FavoritesRunning = false;
            FavoritesManager::UseSavedFavoritesRunning = true;
        }

        if (IRBTN > 300 && IRBTN < 300 + MODE_AMOUNT)
        { //Выбор папки 301-399
            modes[EFF_VOICE].Brightness -= modes[EFF_VOICE].Brightness % count_mp3_folders;
            modes[EFF_VOICE].Brightness += IRBTN - 300;
        }
    }
    //uint8_t delta = modes[currentMode].Brightness < 10U ? 1U : 5U;
    if (BTN.UpIsPress())
        if (modes[currentMode].Brightness < 250)
            modes[currentMode].Brightness += delta;
    if (BTN.DownIsPress())
        if (modes[currentMode].Brightness > delta)
            modes[currentMode].Brightness -= delta;
    FastLED.setBrightness(modes[currentMode].Brightness);
    delta = modes[EFF_VOICE].Speed < 10U ? 1U : 5U;
    if (BTN.RightIsPress())
        if (modes[EFF_VOICE].Speed < 250)
            modes[EFF_VOICE].Speed += delta;
    if (BTN.LeftIsPress())
        if (modes[EFF_VOICE].Speed > delta)
            modes[EFF_VOICE].Speed -= delta;
    set_mp3_volume = map(modes[EFF_VOICE].Speed, 1, 255, 0, 30);

    if (BTN.OkClick())
    {
        ONflag = !ONflag;
        power.change();
        settChanged = true;
        eepromTimeout = millis();
        loadingFlag = true;
    }

    if (BTN.StarClick())
    { // Предыдущий эффукт
        currentMode = (currentMode + MODE_AMOUNT - 1) % MODE_AMOUNT;
    }
    if (BTN.DiezClick())
    { // следующий эффукт
        currentMode = (currentMode + 1) % MODE_AMOUNT;
    } */
//-------------Пример использования

//if (BTN.OkClick()) Serial.println("Нажата Ok");
//if (BTN.UpIsPress()) Serial.println("Нажата Up");
//if (BTN.DownIsPress()) Serial.println("Нажата Down");
//if (BTN.LeftIsPress()) Serial.println("Нажата Left");
//if (BTN.RightIsPress()) Serial.println("Нажата Righ");

//if (BTN.pressed_num)
//    LOG.println(PSTR("IR:^tCode of the pressed button: ") + String(BTN.NumisClick()));

/*class IRButton
{
public:
    void tick();
    uint32_t NumisClick(); // возвращает true при вводе числа. Сбрасывается после вызова
    bool UpIsPress();      // возвращает true при нажатии на кнопку.
    bool DownIsPress();    // возвращает true при нажатии на кнопку.
    bool LeftIsPress();    // возвращает true при нажатии на кнопку.
    bool RightIsPress();   // возвращает true при нажатии на кнопку.

    bool OkClick();   // возвращает true при клике Ok. Сбрасывается после вызова
    bool StarClick(); // возвращает true при клике *. Сбрасывается после вызова
    bool DiezClick(); // возвращает true при клике #. Сбрасывается после вызова
    bool pressed;
    bool pressed_num;

private:
    void clearBTN();
    uint32_t delay;
    uint32_t NUM_delay;
    uint32_t XX;
    bool XX_S = false;
    bool XX_byte = false;
    decode_results results;
    bool pressed_ok;
    bool pressed_star;
    bool pressed_diez;

    bool pressed_up;
    bool pressed_down;
    bool pressed_left;
    bool pressed_right;
    //Коды клавиш
    uint64_t up = 0xFF18E7;
    uint64_t down = 0xFF4AB5;
    uint64_t left = 0xFF10EF;
    uint64_t right = 0xFF5AA5;
    uint64_t Ok = 0xFF38C7;
    uint64_t star = 0xFF6897;
    uint64_t diez = 0xFFB04F;

    uint64_t repeat = 0xFFFFFFFFFFFFFFFF;
    uint64_t Digit[10] = {0xFF9867, 0xFFA25D, 0xFF629D, 0xFFE21D, 0xFF22DD, 0xFF02FD, 0xFFC23D, 0xFFE01F, 0xFFA857, 0xFF906F}; // 0..9
};

void IRButton::clearBTN()
{
    pressed_up = false;
    pressed_down = false;
    pressed_left = false;
    pressed_right = false;
}

void IRButton::tick()
{
    if (irrecv.decode(&results))
    {
        if (results.value == Ok)
        {
            clearBTN();
            pressed_ok = true;
            pressed = true;
        }

        if (results.value == star)
        {
            clearBTN();
            pressed_star = true;
            pressed = true;
        }

        if (results.value == diez)
        {
            clearBTN();
            pressed_diez = true;
            pressed = true;
        }

        //Up
        if (results.value == up)
        {
            clearBTN();
            pressed_up = true;
            pressed = true;
            delay = millis();
        }
        if (results.value == down)
        {
            clearBTN();
            pressed_down = true;
            pressed = true;
            delay = millis();
        }
        if (results.value == left)
        {
            clearBTN();
            pressed_left = true;
            pressed = true;
            delay = millis();
        }
        if (results.value == right)
        {
            clearBTN();
            pressed_right = true;
            pressed = true;
            delay = millis();
        }

        if (results.value == repeat)
        {
            pressed = true;
            delay = millis();
        }
        // Ввод числа
        for (int i = 0; i < 10; i++)
            if (results.value == Digit[i])
            {
                if (millis() - NUM_delay > 1000) //Если число не вводилось более секунды назад
                {
                    XX = i;
                    XX_S = true;
                    NUM_delay = millis();
                }
                else
                {
                    if (XX_S)
                        XX = XX * 10 + i;
                    NUM_delay = millis();
                }
            }
#if USE_DEBUG && DEBUG_IR
        LOG.printf(PSTR("IR:\tCode of the pressed button: "));
        serialPrintUint64(results.value, HEX);
        Serial.println();
#endif
        irrecv.resume(); // Receive the next value
    }
    //Таймоут ввода числа
    if (millis() - NUM_delay > 1000 && XX_S) //По истечении 1000ms считается окончание ввода числа
    {
        XX_S = false;
        pressed_num = true;
    }
    if (millis() - delay > 1000)
        clearBTN();
}

uint32_t IRButton::NumisClick()
{
    pressed = false;
    pressed_num = false;
    Serial.println(XX);
    return XX;
}
bool IRButton::OkClick()
{
    if (pressed && pressed_ok)
    {
        pressed = false;
        pressed_ok = false;
        return true;
    }
    return false;
}
bool IRButton::StarClick()
{
    if (pressed && pressed_star)
    {
        pressed = false;
        pressed_star = false;
        return true;
    }
    return false;
}
bool IRButton::DiezClick()
{
    if (pressed && pressed_diez)
    {
        pressed = false;
        pressed_diez = false;
        return true;
    }
    return false;
}

bool IRButton::UpIsPress()
{
    if (pressed && pressed_up)
    {
        pressed = false;
        return true;
    }
    return false;
}
bool IRButton::DownIsPress()
{
    if (pressed && pressed_down)
    {
        pressed = false;
        return true;
    }
    return false;
}
bool IRButton::LeftIsPress()
{
    if (pressed && pressed_left)
    {
        pressed = false;
        return true;
    }
    return false;
}
bool IRButton::RightIsPress()
{
    if (pressed && pressed_right)
    {
        pressed = false;
        return true;
    }
    return false;
}
//Перевод времени с IR на time
uint16_t IRtoTime(uint32_t irt)
{
    //printTime((irt % 100) + irt / 100 * 60, true, true);
    return (irt % 100) + irt / 100 * 60;
}*/

//IRButton BTN;

#endif
#endif
