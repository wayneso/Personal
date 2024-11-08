/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include <stdint.h>
#include <string.h>

//根据需要修改常量
#define TICKS_INTERVAL    5	// 时间间隔（毫秒）
#define DEBOUNCE_TICKS    3	// 去抖动次数，最大为 7 (0 ~ 7)
#define SHORT_TICKS       (300 /TICKS_INTERVAL)  // 短按超时（单位：tick）
#define LONG_TICKS        (1000 /TICKS_INTERVAL) // 长按超时（单位：tick）

// 按钮回调函数类型
typedef void (*BtnCallback)(void*);

// 按钮事件类型枚举
typedef enum {
	PRESS_DOWN = 0,         // 按下事件
	PRESS_UP,               // 松开事件
	PRESS_REPEAT,           // 按压重复事件
	SINGLE_CLICK,           // 单击事件
	DOUBLE_CLICK,           // 双击事件
	LONG_PRESS_START,       // 长按开始事件
	LONG_PRESS_HOLD,        // 长按保持事件
	number_of_event,        // 事件总数
	NONE_PRESS              // 无按压事件
} PressEvent;

// 按钮结构体
typedef struct Button {
	uint16_t ticks;                  // 按钮按下的计时器
	uint8_t  repeat : 4;             // 按钮重复次数（最大值为15）
	uint8_t  event : 4;              // 当前按钮事件
	uint8_t  state : 3;              // 当前按钮状态
	uint8_t  debounce_cnt : 3;       // 去抖动计数
	uint8_t  active_level : 1;       // 按钮按下时的电平（高或低）
	uint8_t  button_level : 1;      // 当前读取的按钮电平
	uint8_t  button_id;             // 按钮ID
	uint8_t  (*hal_button_Level)(uint8_t button_id_); // 读取按钮电平的函数指针
	BtnCallback  cb[number_of_event]; // 事件回调函数数组
	struct Button* next;             // 指向下一个按钮结构体的指针
} Button;

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  初始化按钮结构体句柄。
  * @param  handle: 按钮句柄结构体。
  * @param  pin_level: 读取连接的按钮GPIO电平函数。
  * @param  active_level: 按下时的GPIO电平。
  * @param  button_id: 按钮ID。
  * @retval 无
  */
void button_init(struct Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);

/**
  * @brief  附加按钮事件回调函数。
  * @param  handle: 按钮句柄结构体。
  * @param  event: 触发的事件类型。
  * @param  cb: 回调函数。
  * @retval 无
  */
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);

/**
  * @brief  获取按钮事件类型。
  * @param  handle: 按钮句柄结构体。
  * @retval 返回当前按钮事件类型。
  */
PressEvent get_button_event(struct Button* handle);

/**
  * @brief  启动按钮工作，将按钮句柄添加到工作列表中。
  * @param  handle: 目标按钮句柄结构体。
  * @retval 0: 成功，-1: 已存在。
  */
int button_start(struct Button* handle);

/**
  * @brief  停止按钮工作，将按钮句柄从工作列表中移除。
  * @param  handle: 目标按钮句柄结构体。
  * @retval 无
  */
void button_stop(struct Button* handle);

/**
  * @brief  后台计时器，每隔5ms调用一次，用于处理按钮状态。
  * @param  无
  * @retval 无
  */
void button_ticks(void);

#ifdef __cplusplus
}
#endif

#endif
