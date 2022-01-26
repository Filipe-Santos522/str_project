#include <stdint.h>
#include <xc.h>
#include "EPROMlib.h"
#include "mcc_generated_files/memory.h"
#include "stdio.h"

uint16_t operADDR(uint8_t operation){
    uint16_t dataAddr;
    switch (operation){
        case OPER_MIN_TEMP:     //NOTE Temperature minimum
            dataAddr = 0x7000;
            break;

        case OPER_MAX_TEMP:     //NOTE Temperature maximum
            dataAddr = 0x7005;
            break;

        case OPER_MIN_LUM:     //NOTE Luminosity minimum
            dataAddr = 0x700A;
            break;

        case OPER_MAX_LUM:     //NOTE Luminosity maximum
            dataAddr = 0x700F;
            break;

        default:
            dataAddr = 0xF0FF;
            break;

    }
    return dataAddr;
}

void initializeEPROM(){
    DATAEE_WriteByte(INITIALIZATION_ADDRESS,(uint8_t) MAGIC_WORD);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 1,(uint8_t) NREG_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 2,(uint8_t) NR_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 3,(uint8_t) WI_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 4,(uint8_t) RI_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 5,(uint8_t) PMON_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 6,(uint8_t) TALA_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 7,(uint8_t) ALAT_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 8,(uint8_t) ALAL_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 9,(uint8_t) ALAF_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 10,(uint8_t) CLKH_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 11,(uint8_t) CLKM_EPROM);
    DATAEE_WriteByte(INITIALIZATION_ADDRESS + 12,(uint8_t) CHECKSUM);
}

void storeEPROMBuild(uint8_t hours,uint8_t minutes,uint8_t seconds,uint8_t temperature,uint8_t luminosity,uint8_t operation){
    uint16_t dataAddr;

    dataAddr = operADDR(operation);
    DATAEE_WriteByte(dataAddr, hours);
    DATAEE_WriteByte(dataAddr + 1, minutes);
    DATAEE_WriteByte(dataAddr + 2, seconds);
    DATAEE_WriteByte(dataAddr + 3, temperature);
    DATAEE_WriteByte(dataAddr + 4, luminosity);
}

void parseEPROMReading(uint8_t* hours,uint8_t* minutes,uint8_t* seconds,uint8_t* temperature,uint8_t* luminosity,uint8_t operation){
    uint16_t dataAddr;
    dataAddr = operADDR(operation);
    *hours = DATAEE_ReadByte(dataAddr);
    *minutes = DATAEE_ReadByte(dataAddr + 1);
    *seconds = DATAEE_ReadByte(dataAddr + 2);
    *temperature = DATAEE_ReadByte(dataAddr + 3);
    *luminosity = DATAEE_ReadByte(dataAddr + 4);
}

uint8_t parseEPROMInitialization(uint8_t* magic_word,uint8_t* NREG,uint8_t* NR,uint8_t* WI,uint8_t* RI,uint8_t* PMON,uint8_t* TALA,uint8_t* ALAT,uint8_t* ALAL,uint8_t* ALAF,uint8_t* CLKH,uint8_t* CLKM,uint8_t* checksum){
    *magic_word = DATAEE_ReadByte(INITIALIZATION_ADDRESS);
    *NREG = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 1);
    *NR = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 2);
    *WI = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 3);
    *RI = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 4);
    *PMON = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 5);
    *TALA = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 6);
    *ALAT = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 7);
    *ALAL = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 8);
    *ALAF = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 9);
    *CLKH = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 10);
    *CLKM = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 11);
    *checksum = DATAEE_ReadByte(INITIALIZATION_ADDRESS + 12);
    
    if ((*magic_word + *NREG + *NR + *WI + *RI + *PMON + *TALA + *ALAT + *ALAL + *ALAF + *CLKH + *CLKM) != *checksum){
        return 0x1;
    }
    return 0x0;
}