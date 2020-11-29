#include <stdint.h>
/*  Скетч к проекту "Многофункциональный RGB светильник"
    Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
    Исходники на GitHub: https://github.com/AlexGyver/GyverLamp/
    Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
    Автор: AlexGyver, AlexGyver Technologies, 2019 https://AlexGyver.ru/
    // Ссылка для менеджера плат:
    // https://arduino.esp8266.com/stable/package_esp8266com_index.json

    Данная модификация основана: на версии 1.4 от gunner47 https://github.com/gunner47/GyverLamp
                                 на версии 1.55 от kDn
                                 на версии 1.6 доработок пользователей 
																 */
/*  --- Доработки ---
v1.7.00b (20.03.2020) - by PalPalych

v1.7.01b (22.03.2020) - by PalPalych
	- исправлены некоторые эффекты; переназначен пин ленты (конфликт со встроенным светодиоодом);
	- повторное удержание до 1,5сек не вызывает перехода в 0 режим / короткая пауза до 0,75с - "довесок" (для точной регулировки) без реверса
	- восстановлен MQTT; дневная/ночная яркость;

v1.7.02b (25.03.2020) - by PalPalych
	- добавлена локализация интерфейса (constants_localization.h),
	- ведение изменений в настройках по группам для отслеживания в уже существующих релизах,
	- приветственная фраза при включении,
	- режим "Лампа" (включение - удерживание кнопки из режимов OFF/SLEEP, выключение - 1 нажатие из режима LAMP)
	- поправлен режим дизеринга FastLED - для точной прорисовки оттенков при малой яркости матрицы
	- поправлены эффекты, в т.ч. Радужня комета

v1.7.03b (25.03.2020) - by PalPalych
	- реализован вывод на экран IP (из режимаа OFF/SLEEP) и FPS (из режимаа ON)
	- единый Hostname для всех типов сетей LedLamp_XXXXXX, где XXXXXX - id контроллера
	- отключен OneTouch Clock для ESP_MODE = 0

v1.7.04b (26.03.2020) - by PalPalych
	- поправлены значки в тексте перехода эффектов/бэкграунла
	- вертикальная и горизонтальные радуги (новый движок)

v1.7.05b (27.03.2020) - by PalPalych
	- работают Снегопад и Матрица
	- добавлен эффект Пульс
	- добавлены эффекты от SottNick: Спирали, Бассейн, Нестыдная Матрица и Прыгающие Мячики

v1.7.06b (30.03.2020) - by PalPalych
	- переход на PlatformIO (с теоретической возможностью работы в Arduino IDE)
	- поправлен загрузчик. Расставлены защитные интервалы между критичными вызовами процедур
	- увеличен FPS (до 100 на самых быстрых эффектах). Теоретически - лучшая работа с управлением
	- поправлено управление эффектами от SottNick. Прыгающие шарики теперь с траекторией.
	- реализация макрокомманд для управления (для последующих проектов)
	- реализация концепции Демо-режима

v1.7.07b (31.03.2020) - by PalPalych
	- убрана глобальная иниацилизация класса WiFiManager
	- перенос настроек эффектов во Flash (кроме описания эффектов)
	- расширен и настроен режим DEMO - управление яркостью и скоростью смены эффектов / + случайное переключение
	- поправлен и переписан на новую логику эффект НОЧНИК (WHITE STRIPE LAMP) 

v1.7.08b (03.04.2020) - by PalPalych
	- реализован экспорт списка эффектов в приложение от Koteyka https://koteyka-net.000webhostapp.com/firelamp/
	- поправлен загрузчик (вход в конфигурацию)
	- поправлен вывод текста, и режим DEMO (запрет смены эффекта во время бегущей строки)
	- Прыгающие мячики (+ белые, + псевдо 3D, + меняют цвет при отскоке)
	- Реализована возможность установки индикаторных светодиодов до и после ленты/матрицы
	- оптимизирована библиотека LEDraw 
		(+ замена описание функций drawFillRect2_fast, gradientVRect2HSV_fast было (x1, y1, x2, y2...) -> стало (x1, x2, y1, y2...))

v1.7.09b (05.04.2020) - by PalPalych
	- Эффект Светлячки: + с хвостами
	- Эффект Блуждающий кубик: + с хвостом, + дизеринг
	- + Эффект Блуждающий Шарик (к Кубику) 2в1 (новый движок). + Случайный отскок с эффектом "встряски"
	- Библиотека LEDraw пополнена функцией drawOval2_fast для рисования эллипсов в коробочке сверхмалого размера

v1.7.10b (07.04.2020) - by PalPalych
	- Эффект PULSE на новом движке
	- LEDraw, добавлено: drawOval2 (safe), drawHLine2 (safe)
	- Работает вывод бегущей строки из приложения от Koteyka
	- Работает рисовалка из приложения от Koteyka, введен режим PAINT (Рисовалка)
	- Эффект Black Screen переработан под рисовалку (регулятор скорость - плавное затемнение нарисованного)
	- Эффект PULSE: +дизеринг, + делитель кадров, + одновременное нарождение нескольких пузырей

v1.7.11b (08.04.2020) - by PalPalych
	- добавлены последние библиотеки в прошивку
	- возможно поправлена работа с OTA и MQTT (проверяем)
	- добавлен эффект PRISMATA by SottNick Mod, + управление масштабом от kDn, + размытие
	- поправлен эффект SPIRO by SottNick Mod, + управление масштабом от kDn, + размытие
	- оптимизированна стуктура памяти для работы с индикаторами и совместимости со старыми эффектами
	- поправлена функция размытия LEDraw для повернутых матриц, + добавлена простая функция blurAll() 

v1.7.12b (13.04.2020) - by PalPalych
	- плавная регулировка скорости во всем диапазоне классических эффектов
	- введена возможность конфигурации пользовательских прошивок см. USE_CUSTOMIZATION в constants.h
	- библиотека LEDraw дополнена набором палитр
	- эффект SPIRO и PRISMATA работают со множеством палитр с новой реализацией переключения
	- эффект SINUSOID 3 на новом движке
	- +эффекты: SWIRL (Вихрь), FLOCK (Стадо), PREDATOR (Хищник), RAIN (Дождь), WAVE (Волна), DRIFT (Дрифт), DRIFT (Дрифт 2)

v1.7.13b (24.04.2020) - by PalPalych
	- исправлены проблемы списка эффектов приложения от Koteyka
	- +эффект BOUNCE (92FPS+) - альтернатива BOUNCE BALLS, с великолепной плавностью прорисовки и горизонтальным перемещением
	- +эффект SIMPLEX NOISE 8in1 (80FPS+) - альтернатива Лава/Лес/Океан/Радуга3D..., с великолепной плавностью прорисовки
			регулятор масштаба: выбор из 8 палитр / масштаб (от супер крупного, до приемлемого)
			регулятор скорости: скорость перемещения / включение/выключение цикличности палитры
	- +эффекты "COLORED RAIN", "SIMPLE RAIN", "STORMY RAIN", "FIRE 2012", "FIRE 2018"... (пока не оптимизированы)
	- скрыты повторяющиеся старые классические эффекты (заменены новыми с большой плавностью прорисовки)
	- повторно переработан движок формирования кадров. Новый алгоритм обеспечения линейности регулятора скорости для старых эффектов
	- рисовалка от Koteyka: регулятор скорости - угасание рисунка, регулятор масштаба - размытие рисунка
		!!! для плавной работы рисовалки желательно отключить вывод тестовых сообщений в рабочей прошивке.
	- оптимизированы алгоритмы координат для матриц различного типа
	- оптимизированы эффекты COLOR, COLORS, SNOWSTORM/STARFALLS

v1.7.14b (25.04.2020) - by PalPalych
	- поправлен эффект PREDATOR (хищник теперь не ленивый)
	- причесан код по варнингам (остались только контролируемые  предупреждения)
	- восстановлена работа MQTT

v1.7.15b (27.04.2020) - by PalPalych
	- приделан модуль работы с пультом ДУ. Спасибо Shaitan! (концепция управления изменена. Используется легкий код дешифровки команд пульта)

v1.7.16b (04.05.2020) - by PalPalych  
	- расширена система комманд для Пульта ДУ
	- оптимизированны команды пульта ДУ
	- при выборе эффекта с пульта отображается его номер 
	- расширена строка информации выводимая на экран
	- эффект METABALLS (FPS80+) теперь с палитрами
	- эффект WAVES (FPS93+) теперь регуируется - Scale: Палитра + Затухание, Speed: Скорость + направление волны (4 стороны) + 1 или 2 волны
	- эффекты FLOCK и PREDATOR объеденены в 1 (FPS43+) теперь регулируется - Scale: Палитра + Затухание, Speed: Скорость + Включение хищника
	- эффекты COLOREDRAIN, SIMPLERAIN, STORMYRAIN - + Pseudo3D
	- поправлены эффекты PAINTBALL, SWIRL

v1.7.17b (25.05.2020) - by PalPalych
	- добавлены эффекты SMOKE, RUBIK, CODELOCK от Сотнег
	- добавлены часы для ночного режима CLOCK (включаются сменой бэкграунда режима OFF *** - троекратное нажатие кнопки)
		+ поправлен шрифт цифр на более хипстерский, добавлен индикатор секунд
		! (так как пока не отточена работа главной кнопки, выставить положение и яркость часов можно только в прошивке. С пульта сделать такое действие вполне возможно.)
	- LEDraw: добавлены семейства функций addPixel, subPixel; функция clearAll...
	- оптимизирована структура текущего эффекта
	- оптимизированы вызовы функций эффектов
	- эффект SINUSOID3 стал менее глючным
	- эффект FLOCK и PREDATOR - хищник теперь больше времени на экране
	- эффект WAWES стал более разнообразным
	- пришло обновление библиотек ESP8266, IRremote8266, WiFimanager. Проверьте!

v1.7.18b (06.06.2020) - by PalPalych
	- LEDraw:
		+ добавлены функции mirrorX, mirrorY ... 
		+ добавлено семейство функций gradientVRect2 (рисовать прямоугольник с заполоненным градиентом)
	- структура менеджера текущего эффекта: 
		- оптимизированы константы. В общем, дает прирост скорости прорисовки эффекта +7%
		+ добавлена глобальная яркость для каждого режима лампы
		= оптимизация управления яркостью, теперь логарифмическая
	- переключение глобальной яркости/яркости эффекта на ИК-пульте 
	- логика управления кнопкой:
		- глобальная яркость: - (долгое удержание)
		- яркость эффекта: ***- (тройное нажатие + долгое удержание)
		+ добавлен профиль исполняемых команд по умолчанию
		+ возможность установки параметров разных режимов, к примеру, бэкграунда (в том числе и часов) выключенного режима 
	- добавлена структура менеджера памяти EEPROM
	- эффект CLOCK: дополнительно подправлен шрифт 3х5 для лучшего считывания
	+ структура менеджера питания:
	+ первая практическая реализация полноцветных индикаторов (лампочки состояния, подсветки кнопок, индикаторов уровня(tbd))
		+ добавлена структура bulb (лампочка)
		+ добавлены функции включения/выключения + 6 эффектов переливания
	- исправлены баги/произведена оптимизация кода/уточнен интерфейс...

v1.7.19b (15.06.2020) - by PalPalych
	+ добавлен механизм комплексной регулировки уровня яркости. Позволяет повысить детализацию при минимальной яркости, и не оттеняет индикаторы (как графические, так и физические)
	- переделаны часы выключенного режима:
		+ добавлена функция печати uint_8 числа целиком drawNumber3x5 (x, y - координаты нижнего правого угла)
		+ добавлен режим "Rainbow Around a Day" - цвет чисел в зависимости от времени суток
		+ добавлен градиентный индикатор хода часов
		+ поправлен шрифт часов на еще более читаемый
		+ добавлен первый эффект смены цифр часов (вращение)
		+ эффект "отбивки" цветом каждые 15 минут
		+ исправлен алгоритм вычисления цвета "Rainbow Around a Day": 
			- теперь каждая минута со своим оттенком! 
			- в ночном режиме не наблюдается чрезмерного переосвещения матрицы 
				(при минимальной яркости теперь время от 04 до 12 часов - темнозеленый, 12...20 - темнокрасный, 20...04 - темносиний.
					При повышении яркости появляется градиент, который будет точно сменяться в точное время)
	+ структура менеджера кнопки - button
	+ структура менеджера пульта ДУ - remote
	+ структура менеджера времени - chronometer
		- реализованы независимые синхронизируемые системные часы - для автоматического хода при отсутствии интернета / для реализации эффектов требующих точного времени
		- исправлены некоторые функции таймера timerMinim
	+ все структуры могут работать в связке, что упрощает написание программ управления или макросов (не требуется введения множества несвязанных друг с другом переменных)

v1.7.20b (15.06.2020) - by PalPalych
	+ режим "темнее минимального яркого" позволяет сильнее приглушить свечение матрицы в ночном режиме
	+ раздельная глобальная яркость для дневного/ночного режима

v1.7.21b (26.09.2020) - by PalPalych
	+ введен выбор типа светодиодной ленты для простой полоски(гирлянды)/матрицы
	+ начало оптимизации одномерных эффектов для Диско-лампы нового поколения
	+ добавлена опция пользовательского набора эффектов EFFECTS_SET
	+ разделена библиотека LEDraw на математическике, 1-, 2-, 3-мерные функции и для работы с палитрами
	+ отимизирован режим индикации загрузчика при наличии индикаторов 
	+ отключаемые настройки использования WiFi и OSD (экранная графика)
	+ добавлены функции led-индикатора Pulse (1, 2, 4 и 8 вспышек за секунду)- используются индикаторами уровня
	+ исправлен баг с яркостью отдельного эффекта
	+ введены функции управления мотором (заместо канала яркости эффекта. Для гирлянды и диско-лампы этот канал управления не является необходимым)

v2.0.00γ (29.11.2020) - by PalPalych
	Проект переименован в LEDraw Project - универсальная среда для светодиодной графики
*/

