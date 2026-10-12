#include "stm32f10x.h"                  // Device header

#define MySPI_W_CS(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)(x))
#define MySPI_W_CLK(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define MySPI_W_MOSI(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(x))

uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
}

void MYSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_13;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	MySPI_W_CS(1);//不选中任何片选
	MySPI_W_CLK(0);//根据你选的spi模式自己定
}

//起始信号单纯拉低片选
void MYSPI_Start(void)
{
	MySPI_W_CS(0);
}
//终止信号单纯拉高片选
void MYSPI_Stop(void)
{
	MySPI_W_CS(1);
}



//交换一个字节
uint8_t MYSPI_SwapByte(uint8_t SendByte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
	MySPI_W_MOSI(SendByte & 0x80);
	SendByte <<= 1;
	MySPI_W_CLK(1);
	if (MySPI_R_MISO()==1){SendByte|= 0x01;}
	MySPI_W_CLK(0);
	}
	return SendByte;
}
