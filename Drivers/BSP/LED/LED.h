#ifndef __LED_H__
#define __LED_H__
#include "sys.h"
#define RCC_LED_GPIO   RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOF
#define LED0_GPIO    	GPIOF
#define LED0_Pin   		GPIO_Pin_9

#define LED1_GPIO    	GPIOF
#define LED1_Pin   		GPIO_Pin_10


/* LED¶Ë¿Ú¶¨Òå */
#define LED0(x)   do{ x ? \
                      GPIO_ResetBits(		LED0_GPIO,	LED0_Pin): \
                      GPIO_SetBits(	LED0_GPIO,	LED0_Pin); \
                  }while(0)       /* RGB_R = RED */
#define LED1(x)   do{ x ? \
                      GPIO_ResetBits(		LED1_GPIO,	LED1_Pin): \
                      GPIO_SetBits(	LED1_GPIO,	LED1_Pin); \
                  }while(0)       /* RGB_R = RED */

void LED_Init(void);
void LED0_TOGGLE(void);
void LED1_TOGGLE(void);
#endif
