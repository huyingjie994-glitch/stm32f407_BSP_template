#ifndef	_SYS_H_
#define _SYS_H_
#include "stm32f4xx.h"                  // Device header
#include "stdio.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

// STM32F407 GPIO地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE + 0x14) // GPIOA 输出数据寄存器
#define GPIOB_ODR_Addr    (GPIOB_BASE + 0x14) // GPIOB 输出数据寄存器
#define GPIOC_ODR_Addr    (GPIOC_BASE + 0x14) // GPIOC 输出数据寄存器
#define GPIOD_ODR_Addr    (GPIOD_BASE + 0x14) // GPIOD 输出数据寄存器
#define GPIOE_ODR_Addr    (GPIOE_BASE + 0x14) // GPIOE 输出数据寄存器
#define GPIOF_ODR_Addr    (GPIOF_BASE + 0x14) // GPIOF 输出数据寄存器
#define GPIOG_ODR_Addr    (GPIOG_BASE + 0x14) // GPIOG 输出数据寄存器
#define GPIOH_ODR_Addr    (GPIOH_BASE + 0x14) // GPIOH 输出数据寄存器
#define GPIOI_ODR_Addr    (GPIOI_BASE + 0x14) // GPIOI 输出数据寄存器

#define GPIOA_IDR_Addr    (GPIOA_BASE + 0x10) // GPIOA 输入数据寄存器
#define GPIOB_IDR_Addr    (GPIOB_BASE + 0x10) // GPIOB 输入数据寄存器
#define GPIOC_IDR_Addr    (GPIOC_BASE + 0x10) // GPIOC 输入数据寄存器
#define GPIOD_IDR_Addr    (GPIOD_BASE + 0x10) // GPIOD 输入数据寄存器
#define GPIOE_IDR_Addr    (GPIOE_BASE + 0x10) // GPIOE 输入数据寄存器
#define GPIOF_IDR_Addr    (GPIOF_BASE + 0x10) // GPIOF 输入数据寄存器
#define GPIOG_IDR_Addr    (GPIOG_BASE + 0x10) // GPIOG 输入数据寄存器
#define GPIOH_IDR_Addr    (GPIOH_BASE + 0x10) // GPIOH 输入数据寄存器
#define GPIOI_IDR_Addr    (GPIOI_BASE + 0x10) // GPIOI 输入数据寄存器

// IO口操作，针对单一的IO口操作宏，确保n值小于16!
//将 GPIOAx 端口的第 n 个引脚设置为输出状态。
//  PAout(2) = x	x: 1:输出高电平    0：输出低电平
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  //将 GPIOAx 端口的第 n 个引脚设置为输出状态。
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  // 输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  // 输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  // 输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  // 输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)  // 输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)  // 输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr, n)  // 输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr, n)  // 输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr, n)  // 输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr, n)  // 输入

void NVIC_Config(void);
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);	//设置堆栈地址
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);
#endif
