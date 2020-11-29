#ifndef _favorites_h
#define _favorites_h
#pragma once

/*class FavoritesManager {
  public:
    static uint8_t FavoritesRunning;                        // флаг "работает режим автоматической смены избранных эффектов"
    static uint16_t Interval;                               // статический интервал (время между сменами эффектов)
    static uint16_t Dispersion;                             // дополнительный динамический (случайный) интервал (время между сменами эффектов)
    static uint8_t UseSavedFavoritesRunning;                // флаг, определяющий, нужно ли использовать сохранённое значение FavoritesRunning при перезапуске; еслин нет, "избранное" будет выключено при старте
    static uint8_t Favoriteeffects[effs_count];              // массив, каждый элемент которого соответствует флагу "эффект №... добавлен в избранные"

    static void SetStatus(char* statusText)                 // помещает в statusText состояние режима работы избранных эффектов
    {
      char buff[6];
      strcpy_P(statusText, PSTR("FAV "));

      itoa(FavoritesRunning, buff, 10);
      strcat(statusText, buff);
      strcat_P(statusText, PSTR(" "));
      buff[0] = '\0';

      itoa(Interval, buff, 10);
      strcat(statusText, buff);
      strcat_P(statusText, PSTR(" "));
      buff[0] = '\0';

      itoa(Dispersion, buff, 10);
      strcat(statusText, buff);
      strcat_P(statusText, PSTR(" "));
      buff[0] = '\0';

      itoa(UseSavedFavoritesRunning, buff, 10);
      strcat(statusText, buff);
      strcat_P(statusText, PSTR(" "));
      buff[0] = '\0';

      for (uint8_t i = 0; i < effs_count; i++)
      {
        itoa(Favoriteeffects[i], buff, 10);
        strcat(statusText, buff);
        if (i < effs_count - 1) strcat_P(statusText, PSTR(" "));
        buff[0] = '\0';
      }

      statusText += '\0';
    }

    static void ConfigureFavorites(const char* statusText)  // принимает statusText, парсит его и инициализирует свойства класса значениями из statusText'а
    {
      FavoritesRunning = getFavoritesRunning(statusText);
      if (FavoritesRunning == 0)
      {
        nextModeAt = 0;
      }
      Interval = getInterval(statusText);
      Dispersion = getDispersion(statusText);
      UseSavedFavoritesRunning = getUseSavedFavoritesRunning(statusText);
      for (uint8_t i = 0; i < effs_count; i++)
      {
        Favoriteeffects[i] = getModeOnOff(statusText, i);
      }
    }

    static bool HandleFavorites(                            // функция, обрабатывающая циклическое переключение избранных эффектов; возвращает true, если эффект был переключен
      uint8_t* power.mode,
      uint8_t* eff_on_current,
      bool* isNewFrame()
#if USE_NTP
      , bool* dawnFlag
#endif
    )
    {
      if (FavoritesRunning == 0 ||
          *power.mode != SLEEP                                          // лампа не переключается на следующий эффект при выключенной матрице
#if USE_NTP
          || *dawnFlag                                      // лампа не переключается на следующий эффект при включенном будильнике
#endif
         )
      {
        return false;
      }

      if (nextModeAt == 0)                                  // лампа не переключается на следующий эффект сразу после включения режима избранных эффектов
      {
        nextModeAt = getNextTime();
        return false;
      }

      if (millis() >= nextModeAt)  {
        *eff_on_current = getNextFavoriteMode(eff_on_current);
        *isNewFrame() = true;
        nextModeAt = getNextTime();

#if USE_DEBUG
        LOG.printf_P(PSTR("Переключение на следующий избранный режим: %d\n\n"), (*eff_on_current));
#endif

        return true;
      }

      return false;
    }

    static void ReadFavoritesFromEeprom() {
      Interval = ReadUint16(EEPROM_FAVORITES_START_ADDRESS + 1);
      Dispersion = ReadUint16(EEPROM_FAVORITES_START_ADDRESS + 3);
      UseSavedFavoritesRunning = EEPROM.read(EEPROM_FAVORITES_START_ADDRESS + 5);
      FavoritesRunning = UseSavedFavoritesRunning > 0 ? EEPROM.read(EEPROM_FAVORITES_START_ADDRESS) : FavoritesRunning;

      for (uint8_t i = 0; i < effs_count; i++) {
        Favoriteeffects[i] = EEPROM.read(EEPROM_FAVORITES_START_ADDRESS + i + 6);
        Favoriteeffects[i] = Favoriteeffects[i] > 0 ? 1 : 0;
      }
    }

    static void SaveFavoritesToEeprom() {
      EEPROM.put(EEPROM_FAVORITES_START_ADDRESS, FavoritesRunning);
      WriteUint16(EEPROM_FAVORITES_START_ADDRESS + 1, Interval);
      WriteUint16(EEPROM_FAVORITES_START_ADDRESS + 3, Dispersion);
      EEPROM.put(EEPROM_FAVORITES_START_ADDRESS + 5, UseSavedFavoritesRunning);

      for (uint8_t i = 0; i < effs_count; i++) {
        EEPROM.put(EEPROM_FAVORITES_START_ADDRESS + i + 6, Favoriteeffects[i] > 0 ? 1 : 0);
      }

      EEPROM.commit();
    }

    static void TurnFavoritesOff() {
      FavoritesRunning = 0;
      nextModeAt = 0;
    }

  private:
    static uint32_t nextModeAt;                             // ближайшее время переключения на следующий избранный эффект (millis())

    static bool isStatusTextCorrect(const char* statusText) // валидирует statusText (проверяет, правильное ли количество компонентов он содержит)
    {
      char buff[MAX_UDP_BUFFER_SIZE];
      strcpy(buff, statusText);

      uint8_t lexCount = 0;
      char* p = strtok(buff, " ");
      while (p != NULL)                                   // пока есть лексемы...
      {
        lexCount++;
        p = strtok(NULL, " ");
      }

      return lexCount == getStatusTextNormalComponentsCount();
    }

    static uint8_t getStatusTextNormalComponentsCount()     // возвращает правильное ли коичество компонентов для statusText в зависимости от определённого формата команды и количества эффектов
    {
      // "FAV 0/1 <цифра> <цифра> 0/1 <массив цифр 0/1 для каждого режима>" (вкл/выкл, интервал в секундах, разброс в секундах, использовать ли хранимый вкл/выкл, вкл/выкл каждого эффекта в избранные)
      return
        1 +          // "FAV"
        1 +          // вкл/выкл
        1 +          // интервал
        1 +          // разброс
        1 +          // использовать ли хранимый вкл/выкл
        effs_count; // 0/1 для каждого эффекта
    }

    static uint8_t getFavoritesRunning(const char* statusText)        // возвращает признак вкл/выкл режима избранных эффектов из statusText
    {
      char lexem[2];
      memset(lexem, 0, 2);
      strcpy(lexem, getLexNo(statusText, 1));
      return lexem != NULL
             ? !strcmp(lexem, "1")
             : 0;
    }

    static uint16_t getInterval(const char* statusText)     // возвращает интервал (постоянную составляющую) переключения избранных эффектов из statusText
    {
      char lexem[6];
      memset(lexem, 0, 6);
      strcpy(lexem, getLexNo(statusText, 2));
      return lexem != NULL
             ? atoi((const char*)lexem)
             : DEFAULT_FAVORITES_INTERVAL;
    }

    static uint16_t getDispersion(const char* statusText)   // возвращает разброс (случайную составляющую) интервала переключения избранных эффектов из statusText
    {
      char lexem[6];
      memset(lexem, 0, 6);
      strcpy(lexem, getLexNo(statusText, 3));
      return lexem != NULL
             ? atoi((const char*)lexem)
             : DEFAULT_FAVORITES_DISPERSION;
    }

    static uint8_t getUseSavedFavoritesRunning(const char* statusText)// возвращает признак вкл/выкл режима избранных эффектов из statusText
    {
      char lexem[2];
      memset(lexem, 0, 2);
      strcpy(lexem, getLexNo(statusText, 4));
      return lexem != NULL
             ? !strcmp(lexem, "1")
             : 0;
    }

    static bool getModeOnOff(const char* statusText, uint8_t modeId)  // возвращает признак включения указанного эффекта в избранные эффекты
    {
      char lexem[2];
      memset(lexem, 0, 2);
      strcpy(lexem, getLexNo(statusText, modeId + 5));
      return lexem != NULL
             ? !strcmp(lexem, "1")
             : false;
    }

    static char* getLexNo(const char* statusText, uint8_t pos)        // служебная функция, разбивает команду statusText на лексемы ("слова", разделённые пробелами) и возвращает указанную по счёту лексему
    {
      if (!isStatusTextCorrect(statusText))
      {
        return NULL;
      }

      const uint8_t buffSize = MAX_UDP_BUFFER_SIZE;
      char buff[buffSize];
      memset(buff, 0, buffSize);
      strcpy(buff, statusText);

      uint8_t lexPos = 0;
      char* p = strtok(buff, " ");
      while (p != NULL)                                   // пока есть лексемы...
      {
        if (lexPos == pos)
        {
          return p;
        }

        p = strtok(NULL, " ");
        lexPos++;
      }

      return NULL;
    }

    static uint8_t getNextFavoriteMode(uint8_t* eff_on_current)  // возвращает следующий (случайный) включенный в избранные эффект
    {
      int8_t result = *eff_on_current;

      for (int8_t tryNo = 0; tryNo <= random(0, effs_count); tryNo++)// случайное количество попыток определения следующего эффекта; без этого будет выбран следующий (избранный) по порядку после текущего
      {
        for (uint8_t i = (result + 1); i <= (result + effs_count); i++)
        {
          if (Favoriteeffects[i < effs_count ? i : i - effs_count] > 0)
          {
            result = i < effs_count ? i : i - effs_count;
            break;
          }
        }
      }

      return result;
    }

    static uint32_t getNextTime()                           // определяет время следующего переключения на следующий избранный эффект
    {
      return millis() + Interval * 1000 + random(0, Dispersion + 1) * 1000;
    }
};

uint8_t FavoritesManager::FavoritesRunning = 0;
uint16_t FavoritesManager::Interval = DEFAULT_FAVORITES_INTERVAL;
uint16_t FavoritesManager::Dispersion = DEFAULT_FAVORITES_DISPERSION;
uint8_t FavoritesManager::UseSavedFavoritesRunning = 0;
uint8_t FavoritesManager::Favoriteeffects[effs_count]; // static = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t FavoritesManager::nextModeAt = 0UL;
*/
#endif