#include "config.h"
#ifdef USE_CUSTOMIZATION
#include USE_CUSTOMIZATION
#endif

#ifdef USE_LOCALIZATION
#include USE_LOCALIZATION
#else
#include "localization/ru.h"
#endif
#ifdef USE_REMOTECONTROL
#include USE_REMOTECONTROL
#endif
#include "types.h"
#include "variables.h"
#include EFFECTS_SET
#include "chronometer.h"
#include "functions.h"

		void
		setup()
{
	Serial.begin(115200);
	delay(100);
#if USE_DEBUG
	LOG.println(PSTR("\n\n"));
	LOG.println(FPSTR(log_init_greetings));
#endif
	delay(100);
	sprintf_P(host_name, PSTR("%s_%s"), NETNAME_PREFIX, String(ESP.getChipId(), HEX).c_str());

	// ----- ЛЕНТА/МАТРИЦА -----                           - перенесено в начало ради сокращения времени когда экран засвечивается хаотичным узором при включении
	FastLED.setDither(FASTLED_USE_DITHERING);
	FastLED.addLeds<LEDTYPE, LED_PIN, LEDORDER>(ledstripe, NUM_LEDS_all); //.setCorrection(TypicalLEDStrip);
	clearAll(ledstripe);

#if INDICATORS
	indicators.bulbs[0].screw(&ledstripe[0]);
#endif

#if USE_MOTOR
	motor.init();
#endif

	if (CURRENT_LIMIT)
		FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT); // установка максимального тока БП
	FastLED.setBrightness(BRIGHT);															// установка яркости

