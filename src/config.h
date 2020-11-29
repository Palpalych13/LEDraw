// ***** LEDLAMP SETTINGS ***** last mod v1.7.14b
#ifndef _constants_h
#define _constants_h
#pragma once
#include <stdint.h>

// ----- LIBRARIES / БИБЛИОТЕКИ -----
#include <pgmspace.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

// ----- FEATURES / ОСОБЕННОСТИ ----- last mod v1.7.21b
#define USE_BUTTON (true) // если строка не закомментирована, должна быть подключена кнопка (иначе ESP может регистрировать "фантомные" нажатия и некорректно устанавливать яркость)
#define USE_WIFI (true)		// используется WiFi
#define USE_OTA (true)		// если строка не закомментирована, модуль будет ждать два последдовательных запроса пользователя на прошивку по воздуху (см. документацию в "шапке")
#define USE_NTP (true)		// закомментировать или удалить эту строку, если нужно, чтобы устройство не лезло в интернет
#define USE_MQTT (false)	// используется или нет MQTT клиент
#define USE_IR (false)		// используется или нет пульт дистанционного управления
#define USE_OSD (true)		// используется или нет вывод текста и индикаторов на матричный экран
#define USE_MOTOR (false) // используется или нет моторчик
#define USE_DEBUG (true)	// если строка не закомментирована, будут выводиться отладочные сообщения

// ----- CUSTOMIZATION / МОДИФИЦИРОВАННЫЕ ПРОШИВКИ ----- last mod v1.7.17b
// ----- ATTENTION, ВНИМАНИЕ, ВАЖНО!!! -----
// для организации пользовательских настроек требуется наличие конфигурационного файла с переписанными или измененными дефайнами из этого в необновляемой папке "custom/"
// можно создавать сколько угодно под различные похожие проекты с индивидуальными характеристиками такими как, разрешение и тип матрицы, работа с инетом, MQTT, пульом...
// первоначально нужно скопировать содержимое папки "custom/" в свой рабочий каталог с прошивкой
// при поседующем обновении прошивки копировать эту папку не нужно!

#define CUSTOMIZATION 11

#if (CUSTOMIZATION == 1)
#define USE_CUSTOMIZATION "custom/config_LedLamp1.h" // файл с индивидуальными настройками лампы / закомменчивание отключает индивидуальные настроки
#define USE_LOCALIZATION "localization/ru.h"
#define EFFECTS_SET "effects/custom_effects_gyverlamp.h"
#endif
#if (CUSTOMIZATION == 2)
#define USE_CUSTOMIZATION "custom/config_LedLamp2.h"
#define USE_LOCALIZATION "localization/ru.h"
#define EFFECTS_SET "effects/custom_effects_gyverlamp.h"
#endif
#if (CUSTOMIZATION == 3)
#define USE_CUSTOMIZATION "custom/config_LedLamp3.h"
#define USE_LOCALIZATION "localization/ru.h"
#define EFFECTS_SET "effects/custom_effects_gyverlamp.h"
#endif
#if (CUSTOMIZATION == 10)
#define USE_CUSTOMIZATION "custom/config_Disco1d.h"
#define USE_LOCALIZATION "localization/ru.h"
#define EFFECTS_SET "effects/custom_effects_disco1d.h"
#define USE_REMOTECONTROL "ir/HP Pavilion CP04 PC6.h"
#endif
#if (CUSTOMIZATION == 11)
#define USE_CUSTOMIZATION "custom/config_garland1.h"
#define USE_LOCALIZATION "localization/ru.h"
#define EFFECTS_SET "effects/custom_effects_disco1d.h"
#define USE_REMOTECONTROL "ir/SONY CarStereo RMX-166.h"
#endif

// ----- PROPERTIES / СВОЙСТВА ----- last mod v1.7.02b
#define ESP_MODE 1U // 0U - WiFi точка доступа, 1U - клиент WiFi (подключение к роутеру)

// ----- ESP I/O ----- last mod v1.7.21b
#define BTN_PIN 4U							 // пин кнопки               (D2)
#define PULL_MODE (LOW_PULL)		 //PULL_MODE -> LOW_PULL - подтяжка входа к нулю (для сенсорных кнопок на TP223) \
																								HIGH_PULL - подтяжка входа к питанию (для механических кнопок с замыканием на массу)
#define CONTACT_TYPE (NORM_OPEN) //CONTACT_TYPE -> NORM_OPEN - нормально открытые (большинство механических кнопок и сенсорные кнопки TP223) \
																									 NORM_CLOSE - нормально закрытые
