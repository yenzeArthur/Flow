
#ifndef FLOW_H
    #define FLOW_H

    typedef enum DRV_STATUS{
      DRV_WORKING,
      DRV_SUCCEEDED,
      DRV_ERROR,
      DRV_NO_STATUS
    } 
    DRV_STATUS;

    typedef struct PARAMS{
      volatile long pulse_value;
      float volume;
      float consumption;
    };
    
    PARAMS water_meter_params;

    DRV_STATUS rtc_innit(void);
    DRV_STATUS sd_innit(void);
    DRV_STATUS sd_write(void);
    void print2Digits(int num);
    
#endif 