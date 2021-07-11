#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>

// Pin Defintions
const int SD_PIN = 10;
const int BT_RX = 4;
const int BT_TX = 5;
const int BATTERY_PIN = A0;
const int PMOS_PIN = 5;   // PMOS LOW when neopixels ON, HIGH otherwise
const int NMOS_PIN = 11;  // NMOS LOW before first boot, High afterward
const int NEOPIX_PIN = 3;
const int BUTTON_PIN = 6; 

// Constant values
const int MAX_THROUGHPUT = 20;  // max BLE packet payload
const int NUM_PIXELS = 15;

// Global vars
File logFile;
char logFileChars[MAX_THROUGHPUT] = {'0'};
bool FIRST_BOOT = true;

// Fine-Tune Variables
const float idleDraw = 0.005083;
const int maxBattery = 7330;
const float currentAdjustment = 0.42;    // adds adjustment value to measured current
const float currentCutOff = 0.20;        // discards currents between -cutoff and +cuttoff
const float chargeRate = 0.10;           // rate of battery charge
const float dischargeRate = 0.12;        // rate of battery discharge
const int bleCountdown = 15000;          // time in ms the ble module stays on after a button press

SoftwareSerial BTserial(BT_RX, BT_TX);
StaticJsonDocument<MAX_THROUGHPUT> energyData;
StaticJsonDocument<MAX_THROUGHPUT> batteryData;
StaticJsonDocument<MAX_THROUGHPUT> currentData;
StaticJsonDocument<MAX_THROUGHPUT> carbonData;

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIX_PIN, NEO_GRB + NEO_KHZ800);

int buttonState = 0;
int lastButtonState = 0;

auto timer = timer_create_default();

int energy = 0;           // energy (wh) produced by station in its lifetime
float rawBattery = 0;     // sation's battery level estimate (0-maxBattery). Previously "currentcharge".
int battery = 0;          // stations's battery level in (%)
float current = 0.0;      // current (amps) being drawn by the station
float totalCurrent = 0.0; // amount of current accummulated by the station in an hour
float carbon = 0.0;       // carbon (kg of C02) "saved" by station in its lifetime

void setup() 
{
  Serial.begin(9600);

  // Init Pushbutton
  pinMode(BUTTON_PIN, INPUT);

  // Init MOSFETS
  pinMode(PMOS_PIN, OUTPUT);
  digitalWrite(PMOS_PIN, HIGH);
  pinMode(NMOS_PIN, OUTPUT);
  pinMode(NMOS_PIN, LOW);

  // Init NeoPixels
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Init SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PIN))
  {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
  
  // Recover energy produced by station on startup
  // read_SD();
  // energy = atof(logFileChars);

  // Init BLE module comms
  pinMode(A2, OUTPUT);
  digitalWrite(A2, LOW);
  BTserial.begin(9600);
  timer.every(8000, send_data);

  // Init Battery Measurements
  timer.every(300, measure_batteryStatus);

  // Init Energy Calculations
  timer.every(3600000, compute_energyData);

  // Print Testing Logs
  timer.every(1000, log_data);
}

void loop() 
{
  // Start the timers
  timer.tick();
  // Continuously check for button press
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      display_batteryStatus();
      toggle_BLE();
      if (FIRST_BOOT) {
        digitalWrite(NMOS_PIN, HIGH);
        FIRST_BOOT = false;
      }      
    }
    lastButtonState = buttonState;
  }

   // Serial.print("timers: "); Serial.println(timer.size());
}


/**
 * Logs data to SD 
 */
bool log_data(void *)
{
  write_SD();   
  return true;                       
}

/**
 * Turn on bluetooth
 */
 void toggle_BLE() 
 {
  static Timer<>::Task bleCountdownTask;
  digitalWrite(A2, HIGH);
  timer.cancel(bleCountdownTask);
  bleCountdownTask = timer.in(bleCountdown, kill_BLE);
 }

 bool kill_BLE(void *)
 {
  digitalWrite(A2, LOW);
  return true;
 }

/**
 * Display station's battery status using the 
 * NeoPixels LED ring
 */
void display_batteryStatus()
{
  digitalWrite(PMOS_PIN, LOW);

  // Determine number of ring bars light
  int numBars = map(battery, 0, 100, 0, 11);

  // Set ring LEDs according to battery level (0-11)
  for (int i = 0; i < numBars; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255, 255, 155));
  }

  // Set central LEDs
  pixels.setPixelColor(11, pixels.Color(255, 255, 155));
  pixels.setPixelColor(12, pixels.Color(255, 255, 155));
  pixels.setPixelColor(13, pixels.Color(255, 255, 155));
  pixels.setPixelColor(14, pixels.Color(255, 255, 155));

  pixels.show();

  static Timer<>::Task clearPixelsTask;
  timer.cancel(clearPixelsTask);
  clearPixelsTask = timer.in(5000, clearPixels);
}

/** Turns off NeoPixels */
bool clearPixels(void *)
{
  pixels.clear();
  pixels.show();
  digitalWrite(PMOS_PIN, HIGH);
  return true;
}


/**
 * To-Do: Better Explanation
 * We measure battery status by tracking the 
 * cummulative current being drawn/output by the station
 * Fully charged battery -> 7330 mA
 */
