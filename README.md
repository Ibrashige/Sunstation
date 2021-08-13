# Sunstation

This repository contains all the updated PCB schematics, MCU codes, & research information required to recreate the Sunstation electronics. Written by Loïc Darboux with guidance from Chris Falconi and Ibrahim Qasim.

## Prerequisites

The following libraries must be installed on your computer for the main code to compile:
- <a href="https://github.com/adafruit/Adafruit_NeoPixel"> Adafruit_Neopixel </a>
- <a href="https://github.com/bblanchon/ArduinoJson"> ArduinoJson </a>
- <a href="https://github.com/contrem/arduino-timer"> arduino-timer </a>

*You must add the Arduino Nano board to your IDE. The instructions for doing so on the Arduino IDE can be found [here](https://www.arduino.cc/en/Guide/ArduinoNano).*

## Installation

Simply flash ```SunStationFirmware.ino``` to the Arduino Nano using the Arduino IDE or your editor of preference.

## Usage 

1. Place the Sunstation on a south-facing window with plenty of sunlight.
2. Push the tactile push button on the side to show battery LED display and toggle BLE bluetooth on.
3. Connect iOS or Android device to the Sunstation by downloading the Sunstation app, turning bluetooth on, and clicking the magnifying glass to search for Sunstation and show performance metrics.
4. Connect iOS device to either charging port and place it on detachable platform.


## FlowChart
![Image of Flowchart](https://github.com/Ibrashige/Sunstation/blob/master/Updated%20Flowchart.png)


## Parts & Wiring

The following electronic parts are used: ```WSL-P2006 6.3V 20W Solar Panel```,```Arduino Nano Board, Version 3.0```,```TI BQ24650 Charging/Buck Converter IC```, ```Jauch LP90609JH, 6,000 mAh 3.7 V LiPo Battery```,```TI TPS3700 Comparator```,```Pololu U3V70F5 Boost Converter Module```, ```Worldsemi WS2812B-Mini Neopixel LEDs```, ```Adafruit STEMMA Non-Latching Mini Relay Module```, ```TE Connectivity 1825027-5 Tactile Push-Button```, ```Pololu ACS711EX Current Sensor Module```, ```Sunfounder HM-10 Bluetooth 4.0 Module```, ```Molex 1054500101 USB Type C Female Connector```, ```Jing Extension of the Electronic Co. 902-131A1021D10100 USB Type A Female Connector```, etc..Their wiring is described below.

### WSL-P2006 Solar Panel (To designated PCB terminals)
```
+Ve Junction Box Terminal -> Solar+
-Ve Junction Box Terminal -> Solar-
```
### TI BQ24650 Charging/Buck Converter IC 
```
+Ve Input  -> Solar+
-Ve Input  -> Solar-
+Ve Output -> Bat+
-Ve Output -> Bat-
```
*+Ve Output/Bat+ and -Ve Output/Bat- lines are shared between the battery, comparator, current sensor module, boost converter module, and battery charging module.*

### Jauch LP90609JH LiPo Battery (To designated PCB terminals)
``` 
+Ve Wire (RED) -> Bat+
-Ve Wire (BLACK)-> Bat-
```
### TI TPS3700 Comparator (To boost converter)
```
VDD  -> VBAT
INA+ -> Voltage divider out (R1 = 154Kohm, R2 = 21.3Kohm)
OUTA -> EN
GND  -> PGND
```
*VBAT = +Ve Output of BQ24650.*

### Pololu U3V70F5 Boost Converter Module (To designated Arduino Nano & VBM2610N PMOS)  
```
VOUT -> Vin/PMOS Source 
VIN  -> VBAT 
EN   -> OUTA 
GND  -> PGND
```
### Worldsemi WS2812B-Mini Neopixel LEDs (To designated Arduino Nano & VBM2610N PMOS)
``` 
DOUT -> DIN of next LED 
DIN  -> 330ohm Resistor -> D3 
VDD  -> PMOS Drain
VSS  -> PGND -> 100nF -> PMOS Drain
```
### Adafruit STEMMA Non-Latching Mini Relay Module (To designated Arduino Nano)
``` 
Sig  -> D8 
VIN  -> 5V 
NO   -> USB Types A&C
COM  -> VIN
GND  -> PGND
```
## TE Connectivity Tactile Push-Button (To designated Arduino Nano)
```
Terminal 1  -> D6 & D7 -> 10kOhm Resistor -> PGND 
Terminal 2  -> 5V 
```
## Pololu ACS711EX Current Sensor Module (To designated Arduino Nano)
```
IP+   -> VBat 
IP-   -> Bat+ 
VCC   -> 5V
VIOUT -> A0
GND   -> PGND
```
## Sunfounder HM-10 Bluetooth 4.0 Module (To designated Arduino Nano)
```
5V   -> A2 
TX   -> D4
RX   -> D5
BRK  -> D9
GND  -> GND
```
## USB Type A&C Female Connectors (To designated Arduino Nano)
```
Type C: 
A7/B7         -> D-
A6/B6         -> D+
A1/A12/B1/B12 -> PGND
A4/A9/B4/B9   -> NO
```
```
Type A:
VCC -> NO
D+  -> D+
D-  -> D-
GND -> PGND
```
*D+, NO, and D- lines are shared between each connector (i.e connect Relay's NO pin to VCC pins on both connectors and so on).*

## Known Bugs
1. Mobile charging is exclusively for iOS devices, Android devices are not recommended to be used.  
2. iOS scrolling issue on the Sunstation app. 
