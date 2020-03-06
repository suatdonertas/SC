
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "SHTSensor.h"
#include "WifiSensor_SHT85.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



/***** declare global wifi variables to hold the credentials for the network  */

const char* ssid = "TkNet";
const char* password = "2Y%dv6qMl+QPBE";


/***** and then declare the information and credentials of the MQTT server */
#define mqtt_server "192.168.42.1"
#define mqtt_client_id "ESP"


/****** Data wire is plugged into pin 4 on the Arduino **/
#define ONE_WIRE_BUS 4


/****** Software I2C pins used -hence SDA and SCL **/
#define SDA_PIN 13
#define SCL_PIN 14

/****** Topics for MQTT **/

#define temperature_topic_ds "sensor/fridge/temperatureds"
#define humidity_topic "sensor/room/humidity"
#define temperature_topic_sht "sensor/room/temperaturesht"
#define dewpoint_topic "sensor/room/dewpoint"

/****** declare the objects here; **/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature ds18b20(&oneWire);

char temperatureString[7]; //this is to publish them via mqtt as a string

SHTSensor sht85(SHTSensor::SHT3X);
WiFiClient espClient;
PubSubClient client(espClient);



long then = 0;

void setup()
{
  // start serial port
  Serial.begin(115200);

  // start WiFi
  delay(10);
  WiFi.begin(ssid, password);
  espClient.setTimeout(1000);

  // Start up the library
  ds18b20.begin();

  Wire.begin(SDA_PIN, SCL_PIN);
  sht85.init();

  if (! sht85.init()) // check the state of the sensor
  {
    Serial.println("Couldn't find SHT85");

    Wire.begin(SDA_PIN, SCL_PIN);

    //while (1) delay(1);
  }
  else {
    Serial.println("Found SHT85");
  }


  /* specify the address and the port of the MQTT server */
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void connectWifi() {
  delay(10);
  // Connecting to a WiFi network
  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP address ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void connectMQTT() {
  // Wait until we're connected
  while (!client.connected()) {

    Serial.printf("MQTT connecting as client %s...\n", mqtt_client_id);
    // Attempt to connect
    if (client.connect(mqtt_client_id)) {
      Serial.println("MQTT connected");

    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);

    }

  }
}


//void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength) {}



double dewpoint(float t, float h)
{
  float tn = t < 0.0 ? 272.62 : 243.12;
  float m = t < 0.0 ? 22.46 : 17.62;

  float l = logf(h / 100.0);
  float r = m * t / (tn + t);

  return tn * (l + r) / (m - l - r);
}




void loop()
{
  if (!client.connected())
  {
    client.connect(mqtt_client_id);
  }
  client.loop();

  long now = millis();  //insert a sampling frequency of 5 secs
  if (now - then < 5000) return;

  then = now;

  //bool statusds 
  ds18b20.requestTemperatures();
  float temp = ds18b20.getTempCByIndex(0);

  //bool statussht = sht85.readSample();
  float tempsht = sht85.getTemperature();
  float hum = sht85.getHumidity();
  float dp = dewpoint(tempsht, hum);

  if (! isnan(temp))
  {

    Serial.print("TempDS = ");
    Serial.println(temp);
  }
  else
  {
    Serial.println("Failed to read temperature");
  }



  if (! isnan(tempsht))
  {
    //Serial.print("Requesting temperature from SHT85");
    Serial.print("TempSHT = ");
    Serial.println(tempsht);
    // delay(100);
  }
  else
  {
    Serial.println("Failed to read temperature");
  }

  if (! isnan(hum))
  {
    //Serial.print("Requesting humidity from SHT85");
    Serial.print("Hum = ");
    Serial.println(hum);
    //  delay(100);
  }
  else
  {
    Serial.println("Failed to read humidity");
  }

  if (! isnan(dp))
  {
    //Serial.print("Requesting humidity from SHT85");
    Serial.print("Dewpoint = ");
    Serial.println(dp);
    //  delay(100);
  }
  else
  {
    Serial.println("Failed to read dewpoint");

  }

  /******power interlock*****/

  if (temp >= 40) {
    Serial.println(" temp is too high ");
    digitalWrite( 16 , HIGH);
  }
  else if (tempsht >= 50) {
    Serial.println(" temp is too high ");
    digitalWrite( 16 , HIGH);
  }


  /******* Publish them now ********/

  //client.publish(temperature_topic_ds, (uint8_t *)&temp, sizeof(temp));
  client.publish(temperature_topic_ds, String(temp).c_str());
  client.publish(temperature_topic_sht, String(tempsht).c_str());
  client.publish(humidity_topic, String(hum).c_str());
  client.publish(dewpoint_topic, String(dp).c_str());

}