#define LED_PIN 15U							 // пин ленты                (D8) / не рекомендуется использовать D4(GPIO2), так как он может использоваться встроенным светодиодом
#define MOSFET_PIN 5U						 // пин MOSFET транзистора   (D1) / может быть использован для управления питанием матрицы/ленты
#define MOSFET_LEVEL (HIGH)			 // логический уровень, в который будет установлен пин MOSFET_PIN, когда матрица включена - HIGH или LOW
#define ALARM_PIN 16U						 // пин состояния будильника (D0) / может быть использован для управления каким-либо внешним устройством на время работы будильника
#define ALARM_LEVEL (HIGH)			 // логический уровень, в который будет установлен пин ALARM_PIN, когда "рассвет"/будильник включен
#define IR_PIN 0U								 // Пин, куда подключет IR   (D3)
#define MOTOR_PIN 13U						 // пин мотрчика  					 (13U)
#define MOTOR_INVERSE (false)		 // инвертирование сигналов на пине
#define MOTOR_USE_REVERSE (true) // используется или нет двунаправленный драйвер мотора типа FAN8082, MX1508, L298N...
#define MOTOR_REVERSE_PIN 12U		 // пин реверса мотрчика 		 (12U)

// ----- FASTLED ----- last mod v1.7.02b
#define FASTLED_INTERRUPT_RETRY_COUNT 0		// default: 2; // Use this to determine how many times FastLED will attempt to re-transmit a frame if interrupted for too long by interrupts
#define FASTLED_ALLOW_INTERRUPTS 0				// default: 1; // Use this to force FastLED to allow interrupts in the clockless chipsets (or to force it to disallow), overriding the default on platforms that support this. Set the value to 1 to allow interrupts or 0 to disallow them.
#define FASTLED_ESP8266_RAW_PIN_ORDER			// FASTLED_ESP8266_RAW_PIN_ORDER, FASTLED_ESP8266_D1_PIN_ORDER or FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_USE_DITHERING (true)			// использовать дизеринг?
#define FASTLED_DITHERING_OVERSCAN (true) // многократная перерисовка кадра?
#define CURRENT_LIMIT 2500U								// лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

// ----- LEDS ARRAY / МАССИВ СВЕТОДИОДОВ ----- last mod v1.7.21b
#define STRIPE 0
#define MATRIX 1
#define RINGS 2

#define LEDORDER (GRB)				// color order / порядок цветов GRB / RGB / BRG
#define LEDTYPE (WS2812B)			// type of leds / тип светодиодов индикаторов APA102|DOTSTAR / TM1809|TM1812 / TM1803 / TM1804 \
														 		  WS2811 / WS2812B|NEOPIXEL / WS2801 / UCS1903 / UCS2903 / LPD8806 / P9813 / SM16716
#define LEDSARRAY_TYPE MATRIX // type of leds array / тип массива светодиодов 0 = STRIPE / 1 = MATRIX / 2 = (RINGS)

// ----- STRIPE / ПОЛОСА ----- last mod v1.7.21b
#if (LEDSARRAY_TYPE == STRIPE)
#define NUM_LEDS 32 // длина ленты матрицы
#endif

#if (LEDSARRAY_TYPE == MATRIX)
// ----- MATRIX / МАТРИЦА ----- last mod v1.7.08b
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/
// также есть возможность установить перед или после ленты/матрицы дополнительные светодиоды, которые будут служить индикаторами режимов работы
// главное, чтобы эти светодиоды имели одинаковый LEDORDER и LEDTYPE
#define WIDTH 16							 // ширина матрицы
#define HEIGHT 16							 // высота матрицы
#define MATRIX_TYPE 0U				 // тип матрицы: 0 - зигзаг, 1 - параллельная
#define MATRIX_CONNECTION 1U	 // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define MATRIX_DIRECTION 3U		 // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
#define MATRIX_VMIRROR (false) // отразить матрицу по вертикали (false / true)
#define MATRIX_HMIRROR (false) // отразить матрицу по горизонтали (false / true)
#endif

#define PREINDICATORS (0) // количество индикаторных светодиодов подключенных перед лентой/матрицей
#if PREINDICATORS
#define INDICATOR_GENERAL (1) // порядковый номер главного индикатора (на кнопке включения) в этом отрезке. 0 - отсутствует
#endif

#define POSTINDICATORS (0) // количество индикаторных светодиодов подключенных после ленты/матрицы
#if POSTINDICATORS
#if !PREINDICATORS
#define INDICATOR_GENERAL (0) // порядковый номер главного индикатора (на кнопке включения) в этом отрезке. 0 - отсутствует
#endif
#endif

