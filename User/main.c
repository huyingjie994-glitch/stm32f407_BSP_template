#include "sys.h"
#include <DELAY/delay.h>
#include "string.h"
#include "USART/usart.h" 
#include "KEY/key.h"

/******************************************** */
KeyConfig_t key1_config;
int8_t key1_id;
/******************************************** */
/* 按键回调函数 */
void key_callback(uint8_t key_id, KeyEvent_t event)
{
    switch (event) {
        case KEY_EVENT_DOWN:
            printf("Key %d: Pressed down\r\n", key_id);
            break;
            
        case KEY_EVENT_UP:
            printf("Key %d: Released\r\n", key_id);
            break;
            
        case KEY_EVENT_SHORT_CLICK:
            printf("Key %d: Short click\r\n", key_id);
            break;
            
        case KEY_EVENT_LONG_PRESS_START:
            printf("Key %d: Long press start\r\n", key_id);
            break;
            
        case KEY_EVENT_LONG_PRESS_HOLD:
            printf("Key %d: Long press holding\r\n", key_id);
            break;
            
        case KEY_EVENT_LONG_PRESS_UP:
            printf("Key %d: Long press released\r\n", key_id);
            break;
            
        case KEY_EVENT_DOUBLE_CLICK:
            printf("Key %d: Double click\r\n", key_id);
            break;
            
        case KEY_EVENT_REPEAT:
            printf("Key %d: Repeat click\r\n", key_id);
            break;
            
        default:
            break;
    }
}

int main(void)
{
	UARTx_Init(USART1,115200);		/* 初始化USART1串口，波特率为115200 */
	
	KEY_Init();		/* 初始化使能按键 */
	KEY_CreateConfigDefault(&key1_config, GPIOA, GPIO_Pin_0);	/* 创建按键1的默认配置，使用GPIOA的Pin0 */
	key1_id = KEY_Register(&key1_config, key_callback, NULL);		/* 注册按键1的配置、回调函数和用户数据 */
	if (key1_id < 0) {
        printf("Key 1 registration failed!\r\n");
    }
	while(1)
	{
		/* 按键扫描任务 */
        KEY_Task();
	}
}
