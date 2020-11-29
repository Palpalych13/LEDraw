

// работа с бегущим текстом
/*
int32_t offset = (MATRIX_VMIRROR ? 0 : WIDTH);
uint32_t scrollTimer = 0LL;

bool fillStringManual(const char* text, CRGB letterColor, bool stopText)
{
  if (!text || !strlen(text))
  {
    return true;
  }

  uint16_t i = 0, j = 0;
  while (text[i] != '\0')
  {
    if ((uint8_t)text[i] > 191)                           // работаем с русскими буквами
    {
      i++;
    }
    else
    {
      if (!MATRIX_VMIRROR)
        drawLetter(text[i], offset + (int16_t)j * (LET_WIDTH + SPACE), letterColor);
      else
        drawLetter(text[i], offset - (int16_t)j * (LET_WIDTH + SPACE), letterColor);
      i++;
      j++;
    }
  }

  if (!stopText)
    (MATRIX_VMIRROR ? offset++ : offset--);
  if ((!MATRIX_VMIRROR && offset < (int32_t)(-j * (LET_WIDTH + SPACE))) || (MATRIX_VMIRROR && offset > (int32_t)(j * (LET_WIDTH + SPACE)) + WIDTH))     // строка убежала
  {
    offset = (MATRIX_VMIRROR ? 0 : WIDTH);
    return true;
  }

  return false;
}

bool fillString(const char* text, CRGB letterColor)
{
  if (!text || !strlen(text))
  {
    return true;
  }

  if (millis() - scrollTimer >= 150)
  {
    scrollTimer = millis();
    //FastLED.clear();
    uint16_t i = 0, j = 0;
    while (text[i] != '\0')
    {
      if ((uint8_t)text[i] > 191)                           // работаем с русскими буквами
      {
        i++;
      }
      else
      {
        if (!MATRIX_VMIRROR)
          drawLetter(text[i], offset + (int16_t)j * (LET_WIDTH + SPACE), letterColor);
        else
          drawLetter(text[i], offset - (int16_t)j * (LET_WIDTH + SPACE), letterColor);
        i++;
        j++;
      }
    }

    (MATRIX_VMIRROR ? offset++ : offset--);
    if ((!MATRIX_VMIRROR && offset < (int32_t)(-j * (LET_WIDTH + SPACE))) || (MATRIX_VMIRROR && offset > (int32_t)(j * (LET_WIDTH + SPACE)) + WIDTH))     // строка убежала
    {
      offset = (MATRIX_VMIRROR ? 0 : WIDTH);
      return true;
    }
    FastLED.show();
  }

  return false;
}

bool printTime(uint32_t thisTime, bool onDemand, bool ONflag, bool isManual, bool isStopped) // периодический вывод времени бегущей строкой; onDemand - по требованию, вывод текущего времени; иначе - вывод времени по расписанию; isManual == true (ручной режим)
{
#if USE_NTP && defined(PRINT_TIME)               // вывод, только если используется синхронизация времени и если заказан его вывод бегущей строкой

  if (isWifiOffMode) return false; // при отключенном WiFi - сразу на выход

  if (ESP_mode != 1U || !ntpServerAddressResolved || !timeSynched)     // вывод только в режиме WiFi клиента и только, если имя сервера времени разрезолвлено
  {
    //showWarning(CRGB::Red, 4000U, 500U);                    // мигание красным цветом 4 секунды - смена рабочего режима лампы, перезагрузка
    return false;
  }

  CRGB letterColor = CRGB::Black;
  bool needToPrint = false;

#if (PRINT_TIME >= 1U)                                    // вывод только каждый час (красным цветом)
  if (thisTime % 60U == 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Red;
  }
#endif

#if (PRINT_TIME == 2U)                                    // вывод каждый час (красным цветом) + каждые 30 минут (синим цветом)
  if (thisTime % 60U != 0U && thisTime % 30U == 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Blue;
  }
#endif

#if (PRINT_TIME == 3U)                                    // вывод каждый час (красным цветом) + каждые 15 минут (синим цветом)
  if (thisTime % 60U != 0U && thisTime % 15U == 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Blue;
  }
#endif

#if (PRINT_TIME == 4U)                                    // вывод каждый час (красным цветом) + каждые 10 минут (синим цветом)
  if (thisTime % 60U != 0U && thisTime % 10U == 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Blue;
  }
#endif

#if (PRINT_TIME == 5U)                                    // вывод каждый час (красным цветом) + каждые 5 минут (синим цветом)
  if (thisTime % 60U != 0U && thisTime % 5U == 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Blue;
  }
#endif

#if (PRINT_TIME == 6U)                                    // вывод каждый час (красным цветом) + каждую минуту (синим цветом)
  if (thisTime % 60U != 0U)
  {
    needToPrint = true;
    letterColor = CRGB::Blue;
  }
#endif

  if (onDemand)
  {
    letterColor = CRGB::White;
  }

  if ((needToPrint && thisTime != lastTimePrinted) || onDemand)
  {
    char stringTime[10U];                                   // буффер для выводимого текста, его длина должна быть НЕ МЕНЬШЕ, чем длина текста + 1
    sprintf_P(stringTime, PSTR("-> %u:%02u"), (uint8_t)((thisTime - thisTime % 60U) / 60U), (uint8_t)(thisTime % 60U));
    
    if (lampMode == MODE_DEMO && GlobalBrightness > 0)
      FastLED.setBrightness(GlobalBrightness);
    else
      FastLED.setBrightness(getBrightnessForPrintTime(thisTime, ONflag));
    delay(1);

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)        // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
    digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
#endif

    if (!isManual)
      while (!fillString(stringTime, letterColor)) {
        delay(1);
        ESP.wdtFeed();
      }
    else if (!fillStringManual(stringTime, letterColor, isStopped)) {
      return false;
    }

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)        // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы или будильника
    digitalWrite(MOSFET_PIN, ONflag || (dawnFlag && !manualOff) ? MOSFET_LEVEL : !MOSFET_LEVEL);
#endif

    lastTimePrinted = thisTime;

    return true;
  }
  return false;
#endif
}



uint8_t getBrightnessForPrintTime(uint32_t thisTime, bool ONflag)     // определение яркости для вывода времени бегущей строкой в зависимости от ESP_MODE, USE_NTP, успешности синхронизации времени,
// текущего времени суток, настроек дневного/ночного времени и того, включена ли сейчас матрица
{
#if USE_NTP && defined(PRINT_TIME)

  if (ESP_mode != 1U || !ntpServerAddressResolved || ONflag)
  {
    return effects[eff_on_current].Bright();
  }

  if (NIGHT_HOURS_START >= NIGHT_HOURS_STOP)                          // ночное время включает переход через полночь
  {
    if (thisTime >= NIGHT_HOURS_START || thisTime <= NIGHT_HOURS_STOP)// период действия ночного времени
    {
      return (NIGHTBRIGHT >= 0)
             ? NIGHTBRIGHT
             : effects[eff_on_current].Bright();
    }
  }
  else                                                                // ночное время не включает переход через полночь
  {
    if (thisTime >= NIGHT_HOURS_START && thisTime <= NIGHT_HOURS_STOP)// период действия ночного времени
    {
      return (NIGHTBRIGHT >= 0)
             ? NIGHTBRIGHT
             : effects[eff_on_current].Bright();
    }
  }

  return (DAY_HOURS_BRIGHT >= 0)                                  // дневное время
         ? DAY_HOURS_BRIGHT
         : effects[eff_on_current].Bright();

#endif

  return effects[eff_on_current].Bright();
}


void drawLetter(uint16_t letter, int16_t offset, CRGB letterColor)
{
  uint16_t start_pos = 0, finish_pos = LET_WIDTH + SPACE;

  if (offset < (int16_t) - LET_WIDTH || offset > (int16_t)WIDTH)
  {
    return;
  }
  if (offset < 0)
  {
    start_pos = (uint16_t) - offset;
  }
  if (offset > (int16_t)(WIDTH - LET_WIDTH))
  {
    finish_pos = (uint16_t)(WIDTH - offset);
  }

  for (uint16_t i = start_pos; i < finish_pos; i++)
  {
    uint8_t thisByte;

    if ((finish_pos - i <= SPACE) || ((LET_WIDTH_d - i) < 0))
      thisByte = 0x00;
    else if (MATRIX_VMIRROR)
    {
      thisByte = getFont(letter, (uint16_t)(LET_WIDTH_d - i));
    }
    else
    {
      thisByte = getFont(letter, i);
    }

    for (uint16_t j = 0; j < LET_HEIGHT; j++)
    {
      bool thisBit = MATRIX_HMIRROR
                     ? thisByte & (1 << j)
                     : thisByte & (1 << (LET_HEIGHT_d - j));

      if (TEXT_DIRECTION) {
        if (MATRIX_VMIRROR) {
          drawPixel_fast(offset - 1, TEXT_HEIGHT + j, 0x000000); // очистить предыдущий
        }
      } else {
        if (MATRIX_VMIRROR) {
          drawPixel_fast(j + TEXT_HEIGHT, offset - 1, 0x000000); // очистить предыдущий
        }
      }

      // рисуем столбец (i - горизонтальная позиция, j - вертикальная)
      if (TEXT_DIRECTION)
      {
        if (thisBit)
        {
          drawPixel_fast(offset + i, TEXT_HEIGHT + j, letterColor);
        }
        else
        {
          drawPixel_fast(offset + i, TEXT_HEIGHT + j, 0x000000);
        }
      }
      else
      {
        if (thisBit)
        {
          drawPixel_fast(j + TEXT_HEIGHT, offset + i, letterColor);
        }
        else
        {
          drawPixel_fast(j + TEXT_HEIGHT, offset + i, 0x000000); // drawPixel_fast(i, offset + TEXT_HEIGHT + j, 0x000000);
        }
      }
    }
  }
}
*/
