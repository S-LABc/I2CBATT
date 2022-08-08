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

#pragma once
#include <Wire.h>
#include <Arduino.h>

#define I2CBATT_IIC_BATTERY_ADDRESS 0x55
#define I2CBATT_IIC_BATTERY_CLOCK 400000UL

#define I2CBATT_CMD_CONTROL_L 0x00
#define I2CBATT_CMD_CONTROL_H 0x01

/* Table 13. CONTROL_STATUS Flags */
#define I2CBATT_CNTL_CONTROL_STATUS_L 0x00
#define I2CBATT_CNTL_CONTROL_STATUS_H 0x00
enum I2CBATT_ControlStatusFlags {
  I2CBATT_CONTROL_STATUS_QEN,
  I2CBATT_CONTROL_STATUS_VOK,
  I2CBATT_CONTROL_STATUS_RUP_DIS,
  I2CBATT_CONTROL_STATUS_LDMD,
  I2CBATT_CONTROL_STATUS_SLEEP,
  I2CBATT_CONTROL_STATUS_FULLSLEEP,
  I2CBATT_CONTROL_STATUS_HIBERNATE,
  I2CBATT_CONTROL_STATUS_SHUTDWN,
  I2CBATT_CONTROL_STATUS_HOSTIE,
  I2CBATT_CONTROL_STATUS_QMAXUPDATE,
  I2CBATT_CONTROL_STATUS_BCA,
  I2CBATT_CONTROL_STATUS_CCA,
  I2CBATT_CONTROL_STATUS_CALMODE,
  I2CBATT_CONTROL_STATUS_SS,
  I2CBATT_CONTROL_STATUS_FAS,
  I2CBATT_CONTROL_STATUS_SE,
};

#define I2CBATT_CNTL_DEVICE_TYPE_L 0x00
#define I2CBATT_CNTL_DEVICE_TYPE_H 0x01

#define I2CBATT_CNTL_FW_VERSION_L 0x00
#define I2CBATT_CNTL_FW_VERSION_H 0x02

#define I2CBATT_CNTL_HW_VERSION_L 0x00
#define I2CBATT_CNTL_HW_VERSION_H 0x03

#define I2CBATT_CMD_TEMPERATURE_PAIR 0x06

#define I2CBATT_CMD_VOLTAGE_PAIR 0x08

/* Table 14. Flags Bit Definitions */
#define I2CBATT_CMD_FLAGS_PAIR 0x0A
enum I2CBATT_FlagsBitDefinitions {
  I2CBATT_BIT_DEFINITIONS_DSG,
  I2CBATT_BIT_DEFINITIONS_SOCF,
  I2CBATT_BIT_DEFINITIONS_SOC1,
  I2CBATT_BIT_DEFINITIONS_CHG,
  I2CBATT_BIT_DEFINITIONS_IMAX,
  I2CBATT_BIT_DEFINITIONS_RSVD_0, // не задействован
  I2CBATT_BIT_DEFINITIONS_RSVD_1, // не задействован
  I2CBATT_BIT_DEFINITIONS_CHG_SUS,
  I2CBATT_BIT_DEFINITIONS_RSVD_2, // не задействован
  I2CBATT_BIT_DEFINITIONS_FC,
  I2CBATT_BIT_DEFINITIONS_RSVD_3, // не задействован
  I2CBATT_BIT_DEFINITIONS_CHG_INH,
  I2CBATT_BIT_DEFINITIONS_BATLOW,
  I2CBATT_BIT_DEFINITIONS_BATHI,
  I2CBATT_BIT_DEFINITIONS_RSVD_4, // не задействован
  I2CBATT_BIT_DEFINITIONS_RSVD_5, // не задействован
};

#define I2CBATT_CMD_REMAINING_CAPACITY_PAIR 0x10

#define I2CBATT_CMD_FULL_CHARGE_CAPACITY_PAIR 0x12

#define I2CBATT_CMD_AVERAGE_CURRENT_PAIR 0x14

#define I2CBATT_CMD_TIME_TO_EMPTY_PAIR 0x16

#define I2CBATT_CMD_AVERAGE_POWER_PAIR 0x24

#define I2CBATT_CMD_CYCLE_COUNT_PAIR 0x2A

#define I2CBATT_CMD_STATE_OF_CHARGE_PAIR 0x2C

#define I2CBATT_EXTD_CMD_DESIGN_CAPACITY_PAIR 0x3C

#define I2CBATT_EXTD_CMD_DATA_FLASH_BLOCK 0x3F

