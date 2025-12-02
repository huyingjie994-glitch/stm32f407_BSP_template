#ifndef _AT24C02_H_
#define _AT24C02_H_
#include "sys.h"

#define AT24C02		0xA0

uint8_t at24c02_Init(void);
uint8_t at24c02_writebyte(uint8_t addr,uint8_t byte);
uint8_t at24c02_writePagebyte(uint8_t addr,uint8_t *wdata);
uint8_t at24c02_readNbyte(uint8_t addr,uint8_t *rdata,uint16_t datalen);
#endif
