#include "flow.h"
#include <SPI.h>
#include <SD.h>

typedef enum SD_STATES{
  OPEN_FILE,
  WRITE_TO_FILE,
  WRITING_COMPLETE,
  ERROR
} 
SD_STATES;

SD_STATES sd_current_state = OPEN_FILE;
File myFile;

DRV_STATUS sd_innit(void){
  if (!SD.begin(D0)) {
    return DRV_ERROR;
  }
  return DRV_WORKING;
}

DRV_STATUS sd_write(void){
  DateTime now = rtc.now();

  switch(sd_current_state){

    case OPEN_FILE:
        myFile = SD.open("log.csv", FILE_WRITE);
        if (myFile){
          Serial.println("Writing to file...");
          sd_current_state = WRITE_TO_FILE;
        }
        else{
          sd_current_state = ERROR;
        }
    break;

    case WRITE_TO_FILE:
        print2Digits(now.year());
        myFile.print('-');
        print2Digits(now.month());
        myFile.print('-');
        print2Digits(now.day());
        myFile.print("T");
        print2Digits(now.hour());
        myFile.print(':');
        print2Digits(now.minute());
        myFile.print(':');
        print2Digits(now.second());
        myFile.print("Z");
        myFile.print(',');
        myFile.println(water_meter_params.consumption);
        myFile.println("mL");
        myFile.println(water_meter_params.volume);
        myFile.print(',');
        myFile.println("mL/s");
        myFile.close();
        sd_current_state = OPEN_FILE;
        return DRV_SUCCEEDED;
    break;

    case ERROR:
        return DRV_ERROR;
    break;
  }
  return DRV_NO_STATUS;
}

void print2Digits(int num)
{
  if (num<10){
    myFile.print('0');
  }
  myFile.print(num);
}