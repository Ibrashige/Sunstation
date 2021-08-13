#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <SPI.h>
#include <SD.h>
 
//LED Setup
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        3
#define NUMPIXELS 16 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
 
const int buttonPin = 6;     // the number of the pushbutton pin
int offset = 514;
const float vpp = 0.0048828125;
int points;
int interval;
float voltage;
float amperage;
float amperage_last;
float currentin = 0;
File myFile;
float averageamperage = 0;
// value to determine the size of the readings array.
const int numReadings = 10;
double currentcharge = 10000;
float prevamp = 0;
int buttonState = 0;         // variable for reading the pushbutton status
double readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
double average = 0;                // the average
int inputPin = A0;
int brightness1 = 0;
int brightness2 = 0;
 
void setup() {
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  delay(2000);  //afer reset Arduino, 2s to take out SD
 
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
  }
  else
  {
    Serial.println("initialization done.");
  }
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
//LED Initialize
pixels.begin();
pixels.clear(); // Set all pixel colors to 'off'
 
//Button Setup
pinMode(buttonPin, INPUT);
for (int i = 0; i < 10 ; i++)
{
  average_amperage();
  delay(50);
}
}
 
void loop() {
 
  buttonread();
 
  //Calculate Amperage Average
  average_amperage();
 
  batterycharge();
 
   sunray();
}
 
void sunray(){
     if (averageamperage > 0.8){
    brightness1 = brightness1 + 2;
    brightness2++;
 
      if (brightness1 >= 255){
        brightness1 = 255;
      }
      if (brightness2 >= 155){
        brightness2 = 155;
      }     
  }
  else{
    brightness1 = brightness1 - 2;
    brightness2--;
 
      if (brightness1 <= 0){
        brightness1 = 0;
      }
      if (brightness2 <= 0){
        brightness2 = 0;
      }
  }
 
    pixels.setPixelColor(0, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(1, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(2, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(3, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(4, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(5, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(6, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(7, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(8, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(9, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(10, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(11, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(12, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(13, pixels.Color(brightness1, brightness1, brightness2));
    pixels.setPixelColor(14, pixels.Color(brightness1, brightness1, brightness2));
    pixels.show(); 
}
 
void write_data(int amperage_input)
{
 
  myFile = SD.open("test1.csv", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print("The current is: ");
    myFile.println(amperage_input);
    myFile.close();
    Serial.println("Write file successful!"); //print out COM Port
  } else {
    Serial.println("error opening test.txt");
  }
}
 
void average_amperage()
{
   // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
 
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
 
  // calculate the average:
  average = total / numReadings;
 
 
  //
  points = average - offset;
  voltage = points * vpp;
  averageamperage = (voltage / -0.4)*-2.85;
 
  // send it to the computer as ASCII digits
  Serial.print("Average:");
  Serial.println(averageamperage);
}
 
void batterycharge()
{
  currentcharge = currentcharge + (averageamperage*0.1);
  if (currentcharge < 10 ){
    currentcharge = 10;
  }
 
  if (currentcharge > 11000){
    currentcharge = 11000;
  }
 
// Serial.print("Current Charge:");
// Serial.println(currentcharge);
 
}
 
void batterystatus()
{
// Serial.print ("Current Charge =");
  //Serial.println(currentcharge);
  //LEVEL 1
  if (currentcharge < 1000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
    Serial.println("Level 1");
  }
 
   //LEVEL 2
  else if (1001<currentcharge && currentcharge<2000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
   //LEVEL 3
else if (2001<currentcharge && currentcharge<3000){
 
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
   
    //LEVEL 4
  else if (3001<currentcharge && currentcharge<4000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
    //LEVEL 5
   else if (4001<currentcharge && currentcharge<5000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
     //LEVEL 6
   else if (5001<currentcharge && currentcharge<6000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
    //LEVEL 7
   else if (6001<currentcharge && currentcharge<7000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(6, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
     //LEVEL 8
   else if (7001<currentcharge && currentcharge<8000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(6, pixels.Color(255, 255, 155));
    pixels.setPixelColor(7, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
    //LEVEL 9
   else if (8001<currentcharge && currentcharge<9000){
   
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(6, pixels.Color(255, 255, 155));
    pixels.setPixelColor(7, pixels.Color(255, 255, 155));
    pixels.setPixelColor(8, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
    //LEVEL 10
   else if (9001<currentcharge && currentcharge<10000){
  
pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(6, pixels.Color(255, 255, 155));
    pixels.setPixelColor(7, pixels.Color(255, 255, 155));
    pixels.setPixelColor(8, pixels.Color(255, 255, 155));
    pixels.setPixelColor(9, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
 
    //LEVEL 11
   else if (10001<currentcharge && currentcharge<11000){
   
    pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(255, 255, 155));
    pixels.setPixelColor(1, pixels.Color(255, 255, 155));
    pixels.setPixelColor(2, pixels.Color(255, 255, 155));
    pixels.setPixelColor(3, pixels.Color(255, 255, 155));
    pixels.setPixelColor(4, pixels.Color(255, 255, 155));
    pixels.setPixelColor(5, pixels.Color(255, 255, 155));
    pixels.setPixelColor(6, pixels.Color(255, 255, 155));
    pixels.setPixelColor(7, pixels.Color(255, 255, 155));
    pixels.setPixelColor(8, pixels.Color(255, 255, 155));
    pixels.setPixelColor(9, pixels.Color(255, 255, 155));
    pixels.setPixelColor(10, pixels.Color(255, 255, 155));
    pixels.setPixelColor(11, pixels.Color(255, 255, 155));
    pixels.setPixelColor(12, pixels.Color(255, 255, 155));
    pixels.setPixelColor(13, pixels.Color(255, 255, 155));
    pixels.setPixelColor(14, pixels.Color(255, 255, 155));
    pixels.show();
  }
 
delay(10000);
 
 
pixels.clear(); // Set all pixel colors to 'off'
   
}
 
void buttonread()
{
  for (float w=0;w<10000;w++){
    // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
 
  if (buttonState == HIGH){
    currentcharge = currentcharge + 0;
    batterystatus();
  }
}
delay(1);
}
