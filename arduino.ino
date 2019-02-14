#include <ArduinoJson.h>

#include <Eventually.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define EARTHPIN 2
#define DHTPIN A1// Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);
EvtManager mgr;

enum inputMsg{
  SENSOR_INFO,
  EARTHQUAKE 
};

void setup()
{
    Serial.begin(9600);
    pinMode(EARTHPIN, INPUT);
    dht.begin();
    mgr.addListener(new EvtPinListener(EARTHPIN, (EvtAction)earthQuakeSens));
}

bool getTemp()
{
  DynamicJsonDocument doc(25);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    doc["temp"] = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    doc["hmd"] = event.relative_humidity;
  }
  serializeJsonPretty(doc, Serial);
  return false;
}

bool earthQuakeSens() {
    Serial.println(EARTHQUAKE);
    return true;
}

void serialEvent() {
  int data = Serial.parseInt();
  if(data == SENSOR_INFO){
    getTemp();  
  }  
}

USE_EVENTUALLY_LOOP(mgr) // Use this instead of your loop() function.
