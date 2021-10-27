/***************************************************
  This is a library for the SHT85 Digital Humidity & Temp Sensor

  Designed specifically to work with the SHT85-85 Digital sensor from Sensirion
  ----> https://www.sensirion.com/en/environmental-sensors/humidity-sensors/sht85-pin-type-humidity-sensor-enabling-easy-replaceability/

  These sensors use I2C to communicate, 2 pins are required to interface
  

  Written by Antoine Deblaere for IRMP/CP3
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
#ifndef SHT85_H
#define SHT85_H

#include "Arduino.h"
#include "Wire.h"
#define SHT85_DEFAULT_SDA 13
#define SHT85_DEFAULT_SCL 14
#define SHT85_DEFAULT_ADDR         0x44
#define SHT85_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT85_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT85_MEAS_LOWREP_STRETCH  0x2C10
#define SHT85_MEAS_HIGHREP         0x2400
#define SHT85_MEAS_MEDREP          0x240B
#define SHT85_MEAS_LOWREP          0x2416
#define SHT85_READSTATUS           0xF32D
#define SHT85_CLEARSTATUS          0x3041
#define SHT85_SOFTRESET            0x30A2
#define SHT85_HEATEREN             0x306D
#define SHT85_HEATERDIS            0x3066

/**
 * Driver for the Adafruit SHT85
 */
 class SHT85 {
    public:
        /**
         *  Constructor.
         */
        SHT85();

        /**
         * Initialises the I2C bus, and assigns the I2C address to us.
         *
         * @param i2caddr   The I2C address to use for the sensor.
         *
         * @return True if initialisation was successful, otherwise False.
         */
        boolean begin(uint8_t i2caddr = SHT85_DEFAULT_ADDR,int sda_pin = SHT85_DEFAULT_SDA ,int scl_pin = SHT85_DEFAULT_SCL);

        /**
         * Gets a single temperature reading from the sensor.
         *
         * @return A float value indicating the temperature.
         */
        float readTemperature(void);

        /**
         * Gets a single relative humidity reading from the sensor.
         *
         * @return A float value representing relative humidity.
         */
        float readHumidity(void);

        /**
         * Gets the current status register contents.
         *
         * @return The 16-bit status register.
         */
        uint16_t readStatus(void);

        /**
         * Performs a reset of the sensor to put it into a known state.
         */
        void reset(void);

        /**
         * Enables or disabled the heating element.
         *
         * @param h True to enable the heater, False to disable it.
         */
        void heater(boolean h);

        /**
         * Performs a CRC8 calculation on the supplied values.
         *
         * @param data  Pointer to the data to use when calculating the CRC8.
         * @param len   The number of bytes in 'data'.
         *
         * @return The computed CRC8 value.
         */
        uint8_t crc8(const uint8_t *data, int len);

    private:
        /**
         * Placeholder to track the I2C address.
         */
        uint8_t _i2caddr;

        /**
         * Placeholder to track humidity internally.
         */
        float humidity;

        /**
         * Placeholder to track temperature internally.
         */
        float temp;

        /**
         * Internal function to perform a temp + humidity read.
         *
         * @return True if successful, otherwise false.
         */
        boolean  readTempHum(void);

        /**
         * Internal function to perform and I2C write.
         *
         * @param cmd   The 16-bit command ID to send.
         */
        void     writeCommand(uint16_t cmd);

        /**
         * Internal function to read data over the I2C bus.
         *
         * @return True if successful, otherwise False.
         */
        boolean  readData(void);
};

#endif