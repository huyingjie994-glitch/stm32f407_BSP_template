#ifndef _KEY_H_
#define _KEY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/*********************USER include**************************** */
#include "sys.h"
/************************************************************* */
#define KEY_GET_CURRENT_TIME()  (uint32_t)SysTick_GetTick()     // 获取系统时间戳，提供给按键驱动，这里用户自行实现
/* 按键配置宏定义 */
#ifndef KEY_MAX_NUM
#define KEY_MAX_NUM             4      /* 最大支持的按键数量 */
#endif

#ifndef KEY_DEBOUNCE_TIME
#define KEY_DEBOUNCE_TIME       20      /* 消抖时间(ms) */
#endif

#ifndef KEY_LONG_PRESS_TIME
#define KEY_LONG_PRESS_TIME     1000    /* 长按时间(ms) */
#endif

#ifndef KEY_REPEAT_INTERVAL
#define KEY_REPEAT_INTERVAL     200     /* 连按间隔时间(ms) */
#endif

#ifndef KEY_SCAN_INTERVAL
#define KEY_SCAN_INTERVAL       10      /* 按键扫描间隔(ms) */
#endif

/* 按键事件枚举 */
typedef enum {
    KEY_EVENT_NONE = 0,          /* 无事件 */
    KEY_EVENT_DOWN,              /* 按键按下 */
    KEY_EVENT_UP,                /* 按键释放 */
    KEY_EVENT_SHORT_CLICK,       /* 短按点击 */
    KEY_EVENT_LONG_PRESS_START,  /* 长按开始 */
    KEY_EVENT_LONG_PRESS_HOLD,   /* 长按保持 */
    KEY_EVENT_LONG_PRESS_UP,     /* 长按释放 */
    KEY_EVENT_DOUBLE_CLICK,      /* 双击 */
    KEY_EVENT_REPEAT,            /* 连按 */
} KeyEvent_t;

/* 按键回调函数类型 */
typedef void (*KeyCallback)(uint8_t key_id, KeyEvent_t event);

/* 前向声明 结构体 */
typedef struct KeyConfig_t KeyConfig_t;

/* 函数声明 */
/* 驱动管理函数 */
bool KEY_Init(void);
bool KEY_DeInit(void);
bool KEY_Task(void);

/* 按键注册/注销函数 */
int8_t KEY_Register(const KeyConfig_t *config, KeyCallback callback, void *user_data);
bool KEY_Unregister(uint8_t key_id);

/* 按键配置函数 */
bool KEY_CreateConfigDefault(KeyConfig_t*config, GPIO_TypeDef *port, uint16_t pin);
bool KEY_SetDebounceTime(uint8_t key_id, uint16_t time_ms);
bool KEY_SetLongPressTime(uint8_t key_id, uint16_t time_ms);
bool KEY_SetRepeatInterval(uint8_t key_id, uint16_t interval_ms);
bool KEY_EnableLongPress(uint8_t key_id, bool enable);
bool KEY_EnableDoubleClick(uint8_t key_id, bool enable);
bool KEY_EnableRepeat(uint8_t key_id, bool enable);

/* 按键状态获取函数 */
bool KEY_IsPressed(uint8_t key_id);
bool KEY_IsLongPress(uint8_t key_id);
uint16_t KEY_GetPressDuration(uint8_t key_id);
// KeyEvent_t KEY_GetEvent(uint8_t key_id);

/* 事件处理函数 */
// void KEY_ClearEvent(uint8_t key_id);

/* 硬件初始化函数 */
void KEY_HardwareInit(GPIO_TypeDef *port, uint32_t pin);

#endif

