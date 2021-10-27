#ifndef __Disco_Sensorsnew_H__
#define __Disco_Sensorsnew_H__

#include "ESP8266WiFi.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" 

//DS18B20
#define ONE_WIRE_BUS 4



class Disco_Sensorsnew 
{
    public:
		/**
		 *  Constructor
		 */
		//Disco_Sensorsnew(int array_size);
		
		/**
		 *  Initialises the Disco_Sensors Library
		 */
		void begin();
		
		/**
		 *  Read DS18B20
		 */
		float readDS();
		

		float readHIH();

		/**
		 *  Read Temperature and Humidity of HIH
		 */
		//float readHIH();
        
        float readtempHIH();
        float readhumHIH();
        /**
         * Initialise HIH
         */
		void HIH8130_Begin();

		/**
		 *  Read CO2 from CSS Algorithm
		 */
		//uint16_t readSCD30();
		uint16_t readCO2SCD30();
		float readtempSCD30();
		float readhumSCD30();
		

		bool readSCD();
		
		/**
		 *  Read TVOC from CSS Algorithm

		 */
		void getReadings(String *data_array);
		
		/**
		 *  Print value sensors
		 */
		void printValues(String *data_array);
				
	private:
	    float tempC;	
		float tempDS;
		float humiHIH;
		float tempHIH;
        unsigned char buf[4];   
        int16_t _ambientTemp;
        uint16_t _relHumidity; 
		
		
};
#endif  


