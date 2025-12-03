#include "KEY/key.h"
#include "stm32f4xx.h" // Device header

/* 驱动全局实例 */
static KeyDriver_t key_driver = {0};
/* 私有函数声明 */
static bool key_read_gpio(const KeyConfig_t *config);
static bool key_is_pressed(const KeyConfig_t *config);
static void key_process_single(KeyHandle_t *key);
static void key_trigger_event(KeyHandle_t *key, KeyEvent_t event);
/* 按键状态枚举 */
typedef enum
{
    KEY_STATE_RELEASED = 0, /* 释放状态 */
    KEY_STATE_PRESSED,      /* 按下状态 */
    KEY_STATE_DEBOUNCE,     /* 消抖状态 */
    KEY_STATE_LONG_PRESS,   /* 长按状态 */
} KeyState_t;

/* 按键电平类型 */
typedef enum
{
    KEY_ACTIVE_LOW = 0, /* 低电平有效 */
    KEY_ACTIVE_HIGH,    /* 高电平有效 */
} KeyActiveType_t;

/* 按键配置结构体 */
typedef struct
{
    GPIO_TypeDef *port;          /* GPIO端口 */
    uint32_t pin;                /* GPIO引脚 */
    KeyActiveType_t active_type; /* 有效电平类型 */
    uint16_t debounce_time;      /* 消抖时间(ms) */
    uint16_t long_press_time;    /* 长按时间(ms) */
    uint16_t repeat_interval;    /* 连按间隔(ms) */
    bool enable_long_press;      /* 使能长按检测 */
    bool enable_double_click;    /* 使能双击检测 */
    bool enable_repeat;          /* 使能连按检测 */
} KeyConfig_t;

/* 按键控制块结构体 */
typedef struct
{
    bool is_used;             /* 按键是否已注册 */
    KeyConfig_t config;       /* 按键配置 */
    KeyState_t state;         /* 当前状态 */
    KeyState_t last_state;    /* 上次状态 */
    uint32_t press_time;      /* 按下时间戳 */
    uint32_t release_time;    /* 释放时间戳 */
    uint16_t click_count;     /* 点击计数(用于双击检测) */
    uint32_t last_event_time; /* 上次事件时间 */
    KeyCallback callback;     /* 回调函数 */
    void *user_data;          /* 用户数据指针 */
} KeyHandle_t;

/* 按键初始化结构体 */
typedef struct
{
    uint16_t scan_interval; /* 扫描间隔(ms) */
    uint16_t max_keys;      /* 最大按键数 */
} KeyInit_t;

/* 驱动全局状态 */
typedef struct
{
    KeyHandle_t keys[KEY_MAX_NUM];
    uint8_t key_count;
    uint32_t last_scan_time;
    bool is_initialized;
} KeyDriver_t;

/* 驱动初始化 */
bool KEY_Init(void)
{
    /* 检查驱动是否已经初始化过 */
    if (key_driver.is_initialized)
    {
        return false;
    }

    /* 初始化驱动结构体 */
    memset(&key_driver, 0, sizeof(key_driver));
    key_driver.last_scan_time = KEY_GET_CURRENT_TIME();
    key_driver.is_initialized = true;

    return true;
}

/* 驱动反初始化 */
bool KEY_DeInit(void)
{
    if (!key_driver.is_initialized)
    {
        return false;
    }

    memset(&key_driver, 0, sizeof(key_driver));
    return true;
}

/* 按键注册 */
/**
 * @brief 注册一个按键到按键驱动中
 *
 * @param config 指向按键配置结构体的指针，包含按键的硬件配置信息
 * @param callback 按键状态变化时调用的回调函数
 * @param user_data 用户自定义数据指针，会在回调函数中传回
 *
 * @return 成功时返回按键ID(0-KEY_MAX_NUM-1)，失败时返回负值错误码
 *         -1: 驱动未初始化或配置参数为空
 *         -2: 按键数量已达最大值
 *         -3: 未找到空闲的按键控制块
 */
int8_t KEY_Register(const KeyConfig_t *config, KeyCallback callback, void *user_data)
{
    /* 检查驱动是否已初始化以及配置参数是否有效 */
    if (!key_driver.is_initialized || config == NULL)
    {
        return -1;
    }

    /* 检查按键数量是否已达上限 */
    if (key_driver.key_count >= KEY_MAX_NUM)
    {
        return -2;
    }

    /* 查找空闲的按键控制块并进行初始化 */
    for (uint8_t i = 0; i < KEY_MAX_NUM; i++)
    {
        if (!key_driver.keys[i].is_used)
        {
            /* 初始化按键控制块的各项参数 */
            key_driver.keys[i].is_used = true;
            memcpy(&key_driver.keys[i].config, config, sizeof(KeyConfig_t));
            key_driver.keys[i].state = KEY_STATE_RELEASED;
            key_driver.keys[i].last_state = KEY_STATE_RELEASED;
            key_driver.keys[i].press_time = 0;
            key_driver.keys[i].release_time = 0;
            key_driver.keys[i].click_count = 0;
            key_driver.keys[i].last_event_time = 0;
            key_driver.keys[i].callback = callback;
            key_driver.keys[i].user_data = user_data;

            /* 初始化按键对应的硬件GPIO */
            KEY_HardwareInit(config->port, config->pin);

            key_driver.key_count++;
            return i; /* 返回分配的按键ID */
        }
    }

    return -3;
}

