// ***** LOCALIZATION SETTINGS ***** last mod v1.7.07b
#ifndef _constants_localization_h
#define _constants_localization_h
#include <stdint.h>
#include <pgmspace.h>

#if USE_OSD
static const char msg_hello[] PROGMEM = "HELLO, WORLD!"; // сообщение при включении
static const char msg_byebye[] PROGMEM = "BYE-BYE!";		 // сообщение при выключении
static const char msg_mode_wifi[] PROGMEM = "Режим работы ESP изменен на %s. Рестарт...     ";
static const char msg_paint_on[] PROGMEM = "*PAINTBOADRD ON*";
static const char msg_paint_off[] PROGMEM = "*PAINTBOADRD OFF*";
#endif

#if USE_DEBUG
static const char log_init_greetings[] PROGMEM = "Init: ====== LEDraw Project v2.0.00γ ======";
static const char log_init_reset[] PROGMEM = "Init:\tНастройки WiFiManager сброшены\n";
static const char log_init_wifi[] PROGMEM = "Init:\tСтарт в режиме WiFi точки доступа\n";
static const char log_init_wifiStatIP[] PROGMEM = "Init:\tИспользуется статический IP адрес клиента WiFi: ";
static const char log_init_wifiIP[] PROGMEM = "Init:\tИспользуется статический IP адрес точки доступа WiFi: ";
static const char log_init_ConfStatIP[] PROGMEM = "Init:\tСконфигурирован статический IP адрес: %u.%u.%u.%u\n";
static const char log_init_IP[] PROGMEM = "Init:\tIP адрес: ";
static const char log_init_portUDP[] PROGMEM = "Init:\tПорт UDP сервера: %u\n";
static const char log_init_startAutoConfig[] PROGMEM = "Init:\tСтарт автоконфигурации...\n";
static const char log_init_finishAutoConfig[] PROGMEM = "Init:\tАвтоконфигурация звершена\n";
static const char log_init_workWOwifiConn[] PROGMEM = "Init:\tПодключение не выполнено. Работа без сети.\n";
static const char log_init_restart[] PROGMEM = "Init:\tСоединение не выполнено! Перезагрузка...";
static const char log_init_restartIP[] PROGMEM = "Init:\tРестарт для применения заданного статического IP адреса...";
static const char log_init_effect[] PROGMEM = "Init:\tВключение эффекта %s\n";
static const char log_init_workWOwifi[] PROGMEM = "Init:\tВключение без WiFi сети...";

static const char log_wifi_restart[] PROGMEM = "Wifi:\tНет доступных сетей! Выход из конфигурации. Рестарт...";
static const char log_wifi_scan[] PROGMEM = "WiFi:\tСканирование близлежащих сетей...";
static const char log_wifi_connect[] PROGMEM = "WiFi:\tПодключение к WiFi сети: %s\n";
static const char log_wifi_netclosed[] PROGMEM = "WiFi:\tCеть не выбрана, пароль не введен!";
static const char log_wifi_neterror[] PROGMEM = "WiFi:\tОшибка входа в сеть...";
static const char log_wifi_netcount[] PROGMEM = "WiFi:\tКоличество обнаруженных сетей: %u\n";
static const char log_wifi_info[] PROGMEM = "WiFi:\tCеть %s работает на канале: %u\n";
static const char log_wifi_diagnosis[] PROGMEM = "WiFi:\tДиагностика подключения...";
static const char log_wifi_reserveStart[] PROGMEM = "WiFi:\tПопытка резервного запуса сети...";
static const char log_wifi_startClient[] PROGMEM = "WiFi:\tСтарт в режиме WiFi клиента (подключение к роутеру)";
static const char log_wifi_outputPower[] PROGMEM = "WiFi:\tМощность передачи: +%fdBm\n";

static const char log_eff_fadeChange[] PROGMEM = "Effect:\tПлавная смена эффекта %s(%u)\n";
static const char log_eff_fadein[] PROGMEM = "Effect:\tПлавное открытие эффекта %u\n";
static const char log_eff_fadeout[] PROGMEM = "Effect:\tПлавное закрытие эффекта\n";
static const char log_eff_next[] PROGMEM = "Effect:\tВыбран следующий эффект: %s\n";
static const char log_eff_prev[] PROGMEM = "Effect:\tВыбран предыдущий эффект: %s\n";

static const char log_btn_click[] PROGMEM = "Button:\tНажата кнопка %u раз(-а) из состояния %u\n";
static const char log_btn_holdClick[] PROGMEM = "Button:\tУдерживается кнопка с %u нажатий(-ем,-ями) из состояния %u\n";

static const char log_eeprom_reset[] PROGMEM = "EEPROM:\tВсе настройки сброшены до состояния заводских!\n";
static const char log_eeprom_eff_read[] PROGMEM = "EEPROM:\tЧтение параметров текущего эффекта";
static const char log_flash_effects_reset[] PROGMEM = "EEPROM:\tНастройки эффектов сброшены!\n";
static const char log_flash_effect_reset[] PROGMEM = "EEPROM:\tНастройки эффекта %s(%u) сброшены\n";

static const char log_power_change[] PROGMEM = "Power:\tПереход в режим %s\n";
static const char log_power_screenoff[] PROGMEM = "Power:\tМатрица выключена\n";
static const char log_power_screenon[] PROGMEM = "Power:\tМатрица включена\n";

#endif
#endif