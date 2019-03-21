#include <LiquidCrystal.h>
#include <ArduinoJson.h>
#include <Servo.h>

//Devices
#define LED1PIN 6
#define LED2PIN 9
#define FANPIN 10
#define BUZZPIN 7
#define DOORPIN 8

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo door;

enum inputMsg
{
  LCD,
  LED1,
  LED2,
  FAN,
  BUZZ,
  DOOR,
};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  printToLcd(0.0, 0);
  Serial.begin(9600);
  door.attach(DOORPIN);
  closeDoor();
  pinMode(LED1PIN, OUTPUT);
  analogWrite(LED1PIN, 0);
  pinMode(LED2PIN, OUTPUT);
  analogWrite(LED2PIN, 0);
  pinMode(FANPIN, OUTPUT);
  analogWrite(FANPIN, 0);
  pinMode(BUZZPIN, OUTPUT);
  digitalWrite(BUZZPIN, LOW);
}

void loop() {
}

void printToLcd(float temp, int hmd) {
  lcd.clear();
  String tempData = "Temp: " + String(temp);
  lcd.print(tempData);
  lcd.setCursor(0, 1);
  String tempHmd = "Humd: " + String(hmd) + "%";
  lcd.print(tempHmd);
}

void openDoor() {
  door.write(110);  
}

void closeDoor() {
  door.write(70);
}

void serialEvent(){
  DynamicJsonDocument doc(40);
  deserializeJson(doc, Serial);
  int act = doc["action"];

  if (act == LCD) {
    float temp = doc["temp"];
    int hmd = doc["hmd"];
    printToLcd(temp, hmd);
  }
  else if (act == LED1) {
    int value = doc["value"];
    analogWrite(LED1PIN, value);
  }  
  else if (act == LED2) {
    int value = doc["value"];
    analogWrite(LED2PIN, value);
  }  
  else if (act == FAN) {
    int value = doc["value"];
    analogWrite(FANPIN, value);
  } 
  else if (act == BUZZ) {
    bool buzzState = doc["state"];
    Serial.println(buzzState);
    if (buzzState) {
      digitalWrite(BUZZPIN, HIGH);
    }
    else {
      digitalWrite(BUZZPIN, LOW);
    }
  }
  else if (act == DOOR) {
    bool doorState = doc["state"];
    if(doorState) {
        openDoor();  
      }
      else {
        closeDoor();
      }
  }
}
