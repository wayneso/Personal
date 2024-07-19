/*
 * @Author       :王   硕
 * @Date         :2024-01-03 14:47:29
 * @LastEditTime :2024-02-22 16:30:14
 * @Description  :DS18B20驱动
 */

#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "n32g401.h"
#include "main.h"

/*  DS18B20时钟端口,引脚定义 */
#define DS18B20_PORT     GPIOA
#define DS18B20_PIN      GPIO_PIN_10
#define DS18B20_PORT_RCC RCC_AHB_PERIPH_GPIOA

// IO操作函数
#define DS18B20_DQ_IN GPIO_Input_Pin_Data_Get(DS18B20_PORT, DS18B20_PIN)

#define DS18B20_DQ_OUT_1 GPIO_Pins_Set(DS18B20_PORT, DS18B20_PIN)
#define DS18B20_DQ_OUT_0 GPIO_Pins_Reset(DS18B20_PORT, DS18B20_PIN)

void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
u8 DS18B20_Init(void);           // 初始化DS18B20
void DS18B20_Start(void);        // 开始温度转换
void DS18B20_Write_Byte(u8 dat); // 写入一个字节
u8 DS18B20_Read_Byte(void);      // 读出一个字节
u8 DS18B20_Read_Bit(void);       // 读出一个位
u8 DS18B20_Check(void);          // 检测是否存在DS18B20
void DS18B20_Rst(void);          // 复位DS18B20
short DS18B20_Get_Temp(void);    // 获取温度

#endif
