/*
 * I2CBATT_test
 *
 ** Пример использования функций бибилотеки I2CBATT.h для общения
 ** по интерфейсу I2C с bq27546, который находится в АКБ от iPhone.
 * 
 * ########## !!! В А Ж Н О !!! ##########
 ** Если выводы не толерантны к 5В, НЕОБХОДИМО использовать 
 ** согласователь уровней, например TXS0108E(HW-221) или подобный.
 *
 ** Для корректной работы необходимо подключить резисторы номиналом
 ** от 4.7кОм до 10кОм между контактами SDA-SCL и +VBATT АКБ.
 * 
 ** Библиотека использует ссылку на объект Wire. Это позволяет подключать 
 ** другие устройства на одну линию I2C, но частота линии должна быть 400 кГц
 *
 ** Если нужно получить данные из регистров, для которых нет готовых функций,
 ** то для этого есть доплнительные функции.
 *** sendFirstRegister(reg_addr) - отправить первый(один) байт регистра (pointer)
 *** sendCommandManufactureBlock(command) - отправить комманду на получения блока данных
 *** requestSingleRegister() - получить один байт одиночного регистр
 *** requestPairRegisters() - получить два байта подряд парного регистра
 *** writeControlAddresses(low_payload, high_payload) - записать пару байт в регистр Control()
 * 
 * Проект I2CBATT на GitHub - https://github.com/S-LABc/I2CBATT
 * Проект HDQBATT на GitHub - https://github.com/S-LABc/HDQBATT
 * 
 * Контакты:
 ** YouTube - https://www.youtube.com/channel/UCbkE52YKRphgkvQtdwzQbZQ
 ** Telegram - https://www.t.me/slabyt
 ** GitHub - https://github.com/S-LABc
 ** Gmail - romansklyar15@gmail.com
 * 
 * Copyright (C) 2021. v1.0 / Скляр Роман S-LAB
 */

// Подключаем библиотеку
#include <I2CBATT.h>

// Раскомментировать, если нужно использовать второй блок I2C
//TwoWire Wire2 (2, I2C_FAST_MODE);
//#define Wire Wire2

// Создаем объект BATT с указанием ссылки на объект Wire
I2CBATT BATT(&Wire);

void setup() {
  Serial.begin(115200);
  
  // Запускаем соединение, если его не запустила другая бибилиотека
  BATT.begin();

  // Пока не подключен АКБ
  while (!BATT.isConnected()) {
    // Выводим сообщение об отсутствии АКБ
    Serial.println("Battery not detected"); 
    delay(1000);
  }
}

void loop() {
  // Если АКБ подключен
  if (BATT.isConnected()) {
    // Читаем данные и выводим в "Монитор порта"
    readBatteryData();
  }
  else {
    // Выводим сообщение об отсутствии АКБ
    Serial.println("Battery not detected");
  }
  
  delay(3000); // Проверяем раз в 3 секунды
}

