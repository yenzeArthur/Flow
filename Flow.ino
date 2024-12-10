#include "flow.h"
#include<ESP8266WiFi.h>

typedef enum METER_STATES{
  METER_INNIT,
  METER_RUNNING,
  METER_STOPPED,
  METER_ERROR
} METER_STATES;

METER_STATES meter_state;
const char* ssid = "Ster";
const char* password = "Ster0756";

int sensorPin = D3;
volatile long pulse;
volatile long previous_pulse_value;
bool pulse_interrupt_watcher = false;
DRV_STATUS rtc_driver_status;
DRV_STATUS sdcard_module_status;

void setup() {

  Serial.begin(115200);
  Serial.print("yeah");
 
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), increase, RISING);
  
  wifiSetup();
  delay(200);

  rtc_driver_status == rtc_innit();
  if(rtc_driver_status == DRV_ERROR){
    Serial.println("\nCouldn't find RTC");
    while(1) yield();
  }
  Serial.println("RTC ready!");
  delay(200);

  sdcard_module_status = sd_innit();
  if(sdcard_module_status == DRV_ERROR){
    Serial.println("initialization failed!");
  }
  else{
    Serial.println("initialization done.");
  }

}

void loop() {

  switch(meter_state){
    
    case METER_INNIT:
        if(pulse_interrupt_watcher){
          sdcard_module_status = DRV_NO_STATUS;
          meter_state = METER_RUNNING;
        } 
    break;

    case METER_RUNNING:
        if(previous_pulse_value == water_meter_params.pulse_value){
          pulse_interrupt_watcher = false;
          meter_state = METER_STOPPED;
        }
        else if(previous_pulse_value != water_meter_params.pulse_value){
          water_meter_params.pulse_value += pulse;
          previous_pulse_value = water_meter_params.pulse_value;
        }
        else{
          meter_state = METER_ERROR;
        }
        pulse = 0;
    break;

    case METER_STOPPED:
        water_meter_params.consumption = 2.663 * pulse;
        water_meter_params.volume = 2.663 * water_meter_params.pulse_value / 1000 * 30;

        if(water_meter_params.consumption > 0){
          Serial.println(water_meter_params.consumption);
          Serial.print(" mL");
          if(water_meter_params.volume > 0){
            Serial.println(water_meter_params.volume);
            Serial.print(" mL/m");
            sdcard_module_status = sd_write();
            delay(1000);
            if(sdcard_module_status == DRV_SUCCEEDED){
              Serial.println("writing done!");
              meter_state = METER_INNIT;
            }
            else if(sdcard_module_status == DRV_ERROR){
              Serial.println("Error Opening File...");
              meter_state = METER_ERROR;
            }
          }
        }
        else{
          Serial.println("No value to log");
          meter_state = METER_INNIT;
        }
    break;

    case METER_ERROR:
        if(sdcard_module_status == DRV_ERROR){
          Serial.println("Error opening file, please check the file");
        }
        else{
          Serial.println("code was unable to handle pulses in state METER_RUNNING!");
        }
    break;
 }

}

ICACHE_RAM_ATTR void increase() {
  pulse_interrupt_watcher = true;
  pulse++;
}

void wifiSetup(){
  WiFi.mode(WIFI_STA);               //ESP works in two Wifi modes: as an individual station and as a soft access point. Here its as a station.
  WiFi.begin(ssid, password);        //a function that gets the ssid and password to connect to the router.
  Serial.print("Connecting to WiFi"); 
  
  //a check of internet connection
  while(WiFi.status() != WL_CONNECTED){
    Serial.print('.');
    delay(200);}

    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());    //method to print the IP Address
}
