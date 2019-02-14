#include <Automaton.h>

#include <ArduinoJson.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define EARTHPIN 2
#define RAINPIN 8
#define LEDPIN 5
#define FANPIN 6
#define SMOKEPIN A2
#define FIREPIN A3
#define FLOODPIN 7
#define DHTPIN A1// Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

Atm_digital earthquake;
Atm_digital fire;
Atm_digital smoke;
Atm_digital flood;

enum inputMsg{
  SENSOR_INFO,
  EARTHQUAKE,
  SMOKE,
  FIRE,
  FLOOD,
  LED_ON,
  LED_OFF,
  FAN_ON,
  FAN_OFF
};

void setup()
{
    Serial.begin(9600);
    pinMode(EARTHPIN, INPUT);
    pinMode(RAINPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, LOW);
    pinMode(FANPIN, OUTPUT);
    digitalWrite(FANPIN, LOW);
    pinMode(SMOKEPIN, INPUT);
    pinMode(FIREPIN, INPUT);
    pinMode(FLOODPIN, INPUT);
    dht.begin();
    earthquake.begin(EARTHPIN)
              .onChange(HIGH, earthQuakeSens);
    fire.begin(FIREPIN)
        .onChange(HIGH, fireSens);
    smoke.begin(SMOKEPIN)
        .onChange(HIGH, smokeSens);
    flood.begin(FLOODPIN)
         .onChange(HIGH, floodSens);
} 

void loop() {
  automaton.run();  
}

void sendSensorInfo() {
  DynamicJsonDocument doc(38);
  doc["temp"] = getTemp();
  doc["hmd"] = getHumidity();
  doc["rain"] = getRain();
  serializeJsonPretty(doc, Serial);
}

float getTemp()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    return 0;
  }
  else {
    return event.temperature;
  }
}

int getHumidity() {
  sensors_event_t event;
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    return 0;
  }
  else {
    return event.relative_humidity;
  }
}

bool getRain() {
    if(digitalRead(RAINPIN) == 1) {
      return false;  
    }
    return true;
}

void earthQuakeSens(int idx, int v, int up) {
   Serial.println(EARTHQUAKE);
}

void fireSens(int idx, int v, int up) {
  Serial.println(FIRE);  
}

void smokeSens(int idx, int v, int up) {
  Serial.println(SMOKE);
}

void floodSens(int idx, int v, int up) {
  Serial.println(FLOOD);
}

void serialEvent() {
  int data = Serial.parseInt();
  if(data == SENSOR_INFO){
    sendSensorInfo();  
  } else if (data == LED_ON) {
    digitalWrite(LEDPIN, HIGH); 
  } else if (data == LED_OFF) {
    digitalWrite(LEDPIN, LOW); 
  } else if (data == FAN_ON) {
    digitalWrite(FANPIN, HIGH); 
  } else if (data == FAN_OFF) {
    digitalWrite(FANPIN, LOW); 
  }
}
