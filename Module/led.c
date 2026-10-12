#include "stm32f10x.h"                  // Device header

void led_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitSeructure;
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitSeructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitSeructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitSeructure);
}

//因为项目限制这个函数只能开启pin4、5、6、7(i范围0~3)
void ON_led_PA_1234(uint8_t i)
{
	GPIOA->BRR = 0x0010 << i;
}

void OFF_led_PA_1234(uint8_t i)
{
	GPIOA->BSRR = 0x0010 << i;
}
