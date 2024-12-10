#include "flow.h"
#include <RTClib.h>
#include <SPI.h>

RTC_DS1307 rtc;
char Week_days[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DRV_STATUS rtc_innit(void){

  if (!rtc.begin()) {
    while (!Serial); // wait for serial port to connect. Needed for native USB
    return DRV_ERROR;
  }
  else{
    Serial.println("\n RTC found\n");
  }
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   return DRV_WORKING;
}