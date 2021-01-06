/**
  * Broadcast SunStation data over bluetooth.
  * Data must be in the form of a JSON string.
  * That is: "{"property":value}'
  * 
  * Notes:
  * Send one-property objects only
  * String must be a maximum of 20 characters
  * 
  * Wiring: 
  * (Arduino) -> (HM10)
  * GND -> GND
  * 5V  -> VCC
  * TX  -> RX
  * RX  -> TX
*/ 

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <arduino-timer.h>

SoftwareSerial BTserial(4, 5); // RX, TX

StaticJsonDocument<20> powerData;
StaticJsonDocument<20> batteryData;
StaticJsonDocument<20> currentData;
StaticJsonDocument<20> carbonData;

auto timer = timer_create_default();

int power = 100;
int battery = 0; 
float current = 0.0;
float carbon = 0;

void setup() 
{

  Serial.begin(9600);

  BTserial.begin(9600);
  
  timer.every(60013, send_powerData);
  timer.every(30011, send_batteryData);
  timer.every(1511, send_currentData);
  timer.every(20011, send_carbonData);
}
 
void loop()
{
  timer.tick(); 
}

bool send_powerData(void *) {
  powerData["power"] = power;
  serializeJson(powerData, BTserial);
  serializeJson(powerData, Serial);
  power = (power > 1250) ? 100 : power + 5;
  power = round(power);
  return true; // repeat? true
}


bool send_batteryData(void *) {
  batteryData["battery"] = battery;
  serializeJson(batteryData, BTserial);
  serializeJson(batteryData, Serial);
  battery = (battery > 100) ? 0 : battery + 1;
  battery = round(battery);
  return true; // repeat? true
}

bool send_currentData(void *) {
  currentData["current"] = current;
  serializeJson(currentData, BTserial);
  serializeJson(currentData, Serial);
  current = (current > 3.0) ? 0.0: current + 0.1;
  current = ((int)(current * 10.0 + 0.5) / 10.0);
  return true; // repeat? true
}

bool send_carbonData(void *) {
  carbonData["carbon"] = carbon;
  serializeJson(carbonData, BTserial);
  serializeJson(carbonData, Serial);
  carbon = (carbon > 750) ? 0: carbon + 0.5;
  carbon = ((int)(carbon * 10.0 + 0.5) / 10.0);
  return true; // repeat? true
}