#define I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_A 0x01
#define I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_B 0x02
#define I2CBATT_EXTD_CMD_MANUFACTURE_BLOCK_C 0x03

#define I2CBATT_EXTD_CMD_BLOCK_DATA_L 0x40
#define I2CBATT_EXTD_CMD_BLOCK_DATA_H 0x5f

#define I2CBATT_EXTD_CMD_BLOCK_DATA_CHEKSUM 0x60

class I2CBATT {
  private:
    TwoWire *_wire;
    uint8_t _block_data[I2CBATT_EXTD_CMD_BLOCK_DATA_H - I2CBATT_EXTD_CMD_BLOCK_DATA_L]; // Массив для данных из регистров 0x40-0x5f
    void pullBlockData(uint8_t str[]); // Заполнение массива _block_data
    
  public:
    I2CBATT(TwoWire *twi); // Конструктор
    // Настройка частоты на 400 кГц и вызов Wire.begin()
    void begin(void);
    // Для работы с произвольными регистрами из datasheet
    void sendFirstRegister(uint8_t reg_addr);
    void sendCommandManufactureBlock(uint8_t command);
    uint8_t requestSingleRegister(void);
    uint16_t requestPairRegisters(void);
    void writeControlAddresses(uint8_t low_payload, uint8_t high_payload);
    // Проверка по стандартному скечу поиска устройств на линии I2C
    bool isConnected(void);
    // Регистры Status 00 01
    uint16_t getControlStatus(void); // HEX
    // Биты регистров Status
    bool getFlagSEPinIsActive(void);
    bool getFlagIsFullAccessSealedMode(void);
    bool getFlagIsSealedMode(void);
    bool getFlagCalibrationFunctionIsActive(void);
    bool getFlagCoulombCounterCalibrationRoutineIsActive(void);
    bool getFlagBoardCalibrationRoutineIsActive(void);
    bool getFlagQMAXUpdate(void);
    bool getFlagHDQInterruptFunctionIsActive(void);
    bool getFlagShutdownCommandIsSent(void);
    bool getFlagRequestHibernateFromSleepMode(void);
    bool getFlagIsFullSleepMode(void);
    bool getFlagIsSleepMode(void);
    bool getFlagImpedanceTrackAlgorithm(void);
    bool getFlagRaTableUpdatesDisabled(void);
    bool getFlagCellVoltagesOK(void);
    bool getFlagQmaxUpdatesEnabled(void);
    // Регистры 00 01 (00 01, 00 02, 00 03)
    uint16_t getDeviceType(void); // HEX
    float getFirmwareVersion(void);
    float getHardwareVersion(void);
    // Регистры Temperature 06 07
    float getTemperatureKelvin(void);
    float getTemperatureCelsius(void);
    float getTemperatureFahrenheit(void);
    // Регистры Voltage 08 09
    word getVoltageMilli(void);
    float getVoltage(void);
    // Регистры Flags 0A 0B
    uint16_t getFlags(void); // HEX
    // Биты регистров Flags
    bool getFlagBatteryHighIndicating(void);
    bool getFlagBatteryLowIndicating(void);
    bool getFlagChargeInhibitindicates(void);
    bool getFlagFullChargedIsDetected(void);
    bool getFlagChargeSuspend(void);
    bool getFlagIndicatesComputedImax(void);
    bool getFlagChargingAllowed(void);
    bool getFlagStateOfChargeThreshold1(void);
    bool getFlagStateOfChargeThresholdFinal(void);
    bool getFlagDischargingDetected(void);
    // Регистры Remaining Capacity 10 11
    word getRemainingCapacity(void);
    // Регистры FullCharge Capacity 12 13
    word getFullChargeCapacity(void);
    // Регистры Average Current 14 15
    short getAverageCurrentMilli(void);
    float getAverageCurrent(void);
    // Регистры Time to Empty 16 17
    word getTimeToEmpty(void);
    // Регистры Average Power 24 25
    short getAveragePowerMilli(void);
    float getAveragePower(void);
    // Регистры Cycle Count 2A 2B
    word getCycleCount(void);
    // Регистры State of Charge 2C 2D
    byte getStateOfCharge(void);
    // Регистры Design Capacity 3C 3D
    word getDesignCapacity(void);
    // Зипись в регистр Data Flash Block 3F (01, 02, 03); чтение из регистров Block Data 40 - 5F
    char* getManufacturerInfoBlockA(void);
    char* getManufacturerInfoBlockB(void);
    char* getManufacturerInfoBlockC(void); // Не описан в документации, но данные там есть
    // Регистр Block Data Checksum 60
    uint8_t getBlockDataChecksum(void);
};
