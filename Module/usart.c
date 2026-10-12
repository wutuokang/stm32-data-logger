#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>



uint8_t RXdata;
uint8_t RXflag;

void MyUSART_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    USART_InitTypeDef  USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);
    
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
	
    USART_Cmd(USART2, ENABLE);
}

void serial_SendByte(uint8_t byte)
{
    USART_SendData(USART2, byte);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
}

void serial_SendArray(uint8_t *Array, uint16_t len)
{
    uint16_t i = 0;
    for(i = 0; i < len; i++)
    {
        serial_SendByte(Array[i]);
    }
}

void serial_SendString(char *String)
{
    uint8_t i;
    for(i = 0; String[i] != '\0'; i++)
    {
        serial_SendByte(String[i]);
    }
}

uint32_t serial_Pow(uint32_t x, uint32_t y)
{
    uint32_t i = 1;
    while(y--)
    {
        i *= x;
    }
    return i;
}

void serial_SendNumber(uint32_t Number, uint8_t len)
{
    uint8_t i = 0;
    for(i = 0; i < len; i++)
    {
        serial_SendByte(Number / serial_Pow(10, len-i-1) % 10 + '0');
    }
}


int fputc(int ch, FILE *f)
{
    serial_SendByte(ch);
    return ch;
}

void Serial_Printf(char *format, ...)
{
    char String[200];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if(RXflag==1)
	{
		RXflag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxdata(void)
{
	return RXdata;
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		RXdata=USART_ReceiveData(USART2);
		RXflag=1;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}
