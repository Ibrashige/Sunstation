/**
 * Refactoring of SunStation firmware
 */

// SD Setup

#include <SPI.h>
#include <SD.h>

File myFile;
const byte numChars = 20;             // max 20 char transfer rate for BLE
char receivedChars[numChars] = {'0'}; // an array to store the received data

// NeoPixel Setup

#include <Adafruit_NeoPixel.h>

#define NPXPIN 3
#define PMOS 2
#define NUMPIXELS 15

Adafruit_NeoPixel pixels(NUMPIXELS, NPXPIN, NEO_GRB + NEO_KHZ800);

double currentcharge = 10.0;

// Bluetooth Setup

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>

SoftwareSerial BTserial(4, 5); // RX, TX

StaticJsonDocument<20> powerData;
StaticJsonDocument<20> batteryData;
StaticJsonDocument<20> currentData;
StaticJsonDocument<20> carbonData;

// General Setup

#define BATTERYPIN A0

auto timer = timer_create_default();

int energy = 0;
int battery = 0;
float current = 0.0;
float totalCurrent = 0.0;
float carbon = 0.0;

// Functions

void setup()
{
  Serial.begin(9600);

  // Init NeoPixels
  pixels.begin();
  pinMode(PMOS, OUTPUT);
  digitalWrite(PMOS, LOW);
  timer.every(20000, display_batteryStatus);

  // Init SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(10))
  {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
  read_SD();                      // read content on SD on startup
  energy = atof(receivedChars);   // value read is total energy produced by the station

  // Init BLE module
  BTserial.begin(9600);
  timer.every(10000, send_data);

  // Init Battery Measurements
  timer.every(300, get_batteryData);

  // Init Energy Calculations
  timer.every(3600000, get_energyData);
}

void loop()
{
  timer.tick();
}

// Battery Functions
bool get_batteryData(void *)
{
  current = getCurrent();                // nearest centi amp
  totalCurrent = totalCurrent + current; // TO-DO: Explain this
  battery = getBatteryLevel();           // nearest percentage
  return true;                           // repeat? true
}

bool get_energyData(void *)
{
  // Calculate and save energy produced by the Station
  energy = (totalCurrent / 10800 * 3.7) + energy;
  write_SD();
  totalCurrent = 0.0;

  // Calculate carbon saved
  // Energy to carbon emissions conversion factor from:
  // https://www.epa.gov/energy/greenhouse-gases-equivalencies-calculator-calculations-and-references
  carbon = ((int)(energy * 0.000707 * 1000.0 + 0.5) / 1000.0); // nearest gram
  return true;                                                 // repeat? true
}

float getCurrent()
{
  float average, current;

  // Compute moving average of 10 reads (of what?)
  // in order to smooth current reading
  static float readings[10] = {0.0};
  static int index = 0;
  readings[index++] = analogRead(BATTERYPIN); // moving average of 10 reads
  if (index >= 10)
    index = 0; // in order to smooth results
  for (int i = 0; i < 10; i++)
  {
    average += readings[i];
  }
  average = average / 10.0;

  // Compute current (what are these numbers?)
  // Explain here: ...
  current = ((average / 1023) * 36.7) - 18.75;
  current = ((int)(current * 10.0 + 0.5) / 10.0);

  // return only significant current values
  return abs(current) < 0.15 ? 0.0 : current;
}

int getBatteryLevel()
{
  // The calculations were really
  // funky, so left it alone for now
  // Should return batttery level from 0-100%
  return 0;
}

// Neopixel Functions

bool display_batteryStatus()
{
  // Detetmine battery level
  int batteryLevel = (int)(battery + 0.5) / 10;
  Serial.print("Battery Level: ");
  Serial.println(batteryLevel);

  // Set ring LEDs according to battery level (0-11)
  for (int i = 0; i < batteryLevel; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255, 255, 155));
  }

  // Set central LEDs
  pixels.setPixelColor(11, pixels.Color(255, 255, 155));
  pixels.setPixelColor(12, pixels.Color(255, 255, 155));
  pixels.setPixelColor(13, pixels.Color(255, 255, 155));
  pixels.setPixelColor(14, pixels.Color(255, 255, 155));

  pixels.show();

  timer.in(5000, clearPixels);

  return true; // repeat? true
}

bool clearPixels(void *)
{
  pixels.clear();
  pixels.show();
  return true; // repeat? true
}

// SD Functions

bool write_SD()
{
  // open the file
  myFile = SD.open("test.txt", O_WRITE | O_CREAT | O_TRUNC);
  // if the file opened okay, write to it:
  if (myFile)
  {
    Serial.print("Writing to test.txt...");
    myFile.println(energy);
    // close the file:
    myFile.close();
    Serial.println("done.");
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  return true; // repeat? true
}

void read_SD()
{
  myFile = SD.open("test.txt");
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  if (myFile)
  {
    myFile.seek(0); // Position the read cursor at the start of the last record
    // read from the file until there's nothing else in it:
    while (myFile.available())
    {
      rc = myFile.read();

      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }
      }
      else
      {
        receivedChars[ndx] = '\0'; // terminate the string
        ndx = 0;
      }
    }
    myFile.close();
  }
}

// BLE Functions

bool send_data(void *)
{
  timer.in(0, send_powerData);
  timer.in(1500, send_batteryData);
  timer.in(3000, send_currentData);
  timer.in(4500, send_carbonData);
  return true; // repeat? true
}

bool send_powerData(void *)
{
  powerData["power"] = energy;
  serializeJson(powerData, BTserial);
  serializeJson(powerData, Serial);
  // energy = (energy > 1250) ? 100 : energy + 5;
  // energy = round(energy);
  return true; // repeat? true
}

bool send_batteryData(void *)
{
  batteryData["battery"] = battery;
  serializeJson(batteryData, BTserial);
  serializeJson(batteryData, Serial);
  // battery = (battery > 100) ? 0 : battery + 1;
  // battery = round(battery);
  return true; // repeat? true
}

bool send_currentData(void *)
{
  currentData["current"] = current;
  serializeJson(currentData, BTserial);
  serializeJson(currentData, Serial);
  // current = (current > 3.0) ? 0.0 : current + 0.1;
  // current = ((int)(current * 10.0 + 0.5) / 10.0);
  return true; // repeat? true
}

bool send_carbonData(void *)
{
  carbonData["carbon"] = carbon;
  serializeJson(carbonData, BTserial);
  serializeJson(carbonData, Serial);
  return true; // repeat? true
}