#if INDICATORS
	indicators.generalBulb->setON(CRGB::Red);
#else
	drawPixel(0, 1, CRGB::Red); // красный светодиод - начало загрузки
#endif
	delay(10);
	FastLED.show();
	delay(1000); // красный светодиод инициализации "Включение в сеть" (второй по порядку)

	flash.init();

	// ----- ПИНЫ -----
#ifdef MOSFET_PIN // инициализация пина, управляющего MOSFET транзистором в состояние "выключен"
	pinMode(MOSFET_PIN, OUTPUT);
#ifdef MOSFET_LEVEL
	digitalWrite(MOSFET_PIN, !MOSFET_LEVEL);
	delay(10);
#endif
#endif

#ifdef ALARM_PIN // инициализация пина, управляющего будильником в состояние "выключен"
	pinMode(ALARM_PIN, OUTPUT);
#ifdef ALARM_LEVEL
	digitalWrite(ALARM_PIN, !ALARM_LEVEL);
	delay(10);
#endif
#endif

	// ----- TELNET -----
#if USE_DEBUG && DEBUG_TELNET
	telnetServer.begin();
	for (uint8_t i = 0; i < 100; i++)
	{ // пауза 10 секунд в отладочном режиме, чтобы успеть подключиться по протоколу telnet до вывода первых сообщений
		handleTelnetClient();
		delay(100);
	}
