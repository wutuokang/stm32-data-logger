#include "stm32f10x.h"                  // Device header
#include "Timer.h"
void Button_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_InitTypeDef GPIO_InitSeructure;
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitSeructure.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitSeructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitSeructure);
}

uint8_t Button_Scan_Switch(void)
{
	static uint8_t last=1;             

	uint8_t now = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);

	if(last==1 && now==0)
	{
		last=now;
		return 1;
	}

	last=now;
	return 0;
}

uint8_t Button_Scan_Choose(void)
{
	static uint8_t last=1;             

	uint8_t now = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);

	if(last==1 && now==0)
	{
		last=now;
		return 1;
	}

	last=now;
	return 0;
}

