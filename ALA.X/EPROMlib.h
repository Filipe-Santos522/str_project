#ifndef __EPROM_H
#define __EPROM_H
#include <xc.h>

#define INITIALIZATION_ADDRESS 0x7099

#define MAGIC_WORD 0xAA 
#define CHECKSUM 0xE3
#define OPER_MIN_TEMP 100
#define OPER_MAX_TEMP 101
#define OPER_MIN_LUM 102
#define OPER_MAX_LUM 103

#define NREG_EPROM 0xA   
#define NR_EPROM 0x0    
#define WI_EPROM 0x0 
#define RI_EPROM 0x0 
#define PMON_EPROM 0x5  
#define TALA_EPROM 0x3
#define ALAT_EPROM 0x19    
#define ALAL_EPROM 0x2    
#define ALAF_EPROM 0x0
#define CLKH_EPROM 0xC    
#define CLKM_EPROM 0x0    

uint16_t operADDR(uint8_t operation);
void initializeEPROM();
void storeEPROMBuild(uint8_t hours,uint8_t minutes,uint8_t seconds,uint8_t temperature,uint8_t luminosity,uint8_t operation);
void parseEPROMReading(uint8_t* hours,uint8_t* minutes,uint8_t* seconds,uint8_t* temperature,uint8_t* luminosity,uint8_t operation);
uint8_t parseEPROMInitialization(uint8_t* magic_word,uint8_t* NREG,uint8_t* NR,uint8_t* WI,uint8_t* RI,uint8_t* PMON,uint8_t* TALA,uint8_t* ALAT,uint8_t* ALAL,uint8_t* ALAF,uint8_t* CLKH,uint8_t* CLKM,uint8_t* checksum);

#endif