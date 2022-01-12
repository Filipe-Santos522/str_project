/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18875
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "LCD/lcd.h"
#include "stdio.h"
#include "EPROMlib.h"

/*
                         Main application
*/

#define LUM_LVL_01  0xFF
#define LUM_LVL_12  0x1FE
#define LUM_LVL_23  0x2FD
#define START_RING_BUFFER_ADDR 0x7000

#define HIGH_TEMP_REG 0x7053
#define LOW_TEMP_REG 0x7054
#define HIGH_LUX_REG 0x7055
#define LOW_LUX_REG 0x7056

#define MIN_TEMP 0
#define MAX_TEMP 255
#define MIN_LUM 0
#define MAX_LUM 255

uint8_t hours, minutes, seconds, max_luminosity, min_luminosity, last_luminosity, counter;
unsigned char last_temperature, max_temperature, min_temperature;
uint16_t data_address;

void S1(void){
    if(IO_RB4_GetValue()==LOW && IO_RA6_GetValue()==HIGH){
        IO_RA6_SetLow();
    }
}

void timerInterrupt(){
    
   adc_result_t value;
   
    if(counter == 5){
    
        if(last_temperature > max_temperature){
            
            DATAEE_WriteByte(HIGH_TEMP_REG, last_temperature);
            //alarme
            
        }else if(last_temperature < min_temperature){
        
            DATAEE_WriteByte(LOW_TEMP_REG, last_temperature); 
            //alarme
        }
        if(last_luminosity > max_luminosity){
            
            DATAEE_WriteByte(HIGH_LUX_REG, last_luminosity);
            //alarme
            
        }else if(last_luminosity < min_luminosity){
        
            DATAEE_WriteByte(LOW_LUX_REG, last_luminosity);
            //alarme
        } 
        value = ADCC_GetSingleConversion(channel_ANA0);
        unsigned char temperature = readTC74();
        if(temperature!= last_temperature || value!= last_luminosity){
            writeRingBuffer(temperature,  value);
        }
    }
 
    if(counter == 5){
    
        counter == 1;
    }else{      
        counter ++;
    }

    if(IO_RA7_GetValue()==LOW){
        IO_RA7_SetHigh();
    }else{
        IO_RA7_SetLow();
    }
    
    if(value > LUM_LVL_23 || value < LUM_LVL_01){
        IO_RA4_SetHigh();
    }else{
        IO_RA4_SetLow();
    }
}

unsigned char readTC74 (void)
{
	unsigned char value;
do{
	IdleI2C();
	StartI2C(); IdleI2C();
    
	WriteI2C(0x9a | 0x00); IdleI2C();
	WriteI2C(0x01); IdleI2C();
	RestartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x01); IdleI2C();
	value = ReadI2C(); IdleI2C();
	NotAckI2C(); IdleI2C();
	StopI2C();
} while (!(value & 0x40));

	IdleI2C();
	StartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x00); IdleI2C();
	WriteI2C(0x00); IdleI2C();
	RestartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x01); IdleI2C();
	value = ReadI2C(); IdleI2C();
	NotAckI2C(); IdleI2C();
	StopI2C();

	return value;
}

//iniciar os 4 registos com máximos e minimos
//not sure com que valores inicializar os registos
void initializeREG(){

    DATAEE_WriteByte(HIGH_TEMP_REG,(uint8_t) MAX_TEMP);
    DATAEE_WriteByte(LOW_TEMP_REG,(uint8_t) MIN_TEMP);
    DATAEE_WriteByte(HIGH_LUX_REG + 2,(uint8_t) MAX_LUM);
    DATAEE_WriteByte(LOW_LUX_REG + 3,(uint8_t) MIN_LUM);

}

//retorna o writeRingBufferAddr atualizado para a proxima escrita
//ordem de escrita é: horas -> minutos -> segundos -> temperatura -> luminosidade -> horas -> (...)
//writeRingBufferAddr = writeRingBuffer(...)
void writeRingBuffer(unsigned char temperature, uint8_t luminosity){

    DATAEE_WriteByte(data_address, hours);
    DATAEE_WriteByte(data_address + 1, minutes);
    DATAEE_WriteByte(data_address + 2, seconds);
    DATAEE_WriteByte(data_address + 3, temperature);
    DATAEE_WriteByte(data_address + 4, luminosity);   

    last_temperature = temperature;
    last_luminosity = luminosity;

    
    if(data_address == (START_RING_BUFFER_ADDR + 20)){
        
        data_address = START_RING_BUFFER_ADDR;
        
    }else{
        
        data_address =  data_address + 5;
    } 
}

void main(void)
{
    unsigned char c;
    char buf[17];
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    hours=0;
    minutes=0;
    seconds=0;
    max_luminosity=MAX_LUM;
    min_luminosity=MIN_LUM;
    last_luminosity=255;
    counter=1;
    
    last_temperature=MAX_TEMP;
    max_temperature=MAX_TEMP;
    min_temperature=MIN_TEMP;
    data_address=START_RING_BUFFER_ADDR;
    
    IO_RA6_SetHigh();
     
    OpenI2C();
    //I2C_SCL = 1;
    //I2C_SDA = 1;
    //WPUC3 = 1;
    //WPUC4 = 1;
    LCDinit();
    


    initializeEPROM();
    initializeREG();

    storeEPROMBuild(0x55,0x50,0x45,0x40,0x35,OPER_MAX_TEMP);
    
    uint8_t magic_word,NREG,NR,WI,RI,PMON,TALA,ALAT,ALAL,ALAF,CLKH,CLKM,checksum;
    
    LCDcmd(0x80);
    LCDstr("insert hours");
    __delay_ms(1000);
    while(IO_RC5_GetValue()==HIGH){
        if(IO_RB4_GetValue()==LOW){
            hours++;
            __delay_ms(250);
        }
        while (LCDbusy());
        LCDcmd(0x80);
        sprintf(buf, "%02d:%02d:%02d     ", hours, minutes, seconds);
        LCDstr(buf);
    }
    LCDcmd(0x80);
    LCDstr("insert minutes");
    __delay_ms(1000);
    
    while(IO_RC5_GetValue()==HIGH){
        if(IO_RB4_GetValue()==LOW){
            minutes++;
        }
        while (LCDbusy());
        LCDcmd(0x80);
        sprintf(buf, "%02d:%02d:%02d      ", hours, minutes, seconds);
        LCDstr(buf);
        __delay_ms(250);
    }
    LCDcmd(0x80);
    LCDstr("insert seconds");
    __delay_ms(1000);
    
    while(IO_RC5_GetValue()==HIGH){
        if(IO_RB4_GetValue()==LOW){
            seconds++;
        }
        while (LCDbusy());
        LCDcmd(0x80);
        sprintf(buf, "%02d:%02d:%02d      ", hours, minutes, seconds);
        LCDstr(buf);
        __delay_ms(250);
    }
    
    
    TMR1_SetInterruptHandler(timerInterrupt);
    while (1)
    {   
        S1();

        parseEPROMInitialization(&magic_word,&NREG,&NR,&WI,&RI,&PMON,&TALA,&ALAT,&ALAL,&ALAF,&CLKH,&CLKM,&checksum);

        c = readTC74();
        LCDcmd(0x80);       //first line, first column
        while (LCDbusy());
        sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
        LCDstr(buf);
        LCDpos(0,11);
        while (LCDbusy());
        LCDstr("STR");
        
        LCDcmd(0xc0);       // second line, first column
        sprintf(buf, "%02d C", c);
        while (LCDbusy());
        LCDstr(buf);
        __delay_ms(2000);
    }
}
/**
 End of File
*/