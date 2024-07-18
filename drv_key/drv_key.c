#include "drv_key.h"
#include "button.h"

/**
  ******************************************************************
                                                       变量声明
  ******************************************************************
  */

Button_t Button1;
Button_t Button2;

/**
  ******************************************************************
                                                        函数声明
  ******************************************************************
  */

void Btn1_Dowm_CallBack(void *btn)
{
    print_usart1("Button1 one check\r\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,GPIO_PIN_SET);
}

void Btn1_Double_CallBack(void *btn)
{
    print_usart1("Button1 双击!");
}

void Btn1_Long_CallBack(void *btn)
{
    print_usart1("Button1 长按!");
}

void Btn1_Continuos_CallBack(void *btn)
{
    print_usart1("Button1 连按!");
}
void Btn1_ContinuosFree_CallBack(void *btn)
{
    print_usart1("Button1 连按释放!");
}

void Btn2_Dowm_CallBack(void *btn)
{
    print_usart1("Button2 DJ!");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_SET);
}

void Btn2_Double_CallBack(void *btn)
{
    print_usart1("Button2 双击!");
}

void Btn2_Long_CallBack(void *btn)
{
    print_usart1("Button2 长按!");
}

void Btn2_Continuos_CallBack(void *btn)
{
    print_usart1("Button2 连按!");
}
void Btn2_ContinuosFree_CallBack(void *btn)
{
    print_usart1("Button2 连按释放!");
}
/* 按键电平检测 */
uint8_t Read_Button1_Level(void)
{
    return HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN);
}

uint8_t Read_Button2_Level(void)
{
    return HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN);
}

void Key_Init(void)
{

    Button_Create("Button1",
                  &Button1,
                  Read_Button1_Level,
                  1);
    Button_Attach(&Button1, BUTTON_DOWM, Btn1_Dowm_CallBack);                    // 单击
    Button_Attach(&Button1, BUTTON_DOUBLE, Btn1_Double_CallBack);                // 双击
    Button_Attach(&Button1, BUTTON_CONTINUOS, Btn1_Continuos_CallBack);          // 连按
    Button_Attach(&Button1, BUTTON_CONTINUOS_FREE, Btn1_ContinuosFree_CallBack); // 连按释放
    Button_Attach(&Button1, BUTTON_LONG, Btn1_Long_CallBack);                    // 长按

    Button_Create("Button2",
                  &Button2,
                  Read_Button2_Level,
                  1);
    Button_Attach(&Button2, BUTTON_DOWM, Btn2_Dowm_CallBack);                    // 单击
    Button_Attach(&Button2, BUTTON_DOUBLE, Btn2_Double_CallBack);                // 双击
    Button_Attach(&Button2, BUTTON_CONTINUOS, Btn2_Continuos_CallBack);          // 连按
    Button_Attach(&Button2, BUTTON_CONTINUOS_FREE, Btn2_ContinuosFree_CallBack); // 连按释放
    Button_Attach(&Button2, BUTTON_LONG, Btn2_Long_CallBack);                    // 长按

    Get_Button_Event(&Button1);
    Get_Button_Event(&Button2);
}

/********************************END OF FILE***************************************/
