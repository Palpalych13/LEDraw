#ifndef _ledlamp2_h
#define _ledlamp2_h
// ***** CUSTOM SETTINGS *****
#pragma once
#include <stdint.h>
#define EFFECTS_SET "effects/custom_gyverlamp.h"

// ----- FEATURES / ОСОБЕННОСТИ -----
#define USE_BUTTON (true) // если строка не закомментирована, должна быть подключена кнопка (иначе ESP может регистрировать "фантомные" нажатия и некорректно устанавливать яркость)
#define USE_OTA (true)		// если строка не закомментирована, модуль будет ждать два последдовательных запроса пользователя на прошивку по воздуху (см. документацию в "шапке")
#define USE_NTP (true)		// закомментировать или удалить эту строку, если нужно, чтобы устройство не лезло в интернет
#define USE_MQTT (false)	// используется или нет MQTT клиент
#define USE_IR (false)		// используется или нет пульт дистанционного управления

// ----- PROPERTIES / СВОЙСТВА -----
#define ESP_MODE (1U) // 0U - WiFi точка доступа, 1U - клиент WiFi (подключение к роутеру)

// ----- ESP I/O -----
#define BTN_PIN (4U)						 // пин кнопки               (D2)
#define PULL_MODE (HIGH_PULL)		 /*PULL_MODE -> LOW_PULL - подтяжка входа к нулю (для сенсорных кнопок на TP223) \
																							 HIGH_PULL - подтяжка входа к питанию (для механических кнопок с замыканием на массу) */
#define CONTACT_TYPE (NORM_OPEN) /*CONTACT_TYPE -> NORM_OPEN - нормально открытые (большинство механических кнопок и сенсорные кнопки TP223) \
																									 NORM_CLOSE - нормально закрытые */
#define LED_PIN (15U)						 // пин ленты                (D8) / не рекомендуется использовать D4(GPIO2), так как он может использоваться встроенным светодиодом
#define MOSFET_PIN (5U)					 // пин MOSFET транзистора   (D1) / может быть использован для управления питанием матрицы/ленты
#define ALARM_PIN (16U)					 // пин состояния будильника (D0) / может быть использован для управления каким-либо внешним устройством на время работы будильника
#define MOSFET_LEVEL (HIGH)			 // логический уровень, в который будет установлен пин MOSFET_PIN, когда матрица включена - HIGH или LOW
#define ALARM_LEVEL (HIGH)			 // логический уровень, в который будет установлен пин ALARM_PIN, когда "рассвет"/будильник включен

// ----- FASTLED -----
#define FASTLED_INTERRUPT_RETRY_COUNT (2) // default: 2; // Use this to determine how many times FastLED will attempt to re-transmit a frame if interrupted for too long by interrupts
//#define FASTLED_ALLOW_INTERRUPTS        (1)               // default: 1; // Use this to force FastLED to allow interrupts in the clockless chipsets (or to force it to disallow), overriding the default on platforms that support this. Set the value to 1 to allow interrupts or 0 to disallow them.
#define FASTLED_ESP8266_RAW_PIN_ORDER // FASTLED_ESP8266_RAW_PIN_ORDER, FASTLED_ESP8266_D1_PIN_ORDER or FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_USE_DITHERING (true)	// дизеринг
#define CURRENT_LIMIT (2500U)					// лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

// ----- MATRIX / МАТРИЦА -----
#define WIDTH 16							 // ширина матрицы
#define HEIGHT 16							 // высота матрицы
#define LEDORDER (GRB)	 // порядок цветов на ленте  GRB / RGB / BRG
#define LEDTYPE WS2812B /* тип светодиодов индикаторов APA102/DOTSTAR / TM1809/TM1812 / TM1803 / TM1804 \
															 // WS2811 / WS2812B/NEOPIXEL / WS2801 / UCS1903 / UCS2903 / LPD8806 / P9813 / SM16716  */
#define MATRIX_TYPE 0U				 // тип матрицы: 0 - зигзаг, 1 - параллельная
#define MATRIX_CONNECTION 1U	 // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define MATRIX_DIRECTION 3U		 // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
#define MATRIX_VMIRROR (false) // отразить матрицу по вертикали (0 / 1)
#define MATRIX_HMIRROR (false) // отразить матрицу по горизонтали (0 / 1)

#define PREINDICATORS 1U // количество индикаторных светодиодов подключенных перед лентой/матрицей
#if PREINDICATORS
#define INDICATOR_GENERAL 1U // порядковый номер главного индикатора (на кнопке включения) в этом отрезке. 0 - отсутствует
#endif

