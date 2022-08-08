/*
 * Библиотека позволяет получить доступ к некоторым регистрам
 ** контроллера bq27546 от компании Texas Instruments по интерфейсу I2C.
 ** Можно подключится и к другим контроллерам семейства bq2754x
 * 
 ** Библиотека использует ссылку на объект Wire. Это позволяет подключать
 ** другие устройства на одну линию I2C, но частота линии должна быть 400 кГц
 * 
 * Сопроводительная документация:
 ** https://www.ti.com/lit/an/slua408a/slua408a.pdf
 ** https://www.ti.com/lit/an/slva101/slva101.pdf
 ** https://www.ti.com/lit/an/slua917/slua917.pdf
 ** https://www.ti.com/lit/an/slua503/slua503.pdf
 ** https://www.ti.com/lit/ug/sluub74/sluub74.pdf
 ** https://www.ti.com/lit/ds/symlink/bq27541.pdf
 ** https://www.ti.com/lit/ds/symlink/bq27545-g1.pdf
 ** https://www.ti.com/lit/ds/symlink/bq27425-g2a.pdf
 ** https://www.ti.com/lit/ds/symlink/bq27546-g1.pdf
 * 
 * Проект I2CBATT на GitHub - https://github.com/S-LABc/I2CBATT
 * Проект HDQBATT на GitHub - https://github.com/S-LABc/HDQBATT
 * 
 * Контакты:
 ** GitHub - https://github.com/S-LABc
 ** Gmail - romansklyar15@gmail.com
 * 
 * Copyright (C) 2021. v1.1 / Скляр Роман S-LAB
 */

#include "I2CBATT.h"

// ########## CONSTRUCTOR ##########
I2CBATT::I2CBATT(TwoWire *twi) : _wire(twi ? twi : &Wire) {}

// ########## PRIVATE ##########
/*
 * @brief: получить данные из регистров 0x40...0x5F
 * @param str[]: массив для хранения считанных данных
 */
void I2CBATT::pullBlockData(uint8_t str[]) {
  // Запросить 32 байта данных по адресу 0x55
  _wire->requestFrom(I2CBATT_IIC_BATTERY_ADDRESS, (I2CBATT_EXTD_CMD_BLOCK_DATA_H - I2CBATT_EXTD_CMD_BLOCK_DATA_L));
  
  // Прочитать данные из буфера
  if (_wire->available() >= 1 ) {
    for (uint8_t i = 0; i < (I2CBATT_EXTD_CMD_BLOCK_DATA_H - I2CBATT_EXTD_CMD_BLOCK_DATA_L); i ++) {
      str[i] = _wire->read();
    }
  }
  
  // Завершить соединение
  _wire->endTransmission();
}

// ########## PUBLIC ##########
/* 
 * @brief: настройка частоты шины i2c и инициализации
 * @note: следует использовать если те же действия не выполняются в других библиотеках
 */
void I2CBATT::begin(void) {
  // Настройка частоты (400 кГц)
  _wire->setClock(I2CBATT_IIC_BATTERY_CLOCK);
  
  _wire->begin();
}
/* 
 * @brief: передать первый байт регистра (pointer)
 */
void I2CBATT::sendFirstRegister(uint8_t reg_addr) {
  // Начать передачу по адресу 0x55
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  
  // Отправить байт регистра
  _wire->write(reg_addr);
  
  // Завершить соединение
  _wire->endTransmission();
}
/* 
 * @brief: передать первый байт регистра (pointer)
 */
void I2CBATT::sendCommandManufactureBlock(uint8_t command) {
  // Начать передачу по адресу 0x55 для прередачи команды блока с данными производителя
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  _wire->write(I2CBATT_EXTD_CMD_DATA_FLASH_BLOCK);
  _wire->write(command);
  
  // Завершить соединение
  _wire->endTransmission();
  
  // Начать передачу по адресу 0x55 для возврата к младшему байту регистра блока с данными производителя
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  _wire->write(I2CBATT_EXTD_CMD_BLOCK_DATA_L);
  
  // Завершить соединение
  _wire->endTransmission();
}
/* 
 * brief: запросить один байт данных
 * @return: целое число размером один байт uint8_t
 * @note: использовать для одиночного регистра, например 0x60
 */
