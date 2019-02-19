#include <Automaton.h>

#include <ArduinoJson.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//Devices
#define LEDPIN 5
#define FANPIN 6
#define BUZZPIN 9
#define RESETBTNPIN 4

//Weather Sensors.
#define EARTHPIN 2
#define RAINPIN 8
#define CLOUDPIN A0

//Other Sensors.
#define SMOKEPIN A2
#define FIREPIN A3
#define FLOODPIN A4
#define DHTPIN A1
#define FLAMEPIN A5

#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

Atm_digital earthquake;
Atm_digital fire;
Atm_digital smoke;
Atm_digital flood;
Atm_digital flame;
Atm_button reset_btn;

enum inputMsg {
  EARTHQUAKE,
  SMOKE,
  FIRE,
  FLOOD,
  SENSOR_INFO,
  LED,
  FAN,
  BUZZ,
  FLAME,
  RESET
};

void setup()
{
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  analogWrite(LEDPIN, 0);
  pinMode(FANPIN, OUTPUT);
  analogWrite(FANPIN, 0);
  pinMode(BUZZPIN, OUTPUT);
  analogWrite(BUZZPIN, 0);
  pinMode(RESETBTNPIN, INPUT);

  pinMode(DHTPIN, INPUT);
  pinMode(RAINPIN, INPUT);
  pinMode(CLOUDPIN, INPUT);

  pinMode(EARTHPIN, INPUT);
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

  flame.begin(FLAMEPIN)
  .onChange(flameSens);

  reset_btn.begin(RESETBTNPIN)
  .onPress(reset);
}

void loop() 
{
  automaton.run();
}

//Get all sensor info and output Json to serial.
void sendSensorInfo() 
{
  DynamicJsonDocument doc(67);
  int act = SENSOR_INFO;
  doc["action"] = act;
  doc["temp"] = getTemp();
  doc["hmd"] = getHumidity();
  doc["rain"] = getRain();
  doc["cloud"] = getCloud();
  serializeJson(doc, Serial);
  Serial.println();
}

void sendOtherActions(int act)
{
  DynamicJsonDocument doc(15);
  doc["action"] = act;
  serializeJson(doc, Serial);
  Serial.println();
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

int getHumidity() 
{
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

bool getRain() 
{
  if (digitalRead(RAINPIN) == 1) {
    return false;
  }
  return true;
}

bool getCloud() 
{
  if (analogRead(CLOUDPIN) > 500) {
    return false;
  }
  return true;
}

void flameSens(int idx, int v, int up)
{
  DynamicJsonDocument doc(29);
  int act = FLAME;
  doc["action"] = act;
  if(digitalRead(FLAMEPIN))
  {
    doc["state"] = false;
  }
  else 
  {
      doc["state"] = true;
  }
  serializeJson(doc, Serial);
  Serial.println();
}

void reset(int idx, int v, int up)
{
  sendOtherActions(RESET);
}

void earthQuakeSens(int idx, int v, int up) 
{
  sendOtherActions(EARTHQUAKE);
}

void fireSens(int idx, int v, int up) 
{
  sendOtherActions(FIRE);
}

void smokeSens(int idx, int v, int up) 
{
  sendOtherActions(SMOKE);
}

void floodSens(int idx, int v, int up) 
{
  sendOtherActions(FLOOD);
}

//Gets called when serial input detected.
void serialEvent() 
{
  DynamicJsonDocument doc(29);
  deserializeJson(doc, Serial);
  int act = doc["action"];
  int value = doc["value"];
  if (act == SENSOR_INFO) 
  {
    sendSensorInfo();
  } 
  else if (act == LED) 
  {
    analogWrite(LEDPIN, value);
  }  
  else if (act == FAN) 
  {
    analogWrite(FANPIN, value);
  } 
  else if (act == BUZZ) 
  {
    analogWrite(BUZZPIN, value);
  }
}
