#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS			0XD0//ADDERSS：地址

//Write:写 		Reg:Register(寄存器)
void MPU6050_WriteReg(uint8_t RegADDRESS,uint8_t Data) //写寄存器   （指定地址写一个字节）
{
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();//这里可判断应答位做一些纠错算发，但这里不深究
	MyI2C_SendByte(RegADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

void MPU6050_WriteReg_len(uint8_t RegADDRESS,uint8_t* Data,uint8_t len) //写寄存器   （指定地址写多个字节）
{
	uint8_t i;
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegADDRESS);
	MyI2C_ReceiveAck();
	for(i=0;i<len;i++)
	{
		MyI2C_SendByte(Data[i]);
		MyI2C_ReceiveAck();	
	}
	MyI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegADDRESS)//读取一个字节
{
	uint8_t Data;
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegADDRESS);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS+1);
	MyI2C_ReceiveAck();
	Data=MyI2C_ReceiveByte();
	MyI2C_SendAck(1);
	MyI2C_Stop();
	
	return Data;
}


//逻辑就是完成接收时序图
void MPU6050_ReadReg_len(uint8_t RegADDRESS,uint8_t *Data,uint8_t len)
{
	uint8_t i;
	
		if(Data == 0 || len == 0)//无效数据则直接退出
	{
    return;
	}
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegADDRESS);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0X01);
	MyI2C_ReceiveAck();

	for(i=0;i<len;i++)
	{
		Data[i]=MyI2C_ReceiveByte();
		MyI2C_SendAck((i==len-1) ? 1:0);  //条件成立写1，不成立为0
	}

	MyI2C_Stop();
	
}

//好好看寄存器！！！	这里为什么写这些都要理解数据手册（MPU6050）
// MPU6050 初始化函数
// 功能：初始化I2C通信总线，依次配置电源、采样率、低通滤波、传感器量程等核心工作参数
void MPU6050_Init(void)
{
	MyI2C_Init();						// 初始化MCU的I2C外设，搭建与MPU6050的通信硬件基础
	
	// 电源管理寄存器1（地址 0x6B），写入 0x01
	// 二进制：0000 0001
	// Bit7 = 0：不触发芯片软复位
	// Bit6 = 0：关闭睡眠模式，唤醒芯片（MPU6050上电默认处于睡眠状态，必须清零才能正常采集数据）
	// Bit5 = 0：关闭循环低功耗采样模式，保持持续工作
	// Bit4 = 0：启用内部温度传感器
	// Bit2~Bit0 = 001：时钟源选择陀螺仪X轴PLL，精度远高于默认的内部8MHz晶振
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	
	// 电源管理寄存器2（地址 0x6C），写入 0x00
	// 功能：配置各轴待机、低功耗自动唤醒
	// 写入 0x00：加速度计、陀螺仪所有轴均正常工作，不进入待机；禁用低功耗自动唤醒功能
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	
	// 采样率分频寄存器（地址 0x19），写入 0x09
	// 采样率计算公式：采样率 = 陀螺仪基础输出频率 / (1 + 分频值)
	// 陀螺仪基础输出为1000Hz，分频值设为9 → 实际采样率 = 1000/(1+9) = 100Hz
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	
	// 配置寄存器（地址 0x1A），写入 0x06
	// 核心功能：设置数字低通滤波器(DLPF)的截止频率
	// 低三位 DLPF_CFG = 6 → 加速度计、陀螺仪的低通截止频率均为5Hz
	// 作用：滤除高频机械噪声，让采集到的原始数据更平滑
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
	
	// 陀螺仪配置寄存器（地址 0x1B），写入 0x18
	// 二进制：0001 1000
	// Bit4~Bit3（FS_SEL）= 11 → 陀螺仪满量程选择 ±2000°/s
	// 量程越大，可测量的角速度范围越大，但单数值对应的角度精度越低
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	
	// 加速度计配置寄存器（地址 0x1C），写入 0x18
	// 二进制：0001 1000
	// Bit4~Bit3（AFS_SEL）= 11 → 加速度计满量程选择 ±16g
	// g 为重力加速度单位，1g ≈ 9.8m/s²
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,		//Acc是加速度
									 int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ,		//这里是把各个寄存器的值读出来
										int16_t *Temp)
{
	
	uint8_t Data[14];
	MPU6050_ReadReg_len(0x3B,Data,14);
	*AccX=(Data[0]<<8)|Data[1];
	*AccY=(Data[2]<<8)|Data[3];
	*AccZ=(Data[4]<<8)|Data[5];
	*Temp=((Data[6]<<8)|Data[7]);
	*GyroX=(Data[8]<<8)|Data[9];
	*GyroY=(Data[10]<<8)|Data[11];
	*GyroZ=(Data[12]<<8)|Data[13];
//	uint8_t DataH,DataL;
//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
//	*AccX=DataL|(DataH<<8);

//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
//	*AccY=DataL|(DataH<<8);
//	
//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
//	*AccZ=DataL|(DataH<<8);
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
//	*GyroX=DataL|(DataH<<8);
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
//	*GyroY=DataL|(DataH<<8);
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
//	*GyroZ=DataL|(DataH<<8);
//		
}
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
