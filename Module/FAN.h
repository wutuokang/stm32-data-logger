#ifndef		__FAN_H
#define		__FAN_H

//这个函数写0或1时对应电机正反转，2或3代表停转或对峙
void FAN_Motor_Status(uint8_t x);
void FAN_Init(void);
void FAN_CCR4(uint8_t i);


#endif
