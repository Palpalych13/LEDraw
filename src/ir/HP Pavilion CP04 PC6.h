// ----- IR REMOTE / ПУЛЬТ ДУ ----- last mod v1.7.16b
/*  Для сопряжения практически любого пульта требуется сопоставить константы с принимаемыми кодами пульта
    Любой полученный, и дешифрированный код (Code) будет отображаться в мониторе последовательного порта при включенных USE_DEBUG и DEBUG_IR
    Для обеспечения точности распознавания кода требуется направить пульт ДУ непосредственно на ИК приемник и несколько раз нажать на кнопку
    Четко полученное значение можно просто скопировать и вставить в скетч
    Некоторые системы команд пультов содержат защитныебиты позволяющие отличить, допустим, долгое нажатие от двойного клика, или счетчики кликов или ID пульта
    Для того, чтобы подавить такую информацию и оставить требуемую установите константу ir_CODEMASK
    Пример, допустим, некий пульт при последуюших нажатиях выдает коды 0xC8011040C и 0xC8011840C
        Код 1:      0x0000000C8011040C
        Код 2:      0x0000000C8011840C    
        Маска:      0xFFFFFFFFFFFF0FFF (IR_CODEMASK)
                      ************V***
        Результат:  0x0000000C8011040C (совпадает с кодом 1)                
*/
#if USE_IR
#define IR_START_STEPS 8U				// колчестро первых отсчетов нажатой кнопки при которых будет прибавляться по 1 к значению регулятора (иначе IR_REPEATE_STEP)
#define IR_REPEATE_STEP 4U			// шаг регуляторов яркости/скорости/масштаба (1...127)
#define IR_REPEATE_TIMEOUT 500U // минимальное время между посылками пульта за которое будет опредяляться состояние постоянно нажатой кнопки

// --- HP Pavilion CP04 PC6 ---
#define IR_CODEMASK 0xFFFFFFFFFFFF0FFF // маскировка отличающихся битов в кодовой посылке нажатия одной и той же клавиши
#define IR_ON 0xC8011040C							 // код пульта для Включения/Выключения
#define IR_NEXT 0xC80110412						 // код пульта для Следующего Эффекта
#define IR_PREVIOUS 0xC80110413				 // код пульта для Предыдущего Эффекта
#define IR_NEXT_MODE 0xC8011041A			 // код пульта для Следующего Режима
#define IR_PREVIOUS_MODE 0xC8011041B	 // код пульта для Предыдущего Режима
#define IR_BRIGHT_DOWN 0xC80110411		 // код пульта для понижения Яркости
#define IR_BRIGHT_UP 0xC80110410			 // код пульта для повышения Яркости
#define IR_SPEED_DOWN 0xC8011041F			 // код пульта для понижения Скорости
#define IR_SPEED_UP 0xC8011041E				 // код пульта для повышения Скорости
#define IR_SCALE_DOWN 0xC80110420			 // код пульта для понижения Масштаба
#define IR_SCALE_UP 0xC80110421				 // код пульта для повышения Масштаба
#define IR_BRIGHT_DEF 0xC8011040E			 // код пульта для сброса значения Яркости по умолчанию
#define IR_PARAMS_DEF 0xC80110422			 // код пульта для сброса значений Скорости и Масштаба по умолчанию
#define IR_LAMP 0xC8011040D						 // код пульта для включения режима Лампы
#define IR_DEMO 0xC8011044A						 // код пульта для включения режима Демо
#define IR_INFO 0xC8011040F						 // код пульта для вывода Инфо
#define IR_CLOCK 0xC8011044F					 // код пульта для вывода Времени
#endif