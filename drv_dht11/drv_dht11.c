#include "drv_dht11.h"
#include "gpio.h"
#include "stdint.h"
#include "rtthread.h"
#include "main.h"

// 初始化 DHT11 传感器
void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // 定义 GPIO 初始化结构体

    __HAL_RCC_GPIOB_CLK_ENABLE();  // 使能 GPIOB 时钟

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);  // 初始化时将引脚设置为高电平

    GPIO_InitStruct.Pin = DHT11_PIN;  // 设置引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 设置引脚模式为推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // 使能上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置引脚速度为高
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);  // 初始化引脚
}

// 设置 DHT11 引脚为输入模式
void DHT11_PIN_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // 定义 GPIO 初始化结构体
    GPIO_InitStruct.Pin = DHT11_PIN;  // 设置引脚
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // 设置引脚模式为输入
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);  // 初始化引脚
}

// 设置 DHT11 引脚为输出模式
void DHT11_PIN_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // 定义 GPIO 初始化结构体

    GPIO_InitStruct.Pin = DHT11_PIN;  // 设置引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 设置引脚模式为推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置引脚速度为高
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);  // 初始化引脚
}


// 启动 DHT11 传感器读取
void DHT11_Start(void)
{
    // 发送开始信号给 DHT11，持续 18ms
    DHT11_PIN_OUT();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    rt_thread_mdelay(20);
    // 拉高数据线，等待 DHT11 响应
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    DHT11_PIN_IN();
    rt_hw_us_delay(10);
}

// 检查 DHT11 的响应
uint8_t DHT11_Check_Response(void)
{
    // 等待 DHT11 的响应信号
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) && retry < 100)
    {
        retry++;
        rt_hw_us_delay(1);
    }

    if (retry >= 100)
    {
        rval = 1; // 未收到响应
    }
    else
    {
        retry = 0;

        while (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) && retry < 100)
        {
            retry++;
            rt_hw_us_delay(1);
        }
        if (retry >= 100)
            rval = 1; // 未收到响应
    }

    return rval;
}

// 读取 DHT11 的一个位数据
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) && retry < 100)
    {
        retry++;
        rt_hw_us_delay(1);
    }

    retry = 0;

    while (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) && retry < 100)
    {
        retry++;
        rt_hw_us_delay(1);
    }

    rt_hw_us_delay(40);

    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
    {
        return 1; // 读取到高电平
    }
    else
    {
        return 0; // 读取到低电平
    }
}

// 读取 DHT11 的一个字节数据
uint8_t DHT11_Read_Byte(void)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        // 依次读取 8 位数据组成一个字节
        byte <<= 1;
        byte |= DHT11_Read_Bit();
    }
    return byte;
}

// 读取 DHT11 的湿度和温度数据
void DHT11_Read_Data(DHT11_Data_t *data)
{
    // 存放传感器数据的数组
    uint8_t sensor_data[5] = {0};
    DHT11_Start();
    if (DHT11_Check_Response() == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            sensor_data[i] = DHT11_Read_Byte();
        }
        // 将湿度和温度数据存入结构体
        data->humidity = sensor_data[0];
        data->temperature = sensor_data[2];
    }
}
