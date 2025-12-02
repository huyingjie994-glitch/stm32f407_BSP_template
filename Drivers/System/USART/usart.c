#include "USART/usart.h" 
#include "stm32f4xx.h"                  // Device header
#include "string.h"
#include "stdio.h"

uint8_t U1_RxBuff[U1_RX_SIZE];
UCB_CB U0_CB;
static void USART1_DMA_Init(void);

/****************************************************/
/*
*********************************************************************************************************
*	函 数 名: UARTx_Init
*	功能说明:串口初始化
*	形    参: 
*	返 回 值: 0：句柄有效且第一次创建		0xff：创建失败
*********************************************************************************************************
*/
uint8_t UARTx_Init (USART_TypeDef *USARTx, uint32_t __Baud)
{
	static uint8_t	USART1_ed = RESET;
	static uint8_t	USART2_ed = RESET;
//	static uint8_t	USART3_ed = RESET;
//	static uint8_t	USART4_ed = RESET;
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	USART_InitTypeDef USART_InitStructure = {0};//定义配置串口的结构体变量
	if(USARTx == USART1 && USART1_ed == RESET)
	{
		USART1_ed = SET;
		RCC_USART1_GPIO_ENABLE();
		GPIO_PinAFConfig(USART1_GPIO,USART1_TX_Source,GPIO_AF_USART1);//IO口用作串口引脚要配置复用模式
		GPIO_PinAFConfig(USART1_GPIO,USART1_RX_Source,GPIO_AF_USART1);
		
		RCC_USART1_CLK_ENABLE();
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin           = USART1_TX_PIN;//TX引脚
		GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;//IO口用作串口引脚要配置复用模式
		GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
		GPIO_Init(USART1_GPIO,&GPIO_InitStructure);

		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin           = USART1_RX_PIN;//RX引脚
		GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
		GPIO_Init(USART1_GPIO,&GPIO_InitStructure);
		
		USART_DeInit(USARTx);//大概意思是解除此串口的其他配置
		
		USART_StructInit(&USART_InitStructure);
		USART_InitStructure.USART_BaudRate              = __Baud;//设置波特率
		USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//字节长度为8bit
		USART_InitStructure.USART_StopBits              = USART_StopBits_1;//1个停止位
		USART_InitStructure.USART_Parity                = USART_Parity_No ;//没有校验位
		USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//将串口配置为收发模式
		USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None; //不提供流控 
		USART_Init(USARTx,&USART_InitStructure);//将相关参数初始化给串口1
		
		USART_ClearFlag(USARTx,USART_FLAG_IDLE);//初始配置时清除空闲置位	
		USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);//初始配置空闲中断
		USART_Cmd(USARTx,ENABLE);//开启串口1
		
		USART1_DMA_Init();
		U1_RxBuffPtr_Init();
	}
	else if(USARTx == USART2 && USART2_ed == RESET)
	{
		USART2_ed = SET;
		RCC_USART1_GPIO_ENABLE();
		GPIO_PinAFConfig(USART2_GPIO,USART2_TX_Source,GPIO_AF_USART2);//IO口用作串口引脚要配置复用模式
		GPIO_PinAFConfig(USART2_GPIO,USART2_RX_Source,GPIO_AF_USART2);
		RCC_USART2_CLK_ENABLE();
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin           = USART2_TX_PIN;//TX引脚
		GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;//IO口用作串口引脚要配置复用模式
		GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
		GPIO_Init(USART2_GPIO,&GPIO_InitStructure);

		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin           = USART2_RX_PIN;//RX引脚
		GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
		GPIO_Init(USART2_GPIO,&GPIO_InitStructure);
		
		USART_DeInit(USARTx);//大概意思是解除此串口的其他配置
		
		USART_StructInit(&USART_InitStructure);
		USART_InitStructure.USART_BaudRate              = __Baud;//设置波特率
		USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//字节长度为8bit
		USART_InitStructure.USART_StopBits              = USART_StopBits_1;//1个停止位
		USART_InitStructure.USART_Parity                = USART_Parity_No ;//没有校验位
		USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//将串口配置为收发模式
		USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None; //不提供流控 
		USART_Init(USARTx,&USART_InitStructure);//将相关参数初始化给串口1
		
		USART_ClearFlag(USARTx,USART_FLAG_RXNE);//初始配置时清除接受置位

		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//初始配置接受中断
		USART_Cmd(USARTx,ENABLE);//开启串口1
	}
	else 
	{
		return 0xff;
	}
	return 0;
}

