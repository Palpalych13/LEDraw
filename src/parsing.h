#ifndef _parsing_h
#define _parsing_h
#include <stdint.h>
//#include "power.h"
//#include "chronometer.h"
#if USE_WIFI
void udp_sendCurrent(char *outputBuffer)
{
	sprintf_P(outputBuffer, PSTR("CURR %u %u %u %u %u %u"),
						eff_on_current,
						power.bright,
						effect.speed,
						effect.scale,
						(power.mode == ON), ESP_mode);
#if USE_NTP
	strcat_P(outputBuffer, PSTR(" 1"));
#else
	strcat_P(outputBuffer, PSTR(" 0"));
#endif
	sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)TimerManager::TimerRunning);
	sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)btn_Enabled);
#if USE_NTP
	char timeBuf[9];
	chronometer.getFormattedTime(timeBuf);
	sprintf_P(outputBuffer, PSTR("%s %s"), outputBuffer, timeBuf);
#else
	time_t currentTicks = millis() / 1000UL;
	sprintf_P(outputBuffer, PSTR("%s %02u:%02u:%02u"), outputBuffer, hour(currentTicks), minute(currentTicks), second(currentTicks));
#endif
}

void sendAlarms(char *outputBuffer)
{
	strcpy_P(outputBuffer, PSTR("ALMS"));
	for (byte i = 0; i < 7; i++)
	{
		sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)alarms[i].State);
	}
	for (byte i = 0; i < 7; i++)
	{
		sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, alarms[i].Time);
	}
	sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, dawnMode + 1);
}

void sendTimer(char *outputBuffer)
{
	sprintf_P(outputBuffer, PSTR("TMR %u %u %u"),
						TimerManager::TimerRunning,
						TimerManager::TimerOption,
						(TimerManager::TimerRunning ? (uint16_t)floor((TimerManager::TimeToFire - millis()) / 1000) : 0));
}