#endif

	// ----- КНОПКА -----
#if USE_BUTTON
	touch.setStepTimeout(BUTTON_STEP_TIMEOUT);
	touch.setClickTimeout(BUTTON_CLICK_TIMEOUT);
	touch.setTimeout(BUTTON_TIMEOUT);
	touch.setTickMode(false); // автоматический режим
#if INDICATORS
	indicators.generalBulb->setON(CRGB::Green);
#else
	drawPixel(0, 2, CRGB::Green); // зеленый светодиод инициализации - кнопка присутствует (третий светодиод)
#endif
	delay(10);
	FastLED.show();
	delay(10);
	touch.tick();
	delay(500); // ожидание инициализации модуля кнопки ttp223 (по спецификации 250мс)
	touch.tick();
	delay(500); // ожидание инициализации модуля кнопки ttp223 (по спецификации 250мс)
	touch.tick();
	if (touch.state())
	{
#if INDICATORS
		indicators.generalBulb->setON(CRGB::Blue);
#else
		drawPixel(0, 3, CRGB::Blue);
#endif
		delay(10);
		FastLED.show(); // синий светодиод инициализации - сброс WIFI (четвертый светодиод)
#if USE_DEBUG
		LOG.println(FPSTR(log_init_reset)); // "Init: Настройки WiFiManager сброшены"
#endif
		needconfigure = true; // флаг на запуск конфигурации
		btn_Enabled = true;		// при сбросе параметров WiFi сразу после старта с зажатой кнопкой, также разблокируется кнопка, если была заблокирована раньше
		delay(1000);
	}
#else
	if (WiFi.SSID().length() == 0)
		needconfigure = true;
	delay(100);
#endif

	// ----- WI-FI -----