void USART_SendByte(USART_TypeDef* USARTx,uint8_t Byte)  //发送一个字节
{
	USART_SendData(USARTx,Byte);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
}

void USART_SendShuzu(USART_TypeDef* USARTx,uint8_t *Shuzu,uint16_t Length)    //发送数组
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		USART_SendByte(USARTx,Shuzu[i]);
	}
}

void USART_SendString(USART_TypeDef* USARTx,char *String)     //发送字符串
{
	uint8_t m;
	for(m=0;String[m]!='\0';m++)
	{
		USART_SendByte(USARTx,String[m]);
	}
}

static void USART1_DMA_Init(void)
{
	USART_Cmd(USART1, DISABLE);  //失能串口1
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	/* 配置串口DMA接收*/
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  					// 开启DMA时钟
    DMA_DeInit(DMA2_Stream2);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 							//通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;		//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)U1_RxBuff;	//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;   				//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = U1_RX_MAX+1;						//数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;				    //高等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //不开启FIFO模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           //FIFO阈值
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//外设突发单次传输
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream2, ENABLE); //使能DMA2_Stream2通道

    USART_Cmd(USART1, ENABLE);  //使能串口1
}


void U1_RxBuffPtr_Init(void)
{
	U0_CB.Rxcounter = 0;
	U0_CB.URxDataIN = &U0_CB.UCB_RxDataPtr[0];
	U0_CB.URxDataOUT = &U0_CB.UCB_RxDataPtr[0];
	U0_CB.URxDataEND = &U0_CB.UCB_RxDataPtr[U_RXNUM-1];
	U0_CB.URxDataIN->start = U1_RxBuff;
}

#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0x40) == 0);               /* 等待上一个字符发送完成 */

    USART1->DR = (uint8_t)ch;                       /* 将要发送的字符 ch 写入到DR寄存器 */
	
    return ch;
}



/******** 串口1 中断服务函数 ***********/
void USART1_IRQHandler(void)
{
//	char receivedData = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)//判断是不是真的有中断发生
	{
//		USART_ReceiveData(USART1);
		USART1->SR;
		USART1->DR;
		U0_CB.Rxcounter += U1_RX_MAX +1 - DMA_GetCurrDataCounter(DMA2_Stream2);
		U0_CB.URxDataIN->end = &U1_RxBuff[U0_CB.Rxcounter-1];
		U0_CB.URxDataIN++;
		if(U0_CB.URxDataIN == U0_CB.URxDataEND)
		{
			U0_CB.URxDataIN = &U0_CB.UCB_RxDataPtr[0];
		}
		if(U1_RX_SIZE - U0_CB.Rxcounter > U1_RX_MAX)
		{
			U0_CB.URxDataIN->start = &U1_RxBuff[U0_CB.Rxcounter];
		}
		else
		{
			U0_CB.URxDataIN->start = U1_RxBuff;
			U0_CB.Rxcounter = 0;
		}
		DMA_Cmd(DMA2_Stream2, DISABLE);
		DMA_SetCurrDataCounter(DMA2_Stream2,U1_RX_MAX+1);
		DMA2_Stream2->M0AR = (uint32_t)U0_CB.URxDataIN->start;
		DMA_Cmd(DMA2_Stream2, ENABLE);
//		USART_SendShuzu(USART1,U1_RxBuff,50);
	}
}
