#ifndef __MYSPI_H
#define __MYSPI_H

void MYSPI_Init(void);
uint8_t MYSPI_SwapByte(uint8_t SendByte);
void MYSPI_Start(void);
void MYSPI_Stop(void);
#endif