/* 按键注销 */
bool KEY_Unregister(uint8_t key_id)
{
    if (!key_driver.is_initialized || key_id >= KEY_MAX_NUM)
    {
        return false;
    }

    if (key_driver.keys[key_id].is_used)
    {
        key_driver.keys[key_id].is_used = false;
        key_driver.key_count--;
        return true;
    }

    return false;
}

/**
 * @brief 创建按键配置的默认配置
 * @param config 指向按键配置结构体的指针
 * @param port 按键连接的GPIO端口
 * @param pin 按键连接的GPIO引脚
 * @return bool 配置创建成功返回true，失败返回false
 */
bool KEY_CreateConfigDefault(KeyConfig_t*config, GPIO_TypeDef *port, uint16_t pin)
{
    // 检查配置指针是否为空
    if(config == NULL)
        return false;
    
    // 初始化默认配置参数
    KeyConfig_t my_config = {
        .port = port,
        .pin = pin,
        .active_type = KEY_ACTIVE_LOW,
        .debounce_time = KEY_DEBOUNCE_TIME,
        .long_press_time = KEY_LONG_PRESS_TIME,
        .repeat_interval = KEY_REPEAT_INTERVAL,
        .enable_long_press = true,
        .enable_double_click = true,
        .enable_repeat = false,
    };
    
    // 将默认配置复制到目标配置结构体
    *config = my_config;
    return true;
}


/* 按键任务处理 */
bool KEY_Task(void)
{
    if (!key_driver.is_initialized)
    {
        return false;
    }

    uint32_t current_time = KEY_GET_CURRENT_TIME();

    /* 检查扫描间隔 */
    if ((current_time - key_driver.last_scan_time) < KEY_SCAN_INTERVAL)
    {
        return true;
    }

    key_driver.last_scan_time = current_time;

    /* 遍历所有已注册的按键 */
    for (uint8_t i = 0; i < KEY_MAX_NUM; i++)
    {
        if (key_driver.keys[i].is_used)
        {
            key_process_single(&key_driver.keys[i]);
        }
    }

    return true;
}

/* 处理单个按键 */
static void key_process_single(KeyHandle_t *key)
{
    bool is_pressed = key_is_pressed(&key->config);
    uint32_t current_time = KEY_GET_CURRENT_TIME();

    switch (key->state)
    {
    case KEY_STATE_RELEASED:
        if (is_pressed)
        {
            /* 检测到按下，进入消抖状态 */
            key->state = KEY_STATE_DEBOUNCE;
            key->press_time = current_time;
        }
        break;

    case KEY_STATE_DEBOUNCE:
        if (!is_pressed)
        {
            /* 消抖期间释放，回到释放状态 */
            key->state = KEY_STATE_RELEASED;
        }
        else if ((current_time - key->press_time) >= key->config.debounce_time)
        {
            /* 消抖完成，进入按下状态 */
            key->state = KEY_STATE_PRESSED;
            key_trigger_event(key, KEY_EVENT_DOWN);

            /* 双击检测 */
            if (key->config.enable_double_click)
            {
                if ((current_time - key->release_time) < 500)
                { /* 双击间隔500ms */
                    key->click_count++;
                    if (key->click_count == 2)
                    {
                        key_trigger_event(key, KEY_EVENT_DOUBLE_CLICK);
                        key->click_count = 0;
                    }
                }
                else
                {
                    key->click_count = 1;
                }
            }
        }
        break;

    case KEY_STATE_PRESSED:
        if (!is_pressed)
        {
            /* 按键释放 */
            key->state = KEY_STATE_DEBOUNCE;
            key->release_time = current_time;
            key_trigger_event(key, KEY_EVENT_UP);

            /* 检测短按 */
            if ((current_time - key->press_time) < key->config.long_press_time)
            {
                key_trigger_event(key, KEY_EVENT_SHORT_CLICK);
            }

            /* 检测长按释放 */
            if ((current_time - key->press_time) >= key->config.long_press_time)
            {
                key_trigger_event(key, KEY_EVENT_LONG_PRESS_UP);
            }
        }
        else
        {
            /* 检查长按 */
            if (key->config.enable_long_press)
            {
                if ((current_time - key->press_time) >= key->config.long_press_time)
                {
                    key->state = KEY_STATE_LONG_PRESS;
                    key_trigger_event(key, KEY_EVENT_LONG_PRESS_START);
                }
            }

            /* 检查连按 */
            if (key->config.enable_repeat)
            {
                uint32_t press_duration = current_time - key->press_time;
                if (press_duration > key->config.long_press_time)
                {
                    uint32_t repeat_count = (press_duration - key->config.long_press_time) / key->config.repeat_interval;
                    if (repeat_count > key->click_count)
                    {
                        key_trigger_event(key, KEY_EVENT_REPEAT);
                        key->click_count = repeat_count;
                    }
                }
            }
        }
        break;

    case KEY_STATE_LONG_PRESS:
        if (!is_pressed)
        {
            key->state = KEY_STATE_DEBOUNCE;
            key->release_time = current_time;
            key_trigger_event(key, KEY_EVENT_UP);
            key_trigger_event(key, KEY_EVENT_LONG_PRESS_UP);
        }
        else
        {
            /* 长按保持事件 */
            static uint32_t last_long_press_time = 0;
            if ((current_time - last_long_press_time) >= 200)
            { /* 每200ms触发一次长按保持事件 */
                key_trigger_event(key, KEY_EVENT_LONG_PRESS_HOLD);
                last_long_press_time = current_time;
            }
        }
        break;
    }

    key->last_state = key->state;
}