bool measure_batteryStatus(void *)
{
  measureCurrent();
  if (current > 0) totalCurrent += current;      
  computeBatteryData();
  // write_SD();   <--- Removed for testing
  return true;                       
}

/**
 * Measures current (amps) being drawn/output by station
 */
void measureCurrent()
{
  float average, curr;

  // Compute moving average of 10 reads 
  // in order to smooth fluctuations
  static float readings[10] = {0.0};
  static int index = 0;
  readings[index++] = analogRead(BATTERY_PIN);
  if (index >= 10)
    index = 0; // in order to smooth results
  for (int i = 0; i < 10; i++)
  {
    average += readings[i];
  }
  average = average / 10.0;

  // Calibration below from sensor manufacturer (TO-DO: get link)
  curr = ((average / 1023) * 36.7) - 18.75;
  
  // Round to the nearest centi amp
  curr = ((int)(curr * 10.0 + 0.5) / 10.0);

  // Adjustment found after testing
  current = curr + currentAdjustment;
  
  // Retain only significant current values (above +cuttoff or below -cutoff)
  current = abs(current) < currentCutOff ? 0.0 : current;
}

/**
 * Computes station's raw battery level (0 -> ~ 7330)
 * and battery level in %
 * 
 * idle draw -> 61 battery level units per hour 
 *           -> 0.005083 battery level units per hour
 */
void computeBatteryData()
{
  float rate = (current >= 0) ? chargeRate : dischargeRate;
  rawBattery += (current * rate - idleDraw);
  rawBattery = constrain(rawBattery, 0, maxBattery);
  battery = map((long)rawBattery, 0, maxBattery, 0, 100);
}

/**
 * 1. Compute energy (wH) produced by the station to date 
 * and save that value on the SD card
 * 
 * 2. Compute equivalent of CO2 (kg) saved by the station
 * Energy to carbon emissions conversion factor from:
 * https://www.epa.gov/energy/greenhouse-gases-equivalencies-calculator-calculations-and-references
 */
bool compute_energyData(void *)
{
  // Calculate and save energy produced by the Station
  energy = (totalCurrent / 10800 * 3.7) + energy;
  // write_SD();  <-- Removed for testing
  totalCurrent = 0.0;

  // Calculate carbon saved
  carbon = ((int)(energy * 0.000707 * 1000.0 + 0.5) / 1000.0); // round to nearest gram
  return true;
}
/**
 * Saves energy produced by station in its lifetime on SD card
 */
//bool write_SD()
//{
//  // open log file in write mode, create it if necessary, clear its prev contents
//  logFile = SD.open("logs.txt", O_WRITE | O_CREAT | O_TRUNC);
//  // if the file opened okay, write to it:
//  if (logFile)
//  {
//    Serial.print("Writing to logs.txt...");
//    logFile.println(energy);
//    // close the file:
//    logFile.close();
//    Serial.println("done.");
//  }
//  else
//  {
//    // if the file didn't open, print an error:
//    Serial.println("error opening logs.txt");
//  }
//  return true;
//}
bool write_SD() {
  logFile = SD.open("LOGS.TXT", FILE_WRITE);
  // if the file opened okay, write to it:
  if (logFile) {
    // log to SD
    Serial.println("Writing to LOGS.TXT");
    logFile.print(millis()); logFile.print(",");
    logFile.print(rawBattery); logFile.print(",");
    logFile.print(battery); logFile.println();
    // log to serial monitor
    Serial.print("rawBattery: "); Serial.println(rawBattery); 
    Serial.print("battery: "); Serial.println(battery);
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening logs.txt");
  }
  logFile.close();
  return true;
}


/**
 * Reads the contents of the SD card 
 * into the the logFileChars buffer
 */
void read_SD()
{
  logFile = SD.open("LOGS.TXT");
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  if (logFile) {
    // Place cursor at the start of file
    logFile.seek(0); 
    // read from the file until there's nothing else in it:
    while (logFile.available()) {
      rc = logFile.read();
      if (rc != endMarker) {
        logFileChars[ndx] = rc;
        ndx++;
        if (ndx >= MAX_THROUGHPUT) {
          ndx = MAX_THROUGHPUT - 1;
        }
      }
      else {
        logFileChars[ndx] = '\0'; // terminate the string
        ndx = 0;
      }
    }
    logFile.close();
  }
}

/** Staggers transmission of station data over BLE  */
bool send_data(void *) 
{
  timer.in(0, send_energyData);
  timer.in(1500, send_batteryData);
  timer.in(3000, send_currentData);
  timer.in(4500, send_carbonData);
  return true;
}

/**
 * To-Do: Figure out how to replace functions below
 * with a single customizable one
 */ 
bool send_energyData(void *)
{
  // To-Do: change "power" to "energy"
  energyData["power"] = energy;
  serializeJson(energyData, BTserial);
  serializeJson(energyData, Serial);
  return false;
}

bool send_batteryData(void *)
{
  batteryData["battery"] = battery;
  serializeJson(batteryData, BTserial);
  serializeJson(batteryData, Serial);
  return false;
}

bool send_currentData(void *)
{
  currentData["current"] = current;
  serializeJson(currentData, BTserial);
  serializeJson(currentData, Serial);
  return false;
}

bool send_carbonData(void *)
{
  carbonData["carbon"] = carbon;
  serializeJson(carbonData, BTserial);
  serializeJson(carbonData, Serial);
  return false;
}
