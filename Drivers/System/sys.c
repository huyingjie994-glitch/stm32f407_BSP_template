#include "sys.h"
#include "stm32f4xx.h"                  // Device header
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//中断控制结构体变量定义
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//4级抢占,4级响应。

	NVIC_InitStructure.NVIC_IRQChannel                    	= USART1_IRQn;//中断通道指定为USART1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  	= 3;//主优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         	= 3;//次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd                 	= ENABLE;//确定使能
	NVIC_Init(&NVIC_InitStructure);//初始化配置此中断通道
	

}
//********************************************************************************  
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
