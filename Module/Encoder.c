#include "stm32f10x.h"                  // Device header
#include "Timer.h"
volatile int Count;
void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitSeructure;
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitSeructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitSeructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitSeructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);

	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	

}

int Encoder_Count(void)
{
	return Count;
}

void EXTI0_IRQHandler(void)
{
	static uint32_t last_tick;
	if(EXTI_GetITStatus(EXTI_Line0)==SET)
	{
		uint32_t now=g_ms;
		if(now-last_tick>=60)
		{
			last_tick=now;
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)Count++;
			else Count--;
		}
	
	EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

