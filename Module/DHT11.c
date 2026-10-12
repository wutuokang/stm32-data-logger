#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "usart.h"

#define OUT 1
#define IN  0
#define DHT11_Status(x)  GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)(x))  

/*
	搭配OUT与IN使用
*/
void Dht11_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitSuructure;
	if(mode==1)
	{
		GPIO_InitSuructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitSuructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitSuructure.GPIO_Speed = GPIO_Speed_50MHz;
	}else
	{
		GPIO_InitSuructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitSuructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitSuructure.GPIO_Speed = GPIO_Speed_50MHz;
	}
	GPIO_Init(GPIOA,&GPIO_InitSuructure);
}



uint8_t Dht11_ReadBit(void)
{
	uint8_t i = 0;
	/* 等变低 */
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) && i < 100) { i++; Delay_us(1); }
	i = 0;
	/* 等变高 */
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) && i < 100) { i++; Delay_us(1); }

	Delay_us(40);                                  /* ← 固定延时 40µs */

	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)) return 1;       /* 再采样一次 */
	else return 0;
}


uint8_t Dht11_ReadByte(void)
{
	uint8_t i,data=0;
	for(i=0;i<8;i++)
	{
		data <<= 1;
		data |= Dht11_ReadBit();		
	}
	return data;
}

//主机对dht11发送起始信号
void Dht11_Startsignal(void)
{
	Dht11_Mode(OUT);
	DHT11_Status(0);
	Delay_ms(20);
	DHT11_Status(1);
	Delay_us(13);
}

//dht11回应主机
uint8_t Dht11_Response(void)
{
	uint8_t i=0;
	Dht11_Mode(IN);
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) && i <100)
	{
		i++;
		Delay_us(1);
	}
	if(i >= 100) return 1;
	else	
	{
		i=0;
		while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) && i <100)
		{
			i++;
			Delay_us(1);
		}
	}
	if(i >= 100) return 1;
	else return 0;
}



uint8_t Dht11_ReadData(uint8_t	*Temp_humi)
{
	uint8_t i;
	uint8_t data[5]={0};
	Dht11_Startsignal();
	if(Dht11_Response()==0)
	{
		for(i=0;i<5;i++)
		{
			data[i]=Dht11_ReadByte();
		}
	}else return 1;
//	Serial_Printf("dht的data[0]=%d，[1]=%d，[2]=%d，[3]=%d，[4]=%d\r\n",data[0],data[1],data[2],data[3],data[4]);

	if((data[0]+data[1]+data[2]+data[3])==data[4])
	{
		for(i=0;i<5;i++)
		{
			Temp_humi[i]=data[i];
		}
	}else return 1;
	
	return 0;
}


void Dht11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitSuructure;

	GPIO_InitSuructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitSuructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitSuructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitSuructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);    /* 初始化后输出高 */
}