#if USE_WIFI
	if (!ESP_mode) // если выбран режим WiFi точки доступа
	{
		if (sizeof(AP_STATIC_IP))
		{
			beginAP();
			delay(500);
		}
	}
	else
	{ // режим WiFi клиента
		int attempts = 1;
		float power = 0;
#if USE_DEBUG
		LOG.println(FPSTR(log_wifi_startClient)); //"WiFi: Старт в режиме WiFi клиента (подключение к роутеру)"
#endif
		fillAll(CRGB::Black);
		delay(10);
		FastLED.show(); // очистка матрицы
		delay(10);

		if (needconfigure) // Проверка. Требуется ли дополнительная конфигурация?
		{
			//ESP.wdtDisable();
			WiFiManager wifiManager;
			delay(10);
			wifiManager.resetSettings(); // сброс сохранённых SSID и пароля при старте с зажатой кнопкой, если разрешено
			delay(500);
			ESP.wdtEnable(ESP_CONF_TIMEOUT * 2);
			delay(500);															// установка дежурного счетчика на заведомо большее время ввода конфигурации
			wifiManager.setDebugOutput(DEBUG_WIFI); // вывод отладочных сообщений
			wifiManager.setMinimumSignalQuality();	// установка минимально приемлемого уровня сигнала WiFi сетей (8% по умолчанию)
			//CaptivePortalManager *captivePortalManager = new CaptivePortalManager(&wifiManager);
			wifiManager.setConnectTimeout(ESP_CONN_TIMEOUT); // установка времени ожидания подключения к WiFi сети, затем старт WiFi точки доступа
			delay(500);
			if (WiFi.SSID().length() == 0) // если настройки были сброшены, то запускается процедура подключения через AP
			{
				delay(500);
				FastLED.setBrightness(BRIGHT);
				fillAll(CRGB(0, 0, 16));
				delay(10);
				FastLED.show(); // вся матрица темно синяя = режим конфигурации
				delay(10);
#if USE_DEBUG
				LOG.print(FPSTR(log_init_startAutoConfig)); // "Init: Старт автоконфигурации..."
#endif
				CaptivePortalManager *captivePortalManager = new CaptivePortalManager(&wifiManager);
				wifiManager.setConfigPortalTimeout(ESP_CONF_TIMEOUT); // установка времени работы WiFi точки доступа, можно боллее 8 секунд - стандартного значениея1
				delay(10);
				wifiManager.setBreakAfterConfig(true);
				delay(10);
				wifiManager.autoConnect(host_name, NET_PASSWORD); // пытаемся подключиться к сохранённой ранее WiFi сети; в случае ошибки, будет развёрнута WiFi точка доступа с указанными NETNAME_PREFIX и паролем на время ESP_CONN_TIMEOUT секунд; http://AP_STATIC_IP:ESP_HTTP_PORT (обычно http://192.168.0.1:80) - страница для ввода SSID и пароля от WiFi сети роутера
#if USE_DEBUG
				LOG.print(FPSTR(log_init_finishAutoConfig)); // "Init: Автоконфигурация звершена"
#endif
				delay(10);
				fillAll(CRGB::Black);
				delay(10);
				FastLED.show(); // очистка матрицы
				delay(10);
				ESP.wdtEnable(ESP_CONF_TIMEOUT * 2);
				delay(500);
				String ssid = WiFi.SSID();				// запоминание последнего введеного ID сети
				String pass = WiFi.psk();					// запоминание последнего введеного пароля
				int thisChannel = WiFi.channel(); // запоминаем номер канала, на который подключался ESP
#if USE_DEBUG
				LOG.print("SSID: ");
				LOG.print(WiFi.SSID());
				LOG.print(" ");
				LOG.println(WiFi.SSID().length()); // показать имя текущей сети и его длину в символах
				LOG.print("Введенный пароль: ");
				LOG.print(WiFi.psk());
				LOG.print(" ");
				LOG.println(WiFi.psk().length()); // показать прроль текущей сети и его длину в символах
				LOG.print("Установленный канал при последней попытке соединения: ");
				LOG.println(thisChannel); // показать номер текушего канала
#endif
				delay(500);
				if (ssid == "") // сеть не выбрана, пароль не введен
				{
					//wifiManager.resetSettings(); // обнуление настроек WiFi
					if (WiFi.status() == WL_CONNECT_FAILED)
					{																					// если была неудачная попытка входа в сеть (сеть прусутствует, но роутер по какой-то причине не принял клиента)
						showWarning(CRGB::Yellow, 3000U, 500U); // мигание желтым цветом 0,5 секунды (3 раза) - сеть WiFi найдена, но вход в сеть не осуществлен
#if USE_DEBUG
						LOG.println(FPSTR(log_wifi_neterror)); // "WiFi: Ошибка входа в сеть..."
						delay(10);
#endif
					}
					else
					{
						showWarning(CRGB::Orange, 3000U, 500U); // мигание оранжевым цветом 0,5 секунды (3 раза) - сеть не выбрана
#if USE_DEBUG
						LOG.println(FPSTR(log_wifi_netclosed)); // "WiFi: Cеть не выбрана, пароль не введен!"
						delay(2000);
#endif
						ESP.restart(); // перезагрузка
					}
				}
				delete captivePortalManager; // сброс настроек всплывающего экрана
				captivePortalManager = NULL;
#if USE_DEBUG
				LOG.println(FPSTR(log_wifi_scan)); // "WiFi: Сканирование близлежащих сетей..." Получение списка ближайших сетей
#endif
				int numSsid = WiFi.scanNetworks();
				if (numSsid == -1)
				{
					LOG.println(FPSTR(log_wifi_restart)); // "Wifi: Нет доступных сетей! Выход из конфигурации. Рестарт..."
					//wifiManager.resetSettings();             // обнуление настроек WiFi
					showWarning(CRGB::Magenta, 3000U, 500U); // мигание фиолетовым цветом 0,5 секунды (3 раза) - не найдено ни одной WiFi сети
#if USE_DEBUG
					LOG.println(FPSTR(log_wifi_neterror)); // "WiFi: Ошибка входа в сеть..."
					delay(2000);
#endif
					ESP.restart();
				}
#if USE_DEBUG
				LOG.printf_P(log_wifi_netcount, numSsid); // "WiFi: Количество обнаруженных сетей: %u\n" вывод количества обнаруженных сетей
#endif
				int thisNet = 0;
				for (thisNet = 0; thisNet < numSsid; thisNet++) // вывод листинга обнаруженных сетей
				{
#if USE_DEBUG
					LOG.print(thisNet + 1);
					LOG.print(") ");
					LOG.print("Уровень сигнала: ");
					LOG.print(WiFi.RSSI(thisNet));
					LOG.print("dBm");
					LOG.print("\tКанал: ");
					LOG.print(WiFi.channel(thisNet));
					LOG.print("\t\tSSID: ");
					LOG.println(WiFi.SSID(thisNet));
#endif
					if (ssid == WiFi.SSID(thisNet))
					{
						thisChannel = WiFi.channel(thisNet);
#if USE_DEBUG
						LOG.printf_P(log_wifi_info, ssid.c_str(), thisChannel); // "WiFi: Cеть %s наботает на канале: %u\n"
#endif
					}
				}
#if USE_DEBUG
				LOG.println(FPSTR(log_wifi_diagnosis)); // "WiFi: Диагностика подключения..." если конфигурация устройства не прошла
#endif
				FastLED.setBrightness(BRIGHT);
				fillAll(CRGB(16, 0, 0));
				delay(10);
				FastLED.show(); // вся матрица темно красная = режим аварийной конфигурации
				delay(10);
				if (WiFi.status() != WL_CONNECTED) // если подключение к WiFi не установлено
				{
#if USE_DEBUG
					LOG.println(FPSTR(log_wifi_reserveStart)); // "WiFi: Попытка резервного запуса сети..."
#endif
					attempts = 1;
					power = 0;
					delay(500);
					while (WiFi.status() != WL_CONNECTED)
					{ // старт цикла попыток подключения
						drawPixel(0, attempts, CRGB(32, 32, 32));
						delay(10);
						FastLED.show(); // отображение инициализации попытки n
						delay(10);
						uint8_t myNet = 0;
						numSsid = WiFi.scanNetworks(); // сканирование сетей на присутствие требуемой
						for (thisNet = 0; thisNet < numSsid; thisNet++)
						{
							delay(10);
							if (ssid == WiFi.SSID(thisNet))
								myNet = thisNet; // если канал не совпадает с ранее подключавшимся, запомнить новое значение
						}
#if USE_DEBUG
						LOG.println();
						LOG.print("WiFi: Канал сети: ");
						LOG.print(WiFi.SSID(myNet)); // вывод параметров следующей попытки
						LOG.print(" Сигнал роутера: ");
						LOG.print(WiFi.RSSI(myNet));
						LOG.println("dBm ");
						LOG.print("Попытка: ");
						LOG.print(attempts);
#endif
						delay(10);
						WiFi.mode(WIFI_OFF);
						delay(500); // отключение модуля WiFi/пауза
						drawPixel(0, attempts, CRGB(32, 32, 0));
						delay(10);
						FastLED.show(); // отображение режима попытки n
						delay(10);
						WiFi.mode(WIFI_STA);
						delay(500); // включение модуля WiFi в режиме клиента
						if (power > 20.5)
							power = 20.5; // ограничение уровня мощности передатчика. макс 20,5 дБм
						delay(10);
						WiFi.setOutputPower(power);
						delay(2000);
#if USE_DEBUG
						LOG.printf_P(log_wifi_outputPower, power); // "WiFi: Мощность передачи: +%fdBm\n"
#endif
						delay(10);
						WiFi.begin(ssid, pass);
						delay(500); // попытка входа в сеть
#if USE_DEBUG
						LOG.print("WiFi: Статус: ");
						LOG.print(FPSTR(enum_connStatus[WiFi.status()])); // вывод статуса подключения
						LOG.print(" Канал: ");
						LOG.print(WiFi.channel()); // вывод канала подключения
						LOG.print(" Сигнал: ");
						LOG.print(WiFi.RSSI());
						LOG.print("dBm "); // вывод уровня сигнала
#endif
						int awaiting = 0;
						delay(10);
						while (WiFi.status() != WL_CONNECTED && awaiting < 30)
						{ // цикл подключения
							delay(10);
							if ((WiFi.channel() == thisChannel && WiFi.RSSI() != 31) || WiFi.status() == WL_IDLE_STATUS) // Номер канала корректен, Есть сигнал с роутера (в дБм, 31 = отсутствие сигнала), Есть режим подключения
								LOG.print(".");																																						 // точка ожидания - ждем соединения
							else
							{
								LOG.print("*"); // звездочка ожидания - ждем подключения
								if (awaiting > 7)
									awaiting = 100;
							}
							awaiting++;
							delay(1000);
						}
						attempts++;
						power += 1.3667; // Увеличить мощность передатчика на 1/15
						LOG.println();
						if (attempts > 15)
						{
							delay(10);
							wifiManager.resetSettings(); // сброс ранее введенных настроек
							delay(10);
							showWarning(CRGB::Red, 3000U, 500U); // мигание красным цветом 0,5 секунды (3 раза) - не удалось подключится к WiFi сети, перезагрузка со сбросом настроек
#if USE_DEBUG
							LOG.println(log_init_restart); // "WiFi: Соединение не выполнено! Перезагрузка..."
#endif
							delay(2000);
							ESP.restart(); // перезагркзка
						}
						delay(10);
						ESP.wdtFeed();
					}
				}
#if USE_DEBUG
				LOG.print(FPSTR(log_init_wifiStatIP));
				LOG.print(WiFi.localIP());
#endif
				delay(10);
				WiFi.persistent(true); // сохранение настроек
				delay(10);
				showWarning(CRGB::Green, 3000U, 500U); // мигание зеленым цветом 0,5 секунды (3 раза) - успешный вход в WiFi сеть, перезагрузка для применения настроек
#if USE_DEBUG
				LOG.println(FPSTR(log_init_restartIP)); // "WiFi: Рестарт для применения заданного статического IP адреса..."
#endif
				delay(2000);
				ESP.restart(); // задержка/перезагркзка
			}
			//wifiManager = NULL;
		}

		if (WiFi.SSID().length()) // подключаемся к роутеру, если есть уже сохранённые SSID и пароль (было подключение к сети)
		{
			delay(10);
			WiFi.mode(WIFI_STA); // установка режима клиента WiFi
			delay(1000);
			WiFi.begin(); // запуск подключения со старыми параметрами
			delay(1000);
#if USE_DEBUG
			LOG.printf_P(log_wifi_connect, WiFi.SSID().c_str()); // "WiFi: Подключение к WiFi сети: %s\n";
#endif
			while (WiFi.status() != WL_CONNECTED) // проверка на устойчивое подключеное соединение
			{
				drawPixel(0, attempts, CRGB(0, 12 + attempts * 12, 12 + attempts * 12));
#if INDICATORS
				indicators.generalBulb->led = getPixelColor(0, attempts);
#endif
				delay(10);
				FastLED.show(); // отображение уровня мощности n
				if (power > 20.5)
					power = 20.5; // макс +20,5 дБм
#if USE_DEBUG
				LOG.printf_P(log_wifi_outputPower, power); // "WiFi: Мощность передачи: +%fdBm\n"
#endif
				delay(1333);
				WiFi.setOutputPower(power); // установка мощности передатчика
				power += 1.3667;
				attempts++;	 // увеличение мощности для следующего шага
				delay(1333); // задержка перед увеличением мощности
				if (attempts > 15)
				{
					isWifiOffMode = true;
					showWarning(CRGB::Red, 3000U, 500U); // мигание красным цветом 0,5 секунды (1 раз) - не удалось подключится к WiFi сети, продолжаем без WiFi
					delay(10);
#if USE_DEBUG
					LOG.print(FPSTR(log_init_workWOwifiConn)); // "Init: Подключение не выполнено. Работа без сети."
#endif
					//WiFi.mode(WIFI_OFF); // выхлючить WiFi - раьота лампы без сети
					beginAP();
					delay(10);
				}
				ESP.wdtFeed();
				delay(10);
			}
		}
		else
		{
			isWifiOffMode = true; // Если сеть не была настроена ранее. Работа без WiFi
#if USE_DEBUG
			LOG.println(FPSTR(log_init_workWOwifi)); // "Init: Работа без WiFi сети..."
#endif
			showWarning(CRGB::White, 1000U, 500U); // мигание белым цветом 0,5 секунды (1 раз) - режим работы без сети
			delay(10);
			WiFi.mode(WIFI_OFF); // включение лампы без WI-Fi
													 //beginAP();
		}
		// меняем IP адрес на предустановленный, если он определен
		delay(10);
		if (sizeof(STA_STATIC_IP) && WiFi.localIP() != IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]))
		{ // ВНИМАНИЕ: настраивать статический ip WiFi клиента можно только при уже сохранённых имени и пароле WiFi сети (иначе проявляется несовместимость библиотек WiFiManager и WiFi)
#if USE_DEBUG
			LOG.printf_P(log_init_ConfStatIP, STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]); // "Init: Сконфигурирован статический IP адрес: %u.%u.%u.%u\n"