void readBatteryData() {
  // Флаги регистра CONTROL_STATUS
  Serial.print("Flags Status: ");
  Serial.print("0x");
  Serial.print(BATT.getControlStatus(), HEX); // hex значение регистра CONTROL_STATUS
  Serial.print(" SE="); // Вывод SE активен
  Serial.print(BATT.getFlagSEPinIsActive());
  Serial.print("|FAS="); // Контроллер находится в состоянии FULL ACCESS SEALED 
  Serial.print(BATT.getFlagIsFullAccessSealedMode());
  Serial.print("|SS="); // Контроллер находится в состоянии SEALED
  Serial.print(BATT.getFlagIsSealedMode());
  Serial.print("|CALMODE="); // Функция калибровки активна
  Serial.print(BATT.getFlagCalibrationFunctionIsActive());
  Serial.print("|CCA="); // Процедура CCA активна
  Serial.print(BATT.getFlagCoulombCounterCalibrationRoutineIsActive());
  Serial.print("|BCA="); // Процедура калибровки платы активна
  Serial.print(BATT.getFlagBoardCalibrationRoutineIsActive());
  Serial.print("|QMAXUPDATE="); // Изменилась QMAX
  Serial.print(BATT.getFlagQMAXUpdate());
  Serial.print("|HOSTIE="); // Функция прерывания HDQ активна
  Serial.print(BATT.getFlagHDQInterruptFunctionIsActive());
  Serial.print("|SHUTDWN="); // Команда SET_SHUTDOWN была отправлена
  Serial.print(BATT.getFlagShutdownCommandIsSent());
  Serial.print("|HIBERNATE="); // Выдан запрос на переход в режим HIBERNATE
  Serial.print(BATT.getFlagRequestHibernateFromSleepMode());
  Serial.print("|FULLSLEEP="); // Контроллер находится в режиме FULLSLEEP
  Serial.print(BATT.getFlagIsFullSleepMode());
  Serial.print("|SLEEP="); // Контроллер находится в режиме SLEEP
  Serial.print(BATT.getFlagIsSleepMode());
  Serial.print("|LDMD="); // Используется режим CONSTANT-POWER
  Serial.print(BATT.getFlagImpedanceTrackAlgorithm());
  Serial.print("|RUP_DIS="); // Обновления таблицы Ra отключены
  Serial.print(BATT.getFlagRaTableUpdatesDisabled());
  Serial.print("|VOK="); // Напряжения банки в порядке
  Serial.print(BATT.getFlagCellVoltagesOK());
  Serial.print("|QEN="); // Обновления QMAX включены
  Serial.println(BATT.getFlagQmaxUpdatesEnabled());
  
  // Флаги состояния Flags()
  Serial.print("Flags Definitions: ");
  Serial.print("0x");
  Serial.print(BATT.getFlags(), HEX); // hex значение регистра Flags()
  Serial.print(" RSVD(5)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_5) & 1);
  Serial.print("|RSVD(4)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_4) & 1);
  Serial.print("|BATHI="); // Высокий уровень зяряда
  Serial.print(BATT.getFlagBatteryHighIndicating());
  Serial.print("|BATLOW="); // Низкий уровень зяряда
  Serial.print(BATT.getFlagBatteryLowIndicating());
  Serial.print("|CHG_INH="); // Запрет зарядки при высокой температуре
  Serial.print(BATT.getFlagChargeInhibitindicates());
  Serial.print("|RSVD(3)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_3) & 1);
  Serial.print("|FC="); // Полностью зажен
  Serial.print(BATT.getFlagFullChargedIsDetected());
  Serial.print("RSVD(2)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_2) & 1);
  Serial.print("|CHG_SUS="); // Приостановка зарядки
  Serial.print(BATT.getFlagChargeSuspend());
  Serial.print("|RSVD(1)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_1) & 1);
  Serial.print("|RSVD(0)="); // Так можно достать отдельныве биты
  Serial.print((BATT.getFlags() >> BIT_DEFINITIONS_RSVD_0) & 1);
  Serial.print("|IMAX="); // Готовность вычислений Imax()
  Serial.print(BATT.getFlagIndicatesComputedImax());
  Serial.print("|CHG="); // Разрешена быстрая зарядка
  Serial.print(BATT.getFlagChargingAllowed());
  Serial.print("|SOC1="); // Достугнут порг заряда SOC1
  Serial.print(BATT.getFlagStateOfChargeThreshold1());
  Serial.print("|SOCF="); // Достугнут порг заряда SOCF
  Serial.print(BATT.getFlagStateOfChargeThresholdFinal());
  Serial.print("|DSG="); // Обнаружена разрядка АКБ
  Serial.println(BATT.getFlagDischargingDetected());

  // Модель контроллера
  Serial.print("Device Type: ");
  Serial.println(BATT.getDeviceType(), HEX);
  
  // Версия прошивки
  Serial.print("Firmware Version: ");
  Serial.println(BATT.getFirmwareVersion());

  // Версия оборудования
  Serial.print("Hardware Version: ");
  Serial.println(BATT.getHardwareVersion());
  
  // Температура 
  Serial.print("Temperature: ");
  Serial.print(BATT.getTemperatureKelvin()); // Кельвин
  Serial.print(" K | ");
  Serial.print(BATT.getTemperatureCelsius()); // Цельсий
  Serial.print(" C | ");
  Serial.print(BATT.getTemperatureFahrenheit()); //Фаренгейт
  Serial.println(" F");
  
  // Напряжение
  Serial.print("Voltage: ");
  Serial.print(BATT.getVoltageMilli()); // Милливольты
  Serial.print(" mV | ");
  Serial.print(BATT.getVoltage()); // Вольты
  Serial.println(" V");

  // Оставшийся заряд в АКБ
  Serial.print("Remaining Capacity: ");
  Serial.print(BATT.getRemainingCapacity());
  Serial.println(" mAh");

  // Оставшаяся емкость АКБ
  Serial.print("Full Charge Capacity: ");
  Serial.print(BATT.getFullChargeCapacity());
  Serial.println(" mAh");
  
  // Ток при зарядке/разрядке
  Serial.print("Average Current: ");
  Serial.print(BATT.getAverageCurrentMilli()); // Миллиамперы
  Serial.print(" mA | ");
  Serial.print(BATT.getAverageCurrent()); // Амперы
  Serial.println(" A");
  
  // Время до полной разрядки
  Serial.print("Time To Empty: ");
  Serial.print(BATT.getTimeToEmpty());
  Serial.println(" min");
  
  // Мощность при зарядке/разрядке
  Serial.print("Average Power: ");
  Serial.print(BATT.getAveragePowerMilli()); // Милливатты
  Serial.print(" mW | ");
  Serial.print(BATT.getAveragePower()); // Ватты
  Serial.println(" W");
  
  // Количество циклов зарядок/разрядок
  Serial.print("Cycles: ");
  Serial.print(BATT.getCycleCount());
  Serial.println(" count");
  
  // Уровень заряда
  Serial.print("State Of Charge: ");
  Serial.print(BATT.getStateOfCharge());
  Serial.println(" %");
  
  // Заводская емкость
  Serial.print("Design Capacity: ");
  Serial.print(BATT.getDesignCapacity());
  Serial.println(" mAh");

  // Блоки информации о производителе 
  Serial.print("Manufacturer Info Block A: ");
  Serial.println(BATT.getManufacturerInfoBlockA());
  Serial.print("Manufacturer Info Block B: ");
  Serial.println(BATT.getManufacturerInfoBlockB());
  Serial.print("Manufacturer Info Block C: ");
  Serial.println(BATT.getManufacturerInfoBlockC());

  // Контрольная сумма блока данных
  Serial.print("Block Data Checksum: ");
  Serial.println(BATT.getBlockDataChecksum(), HEX);

  Serial.println();
}
