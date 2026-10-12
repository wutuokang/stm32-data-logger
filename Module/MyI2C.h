#ifndef __MYI2C_H
#define __MYI2C_H

void MyI2C_Init(void);
void MyI2C_Start(void);//开始（起始条件）
void MyI2C_Stop(void);//终止条件
void MyI2C_SendByte(uint8_t Byte);//发送
uint8_t MyI2C_ReceiveByte(void);//收到
void MyI2C_SendAck(uint8_t AckBit);//发送应答 Ack:Acknowledge (确认、承认、在这里表示已收到)
uint8_t MyI2C_ReceiveAck(void);//接收应答

#endif
