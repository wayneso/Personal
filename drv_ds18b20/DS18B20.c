#include "DS18B20.h"
#include "rtthread.h"

/**
 * @description: IO口输出初始化
 * @param none
 * @return none
 */
void DS18B20_IO_OUT(void)
{
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.Pin = DS18B20_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;
  GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
  GPIO_Peripheral_Initialize(DS18B20_PORT, &GPIO_InitStructure);
}

/**
 * @description: IO口输入初始化
 * @param none
 * @return none
 */
void DS18B20_IO_IN(void)
{
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.Pin = DS18B20_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
  GPIO_Peripheral_Initialize(DS18B20_PORT, &GPIO_InitStructure);
}

/**
 * @description: 复位DS18B20
 * @param none
 * @return none
 */
void DS18B20_Rst(void)
{
  DS18B20_IO_OUT();    // 设置为输出
  DS18B20_DQ_OUT_0;    // 拉低DQ
  rt_hw_us_delay(750); // 拉低750us
  DS18B20_DQ_OUT_1;    // DQ=1
  rt_hw_us_delay(15);
}

/**
 * @description: 等待DS18B20的回应
 * @param none
 * @return 返回1:未检测到DS18B20的存在 返回0:存在
 */
u8 DS18B20_Check(void)
{
  u8 retry = 0;
  DS18B20_IO_IN(); // 设置为输入
  while (DS18B20_DQ_IN && retry < 200)
  {
    retry++;
    rt_hw_us_delay(1);
  };
  if (retry >= 200)
  {
    return 1;
  }

  else
  {
    retry = 0;
  }
  while (!DS18B20_DQ_IN && retry < 240)
  {
    retry++;
    rt_hw_us_delay(1);
  };
  if (retry >= 240)
    return 1;
  return 0;
}

/**
 * @description: 从DS18B20读取一个位
 * @param none
 * @return 1/0
 */
u8 DS18B20_Read_Bit(void)
{
  u8 data;
  DS18B20_IO_OUT(); // 设置为输出
  DS18B20_DQ_OUT_0;
  rt_hw_us_delay(2);
  DS18B20_DQ_OUT_1;
  DS18B20_IO_IN(); // 设置为输入
  rt_hw_us_delay(12);
  if (DS18B20_DQ_IN)
  {
    data = 1;
  }
  else
  {
    data = 0;
  }
  rt_hw_us_delay(50);
  return data;
}

/**
 * @description: 从DS18B20读取一个字节
 * @param none
 * @return dat
 */
u8 DS18B20_Read_Byte(void)
{
  u8 i, j, dat;
  dat = 0;
  for (i = 1; i <= 8; i++)
  {
    j = DS18B20_Read_Bit();
    dat = (j << 7) | (dat >> 1);
  }
  return dat;
}

/**
 * @description: 写一个字节到DS18B20
 * @param u8 dat
 * @return dat：要写入的字节
 */
void DS18B20_Write_Byte(u8 dat)
{
  u8 j;
  u8 testb;
  DS18B20_IO_OUT(); // 设置为输出
  for (j = 1; j <= 8; j++)
  {
    testb = dat & 0x01;
    dat = dat >> 1;
    if (testb)
    {
      DS18B20_DQ_OUT_0; // 写1
      rt_hw_us_delay(10);
      DS18B20_DQ_OUT_1;
      rt_hw_us_delay(60);
    }
    else
    {
      DS18B20_DQ_OUT_0; // 写0
      rt_hw_us_delay(60);
      DS18B20_DQ_OUT_1;
      rt_hw_us_delay(10);
    }
  }
}

/**
 * @description: 开始温度转换
 * @param none
 * @return none
 */
void DS18B20_Start(void)
{
  DS18B20_Rst();
  DS18B20_Check();
  DS18B20_Write_Byte(0xcc); // 跳过 ROM
  DS18B20_Write_Byte(0x44); // 转换
}

/**
 * @description: 初始化DS18B20的IO口 DQ 同时检测DS的存在
 * @param none
 * @return 返回1:不存在
 * @return 返回0:存在
 */
u8 DS18B20_Init(void)
{
  RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.Pin = DS18B20_PIN; // 推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;
  GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
  GPIO_Peripheral_Initialize(DS18B20_PORT, &GPIO_InitStructure);

  GPIO_Pins_Set(DS18B20_PORT, DS18B20_PIN); // 拉高

  DS18B20_Rst();

  return DS18B20_Check();
}

/**
 * @description: 从ds18b20得到温度值
 * @description: 精度：0.1C
 * @param none
 * @return 温度值 （-550~1250）
 */
short DS18B20_Get_Temp(void)
{
  u8 temp;
  u8 TL, TH;
  short tem;
  DS18B20_Start(); // ds1820 start convert
  DS18B20_Rst();
  DS18B20_Check();
  DS18B20_Write_Byte(0xcc); // skip rom
  DS18B20_Write_Byte(0xbe); // convert
  TL = DS18B20_Read_Byte(); // LSB
  TH = DS18B20_Read_Byte(); // MSB

  if (TH > 7)
  {
    TH = ~TH;
    TL = ~TL;
    temp = 0; // 温度为负
  }
  else
  {
    temp = 1; // 温度为正
  }

  tem = TH; // 获得高八位
  tem <<= 8;
  tem += TL;                // 获得底八位
  tem = (float)tem * 0.625; // 转换
  if (temp)
  {
    return tem; // 返回温度值
  }

  else
  {
    return -tem;
  }
}
