#ifndef _led_variables_h
#define _led_variables_h
#include <stdint.h>

// ----- CONSTANTS & VARIABLES -----
static const int8_t int8_0 = 0;
static const uint8_t uint8_0 = 0U;
static const uint8_t uint8_255 = 255U;
static const uint16_t uint16_256 = 256U;
static const uint32_t uint32_65536 = 65536U;
//static const uint16_t uint16_255 = 255U;
static const float float_255 = 255U;

// ----- FASTLED SECTION -----
#if (LEDSARRAY_TYPE == STRIPE)
#define WIDTH NUM_LEDS // ширина матрицы
#define HEIGHT 1			 // высота матрицы
#endif
#if (LEDSARRAY_TYPE == MATRIX)
static const int16_t NUM_LEDS = ((int16_t)WIDTH * HEIGHT);
#endif
static const uint16_t NUM_LEDS_all = NUM_LEDS + PREINDICATORS + POSTINDICATORS;
static const uint16_t BUFFER_SIZE = sizeof(CRGB) * NUM_LEDS;
static CRGB ledstripe[NUM_LEDS_all];
static CRGB ledsbuff[NUM_LEDS];
static CRGB *realleds = &ledstripe[PREINDICATORS];
CRGBSet leds(realleds, NUM_LEDS);
static const uint16_t FASTLED_DITHER_TIME = NUM_LEDS / 32U + 1U;
static const uint16_t NUM_LEDS_indc = NUM_LEDS + PREINDICATORS;

#define INDICATORS (PREINDICATORS + POSTINDICATORS)
#if INDICATORS
#include "indicators.h"
#endif

static bool btn_Enabled = true;
//static uint8_t eff_on_current, eff_off_current, eff_lamp_current, eff_demo_current, eff_paint_current;

//глобальные переменные эффектов
//static effect_t effect;        // текущий эффект
//static uint8_t eff_speed;     // значение скорости текущего эффекта для eff_Ticker();
//static uint8_t eff_scale;     // значение масштаба текущего эффекта для eff_Ticker();
static uint8_t eff_offset;		// глобальная переменная для работы эффектов (обычно применяется для циклического пересчета hue, количества кадров и др...)
static uint8_t eff_offset2;		// глобальная переменная для работы эффектов
//static uint16_t eff_offset16; // глобальная переменная для работы эффектов
//static uint8_t eff_hue;				// глобальная переменная для работы эффектов
//static uint8_t eff_hue2;			// глобальная переменная для работы эффектов
//static uint8_t eff_step;			// глобальная переменная для работы эффектов
//static uint8_t eff_delta;			// глобальная переменная для работы эффектов
//static uint8_t eff_theta;			// глобальная переменная угла для работы эффектов
//static uint8_t eff_x;             // глобальная переменная координаты х для работы эффектов
//static uint8_t eff_y;             // глобальная переменная координаты у для работы эффектов

static bool eff_isDemo;														// демо режим
static bool (*action)() = []() { return false; }; // вызываемая функция эффекта

static uint32_t gag_timer;
static bool gag_direction;

static CRGB drw_color = CRGB::White; // цвет пера рисовалки

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
static char textBuffer[256];
//static uint16_t localPort = ESP_UDP_PORT;
static char packetBuffer[MAX_UDP_BUFFER_SIZE]; // buffer to hold incoming packet
static char inputBuffer[MAX_UDP_BUFFER_SIZE];
static char host_name[48];

static const uint8_t dawnOffsets[] PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50, 60}; // опции для выпадающего списка параметра "время перед 'рассветом'" (будильник); синхронизировано с android приложением
uint8_t dawnMode;
bool dawnFlag = false;

bool ONflag = false;
bool needconfigure = false;
bool isWifiOffMode = false;
#if USE_DEBUG
static const char *enum_connStatus[] PROGMEM = {"WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "WL_SCAN_COMPLETED", "WL_CONNECTED", "WL_CONNECT_FAILED", "WL_CONNECTION_LOST", "WL_DISCONNECTED"}; //перечисление статусов сети
#endif

extern void showWarning(CRGB color, uint32_t duration, uint16_t blinkHalfPeriod);
typedef void (*ShowWarningDelegate)(CRGB color, uint32_t duration, uint16_t blinkHalfPeriod);
uint8_t GlobalBrightness = BRIGHT; // глобальная яркость, пока что будет использоваться для демо-режимов

//pwr_bright_t power_getBright(uint8_t); // прототип

// ----- LOG SECTION -----
#if USE_DEBUG && DEBUG_TELNET
WiFiServer telnetServer(TELNET_PORT); // telnet сервер
WiFiClient telnet;										// обработчик событий telnet клиента
bool telnetGreetingShown = false;			// признак "показано приветствие в telnet"
#define LOG telnet
#else
#define LOG Serial
#endif

// ----- TASKMONITOR SECTION -----
#if USE_TASKMONITOR // глобальные константы монитоа времени исполнения цикла loop()
static uint32_t cycleTime = 0LL;
//static uint32_t idletimer = 0LL;
static uint32_t avgLoopTime = 0LL;
static uint32_t maxLoopTime = 0LL;
#endif

// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
uint8_t ESP_mode = ESP_MODE; // ESP_MODE может быть сохранён в энергонезависимую память и изменён в процессе работы лампы без необходимости её перепрошивки
WiFiUDP udp;
MDNSResponder mdns; //назначаем экземпляр класса ESP8266mDNS
WiFiServer wifiServer(ESP_HTTP_PORT);
#include <WiFiManager.h>
#include "captivePortal.h"

// ----- MQTT SECTION -----
#if USE_MQTT
#include "MQTT.h"
#endif

//static uint8_t power.mode = OFF;
//static uint32_t pwr_timer;            // таймер перехода в режим SLEEP
//static uint8_t const bri_step = 2;
//eff_t effect_get(uint8_t); // прототипы

// ----- TIME SECTION -----
#if USE_NTP
#include "chronometer.h"
#endif

// ----- MOTOR SECTION -----
#if USE_MOTOR
#include "motor.h"
#endif

// ----- POWER SECTION -----
#include "power.h"

//#include "FavoritesManager.h"

//uint8_t power_getBright(uint8_t); // прототипы
//uint8_t getSpeed(uint8_t); // прототипы
//uint8_t getScale(uint8_t); // прототипы
//pwr_bright_t power_getBright(uint8_t);
//eff_t effect_get(uint8_t);
#endif