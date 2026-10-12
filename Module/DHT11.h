#ifndef __DHT11_H
#define __DHT11_H

/*
	搭配OUT与IN使用
*/
void Dht11_Mode(uint8_t mode);
uint8_t Dht11_ReadBit(void);
uint8_t Dht11_ReadByte(void);

//主机对dht11发送起始信号
void Dht11_Startsignal(void);

//dht11回应主机
uint8_t Dht11_Response(void);

uint8_t Dht11_ReadData(uint8_t	*Temp_humi);
void Dht11_Init(void);



#endif
