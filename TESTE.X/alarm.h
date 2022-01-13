#ifndef __ALARM_H
#define __ALARM_H
#include <xc.h>

#define HIGH_DUTY_VALUE 0xA
#define MEDIUM_DUTY_VALUE 0x7
#define LOW_DUTY_VALUE 0x3

#define TEMP_ALARM 46
#define LUM_ALARM 64

void PWMOutputEnable();
void PWMOutputDisable();
void activateAlarm(uint8_t alarm);
void deactivateAlarm();
void deactivatePWM();

#endif