#define POSTINDICATORS 0U // количество индикаторных светодиодов подключенных после ленты/матрицы
#if POSTINDICATORS
#if !PREINDICATORS
#define INDICATOR_GENERAL 0U // порядковый номер главного индикатора (на кнопке включения) в этом отрезке. 0 - отсутствует
#endif
#endif

// ----- BUTTON INSTALLATIONS / НАСТРОЙКИ КНОПКИ -----
// as in the main configuration / как в основной конфигурации

//#define SUMMER_WINTER_TIME          // для тех, кому нужен переход на зимнее/летнее время - оставить строку, остальным - закомментировать или удалить
#ifndef SUMMER_WINTER_TIME
#define LOCAL_WEEK_NUM (week_t::Last) // для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_WEEKDAY (dow_t::Sun)		// для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_MONTH (month_t::Mar)		// для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_HOUR (1U)								// для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_OFFSET (3 * 60)					// смещение локального времени относительно универсального координированного времени UTC в минутах
#define LOCAL_TIMEZONE_NAME ("msk.")	// обозначение локального часового пояса; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени
#else
#define SUMMER_WEEK_NUM (week_t::Last) // номер недели в месяце, когда происходит переход на летнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define SUMMER_WEEKDAY (dow_t::Sun)		 // день недели, когда происходит переход на летнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define SUMMER_MONTH (month_t::Mar)		 // месяц, в котором происходит переход на летнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define SUMMER_HOUR (3U)							 // час (по зимнему времени!), когда заканчивается зимнее время и начинается летнее; [0..23]
#define SUMMER_OFFSET (3 * 60)				 // смещение летнего времени относительно универсального координированного времени UTC в минутах
#define SUMMER_TIMEZONE_NAME ("msk#")	 // обозначение летнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЛЕТ"
#define WINTER_WEEK_NUM (week_t::Last) // номер недели в месяце, когда происходит переход на зимнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define WINTER_WEEKDAY (dow_t::Sun)		 // день недели, когда происходит переход на зимнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define WINTER_MONTH (month_t::Oct)		 // месяц, в котором происходит переход на зимнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define WINTER_HOUR (3U)							 // час (по летнему времени!), когда заканчивается летнее время и начинается зимнее; [0..23]
#define WINTER_OFFSET (3 * 60)				 // смещение зимнего времени относительно универсального координированного времени UTC в минутах
#define WINTER_TIMEZONE_NAME ("msk*")	 // обозначение зимнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЗИМ"
#endif

// ----- BRIGHTNESS / ЯРКОСТЬ -----
#define BRIGHT (127U) // средняя яркость (0-255)

// ----- INTERFACE / ИНТЕРФЕЙС -----
#define ONETOUCH_CLOCK (false) // вывод времени при однократном нажатии
#define PRINT_TIME (6U)				 // 0U - не выводить время бегущей строкой; 1U - вывод времени каждый час; 2U - каждый час + каждые 30 минут; 3U - каждый час + каждые 15 минут
//                                                          // 4U - каждый час + каждые 10 минут; 5U - каждый час + каждые 5 минут; 6U - каждый час + каждую минуту
#define NIGHT_HOURS_START (22U * 60U) // начало действия "ночного времени" (в минутах от начала суток, 23:00), текущее время бегущей строкой будет выводиться с яркостью NIGHTBRIGHT
#define NIGHT_HOURS_STOP (8U * 60U)		// конец действия "ночного времени" (в минутах от начала суток, 7:59)

// ----- DEBUGGING -----
#define DEBUG_GENERAL (false)		// если строка не закомментирована, будут выводиться отладочные сообщения
#define DEBUG_WIFI (false)			// вывод отладочных сообщений при подключении к WiFi сети: true - выводятся, false - не выводятся; настройка не зависит от DEBUG_GENERAL
#define DEBUG_TELNET (false)		// true - отладочные сообщения будут выводиться в telnet вместо Serial порта (для удалённой отладки без подключения usb кабелем)
#define TELNET_PORT (23U)				// номер telnet порта
#define USE_TASKMONITOR (false) // используется ли монитор времени исполнения цикла loop()
/*                              // выводит каждую секунду значения среднего и максимального времени исполнения цикла loop()
                                // для тестирования времени исполнения эффектов и дополнительных функций
                                // при частоте обновления 60Гц - время прорисовки эффекта не должно превышать 15 мс! */
#endif