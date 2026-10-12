#include "stm32f10x.h"                  // Device header

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitSeructure;
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitSeructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitSeructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitSeructure);
}

void Buzzer_ON(void)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_SET);
}

void Buzzer_OFF(void)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_RESET);
}
