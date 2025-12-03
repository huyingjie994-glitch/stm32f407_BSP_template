#include "sys.h"
#include "stm32f4xx.h"                  // Device header

volatile uint32_t system_tick;
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//中断控制结构体变量定义
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//4级抢占,4级响应。

	NVIC_InitStructure.NVIC_IRQChannel                    	= SYSTICK_TIMER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  	= 3;//主优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         	= 3;//次优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd                 	= ENABLE;//确定使能
	NVIC_Init(&NVIC_InitStructure);//初始化配置此中断通道


	NVIC_InitStructure.NVIC_IRQChannel                    	= USART1_IRQn;//中断通道指定为USART1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  	= 3;//主优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         	= 3;//次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd                 	= ENABLE;//确定使能
	NVIC_Init(&NVIC_InitStructure);//初始化配置此中断通道
}
void System_Init(void)
{
	/* 定时器基础配置结构体 */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* 使能TIM6时钟 */
	RCC_SYSTICK_TIMER_CLK_ENABLE();

	/* 配置TIM6使用内部时钟源 */
	TIM_InternalClockConfig(SYSTICK_TIMER);

	/* 配置定时器基础参数 */
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1;              /* 设置自动重载值F */
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;                /* 设置预分频系数(不分频) */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* 设置时钟分割为不分频 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 设置计数模式为向上计数 */
	TIM_TimeBaseInit(SYSTICK_TIMER, &TIM_TimeBaseStructure);

	/* 使能更新中断并清除更新标志位，启动定时器 */
	TIM_ITConfig(SYSTICK_TIMER, TIM_IT_Update, ENABLE);     /* 使能TIM6更新中断 */
	TIM_ClearFlag(SYSTICK_TIMER, TIM_FLAG_Update);          /* 清除更新中断标志位 */
	TIM_Cmd(SYSTICK_TIMER, ENABLE);                         /* 启动TIM6定时器 */

	/* 初始化系统滴答计数器 */
	system_tick = 0;
}

uint32_t SysTick_GetTick(void)
{
	return system_tick; 
}

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

void SYSTICK_TIMER_IRQHandler(void)
{
	if(TIM_GetITStatus(SYSTICK_TIMER,TIM_IT_Update)==SET) //溢出中断
	{
		system_tick ++;
		TIM_ClearITPendingBit(SYSTICK_TIMER,TIM_IT_Update);
	}
}
