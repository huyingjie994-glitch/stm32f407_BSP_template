#ifndef	_IIC_H_
#define	_IIC_H_
#include "sys.h"
#define RCC_MY_IIC_CLK()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define IIC_SCL_GPIO_PORT		GPIOB
#define	IIC_SCL_GPIO_PIN		GPIO_Pin_8

#define IIC_SDA_GPIO_PORT		GPIOB
#define	IIC_SDA_GPIO_PIN		GPIO_Pin_9

/******************************************************************************************/

/* IO操作 */
#define IIC_SCL(x)        do{ x ? \
                              GPIO_SetBits(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN) : \
                              GPIO_ResetBits(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN); \
                          }while(0)       /* SCL */
#define IIC_SDA(x)        do{ x ? \
                              GPIO_SetBits(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) : \
                              GPIO_ResetBits(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN); \
                          }while(0)       /* SDA */
#define IIC_READ_SDA     GPIO_ReadInputDataBit(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) /* 读取SDA */
						  
/* IIC所有操作函数 */
void iic_init(void);            /* 初始化IIC的IO口 */
void iic_start(void);           /* 发送IIC开始信号 */
void iic_stop(void);            /* 发送IIC停止信号 */
void iic_ack(void);             /* IIC发送ACK信号 */
void iic_nack(void);            /* IIC不发送ACK信号 */
uint8_t iic_wait_ack(void);     /* IIC等待ACK信号 */
void iic_send_byte(uint8_t txd);/* IIC发送一个字节 */
uint8_t iic_read_byte(unsigned char ack);/* IIC读取一个字节 */						  
			
#endif