uint8_t I2CBATT::requestSingleRegister(void) {
  uint8_t single_byte = 0;
  
  // Запросить байт данных по адресу 0x55
  _wire->requestFrom(I2CBATT_IIC_BATTERY_ADDRESS, 1);
  
  // Прочитать данные из буфера
  if (_wire->available() >= 1 ) {
    single_byte = _wire->read();
  }
  
  // Завершить соединение
  _wire->endTransmission();

  return single_byte;
}
/* 
 * brief: запросить два байта данных
 * @return: целое число размером два байта uint16_t
 * @note: использовать для парных регистров, например 0x06 0x07
 */
uint16_t I2CBATT::requestPairRegisters(void) {
  uint8_t low_byte = 0;
  uint8_t high_byte = 0;
  
  // Запросить два байта данных по адресу 0x55
  _wire->requestFrom(I2CBATT_IIC_BATTERY_ADDRESS, 2);
  
  // Прочитать данные из буфера
  if (_wire->available() >= 1 ) {
    low_byte = _wire->read();
    high_byte = _wire->read();
  }
  
  // Завершить соединение
  _wire->endTransmission();

  return ((high_byte << 8) | low_byte);
}
/*
 * @brief: записать 2 байта в регистр Control()
 * @param low_payload: младший байт полезных данных
 * @param high_payload: старший байт полезных данных
 */
void I2CBATT::writeControlAddresses(uint8_t low_payload, uint8_t high_payload) {
  // Начать передачу по адресу 0x55 для прередачи старшего байта данных в младший регистр
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  _wire->write(I2CBATT_CMD_CONTROL_L);
  _wire->write(high_payload);
  
  // Завершить соединение
  _wire->endTransmission();
  
  // Начать передачу по адресу 0x55 для передачи младшего байта данных в старший регистр
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  _wire->write(I2CBATT_CMD_CONTROL_H);
  _wire->write(low_payload);
  
  // Завершить соединение
  _wire->endTransmission();
  
  // Начать передачу по адресу 0x55 для возврата к младшему байту регистра
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  _wire->write(I2CBATT_CMD_CONTROL_L);
  
  // Завершить соединение
  _wire->endTransmission();
}
/*
 * @brief: узнать подкючен ли АКБ к линии I2C
 * @return: логическое значение bool
 */
bool I2CBATT::isConnected(void) {
  // Начать передачу по адресу 0x55
  _wire->beginTransmission(I2CBATT_IIC_BATTERY_ADDRESS);
  return (!_wire->endTransmission()) ? true : false;
}
/* 
 * @brief: получить флаги из регистра CONTROL_STATUS АКБ
 * @return: целое число размером два байта uint16_t
 */
uint16_t I2CBATT::getControlStatus(void) {
  I2CBATT::writeControlAddresses(I2CBATT_CNTL_CONTROL_STATUS_L, I2CBATT_CNTL_CONTROL_STATUS_H);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить Бит состояния, указывающий, что вывод SE активен
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagSEPinIsActive(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_SE) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что контроллер находится в состоянии FULL ACCESS SEALED
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagIsFullAccessSealedMode(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_FAS) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что контроллер находится в состоянии SEALED
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagIsSealedMode(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_SS) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что функция калибровки активна
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagCalibrationFunctionIsActive(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_CALMODE) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что активна процедура CCA
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagCoulombCounterCalibrationRoutineIsActive(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_CCA) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что процедура калибровки платы активна
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagBoardCalibrationRoutineIsActive(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_BCA) & 1;
}
/* 
 * @brief: получить Бит состояния, переключаемый при измении состояния QMAX
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagQMAXUpdate(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_QMAXUPDATE) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что функция прерывания HDQ активна
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagHDQInterruptFunctionIsActive(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_HOSTIE) & 1;
}
/* 
 * @brief: получить управляющий Бит, указывающий, что команда SET_SHUTDOWN была отправлена
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagShutdownCommandIsSent(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_SHUTDWN) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что запрос на переход в режим HIBERNATE из режима SLEEP был выдан
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagRequestHibernateFromSleepMode(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_HIBERNATE) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что контроллер находится в режиме FULLSLEEP
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagIsFullSleepMode(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_FULLSLEEP) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что контроллер находится в режиме SLEEP
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagIsSleepMode(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_SLEEP) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что алгоритм отслеживания импеданса использует режим CONSTANT-POWER
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagImpedanceTrackAlgorithm(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_LDMD) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что обновления таблицы Ra отключены
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagRaTableUpdatesDisabled(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_RUP_DIS) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что напряжения банки в порядке
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagCellVoltagesOK(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_VOK) & 1;
}
/* 
 * @brief: получить Бит состояния, указывающий, что обновления Qmax включены
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagQmaxUpdatesEnabled(void) {
  return (I2CBATT::getControlStatus() >> I2CBATT_CONTROL_STATUS_QEN) & 1;
}
/* 
 * @brief: получить модель контроллера АКБ (модель: bq27546, результат: 0x0546)
 * @return: целое число размером два байта uint16_t
 */
