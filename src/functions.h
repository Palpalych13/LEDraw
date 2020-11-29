#ifndef _functions_h
#define _functions_h

// ----- EFFECTS CONSTANTS ----- last mod v1.7.21b
static const uint8_t effs_count = sizeof(effects_array) / sizeof(eff_t);

//static const

// ----- EEPROM SECTION -----
#include "flash.h"

// ----- WiFi SECTION -----
#if USE_WIFI
void beginAP()
{
#if USE_DEBUG
	LOG.println(FPSTR(log_init_wifiStatIP)); // "Init: Используется статический IP адрес WiFi точки доступа"
#endif
	WiFi.config(
			IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]), // IP адрес WiFi точки доступа
			IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),							 // первый доступный IP адрес сети
			IPAddress(255, 255, 255, 0));																									 // маска подсети

	if (mdns.begin(host_name, WiFi.localIP()))
	{
		LOG.println("MDNS responder started");
	}
	WiFi.softAP(host_name, NET_PASSWORD); // конфигурация WiFi в режиме точки доступа
#if USE_DEBUG
	LOG.println(FPSTR(log_init_wifi)); // "Init: Старт в режиме WiFi точки доступа"
	LOG.print(FPSTR(log_init_wifiIP));
	LOG.println(WiFi.softAPIP()); // "Init: IP адрес: "
#endif
	wifiServer.begin(); // запуск сервера
	delay(500);
}

#if USE_OTA
#include <ArduinoOTA.h>
#include "OTA.h"
#endif

#endif

/*String eff_record(uint8_t value)
{
	String str;

	eff_item_t eff = effect_getFromList(value, ON);
	str += eff.Name() + ",";
	eff_t effa = effects_array[effects.getIndexByID(effect.id)];
	uint8_t spmin = effa.speed_min;
	uint8_t spmax = effa.speed_max;
	if (spmin == spmax)
	{
		if (spmax == 0)
		{
			spmin = 0;
			spmax = 255;
		}
		else
		{
			spmin = 1;
		}
	}
	str += String(spmin) + ",";
	str += String(spmax) + ",";
	str += String(effa.scale_min) + ",";
	str += String(effa.scale_max) + ",";
	str += String(effa.regulators_type);
#if USE_DEBUG
	LOG.println(str); //
#endif
	return str;
}*/

void changeBrightness(uint8_t value, uint8_t eff) // <-- процедура изменения плавной яркости экрана с сохранением в EEPROM (по номеру эффекта в eff_arr)
{
	LOG.print("Bright: Установлена яркость ");
	LOG.println(value);
	power.bright_real = value;
	flash.writePref(pref_Brightness, eff, value);
	power.bright_real = power.Bright();
}

uint8_t udp_getValue(char *data, char separator, int index)
{
	int8_t found = 0;
	int8_t strS = 0;
	int8_t strE = strlen(data);

	for (int i = 0; i < strE; i++)
	{
		if (data[i] == separator)
		{
			if (found == index)
			{
				strE = i;
				break;
			}
			found++;
			strS = i + 1;
		}
	}

	char buffer[(strE - strS + 1U)];
	strncpy(buffer, data + strS, strE - strS + 1U);
	//LOG.print("atoi: ");
	//LOG.println(atoi(buffer));

	return atoi(buffer);
}

// ----- COMMANDS SECTION -----
#include "commands.h"

// ----- BUTTON SECTION -----
#if USE_BUTTON
#include "button.h"
#endif

// ----- IR REMOTE SECTION -----
#if USE_IR
#include "ir.h"
#endif

#include "effects.h"
#include "parsing.h"
#include "telnetManager.h"
#endif