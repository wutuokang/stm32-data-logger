#include "stm32f10x.h"                  // Device header
volatile uint32_t g_ms;
void TIM_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrcture;
	TIM_TimeBaseInitStrcture.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStrcture.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStrcture.TIM_Period=100-1;
	TIM_TimeBaseInitStrcture.TIM_Prescaler=720-1;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStrcture);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	
	NVIC_InitTypeDef NVIC_InitStrcture;
	NVIC_InitStrcture.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStrcture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrcture.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrcture.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStrcture);
	
	
	TIM_Cmd(TIM2,ENABLE);
	
}


void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){
		
		g_ms++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
