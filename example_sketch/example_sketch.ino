#include "ESP8266WiFi.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" 
#include <SlowControl.h>

//DS18B20
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SlowControl slowControl;

float tempDS;

//SCD30
SCD30 airSensor;

unsigned char buf[4];   
int16_t _ambientTemp;
uint16_t _relHumidity;   

//WiFi Config
//const char* ssid = "vide"; 
//const char* password = "mdmd2764"; 

void setup() {
  //Start Serial Monitor
  Serial.begin(115200);

  //Start Slow Control Library
  slowControl.begin();

  //Start I2C
  Wire.begin(2,14);
  
  //Start DS18B20 Sensors
  sensors.begin();

   //Reset WiFi Settings if needed --> Uncomment --> Upload --> Comment Again --> Upload
  //slowControl.resetWifiSettings();

  //Initialize WiFi Communication
  slowControl.connectToWifi();

 //Set MQTT Server - Uncomment this function if you want to hardcode your MQTT Credentials - If not you will be able to enter it in the Acces Point Configuration
  //slowControl.setMQTTServer("");

  //Initialize MQTT Communication
  slowControl.connectToMQTT(3,"ESP02",true); //Parameter 1 = Number of Try  || Parameter 2 = ID of the Board || Parameter 3 = Subscription to TTl Status
  
  //Check if CO² Sensor is there
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  //Start HI8130
  HIH8130_Begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Monitor WiFi & MQTT Reconnection
  slowControl.run();
  
  //Read DS18B20
  tempDS = readDS();
  Serial.print("DS Temp : ");
  Serial.println(tempDS);
  Serial.println();

  float co2_scd,temp_scd,hum_scd;  
   
  //Read SCD30
  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());
    co2_scd = airSensor.getCO2();
    
    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);
    temp_scd = airSensor.getTemperature();

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);
    hum_scd = airSensor.getHumidity();
    Serial.println();
    Serial.println();
  }
  else
  {
    Serial.println("Waiting for new data");
  }

  //Read HIH8130
  Wire.requestFrom(0x27,4);

  int i = 0;

  for (uint8_t i = 0; i < 4; ++i)
  {
      buf[i] = Wire.read();
  }
  
  uint16_t rawHumidity = ((((uint16_t)buf[0] & 0x3F) << 8) | (uint16_t)buf[1]);
  uint16_t rawTemp = ((uint16_t)buf[2] << 6) | ((uint16_t)buf[3] >> 2);
  
  _relHumidity = (long(rawHumidity) * 10000L) / 16382;
  _ambientTemp = ((long(rawTemp) * 16500L) / 16382) - 4000;
  

  //int reading_hum = (buf[0] << 8) + buf[1];
  //double humidity = reading_hum / 16382.0 * 100.0;
  Serial.print("Humidity Honey : ");
  Serial.println(_relHumidity/100.0);
  Serial.println();
  float hum_honey = _relHumidity/100.0;
  
  //int reading_temp = (buf[2] << 6) + (buf[3] >> 2);
  //double temperature = reading_temp / 16382.0 * 165.0 - 40;
  Serial.print("Temp Honey : ");
  Serial.println(_ambientTemp/100.0);
  Serial.println();
  float temp_honey = _ambientTemp/100.0;
 

  //Loop Delay
  delay(2000);

 
  //Here publish your readings 
  slowControl.publishToMQTT(DS_TEMP_TOPIC,String(tempDS).c_str());
  slowControl.publishToMQTT(HIH_TEMP_TOPIC,String(temp_honey).c_str());
  slowControl.publishToMQTT(HIH_HUMI_TOPIC,String(hum_honey).c_str());
  slowControl.publishToMQTT(SCD_CO2_TOPIC,String(co2_scd).c_str());
  slowControl.publishToMQTT(SCD_TEMP_TOPIC,String(temp_scd).c_str());
  slowControl.publishToMQTT(SCD_HUMI_TOPIC,String(hum_scd).c_str());

}

void HIH8130_Begin()
{
  Wire.beginTransmission(0x27);
  Wire.write(0x00);
  Wire.endTransmission();
}

float readDS()
{
  float tempC;
  
  //Request Temperature 
  sensors.requestTemperatures();
  
  //Get the readings
  tempC = sensors.getTempCByIndex(0);
  
  return tempC;
}