#endif
			delay(10);
			//wifiManager.setSTAStaticIPConfig(
			WiFi.config(
					IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]), // статический IP адрес ESP в режиме WiFi клиента
					IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], 1),								 // первый доступный IP адрес сети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
					IPAddress(255, 255, 255, 0));																											 // маска подсети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
		}
	}
#if USE_DEBUG
	LOG.print(FPSTR(log_init_IP));
	LOG.println(WiFi.localIP());									// "Init: IP адрес: "
	LOG.printf_P(log_init_portUDP, ESP_UDP_PORT); // "Init: Порт UDP сервера: %u\n"
#endif
	delay(10);
	udp.begin(ESP_UDP_PORT); // конфигурация UDP порта
	// ESP.wdtDisable();
	delay(10);
	// NTP
#if USE_NTP
	delay(10);
	timeClient.begin();
	delay(10);
#endif

	// MQTT
#if USE_MQTT
	if (ESP_mode == 1U)
	{
		delay(10);
		mqttClient = new AsyncMqttClient();
		delay(10);
		MqttManager::setupMqtt(mqttClient, inputBuffer, &udp_sendCurrent); // создание экземпляров объектов для работы с MQTT, их инициализация и подключение к MQTT брокеру
		delay(10);
	}
#endif
#endif

	delay(10);
	power.screenOFF(true);
	delay(10);

	ESP.wdtEnable(WDTO_8S);
	delay(10); // запуск ждущего таймера на 8 секунд

