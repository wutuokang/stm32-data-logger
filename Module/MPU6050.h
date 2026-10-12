#ifndef __MPU6050_H
#define __MPU6050_H

void MPU6050_WriteReg(uint8_t RegADDRESS,uint8_t Data);//发送一个字节
void MPU6050_WriteReg_len(uint8_t RegADDRESS,uint8_t* Data,uint8_t len);//发送len个字节
uint8_t MPU6050_ReadReg(uint8_t RegADDRESS);  //接收一个字节，会返回一个字节的数据
//逻辑就是完成接收时序图与发送时序图
void MPU6050_ReadReg_len(uint8_t RegADDRESS,uint8_t *Data,uint8_t len); //接收len个字节，将len个字节传入*Data这个数组
uint8_t MPU6050_GetID(void);
void MPU6050_Init(void);

void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,		//Acc是加速度
									 int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ,		//这里是把各个寄存器的值读出来
										int16_t *Temp);
#endif
