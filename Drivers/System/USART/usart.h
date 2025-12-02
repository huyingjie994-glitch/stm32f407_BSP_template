#ifndef __USART_H__
#define __USART_H__

#include "sys.h"

#if 1
#define		RCC_USART1_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define		RCC_USART1_GPIO_ENABLE()	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE)
#define		USART1_GPIO					GPIOA
#define		USART1_TX_PIN				GPIO_Pin_9
#define		USART1_TX_Source			GPIO_PinSource9
#define		USART1_RX_PIN				GPIO_Pin_10
#define		USART1_RX_Source			GPIO_PinSource10
#endif
#if 0
#define		RCC_USART1_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define		RCC_USART1_GPIO_ENABLE()	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE)
#define		USART1_GPIO					GPIOB
#define		USART1_TX_PIN				GPIO_Pin_6
#define		USART1_TX_Source			GPIO_PinSource6
#define		USART1_RX_PIN				GPIO_Pin_7
#define		USART1_RX_Source			GPIO_PinSource7
#endif
#if 1
#define		RCC_USART2_CLK_ENABLE()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define		RCC_USART2_GPIO_ENABLE()	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE)
#define		USART2_GPIO					GPIOD
#define		USART2_TX_PIN				GPIO_Pin_5
#define		USART2_TX_Source			GPIO_PinSource5
#define		USART2_RX_PIN				GPIO_Pin_6
#define		USART2_RX_Source			GPIO_PinSource6
#endif

#define U1_RX_SIZE	2048
#define U1_RX_MAX	256
#define U_RXNUM		10
typedef struct
{
	uint8_t *start;
	uint8_t *end;
}UCB_RxBuffPtr;

typedef struct
{
	uint16_t Rxcounter;
	UCB_RxBuffPtr UCB_RxDataPtr[U_RXNUM];
	UCB_RxBuffPtr *URxDataIN;
	UCB_RxBuffPtr *URxDataOUT;
	UCB_RxBuffPtr *URxDataEND;
}UCB_CB;

extern UCB_CB U0_CB;

uint8_t UARTx_Init (USART_TypeDef *USARTx, uint32_t __Baud);
void USART_SendByte(USART_TypeDef *USARTx, uint8_t Byte);
void USART_SendShuzu(USART_TypeDef *USARTx, uint8_t *Shuzu, uint16_t Length);
void USART_SendString(USART_TypeDef *USARTx, char *String);
void U1_RxBuffPtr_Init(void);
#endif
