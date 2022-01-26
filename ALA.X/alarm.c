#include <xc.h>
#include <stdint.h>
#include "alarm.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/pwm6.h"
#include "mcc_generated_files/tmr2.h"

void PWMOutputEnable (){
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    // Set D4 as the output of PWM6
    RA6PPS = 0x0D;

    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}

void PWMOutputDisable (){
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    // Set D5 as GPIO pin
    RA6PPS = 0x00;
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}

void activateAlarm(uint8_t alarm){
    if (alarm == TEMP_ALARM){
        IO_RA5_SetHigh();
    }else{
        IO_RA4_SetHigh();
    }
    TMR2_StartTimer();
    PWMOutputEnable();

    PWM6_LoadDutyValue(LOW_DUTY_VALUE);
}

void deactivateAlarm(){
    IO_RA5_SetLow();
    IO_RA4_SetLow();
    deactivatePWM();
}

void deactivatePWM(){
    TMR2_StopTimer();
    PWMOutputDisable();
}