/* 触发按键事件 */
static void key_trigger_event(KeyHandle_t *key, KeyEvent_t event)
{
    if (key->callback != NULL)
    {
        uint8_t key_id = (uint8_t)(key - key_driver.keys);
        key->callback(key_id, event);
    }
}

/* 读取GPIO状态 */
/**
 * @brief 读取按键GPIO引脚状态
 * @param config 按键配置结构体指针，包含端口、引脚和有效电平类型等信息
 * @return bool 按键状态，按下时返回true，未按下时返回false
 *
 * 该函数通过读取指定GPIO引脚的电平状态来判断按键是否被按下。
 * 根据按键的有效电平类型（高电平有效或低电平有效）进行相应的逻辑处理。
 */
static bool key_read_gpio(const KeyConfig_t *config)
{
    /* 读取GPIO引脚的当前电平状态 */
    uint8_t pin_state = GPIO_ReadInputDataBit(config->port, config->pin);

    /* 根据按键的有效电平类型判断按键状态 */
    if (config->active_type == KEY_ACTIVE_LOW)
    {
        return (pin_state == Bit_RESET);
    }
    else
    {
        return (pin_state == Bit_SET);
    }
}

/* 检测按键是否按下 */
static bool key_is_pressed(const KeyConfig_t *config)
{
    return key_read_gpio(config);
}

/* 判断按键是否按下 */
bool KEY_IsPressed(uint8_t key_id)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    return key_is_pressed(&key_driver.keys[key_id].config);
}

/* 判断是否长按 */
bool KEY_IsLongPress(uint8_t key_id)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    return (key_driver.keys[key_id].state == KEY_STATE_LONG_PRESS);
}

/* 获取按键按下持续时间 */
uint16_t KEY_GetPressDuration(uint8_t key_id)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return 0;
    }

    if (key_driver.keys[key_id].state == KEY_STATE_RELEASED)
    {
        return 0;
    }

    return KEY_GET_CURRENT_TIME() - key_driver.keys[key_id].press_time;
}

/* 设置消抖时间 */
bool KEY_SetDebounceTime(uint8_t key_id, uint16_t time_ms)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.debounce_time = time_ms;
    return true;
}

/* 设置长按时间 */
bool KEY_SetLongPressTime(uint8_t key_id, uint16_t time_ms)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.long_press_time = time_ms;
    return true;
}

/* 设置连按间隔 */
bool KEY_SetRepeatInterval(uint8_t key_id, uint16_t interval_ms)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.repeat_interval = interval_ms;
    return true;
}

/* 使能/禁用长按检测 */
bool KEY_EnableLongPress(uint8_t key_id, bool enable)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.enable_long_press = enable;
    return true;
}

/* 使能/禁用双击检测 */
bool KEY_EnableDoubleClick(uint8_t key_id, bool enable)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.enable_double_click = enable;
    return true;
}

/* 使能/禁用连按检测 */
bool KEY_EnableRepeat(uint8_t key_id, bool enable)
{
    if (key_id >= KEY_MAX_NUM || !key_driver.keys[key_id].is_used)
    {
        return false;
    }

    key_driver.keys[key_id].config.enable_repeat = enable;
    return true;
}


/* 硬件GPIO初始化 */
void KEY_HardwareInit(GPIO_TypeDef *port, uint32_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    if (port == GPIOA)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    }
    else if (port == GPIOB)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    }
    else if (port == GPIOC)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    }
    else if (port == GPIOD)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    }
    else if (port == GPIOE)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    }
    else if (port == GPIOF)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    }
    else if (port == GPIOG)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    }

    /* 配置GPIO为上拉输入模式 */
    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; /* 默认使用上拉，低电平有效 */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);
}
