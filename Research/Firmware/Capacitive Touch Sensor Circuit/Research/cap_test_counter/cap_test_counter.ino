#define touchSw 33
#include <LedControl.h>
#include <RTClib.h>

int val = 0; // variable for reading the pin status
int counter = 0;
int touchDelay = 500;
double hold;

LedControl lc = LedControl(4, 15, 2, 1);
RTC_DS1307 rtc;

void setup() {
  rtc.begin();
  lc.shutdown(0, false);
  lc.setIntensity(0, 1); // display brightness
  lc.clearDisplay(0); // erase display
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //set clock as computer clock
  
  pinMode(touchSw,INPUT);
  Serial.begin(115200);
}

void loop() {
  val = digitalRead(touchSw);
  
  if(val == HIGH)
  {
      counter++;// increment the count   
      
   
      Serial.print("Touched ");//print the information
      Serial.print(counter);//print count
      Serial.println(" times.");        
      delay(touchDelay);// touch delay time
      
      hold = millis();
  while ((hold + 5000) > millis() ) 
    {
      setCounter(counter);
    }
  }
  else
  {
    hold = millis();
  while ((hold + 250) > millis() ) 
   {
      DateTime now = rtc.now();
      setCol1(now.hour());
      setCol2(now.minute());
      setCol3(now.second());
    }
  }
}

//========================================================================================
void setCol1 (byte jam) 
{
  String strBuffer = configure(jam);
  byte pjg = strBuffer.length() + 1;
  char val[pjg];
  strBuffer.toCharArray(val, pjg);
  lc.setChar (0, 3, val[0], false);
  lc.setChar (0, 2, val[1], false);
  lc.setLed(0, 2, 0, true); 
}

//========================================================================================
void setCol2 (byte mnt) 
{
  String strBuffer = configure(mnt);
  byte pjg = strBuffer.length() + 1;
  char val[pjg];
  strBuffer.toCharArray(val, pjg);
  lc.setChar (0, 1, val[0], false);
  lc.setChar (0, 0, val[1], false);
}

void setCol3 (byte dtk) 
{
  String strBuffer = configure(dtk);
  byte pjg = strBuffer.length() + 1;
  char val[pjg];
  strBuffer.toCharArray(val, pjg);
  lc.setChar (0, 6, val[0], false);
  lc.setChar (0, 7, val[1], false);
}

void setCounter (byte kim)
{
  String strBuffer = configure(kim);
  byte pjg = strBuffer.length() + 1;
  char val[pjg];
  strBuffer.toCharArray(val, pjg);
  lc.setChar (0, 1, val[1], false);
  lc.setChar (0, 0, val[2], false);
  lc.setChar (0, 2, val[0], false);
  lc.setChar (0, 3, val[3], false);
}

String configure(int num) 
{
  if (num < 10) 
  {
     return "0" + String(num);
  }
  else 
  {
    return String(num);
  }
}
