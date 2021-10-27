/***************************************************
  This is a library for the SHT85 Digital Humidity & Temp Sensor

  Designed specifically to work with the SHT85-85 Digital sensor from Sensirion
  ----> https://www.sensirion.com/en/environmental-sensors/humidity-sensors/sht85-pin-type-humidity-sensor-enabling-easy-replaceability/

  These sensors use I2C to communicate, 2 pins are required to interface
  

  Written by Antoine Deblaere for IRMP/CP3
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
#include "SHT85.h"

SHT85::SHT85() {
  _i2caddr = NULL;
  humidity = 0.0f;
  temp = 0.0f;
}

boolean SHT85::begin(uint8_t i2caddr,int sda_pin,int scl_pin) {
  Wire.begin(sda_pin,scl_pin);
  _i2caddr = i2caddr;
  reset();
  // return (readStatus() == 0x40);
  return true;
}

uint16_t SHT85::readStatus(void) {
  writeCommand(SHT85_READSTATUS);
  Wire.requestFrom(_i2caddr, (uint8_t)3);
  uint16_t stat = Wire.read();
  stat <<= 8;
  stat |= Wire.read();
  //Serial.println(stat, HEX);
  return stat;
}

void SHT85::reset(void) {
  writeCommand(SHT85_SOFTRESET);
  delay(10);
}

void SHT85::heater(boolean h) {
  if (h)
    writeCommand(SHT85_HEATEREN);
  else
    writeCommand(SHT85_HEATERDIS);
}

float SHT85::readTemperature(void) {
  if (!readTempHum())
    return NAN;

  return temp;
}

float SHT85::readHumidity(void) {
  if (!readTempHum())
    return NAN;

  return humidity;
}

boolean SHT85::readTempHum(void) {
  uint8_t readbuffer[6];

  writeCommand(SHT85_MEAS_HIGHREP);

  delay(20);
  Wire.requestFrom(_i2caddr, (uint8_t)6);
  if (Wire.available() != 6)
    return false;
  for (uint8_t i = 0; i < 6; i++) {
    readbuffer[i] = Wire.read();
    //  Serial.print("0x"); Serial.println(readbuffer[i], HEX);
  }

  uint16_t ST, SRH;
  ST = readbuffer[0];
  ST <<= 8;
  ST |= readbuffer[1];

  if (readbuffer[2] != crc8(readbuffer, 2))
    return false;

  SRH = readbuffer[3];
  SRH <<= 8;
  SRH |= readbuffer[4];

  if (readbuffer[5] != crc8(readbuffer + 3, 2))
    return false;

  // Serial.print("ST = "); Serial.println(ST);
  double stemp = ST;
  stemp *= 175;
  stemp /= 0xffff;
  stemp = -45 + stemp;
  temp = stemp;

  //  Serial.print("SRH = "); Serial.println(SRH);
  double shum = SRH;
  shum *= 100;
  shum /= 0xFFFF;

  humidity = shum;

  return true;
}

void SHT85::writeCommand(uint16_t cmd) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(cmd >> 8);
  Wire.write(cmd & 0xFF);
  Wire.endTransmission();
}

uint8_t SHT85::crc8(const uint8_t *data, int len) {
  /*
   *
   * CRC-8 formula from page 14 of SHT spec pdf
   *
   * Test data 0xBE, 0xEF should yield 0x92
   *
   * Initialization data 0xFF
   * Polynomial 0x31 (x8 + x5 +x4 +1)
   * Final XOR 0x00
   */

  const uint8_t POLYNOMIAL(0x31);
  uint8_t crc(0xFF);

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}