/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

File myFile;
long lastRecordStart;
long fileLength;
long recordCount;
double lastsavedvalue = 0;
//char y;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  
}

void loop() {
  // open the file for reading:
  myFile = SD.open("pow.CSV");
  if (myFile) {
      fileLength = myFile.size();
      recordCount = fileLength / 7;
      lastRecordStart = (recordCount - 1) * 7;
      myFile.seek(lastRecordStart); // Position the read cursor at the start of the last record
// Padding//
  while(int i = 0; i < 12; i++)
  

      String powerstring = "";
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    char power = myFile.read();//Serial.write(myFile.read());
    powerstring = powerstring + power;
    }
   lastsavedvalue = powerstring.toFloat();
     //close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  Serial.println(lastsavedvalue);
}
