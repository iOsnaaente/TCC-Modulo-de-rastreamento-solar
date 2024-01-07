#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "sdmmc_cmd.h"

void init_sd_card( void );

void write_sd_byte ( uint16_t addr, uint8_t value );
void write_sd_int  ( uint16_t addr, uint16_t value );
void write_sd_int32( uint16_t addr, uint32_t value );
void write_sd_float( uint16_t addr, float value );

uint8_t read_sd_byte  ( uint16_t addr );
uint16_t read_sd_int  ( uint16_t addr );
uint32_t read_sd_int32( uint16_t addr );
float read_sd_float( uint16_t addr );

#endif 