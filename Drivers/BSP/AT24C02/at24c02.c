#include "AT24C02/at24c02.h"
#include "AT24C02/iic.h"
#include <DELAY/delay.h>
uint8_t at24c02_Init(void)
{
	uint8_t ack;
	iic_init();
	iic_start();    /* 发送起始信号 */
	iic_send_byte(AT24C02);
	ack = iic_wait_ack();
	iic_stop();
	return ack;
}

uint8_t at24c02_writebyte(uint8_t addr,uint8_t byte)
{
	iic_start();    /* 发送起始信号 */
	iic_send_byte(AT24C02);
	if(iic_wait_ack())	return 1;
	iic_send_byte(addr);
	if(iic_wait_ack())	return 2;
	iic_send_byte(byte);
	if(iic_wait_ack())	return 3;
	iic_stop();
	delay_ms(5);
	return 0;
}

uint8_t at24c02_writePagebyte(uint8_t addr,uint8_t *wdata)
{
	uint8_t n;
	if(addr%8)	return 0xff;
	iic_start();    /* 发送起始信号 */
	iic_send_byte(AT24C02);
	if(iic_wait_ack())	return 1;
	iic_send_byte(addr);
	if(iic_wait_ack())	return 2;
	for(n=0; n < 8;n++)
	{
		iic_send_byte(wdata[n]);
		if(iic_wait_ack())	return 3+n;
	}
	iic_stop();
	delay_ms(10);
	return 0;
}

uint8_t at24c02_readNbyte(uint8_t addr,uint8_t *rdata,uint16_t datalen)
{
	uint8_t n;
	iic_start();    /* 发送起始信号 */
	iic_send_byte(AT24C02);
	if(iic_wait_ack())	return 1;
	iic_send_byte(addr);
	if(iic_wait_ack())	return 2;
	iic_start();    /* 发送起始信号 */
	iic_send_byte(AT24C02|0x01);
	if(iic_wait_ack())	return 3;
	for(n=0;n<datalen-1;n++)
	{
		rdata[n] = iic_read_byte(1);
	}
	rdata[datalen-1] = iic_read_byte(0);
	iic_stop();
	return 0;
}
