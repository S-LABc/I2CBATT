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
 ** YouTube - https://www.youtube.com/channel/UCbkE52YKRphgkvQtdwzQbZQ
 ** Telegram - https://www.t.me/slabyt
 ** GitHub - https://github.com/S-LABc
 ** Gmail - romansklyar15@gmail.com
 * 
 * Copyright (C) 2021. v1.0 / Скляр Роман S-LAB
 */

#pragma once
#include <Wire.h>

#define IIC_BATTERY_ADDRESS 0x55
#define IIC_BATTERY_CLOCK 400000UL

#define CMD_CONTROL_L 0x00
#define CMD_CONTROL_H 0x01

/* Table 13. CONTROL_STATUS Flags */
#define CNTL_CONTROL_STATUS_L 0x00
#define CNTL_CONTROL_STATUS_H 0x00
enum ControlStatusFlags {
  CONTROL_STATUS_QEN,
  CONTROL_STATUS_VOK,
  CONTROL_STATUS_RUP_DIS,
  CONTROL_STATUS_LDMD,
  CONTROL_STATUS_SLEEP,
  CONTROL_STATUS_FULLSLEEP,
  CONTROL_STATUS_HIBERNATE,
  CONTROL_STATUS_SHUTDWN,
  CONTROL_STATUS_HOSTIE,
  CONTROL_STATUS_QMAXUPDATE,
  CONTROL_STATUS_BCA,
  CONTROL_STATUS_CCA,
  CONTROL_STATUS_CALMODE,
  CONTROL_STATUS_SS,
  CONTROL_STATUS_FAS,
  CONTROL_STATUS_SE,
};

#define CNTL_DEVICE_TYPE_L 0x00
#define CNTL_DEVICE_TYPE_H 0x01

#define CNTL_FW_VERSION_L 0x00
#define CNTL_FW_VERSION_H 0x02

#define CNTL_HW_VERSION_L 0x00
#define CNTL_HW_VERSION_H 0x03

#define CMD_TEMPERATURE_PAIR 0x06

#define CMD_VOLTAGE_PAIR 0x08

/* Table 14. Flags Bit Definitions */
#define CMD_FLAGS_PAIR 0x0A
enum FlagsBitDefinitions {
  BIT_DEFINITIONS_DSG,
  BIT_DEFINITIONS_SOCF,
  BIT_DEFINITIONS_SOC1,
  BIT_DEFINITIONS_CHG,
  BIT_DEFINITIONS_IMAX,
  BIT_DEFINITIONS_RSVD_0, // не задействован
  BIT_DEFINITIONS_RSVD_1, // не задействован
  BIT_DEFINITIONS_CHG_SUS,
  BIT_DEFINITIONS_RSVD_2, // не задействован
  BIT_DEFINITIONS_FC,
  BIT_DEFINITIONS_RSVD_3, // не задействован
  BIT_DEFINITIONS_CHG_INH,
  BIT_DEFINITIONS_BATLOW,
  BIT_DEFINITIONS_BATHI,
  BIT_DEFINITIONS_RSVD_4, // не задействован
  BIT_DEFINITIONS_RSVD_5, // не задействован
};

#define CMD_REMAINING_CAPACITY_PAIR 0x10

#define CMD_FULL_CHARGE_CAPACITY_PAIR 0x12

#define CMD_AVERAGE_CURRENT_PAIR 0x14

#define CMD_TIME_TO_EMPTY_PAIR 0x16

#define CMD_AVERAGE_POWER_PAIR 0x24

#define CMD_CYCLE_COUNT_PAIR 0x2A

#define CMD_STATE_OF_CHARGE_PAIR 0x2C

#define EXTD_CMD_DESIGN_CAPACITY_PAIR 0x3C

#define EXTD_CMD_DATA_FLASH_BLOCK 0x3F

#define EXTD_CMD_MANUFACTURE_BLOCK_A 0x01
#define EXTD_CMD_MANUFACTURE_BLOCK_B 0x02
#define EXTD_CMD_MANUFACTURE_BLOCK_C 0x03

#define EXTD_CMD_BLOCK_DATA_L 0x40
#define EXTD_CMD_BLOCK_DATA_H 0x5f

#define EXTD_CMD_BLOCK_DATA_CHEKSUM 0x60

class I2CBATT {
  private:
    TwoWire *_wire;
    uint8_t _block_data[EXTD_CMD_BLOCK_DATA_H - EXTD_CMD_BLOCK_DATA_L]; // Массив для данных из регистров 0x40-0x5f
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
    unsigned short getVoltageMilli(void);
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
    unsigned short getRemainingCapacity(void);
    // Регистры FullCharge Capacity 12 13
    unsigned short getFullChargeCapacity(void);
    // Регистры Average Current 14 15
    short getAverageCurrentMilli(void);
    float getAverageCurrent(void);
    // Регистры Time to Empty 16 17
    unsigned short getTimeToEmpty(void);
    // Регистры Average Power 24 25
    short getAveragePowerMilli(void);
    float getAveragePower(void);
    // Регистры Cycle Count 2A 2B
    unsigned short getCycleCount(void);
    // Регистры State of Charge 2C 2D
    byte getStateOfCharge(void);
    // Регистры Design Capacity 3C 3D
    unsigned short getDesignCapacity(void);
    // Зипись в регистр Data Flash Block 3F (01, 02, 03); чтение из регистров Block Data 40 - 5F
    char* getManufacturerInfoBlockA(void);
    char* getManufacturerInfoBlockB(void);
    char* getManufacturerInfoBlockC(void); // Не описан в документации, но данные там есть
    // Регистр Block Data Checksum 60
    uint8_t getBlockDataChecksum(void);
};