// ----- BUTTON INSTALLATIONS / НАСТРОЙКИ КНОПКИ ----- last mod v1.7.20b
#if USE_BUTTON
#define BUTTON_TIMEOUT 400U					 // установка таймаута удержания (по умолчанию 300 мс)
#define BUTTON_STEP_TIMEOUT 500U		 // установка таймаута между прирастаниями (по умолчанию 400 мс) (для регулировки яркости)
#define BUTTON_CLICK_TIMEOUT 500U		 // установка таймаута между кликами (по умолчанию 500 мс)
#define BUTTON_HOLD_TIMEOUT 1000U		 // время запоминания количества кликов последнего удерживания
#define BUTTON_RANGE_TIMEOUT 750U		 // время задержки защитного промежутка между секторами регулятора (по сути двойной - на конце одного и в начале следующего)
#define BUTTON_REVERSE_TIMEOUT 1500U // время срабатывания индикатора на реверс
#endif															 //
#define BUTTON_GAUGE_TIMEOUT 1000U	 // время отоброжения индикатора

// ----- NET CONFIGURATION/FLASHING / КОНФИГУРАЦИЯ СЕТИ/ПРОШИВКА ----- last mod v1.7.08b
#define NETNAME_PREFIX "LedLamp"								 // имя WiFi точки доступа (до 15 символов), используется как при запросе SSID и пароля WiFi сети роутера, так и при работе в режиме ESP_MODE = 0
#define NET_PASSWORD "31415926"									 // пароль WiFi точки доступа
#define MAX_UDP_BUFFER_SIZE 129U								 // максимальный размер буффера UDP сервера
#define ESP_HTTP_PORT 80U												 // номер порта, который будет использоваться во время первой утановки имени WiFi сети (и пароля), к которой потом будет подключаться лампа в режиме WiFi клиента (лучше не менять)
#define ESP_UDP_PORT 8888U											 // номер порта, который будет "слушать" UDP сервер во время работы лампы как в режиме WiFi точки доступа, так и в режиме WiFi клиента (лучше не менять)
const uint8_t STA_STATIC_IP[] = {};							 // статический IP адрес: {} - IP адрес определяется роутером; {192, 168, 1, 66} - IP адрес задан явно (если DHCP на роутере не решит иначе); должен быть из того же диапазона адресов, что разадёт роутер
const uint8_t AP_STATIC_IP[] = {192, 168, 4, 1}; // статический IP точки доступа (лучше не менять)
#define ESP_CONN_TIMEOUT 15U										 // время в секундах, которое ESP будет пытаться подключиться к WiFi сети, после его истечения автоматически развернёт WiFi точку доступа
#define ESP_CONF_TIMEOUT 300U										 // время в секундах, которое ESP будет ждать ввода SSID и пароля WiFi сети роутера в конфигурационном режиме, после его истечения ESP перезагружается
#if USE_OTA
#define ESP_OTA_PORT 8266U			 // номер порта, который будет "прослушиваться" в ожидании команды прошивки по воздуху
#define CONFIRMATION_TIMEOUT 30U // время в сеундах, в течение которого нужно дважды подтвердить старт обновлениЯ по воздуху (иначе сброс в None)
#endif

// ----- ВНЕШНЕЕ УПРАВЛЕНИЕ ----- last mod v1.7.15b
#if USE_MQTT
#define MQTT_RECONNECT_TIME 10U			// время в секундах перед подключением к MQTT брокеру в случае потери подключения
#define MQTT_SERVER "192.168.1.128" // строка с IP адресом MQTT брокера
#define MQTT_PORT 1883U							// порт MQTT брокера
#define MQTT_USER ""								// пользователь MQTT брокера
#define MQTT_PASSWORD ""						// пароль пользователя MQTT брокера
#define MQTT_BASETOPIC "LedLamp"		// базовая часть топиков
#define MQTT_COMMANDTOPIC "cmnd"		// часть командных топиков (входящие команды лампе)
#define MQTT_STATETOPIC "state"			// часть топиков состояния (ответ от лампы)
#endif

// ----- TIME / ВРЕМЯ ----- last mod v1.7.02b
#if USE_NTP
#define RESOLVE_INTERVAL (5UL * 60UL * 1000UL) // интервал проверки подключения к интеренету в миллисекундах (5 минут)
// при старте ESP пытается получить точное время от сервера времени в интрнете
// эта попытка длится RESOLVE_TIMEOUT
// если при этом отсутствует подключение к интернету (но есть WiFi подключение),
// модуль будет подвисать на RESOLVE_TIMEOUT каждое срабатывание таймера, т.е., 3 секунды
// чтобы избежать этого, будем пытаться узнать состояние подключения 1 раз в RESOLVE_INTERVAL (5 минут)
// попытки будут продолжаться до первой успешной синхронизации времени
// до этого момента функции будильника работать не будут
// интервал последующих синхронизаций времени определяён в NTP_INTERVAL (30 минут)
// при ошибках повторной синхронизации времени функции будильника отключаться не будут
#define RESOLVE_TIMEOUT (1500UL) // таймаут ожидания подключения к интернету в миллисекундах (1,5 секунды)

