#ifndef __drv_key_H__
#define __drv_key_H__
#include "stm32f1xx_hal.h" //??HAL?
#include "main.h"

#define KEY1_PORT GPIOA
#define KEY2_PORT GPIOA

#define KEY1_PIN GPIO_PIN_2
#define KEY2_PIN GPIO_PIN_3

void Key_Init(void);

#endif