uint16_t I2CBATT::getDeviceType(void) {
  I2CBATT::writeControlAddresses(I2CBATT_CNTL_DEVICE_TYPE_L, I2CBATT_CNTL_DEVICE_TYPE_H);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить версию прошивки контроллера АКБ
 * @return: число с плавающей точкой float
 */
float I2CBATT::getFirmwareVersion(void) {
  I2CBATT::writeControlAddresses(I2CBATT_CNTL_FW_VERSION_L, I2CBATT_CNTL_FW_VERSION_H);
  float f = I2CBATT::requestSingleRegister() * 0.01;
  return I2CBATT::requestSingleRegister() + f;
}
/* 
 * @brief: получить версию оборудования контроллера АКБ
 * @return: число с плавающей точкой float
 */
float I2CBATT::getHardwareVersion(void) {
  I2CBATT::writeControlAddresses(I2CBATT_CNTL_HW_VERSION_L, I2CBATT_CNTL_HW_VERSION_H);
  float f = I2CBATT::requestSingleRegister() * 0.01;
  return I2CBATT::requestSingleRegister() + f;
}
/* 
 * brief: получить температуру АКБ (градусы Кельвина)
 * @return: число с плавающей точкой float
 */
float I2CBATT::getTemperatureKelvin(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_TEMPERATURE_PAIR);
  return I2CBATT::requestPairRegisters() * 0.1;
}
/* 
 * brief: получить температуру АКБ (градусы Цельсия)
 * @return: число с плавающей точкой float
 */
float I2CBATT::getTemperatureCelsius(void) {  
  return I2CBATT::getTemperatureKelvin() - 273.1;
}
/* 
 * brief: получить температуру АКБ (градусы Фаренгейта)
 * @return: число с плавающей точкой float
 */
float I2CBATT::getTemperatureFahrenheit(void) {
  return I2CBATT::getTemperatureCelsius() * 1.8 + 32;
}
/* 
 * brief: получить напряжение на клемах АКБ (милливольты)
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getVoltageMilli(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_VOLTAGE_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * brief: получить напряжение на клемах АКБ (вольты)
 * @return: число с плавающей точкой float
 */
float I2CBATT::getVoltage(void) {
  return I2CBATT::getVoltageMilli() * 0.001;
}
/* 
 * @brief: получить флаги из регистра FLAGS АКБ
 * @return: целое число размером два байта uint16_t
 */
uint16_t I2CBATT::getFlags(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_FLAGS_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить Бит высокого уровня заряда
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagBatteryHighIndicating(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_BATHI) & 1;
}
/* 
 * @brief: получить Бит низкого уровня заряда
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagBatteryLowIndicating(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_BATLOW) & 1;
}
/* 
 * @brief: получить Бит запрета зарядки при высокой температуре
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagChargeInhibitindicates(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_CHG_INH) & 1;
}
/* 
 * @brief: получить Бит полностью заряжен
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagFullChargedIsDetected(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_FC) & 1;
}
/* 
 * @brief: получить Бит приостановки зарядки
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagChargeSuspend(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_CHG_SUS) & 1;
}
/* 
 * @brief: получить Бит готовности вычисления Imax()
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagIndicatesComputedImax(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_IMAX) & 1;
}
/* 
 * @brief: получить Бит разрешенной быстрой зарядки
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagChargingAllowed(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_CHG) & 1;
}
/* 
 * @brief: получить Бит достижения порога заряда SOC1
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagStateOfChargeThreshold1(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_SOC1) & 1;
}
/* 
 * @brief: получить Бит достижения порога заряда SOCF
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagStateOfChargeThresholdFinal(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_SOCF) & 1;
}
/* 
 * @brief: получить Бит обнаружения разрядки АКБ
 * @return: логическое значение bool
 */
