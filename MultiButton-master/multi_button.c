/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#include "multi_button.h"

#define EVENT_CB(ev)                                                                                                   \
    if (handle->cb[ev])                                                                                                \
    handle->cb[ev]((void *)handle)
#define PRESS_REPEAT_MAX_NUM 15 /*!< 按钮重复按压的最大次数 */

// 按钮句柄链表头
static struct Button *head_handle = NULL;

static void button_handler(struct Button *handle);

/**
 * @brief  初始化按钮结构体句柄。
 * @param  handle: 按钮句柄结构体。
 * @param  pin_level: 读取连接的按钮GPIO电平。
 * @param  active_level: 按下时的GPIO电平。
 * @param  button_id: 按钮ID。
 * @retval 无
 */
void button_init(struct Button *handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id)
{
    memset(handle, 0, sizeof(struct Button));
    handle->event = (uint8_t)NONE_PRESS;
    handle->hal_button_Level = pin_level;
    handle->button_level = !active_level;
    handle->active_level = active_level;
    handle->button_id = button_id;
}

/**
 * @brief  附加按钮事件回调函数。
 * @param  handle: 按钮句柄结构体。
 * @param  event: 触发的事件类型。
 * @param  cb: 回调函数。
 * @retval 无
 */
void button_attach(struct Button *handle, PressEvent event, BtnCallback cb)
{
    handle->cb[event] = cb;
}

/**
 * @brief  查询按钮事件发生情况。
 * @param  handle: 按钮句柄结构体。
 * @retval 按钮事件类型。
 */
PressEvent get_button_event(struct Button *handle)
{
    return (PressEvent)(handle->event);
}

/**
 * @brief  按钮驱动核心函数，驱动状态机。
 * @param  handle: 按钮句柄结构体。
 * @retval 无
 */
static void button_handler(struct Button *handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level(handle->button_id);

    // 计时器工作...
    if ((handle->state) > 0)
        handle->ticks++;

    /*------------按钮消抖处理---------------*/
    if (read_gpio_level != handle->button_level)
    { // 与之前的电平不同
        // 继续读取相同的电平变化3次
        if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else
    { // 电平未变化，计数器重置
        handle->debounce_cnt = 0;
    }

    /*-----------------状态机-------------------*/
    switch (handle->state)
    {
    case 0:
        if (handle->button_level == handle->active_level)
        { // 开始按下
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            handle->ticks = 0;
            handle->repeat = 1;
            handle->state = 1;
        }
        else
        {
            handle->event = (uint8_t)NONE_PRESS;
        }
        break;

    case 1:
        if (handle->button_level != handle->active_level)
        { // 释放按键
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->ticks = 0;
            handle->state = 2;
        }
        else if (handle->ticks > LONG_TICKS)
        {
            handle->event = (uint8_t)LONG_PRESS_START;
            EVENT_CB(LONG_PRESS_START);
            handle->state = 5;
        }
        break;

    case 2:
        if (handle->button_level == handle->active_level)
        { // 再次按下
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            if (handle->repeat != PRESS_REPEAT_MAX_NUM)
            {
                handle->repeat++;
            }
            EVENT_CB(PRESS_REPEAT); // 重复按压
            handle->ticks = 0;
            handle->state = 3;
        }
        else if (handle->ticks > SHORT_TICKS)
        { // 释放超时
            if (handle->repeat == 1)
            {
                handle->event = (uint8_t)SINGLE_CLICK;
                EVENT_CB(SINGLE_CLICK);
            }
            else if (handle->repeat == 2)
            {
                handle->event = (uint8_t)DOUBLE_CLICK;
                EVENT_CB(DOUBLE_CLICK); // 重复按压
            }
            handle->state = 0;
        }
        break;

    case 3:
        if (handle->button_level != handle->active_level)
        { // 释放按键
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            if (handle->ticks < SHORT_TICKS)
            {
                handle->ticks = 0;
                handle->state = 2; // 重复按下
            }
            else
            {
                handle->state = 0;
            }
        }
        else if (handle->ticks > SHORT_TICKS)
        { // SHORT_TICKS < 按住时间 < LONG_TICKS
            handle->state = 1;
        }
        break;

    case 5:
        if (handle->button_level == handle->active_level)
        {
            // 继续保持触发
            handle->event = (uint8_t)LONG_PRESS_HOLD;
            EVENT_CB(LONG_PRESS_HOLD);
        }
        else
        { // 释放
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->state = 0; // 重置
        }
        break;
    default:
        handle->state = 0; // 重置
        break;
    }
}

/**
 * @brief  启动按钮工作，将句柄添加到工作列表中。
 * @param  handle: 目标句柄结构体。
 * @retval 0: 成功。 -1: 已经存在。
 */
int button_start(struct Button *handle)
{
    struct Button *target = head_handle;
    while (target)
    {
        if (target == handle)
            return -1; // 已经存在。
        target = target->next;
    }
    handle->next = head_handle;
    head_handle = handle;
    return 0;
}

/**
 * @brief  停止按钮工作，将句柄从工作列表中移除。
 * @param  handle: 目标句柄结构体。
 * @retval 无
 */
void button_stop(struct Button *handle)
{
    struct Button **curr;
    for (curr = &head_handle; *curr;)
    {
        struct Button *entry = *curr;
        if (entry == handle)
        {
            *curr = entry->next;
            //			free(entry);
            return; // glacier add 2021-8-18
        }
        else
        {
            curr = &entry->next;
        }
    }
}

/**
 * @brief  后台计时器，每隔5ms调用一次。
 * @param  无
 * @retval 无
 */
void button_ticks(void)
{
    struct Button *target;
    for (target = head_handle; target; target = target->next)
    {
        button_handler(target);
    }
}
