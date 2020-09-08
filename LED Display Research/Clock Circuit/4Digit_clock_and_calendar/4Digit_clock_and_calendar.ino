#include <LedControl.h>
#include <RTClib.h>
//=======================================================================================
// data pin, clock, latch

double hold;

LedControl lc = LedControl(4, 15, 2, 1); // pro mini LedControl(9, 5, 6, 1); // Original LedControl(4, 2, 3, 1); 
RTC_DS1307 rtc;
//========================================================================================

void setup() {
rtc.begin();
lc.shutdown(0, false);
lc.setIntensity(0,15); // display brightness
lc.clearDisplay(0); // erase display
rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //set clock as computer clock
//rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

}
//========================================================================================
void loop() 
{   
  hold = millis();
  while ((hold + 20000) > millis() ) 
  {
    DateTime now = rtc.now();
    setCol1(now.hour());
    setCol2(now.minute());
    setCol3(now.second());
    //lc.setLed (0, 3, '.',false);
    //lc.setChar (0, 2, '-', false);````````````````````````````````````````````````````````````````````````
    //lc.setChar (0, 5, '-', false);
  }
  //hold = millis();
  //while ((hold + 5000) > millis() ) {
  //DateTime now = rtc.now();
  //setCol1(now.day());
  //setCol2(now.month());
  //setCol3(now.year() - 2000);
  //lc.setChar (0, 0, '.', false);
  //lc.setChar (0, 2, '.', false);
  //lc.setChar (0, 2, '-', false);
  //lc.setChar (0, 5, '-', false);
}
 
//}

//========================================================================================
void setCol1 (byte jam) {
String strBuffer = Konversi(jam);
byte pjg = strBuffer.length() + 1;
char val[pjg];
strBuffer.toCharArray(val, pjg);
lc.setChar (0, 3, val[0], false);
lc.setChar (0, 2, val[1], false);
lc.setLed(0, 2, 0, true);
lc.setDigit(0, 0, val[1], true);  
}

//========================================================================================
void setCol2 (byte mnt) {
String strBuffer = Konversi(mnt);
byte pjg = strBuffer.length() + 1;
char val[pjg];
strBuffer.toCharArray(val, pjg);
lc.setChar (0, 1, val[0], false);
lc.setChar (0, 0, val[1], false);
}

void setCol3 (byte dtk) {
String strBuffer = Konversi(dtk);
byte pjg = strBuffer.length() + 1;
char val[pjg];
strBuffer.toCharArray(val, pjg);
lc.setChar (0, 6, val[0], false);
lc.setChar (0, 7, val[1], false);
}

//void countDown() {                                                                               //Shut Down mode
//    int i=9; 
//    lc.setDigit(0,(byte)i,false); 
//    //The digit '9' appears on the display 
//    delay(1000); 
//    //Go into shutdown mode 
//    lc.shutdown(0,true); 
//    //and count down silently 
//    while(i>1) { 
//        //data is updated, but not shown
//        lc.setDigit(0,(byte)i,false); 
//        i--; 
//        delay(1000); 
//    } 
//    //Coming out of shutdown mode we have already reached '1' 
//    lc.shutdown(0,false);
//    lc.setDigit(0,(byte)i,false); 
//} 

String Konversi(int nilai) {
if (nilai < 10) {
return "0" + String(nilai);
}
else {
return String(nilai);
}
}