bool I2CBATT::getFlagDischargingDetected(void) {  
  return (I2CBATT::getFlags() >> I2CBATT_BIT_DEFINITIONS_DSG) & 1;
}
/* 
 * brief: получить скомпенсированный оставшийся заряд в АКБ (миллиампер-часы)
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getRemainingCapacity(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_REMAINING_CAPACITY_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * brief: получить скомпенсированнаую оставшуюся емкость АКБ (миллиампер-часы)
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getFullChargeCapacity(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_FULL_CHARGE_CAPACITY_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить ток зарядки/разрядки АКБ (миллиамперы). Знак "-" указывает на разрядку
 * @return: число со знаком размером два байта short
 */
short I2CBATT::getAverageCurrentMilli(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_AVERAGE_CURRENT_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить ток зарядки/разрядки АКБ (амперы). Знак "-" указывает на разрядку
 * @return: число с плавающей точкой float
 */
float I2CBATT::getAverageCurrent(void) {
  return I2CBATT::getAverageCurrentMilli() * 0.001;
}
/* 
 * @brief: получить время до полного разряда (минуты)
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getTimeToEmpty(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_TIME_TO_EMPTY_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить потребляемую мощность при зарядке/разрядке (милливатты). Знак "-" указывает на разрядку
 * @return: целое число размером два байта unsigned short
 */
short I2CBATT::getAveragePowerMilli(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_AVERAGE_POWER_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить потребляемую мощность при зарядке/разрядке (ватты). Знак "-" указывает на разрядку
 * @return: число с плавающей точкой float
 */
float I2CBATT::getAveragePower(void) {
  return I2CBATT::getAveragePowerMilli() * 0.001;
}
/* 
 * @brief: получить количество циклов перезарядки АКБ
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getCycleCount(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_CYCLE_COUNT_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить оставшийся заряд АКБ (проценты)
 * @return: целое число размером один байт byte
 */
byte I2CBATT::getStateOfCharge(void) {
  I2CBATT::sendFirstRegister(I2CBATT_CMD_STATE_OF_CHARGE_PAIR);
  return I2CBATT::requestPairRegisters();
}
/* 
 * @brief: получить емкость АКБ установленную на заводе (миллиампер-часы)
 * @return: целое число размером два байта unsigned short
 */
word I2CBATT::getDesignCapacity(void) {
  I2CBATT::sendFirstRegister(I2CBATT_EXTD_CMD_DESIGN_CAPACITY_PAIR);
  return I2CBATT::requestPairRegisters();
}
/*
 * @brief: получить информацию о производителе. Блок A
 * @return: указатель на массив символов (строка)
 */
char* I2CBATT::getManufacturerInfoBlockA(void) {
  I2CBATT::sendCommandManufactureBlock(I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_A);
  I2CBATT::pullBlockData(_block_data);
  return (char*)_block_data;
}
/*
 * @brief: получить информацию о производителе. Блок B
 * @return: указатель на массив символов (строка)
 */
char* I2CBATT::getManufacturerInfoBlockB(void) {
  I2CBATT::sendCommandManufactureBlock(I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_B);
  I2CBATT::pullBlockData(_block_data);
  return (char*)_block_data;
}
/*
 * @brief: получить информацию о производителе. Блок C
 * @return: указатель на массив символов (строка)
 */
char* I2CBATT::getManufacturerInfoBlockC(void) {
  I2CBATT::sendCommandManufactureBlock(I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_C);
  I2CBATT::pullBlockData(_block_data);
  return (char*)_block_data;
}
/*
 * @brief: получить контрольную сумму блока данных в диапазоне 0x40...0x5f в шестнадцатиричном виде
 * @return: целое число размером один байт uint8_t
 */
uint8_t I2CBATT::getBlockDataChecksum(void) {
  I2CBATT::sendFirstRegister(I2CBATT_EXTD_CMD_BLOCK_DATA_CHEKSUM);
  return I2CBATT::requestSingleRegister();
}