#if USE_OSD
	osd.scrollText(FPSTR(msg_hello), CRGB::Fuchsia, 5, 0x04);
#endif

#if USE_IR
	remote.setup();
#endif

	// ОСТАЛЬНОЕ
	randomSeed(micros());
	effect.set(EFFs_BLACKSCREEN);
	power.change(pwr_OFF, 255);
};

#if USE_WIFI
void parseUDP(); // прототипы
#endif

void loop()
{

#if USE_TASKMONITOR && USE_DEBUG
	cycleTime = millis(); // начало цикла монитоа loop()
#endif

	power.tick();

#if INDICATORS
	indicators.tick();
#endif

#if USE_BUTTON
	button.tick();
#endif

#if USE_IR
	remote.tick();
#endif

	effects.tick();
	flash.tick();

#if USE_MOTOR
	motor.tick();
#endif

#if USE_WIFI
	parseUDP();
#if USE_NTP
	chronometer.tick();
#endif

#if USE_OTA
	ota.HandleOtaUpdate(); // ожидание и обработка команды на обновление прошивки по воздуху
#endif

#if USE_MQTT
	if (ESP_mode == 1U && mqttClient && WiFi.isConnected() && !mqttClient->connected())
	{
		MqttManager::mqttConnect(); // библиотека не умеет восстанавливать соединение в случае потери подключения к MQTT брокеру, нужно управлять этим явно
		MqttManager::needToPublish = true;
	}

	if (MqttManager::needToPublish)
	{
		if (strlen(inputBuffer) > 0) // проверка входящего MQTT сообщения; если оно не пустое - выполнение команды из него и формирование MQTT ответа
		{
			processInputBuffer(inputBuffer, MqttManager::mqttBuffer, true);
		}

		MqttManager::publishState();
	}
#endif

	/* TimerManager::HandleTimer(&ONflag, &forceToSave,          // обработка событий таймера отключения лампы //&ONflag
                           &flashTS32, &power.change); */

	/*if (FavoritesManager::HandleFavorites( // обработка режима избранных эффектов
          &power.mode,
          &eff_on_current,
          &isNewFrame()
#if USE_NTP
          ,
          &dawnFlag
#endif
          ))*/
#endif

#if USE_DEBUG && DEBUG_TELNET
	handleTelnetClient();
#endif

	delay(1);
	yield();
	randomSeed(millis());
	ESP.wdtFeed(); // пнуть собаку

#if USE_TASKMONITOR && USE_DEBUG // обработка цикла монитоа loop()
	cycleTime = millis() - cycleTime;
	EVERY_N_SECONDS(10)
	{
		LOG.printf_P(PSTR("DEBUG LOOP:\tAVG %ums  MAX %ums  Free memory: %u  FPS: %u\n"), avgLoopTime, maxLoopTime, ESP.getFreeHeap(), FastLED.getFPS());
		//LOG.printf_P(PSTR("DEBUG EFFECT:\tspeed: %u  scale: %u\n"), effect.speed, effect.scale);
		//LOG.printf_P(PSTR("DEBUG MOTOR:\tspeed: %u  vector: %u  reverse: %u\n"), motor.speed, motor.vector, motor.reverse); // &RainbowColors_p, &OceanColors_p,
		//LOG.printf("DEBUG FADER:\tcurrent %u   status %X\n", effects.fader.current, effects.fader.status);
		//LOG.printf("DEBUG OSD:\tcurrent %u   status %X\n", effects.osd.current, effects.osd.status);

		avgLoopTime = cycleTime;
		maxLoopTime = cycleTime;
	}
	else
	{
		avgLoopTime = (avgLoopTime + cycleTime) / 2;
		maxLoopTime = max(maxLoopTime, cycleTime);
	}
#endif
}
