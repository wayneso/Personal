#include "drv_dht11.h"
#include "gpio.h"
#include "stdint.h"

void DHT11_Start(void) {
    // 拉低总线至少 18ms
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);

    // 拉高总线，并等待 DHT11 响应
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    HAL_Delay(30);
}

uint8_t DHT11_Check_Response(void) {
    // 等待 DHT11 拉低总线作为响应信号
    uint32_t timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        timeout++;
        if (timeout > 100) return 0;  // 超时返回错误
        HAL_Delay(1);
    }

    // 等待 DHT11 拉高总线
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        timeout++;
        if (timeout > 100) return 0;  // 超时返回错误
        HAL_Delay(1);
    }

    return 1; // 响应成功
}

uint8_t DHT11_Read_Bit(void) {
    uint8_t bit = 0;
    // 等待高电平，超时返回错误
    uint32_t timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        timeout++;
        if (timeout > 100) return 0;
        HAL_Delay(1);
    }

    // 等待低电平，超时返回错误
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        timeout++;
        if (timeout > 100) return 0;
        HAL_Delay(1);
    }

    // 判断高电平持续时间来确定数据位
    HAL_Delay(40);
    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        bit = 1;
    }

    return bit;
}

uint8_t DHT11_Read_Byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        // 依次读取 8 位数据并合成字节
        byte <<= 1;
        byte |= DHT11_Read_Bit();
    }
    return byte;
}

void DHT11_Read_Data(DHT11_Data_t *data) {
    // 读取温湿度数据
    uint8_t sensor_data[5] = {0};
    DHT11_Start();
    if (DHT11_Check_Response()) {
        for (int i = 0; i < 5; i++) {
            sensor_data[i] = DHT11_Read_Byte();
        }
        // 将湿度和温度数据存储到结构体中
        data->humidity = sensor_data[0];
        data->temperature = sensor_data[2];
    }
}
