#ifndef __USART_H
#define __USART_H

void MyUSART_Init(void);
void serial_SendByte(uint8_t byte);
void serial_SendArray(uint8_t *Array,uint16_t len);
void serial_SendString(char *String);
void serial_SendNumber(uint32_t Number,uint8_t len);
void Serial_Printf(char *format,...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxdata(void);

#endif
