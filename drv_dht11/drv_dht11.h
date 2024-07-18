#ifndef __drv_dht11_H__
#define __drv_dht11_H__
#include "stm32f1xx_hal.h"

#include "main.h"
#include "stm32f1xx_hal.h"

#define DHT11_PIN GPIO_PIN_1
#define DHT11_PORT GPIOA

typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data_t, *DHT11_Data_p;

void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
void DHT11_Read_Data(DHT11_Data_t *data);

#endif
