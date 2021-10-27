#include <Disco_Sensorsnew.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//SCD30
SCD30 airSensor;

/*Disco_Sensorsnew::Disco_Sensors(int array_size)
{
   tempC = 0.00;
   tempBME = 0.00;
   humiBME = 0.00;
   bmeDisconnected = false;
   readAlgorithm = false;
   my_array_size = array_size;
   
   //Default alarms setup
   alarmTemp = false;
   alarmHumi = false;
   alarmCO2 = false;
   alarmTVOC = false;
   
   //Declare Array of Strings to store data
   //String* data_array [array_size];
}
*/
void Disco_Sensorsnew::begin()
{
  //Start IC Communication 
  Wire.begin(2,14);
 
	//Start DS18B20 Sensors
	sensors.begin();
	
  //Check if CO² Sensor is there
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
      
  }
	
  //Start HI8130
  HIH8130_Begin();
}

void Disco_Sensorsnew::HIH8130_Begin()
{
  Wire.beginTransmission(0x27);
  Wire.write(0x00);
  Wire.endTransmission();
}

float Disco_Sensorsnew::readDS()
{
	//Request Temperature 
	sensors.requestTemperatures();
	
	//Get the readings
	tempC = sensors.getTempCByIndex(0);
	
	return tempC;

  /*Serial.print("DS Temperature");
  Serial.print(tempC);*/
}


float Disco_Sensorsnew::readHIH()
{
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

  return _relHumidity;
  return _ambientTemp;
}

float Disco_Sensorsnew::readtempHIH()
{
  float tempHIH = _ambientTemp/100.0;
  
  return tempHIH;
}

float Disco_Sensorsnew::readhumHIH()
{
  float humHIH = _relHumidity/100.0;

  return humHIH; 
}


uint16_t Disco_Sensorsnew::readCO2SCD30()
{
    
    int16_t CO2 = airSensor.getCO2();
    return CO2;

}

float Disco_Sensorsnew::readtempSCD30()
{
 
    float tempSCD = airSensor.getTemperature();
    return tempSCD;

}

float Disco_Sensorsnew::readhumSCD30()
{
  
    float humiSCD = airSensor.getHumidity();
    return humiSCD;

}

bool Disco_Sensorsnew::readSCD()
{  
  if (airSensor.dataAvailable())
  {
    return true;
  }
  else 
  {
    return false;
  }
}

void Disco_Sensorsnew::getReadings(String *data_array)
{
  //Read DS18B20
  float ds_temp_compensate = readDS();
  data_array[0] = String(ds_temp_compensate);
  
  //Read BME - Temperature
  data_array[1] = String(readtempHIH());
  
  //Read BME - Humidity
  //float bme_humi_compensate = readhumHIH();
  data_array[2] = String(readhumHIH());
  
  //Read CCS - CO2
  data_array[3] = String(readCO2SCD30());
  
  //Read CCS - TVOC
  data_array[4] = String(readtempSCD30());

  //Read CCS - TVOC
  data_array[5] = String(readhumSCD30());

  
  //Set Environmental Data for compensation for next readings
  //my_ccs.setEnvironmentalData(ds_temp_compensate,bme_humi_compensate);
  
}

void Disco_Sensorsnew::printValues(String *data_array)
{
  for (int i=0;i<data_array->length();i++)
  {
    Serial.println(data_array[i]);
  }
}