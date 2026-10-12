#include "stm32f10x.h"                  // Device header

#define pwm GPIO_Pin_9
#define AIN1 GPIO_Pin_10
#define AIN2 GPIO_Pin_11

//这个函数写0或1时对应电机正反转，2或3代表停转或对峙
void FAN_Motor_Status(uint8_t x)
{
	switch(x)
	{
		case 0:{GPIO_WriteBit(GPIOB,AIN1,Bit_SET);GPIO_WriteBit(GPIOB,AIN2,Bit_RESET);};break;
		case 1:{GPIO_WriteBit(GPIOB,AIN1,Bit_RESET);GPIO_WriteBit(GPIOB,AIN2,Bit_SET);};break;
		case 2:{GPIO_WriteBit(GPIOB,AIN1,Bit_RESET);GPIO_WriteBit(GPIOB,AIN2,Bit_RESET);};break;
		case 3:{GPIO_WriteBit(GPIOB,AIN1,Bit_SET);GPIO_WriteBit(GPIOB,AIN2,Bit_SET);};break;

	}
}

void FAN_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitSeructure;
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitSeructure.GPIO_Pin=AIN1|AIN2;
	GPIO_InitSeructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitSeructure);
	GPIO_InitSeructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitSeructure.GPIO_Pin=pwm;
	GPIO_Init(GPIOB,&GPIO_InitSeructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrcture;
	TIM_TimeBaseInitStrcture.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStrcture.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStrcture.TIM_Period = 100-1;
	TIM_TimeBaseInitStrcture.TIM_Prescaler = 720-1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStrcture);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	
	
	TIM_Cmd(TIM4,ENABLE);
}

void FAN_CCR4(uint8_t i)
{
	TIM_SetCompare4(TIM4,i);
}
