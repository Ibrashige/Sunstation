#include "Wire.h"  // I2C
#include "Time.h"  // Time Manipulation
#include "DS1307RTC.h"  // DS1307 RTC

char timebuf[10];  // Time
char datebuf[10];  // Date
int year2digit;  // 2 digit year
int year4digit;  // 4 digit year

void setup(void) {
  Wire.begin();
  Serial.begin(9600);
}

void loop(void) {
  tmElements_t tm;
    if (RTC.read(tm)) {
        year2digit = tm.Year - 30;  // 2 digit year variable
   //   year4digit = tm.Year + 1970;  // 4 digit year variable
      
      //sprintf(timebuf, "%02d:%02d:%02d",tm.Hour, tm.Minute, tm.Second); // format time
      //sprintf(datebuf, "%02d/%02d/%02d",tm.Day, tm.Month, year2digit);  // format date

      sprintf(timebuf, "%02d:%02d:%02d",tm.Hour, tm.Minute, tm.Second); // format time
      sprintf(datebuf, "%02d/%02d/%02d",tm.Day, tm.Month, year2digit);  // format date
  }
  Serial.println("Time:");
  Serial.println(timebuf);
  Serial.println("Date:");
  Serial.println(datebuf);
  delay(1000);  // Delay of 1sec

}