#define NTP_ADDRESS ("ntp2.colocall.net")		// сервер времени
#define NTP_INTERVAL (30UL * 60UL * 1000UL) // интервал синхронизации времени (30 минут)
#endif

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

// ----- BRIGHTNESS / ЯРКОСТЬ ----- last mod v1.7.21b
#define BRIGHT 127U								// средняя яркость (0-255)
#define BRIGHT_INDICATORS_MIN 24U // минимальная яркость индикаторов
#define BRIGHT_STEPTIME 2U				// шаг изменения плавного регулятора яркости в мс
#define FADER_DURATION 12U				// время действия фэйдера (в четвертях секунды, для полного цикла - в полусекундах)
#define FADER_DEMO_DURATION 8U		// время действия фэйдера демо-режима (в четвертях секунды, для полного цикла - в полусекундах)

// ----- INTERFACE / ИНТЕРФЕЙС ----- last mod v1.7.16b
#define TEXT_BTN_CHANGE_EFFECT (true) // вывод имени эффекта при смене его кнопкой
#define TEXT_IR_CHANGE_EFFECT (true)	// вывод имени эффекта при смене его с пульта
#define ONETOUCH_CLOCK (true)					// вывод времени при однократном нажатии
#define PRINT_TIME (6U)								// 0U - не выводить время бегущей строкой; 1U - вывод времени каждый час; 2U - каждый час + каждые 30 минут; 3U - каждый час + каждые 15 минут
//                                                          // 4U - каждый час + каждые 10 минут; 5U - каждый час + каждые 5 минут; 6U - каждый час + каждую минуту
#define NIGHT_HOURS_START (22U * 60U) // начало действия "ночного времени" (в минутах от начала суток, 23:00), текущее время бегущей строкой будет выводиться с яркостью NIGHTBRIGHT
#define NIGHT_HOURS_STOP (8U * 60U)		// конец действия "ночного времени" (в минутах от начала суток, 7:59)

// ----- FAVORITES / ФАВОРИТЫ ----- last mod v1.7.02b
#define DEFAULT_FAVORITES_INTERVAL (300U) // значение по умолчанию для интервала переключения избранных эффектов в секундах
#define DEFAULT_FAVORITES_DISPERSION (0U) // значение по умолчанию для разброса интервала переключения избранных эффектов в секундах

// ----- DAWN / РАССВЕТ ----- last mod v1.7.02b
#define DAWN_BRIGHT (200U)	 // максимальная яркость рассвета (0-255)
#define DAWN_TIMEOUT (1U)		 // сколько рассвет светит после времени будильника, минут
#define NUMHOLD_TIME (1500U) // время отображения индикатора уровня яркости/скорости/масштаба в мс
#define DEMOTIME (30U)			 // в секундах
#define RANDOM_DEMO (1)			 // 0, 1 - рандомный выбор режима демо

// ----- DEBUGGING ----- last mod v1.7.21b
#if USE_DEBUG
#define DEBUG_WIFI (true)				// вывод отладочных сообщений при подключении к WiFi сети: true - выводятся, false - не выводятся; настройка не зависит от USE_DEBUG
#define DEBUG_IR (true)					// вывод отладочных сообщений при работе с пультом: true - выводятся, false - не выводятся; настройка не зависит от USE_DEBUG
#define DEBUG_TELNET (false)		// true - отладочные сообщения будут выводиться в telnet вместо Serial порта (для удалённой отладки без подключения usb кабелем)
#define TELNET_PORT (23U)				// номер telnet порта
#define USE_TASKMONITOR (false) // используется ли монитор времени исполнения цикла loop()
/*                              // выводит каждую секунду значения среднего и максимального времени исполнения цикла loop()
                                // для тестирования времени исполнения эффектов и дополнительных функций
                                // при частоте обновления 60Гц - время прорисовки эффекта не должно превышать 15 мс! */
#endif
// ----- BRIGHTNESS LIST ----- last mod v1.7.19b
typedef struct pwr_bright_t // pwr_bright_t структура шкалы яркости
{
	uint8_t night_min;
	uint8_t night_max;
	uint8_t day_min;
	uint8_t day_max;
};

static const pwr_bright_t pwr_bright[] = {
		// установка ночных/дневных диапазонов яркости для кажлого профиля
		{0, 0, 0, 0},			// SLEEP
		{0, 8, 4, 96},		// OFF
		{4, 96, 12, 160}, // ON
		{4, 96, 12, 160}, // LAMP
		{4, 96, 12, 160}, // DEMO
		{4, 96, 12, 160}	// PAINT
};
#endif
