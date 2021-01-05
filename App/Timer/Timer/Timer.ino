/*
 * Time Test
 *
 */

#include <arduino-timer.h>

int num_readings = 0;

auto timer = timer_create_default(); // create a timer with default settings

bool take_reading(void *) {
  num_readings++;
  Serial.print("Taking a reading: reading#: ");
  Serial.println(num_readings);
  return true; // repeat? true
}

bool do_something(void *) {
  Serial.print("Doing Something : reading#: ");
  Serial.println(millis());
  return true; // repeat? true
}

bool send_data(void *) {
  
  // Place JSON creation code here
  // Include delats here as well
  
  return true; // repeat? true
}

void setup() {
  Serial.begin(9600);

  // call the take_reading function 300 millis
  timer.every(300, take_reading);

  // call the do_something function every 13500 millis (45 readings)
  timer.every(13500, do_something);
  
  // send current data to app every 1500 ms
  timer.every(1500, send_data);
  
}

void loop() {
  timer.tick(); // tick the timer
}