void processInputBuffer(char *inputBuffer, char *outputBuffer, bool generateOutput)
{
	char buff[MAX_UDP_BUFFER_SIZE], *endToken = NULL;

	if (!strncmp_P(inputBuffer, PSTR("DEB"), 3))
	{
#if USE_NTP
		//getFormattedTime(inputBuffer);
		sprintf_P(inputBuffer, PSTR("OK %s"), inputBuffer);
#else
		strcpy_P(inputBuffer, PSTR("OK --:--"));
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("GET"), 3))
	{
		//if (power.mode == ON)
		udp_sendCurrent(inputBuffer);
	}

	else if (!strncmp_P(inputBuffer, PSTR("TXT"), 3)) // Принимаем текст для бегущей строки
	{
		memcpy(buff, &inputBuffer[3], strlen(inputBuffer));
		osd.scrollText(buff, CHSV(random8(), 192U, 255U), 5);
	}

	else if (!strncmp_P(inputBuffer, PSTR("LIST"), 4)) // Список эффектов 1
	{
		if (inputBuffer[4] == '1')
		{

			String str = ("LIST1;");
			for (uint8_t i = 0; i < (effects_list_on_size / 2); i++)
			{
				str += (eff_record(i).c_str());
				str += (";");
			}
			str += ("\n");
			udp.write(str.c_str());
			//LOG.println(str.c_str());
		}
		if (inputBuffer[4] == '2')
		{
			String str = ("LIST2;");
			for (uint8_t i = (effects_list_on_size / 2); i <= effects_list_on_size; i++)
			{
				str += (eff_record(i).c_str());
				str += (";");
			}
			str += ("\n");
			udp.write(str.c_str());
			//LOG.println(str.c_str());
		}
	}

	else if (!strncmp_P(inputBuffer, PSTR("GBR"), 3)) // Глобальная яркость (не реализовано)
	{
		/*memcpy(buff, &inputBuffer[3], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    uint8_t ALLbri = constrain(atoi(buff), 1, 255);
    for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
      modes[i].Brightness = ALLbri;
    }
    FastLED.setBrightness(ALLbri);
    loadingFlag = true;*/
	}

	else if (!strncmp_P(inputBuffer, PSTR("RESET"), 5)) // сброс настроек WIFI по запросу от приложения
	{
		//wifiManager.resetSettings();
	}

	else if (!strncmp_P(inputBuffer, PSTR("DRW"), 3))
	{
		uint8_t xx = udp_getValue(inputBuffer, ';', 1);
		uint8_t yy = udp_getValue(inputBuffer, ';', 2);
		drawPixel(xx, yy, drw_color);
	}

	else if (!strncmp_P(inputBuffer, PSTR("CLR"), 3))
	{
		fillAll(CRGB::Black);
	}

	else if (!strncmp_P(inputBuffer, PSTR("COL"), 3))
	{
		uint8_t R = udp_getValue(inputBuffer, ';', 1);
		uint8_t G = udp_getValue(inputBuffer, ';', 2);
		uint8_t B = udp_getValue(inputBuffer, ';', 3);
		drw_color = CRGB(R, G, B);
	}

	else if (!strncmp_P(inputBuffer, PSTR("DRAWOFF"), 7))
	{
		power.change(ON);
		osd.scrollText(FPSTR(msg_paint_off), CRGB::DarkRed, 3);
	}

	else if (!strncmp_P(inputBuffer, PSTR("DRAWON"), 6))
	{
		power.change(PAINT);
		osd.scrollText(FPSTR(msg_paint_on), CRGB::Green, 3);
	}

	else if (!strncmp_P(inputBuffer, PSTR("EFF"), 3))
	{
		memcpy(buff, &inputBuffer[3], strlen(inputBuffer)); // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
		if (power.mode != ON)
			power.change(ON);
		eff_on_current = (uint8_t)atoi(buff);
		effect.change(effects.getIndexByID(eff_on_current, ON), 0, false);
		flash.writeCurrentEffect(eff_on_current, ON);
#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}
	else if (!strncmp_P(inputBuffer, PSTR("BRI"), 3)) // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
	{
		memcpy(buff, &inputBuffer[3], strlen(inputBuffer));
		changeBrightness(constrain(atoi(buff), 1, 255), effects.getIndexByID(eff_on_current, ON));
		//udp_sendCurrent(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("SPD"), 3)) // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
	{
		memcpy(buff, &inputBuffer[3], strlen(inputBuffer));
		flash.writePref(pref_Speed, effects.getIndexByID(eff_on_current, ON), effect.setRealSpeed(atoi(buff)));
		//udp_sendCurrent(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("SCA"), 3)) // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
	{
		memcpy(buff, &inputBuffer[3], strlen(inputBuffer));
		effect.scale = atoi(buff);
		flash.writePref(pref_Scale, effects.getIndexByID(eff_on_current, ON),effect.scale);
		//udp_sendCurrent(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("P_ON"), 4))
	{
		flash.forceToSave = true;
		//flash.flashTS32 = millis();
		power.change(ON, 255);
		udp_sendCurrent(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("P_OFF"), 5))
	{
		flash.forceToSave = true;
		//flash.flashTS32 = millis();
		power.change(OFF, 255);
		udp_sendCurrent(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("ALM_SET"), 7))
	{
		uint8_t alarmNum = (char)inputBuffer[7] - '0';
		alarmNum -= 1;
		if (strstr_P(inputBuffer, PSTR("ON")) - inputBuffer == 9)
		{
			alarms[alarmNum].State = true;
			sendAlarms(inputBuffer);
		}
		else if (strstr_P(inputBuffer, PSTR("OFF")) - inputBuffer == 9)
		{
			alarms[alarmNum].State = false;
			sendAlarms(inputBuffer);
		}
		else
		{
			memcpy(buff, &inputBuffer[8], strlen(inputBuffer)); // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 9
			alarms[alarmNum].Time = atoi(buff);
			sendAlarms(inputBuffer);
		}
		//SaveAlarmsSettings(&alarmNum, alarms);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			strcpy(MqttManager::mqttBuffer, inputBuffer);
			MqttManager::needToPublish = true;
		}
#endif
	}
	else if (!strncmp_P(inputBuffer, PSTR("ALM_GET"), 7))
	{
		sendAlarms(inputBuffer);
	}

	else if (!strncmp_P(inputBuffer, PSTR("DAWN"), 4))
	{
		memcpy(buff, &inputBuffer[4], strlen(inputBuffer)); // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 5
		dawnMode = atoi(buff) - 1;
		SaveDawnMode(&dawnMode);
		sendAlarms(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("DISCOVER"), 8)) // обнаружение приложением модуля esp в локальной сети
	{
		if (ESP_mode == 1U) // работает только в режиме WiFi клиента
		{
			sprintf_P(inputBuffer, PSTR("IP %u.%u.%u.%u:%u"),
								WiFi.localIP()[0],
								WiFi.localIP()[1],
								WiFi.localIP()[2],
								WiFi.localIP()[3],
								ESP_UDP_PORT);
		}
	}

	else if (!strncmp_P(inputBuffer, PSTR("WHOAMI"), 8)) // расширенная информация
	{
		if (ESP_mode == 1U) // работает только в режиме WiFi клиента
		{
			sprintf_P(inputBuffer, PSTR("IP %u.%u.%u.%u:%u\nLAMPNAME: %s"),
								WiFi.localIP()[0],
								WiFi.localIP()[1],
								WiFi.localIP()[2],
								WiFi.localIP()[3],
								ESP_UDP_PORT,
								host_name);
		}
	}

	else if (!strncmp_P(inputBuffer, PSTR("GLB_BRI"), 7)) // расширенная информация
	{
		memcpy(buff, &inputBuffer[7], strlen(inputBuffer)); // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 7
		GlobalBrightness = atoi(buff) - 1;
		//SaveGlobalBrightness(&GlobalBrightness);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("effects_id_e"), 8)) // перечисление эффектов
	{
		//TODO: написать :)
	}

	else if (!strncmp_P(inputBuffer, PSTR("TMR_GET"), 7))
	{
		sendTimer(inputBuffer);
	}

	else if (!strncmp_P(inputBuffer, PSTR("TMR_SET"), 7))
	{
		memcpy(buff, &inputBuffer[8], 2); // взять подстроку, состоящую из 9 и 10 символов, из строки inputBuffer
		TimerManager::TimerRunning = (bool)atoi(buff);

		memcpy(buff, &inputBuffer[10], 2); // взять подстроку, состоящую из 11 и 12 символов, из строки inputBuffer
		TimerManager::TimerOption = (uint8_t)atoi(buff);

		memcpy(buff, &inputBuffer[12], strlen(inputBuffer)); // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 13
		TimerManager::TimeToFire = millis() + strtoull(buff, &endToken, 10) * 1000;

		TimerManager::TimerHasFired = false;
		sendTimer(inputBuffer);

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("FAV_GET"), 7))
	{
		//FavoritesManager::SetStatus(inputBuffer);
	}

	else if (!strncmp_P(inputBuffer, PSTR("FAV_SET"), 7))
	{
		//FavoritesManager::ConfigureFavorites(inputBuffer);
		//FavoritesManager::SetStatus(inputBuffer);
		flash.forceToSave = true;
		//flash.flashTS32 = millis();

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			MqttManager::needToPublish = true;
		}
#endif
	}

	else if (!strncmp_P(inputBuffer, PSTR("OTA"), 3))
	{
#if USE_OTA
		ota.RequestOtaUpdate();
		delay(50);
		ota.RequestOtaUpdate();
		effect.change(effects.getIndexByID(EFF_MATRIX), 0, false); // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
		fillAll(CRGB::Black);
		delay(1);
		//power.change();
#endif
	}
	else if (!strncmp_P(inputBuffer, PSTR("BTN"), 3))
	{
		if (strstr_P(inputBuffer, PSTR("ON")) - inputBuffer == 4)
		{
			btn_Enabled = true;
			//Savebtn_Enabled(&btn_Enabled);
			udp_sendCurrent(inputBuffer);
		}
		else if (strstr_P(inputBuffer, PSTR("OFF")) - inputBuffer == 4)
		{
			btn_Enabled = false;
			//Savebtn_Enabled(&btn_Enabled);
			udp_sendCurrent(inputBuffer);
		}

#if USE_MQTT
		if (ESP_mode == 1U)
		{
			strcpy(MqttManager::mqttBuffer, inputBuffer);
			MqttManager::needToPublish = true;
		}
#endif
	}
	else
	{
		inputBuffer[0] = '\0';
	}

	if (strlen(inputBuffer) <= 0)
	{
		return;
	}

	if (generateOutput) // если запрошен вывод ответа выполнения команд, копируем его в исходящий буфер
	{
		strcpy(outputBuffer, inputBuffer);
	}
	inputBuffer[0] = '\0'; // очистка буфера, читобы не он не интерпретировался, как следующий входной пакет
}

void parseUDP()
{
	int32_t packetSize = udp.parsePacket();

	if (packetSize)
	{
		int16_t n = udp.read(packetBuffer, MAX_UDP_BUFFER_SIZE);
		packetBuffer[n] = '\0';
		strcpy(inputBuffer, packetBuffer);

#if USE_DEBUG
		LOG.print(PSTR("Inbound UDP packet: "));
		LOG.println(inputBuffer);
#endif

		if (udp.remoteIP() == WiFi.localIP()) // не реагировать на свои же пакеты
		{
			return;
		}
		char reply[MAX_UDP_BUFFER_SIZE];
		processInputBuffer(inputBuffer, reply, true);

#if USE_MQTT // отправка ответа выполнения команд по MQTT, если разрешено
		if (ESP_mode == 1U)
		{
			strcpy(MqttManager::mqttBuffer, reply); // разрешение определяется при выполнении каждой команды отдельно, команды GET, DEB, DISCOVER и USE_OTA, пришедшие по UDP, игнорируются (приходят раз в 2 секунды от приложения)
		}
#endif

		udp.beginPacket(udp.remoteIP(), udp.remotePort());
		udp.write(reply);
		udp.endPacket();

#if USE_DEBUG
		LOG.print(PSTR("Outbound UDP packet: "));
		LOG.println(String(reply));
		LOG.println();
#endif
	}
}
#endif
#endif