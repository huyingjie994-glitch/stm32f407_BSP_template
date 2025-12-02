#include "stm32f4xx.h"                  // Device header
#include "LED/LED.h"

void LED_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_LED_GPIO,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = LED0_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	// 普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	// 推挽输出
	// GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	// 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED0_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_Pin;
	GPIO_Init(LED1_GPIO,&GPIO_InitStructure);
	
	LED0(0);
	LED1(0);
}

void LED0_TOGGLE(void)
{
	static uint8_t sta=0;
	if(sta)
	GPIO_ResetBits(LED0_GPIO,LED0_Pin);
	else
	GPIO_SetBits(LED0_GPIO,	LED0_Pin);
	sta = ~sta;
}

void LED1_TOGGLE(void)
{
	static uint8_t sta=0;
	if(sta)
	GPIO_ResetBits(LED1_GPIO,LED1_Pin);
	else
	GPIO_SetBits(LED1_GPIO,	LED1_Pin);
	sta = ~sta;
}


