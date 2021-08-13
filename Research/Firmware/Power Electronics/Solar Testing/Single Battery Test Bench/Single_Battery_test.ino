#include <SPI.h>
#include <SD.h>

int offset_1 = 517;
int offset_2 = 514;
int enable_pin=5;
const float vpp = 0.0048828125;
int points_one;
int points_two;
int points_three;
int interval;
float voltage_one;
float voltage_two;
float voltage_three;
float amperage_battery1;
float amperage_battery2;
float amperage_solar;

File myFile;

void setup() {
  Serial.begin(9600);
  pinMode(enable_pin, LOW); // sets the digital pin 4 as output
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

}

void loop() {
  interval = 1; //interval to write data
  points_one = analogRead(A0)- offset_1;
  voltage_one = points_one * vpp;
  amperage_battery1 = voltage_one / 0.136;
  Serial.println(amperage_battery1);

//  points_two = analogRead(A1)- offset_2;
//  voltage_two = points_two * vpp;
//  amperage_solar = voltage_two / 0.136;
//  Serial.println(amperage_solar);

  write_data(amperage_battery1);  //write data 
  delay(3000);
}

void write_data(float amperage_one)
{
  myFile = SD.open("test.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print("The first battery current is:,");
    myFile.println(amperage_one);
//    myFile.print("The solar current is:, ");
//    myFile.println(amperage_two);
    myFile.println("---------------------------");
    myFile.close();
    //Serial.println("Write file successful!"); //print out COM Port
  } else {
   //Serial.println("error opening test.txt");
  }
}
