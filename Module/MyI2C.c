#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void MyI2C_SCL_W(uint8_t x)//时钟线写
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,(BitAction) x);
	Delay_us(10);//这个延时可有可无 保险起见加上
}
void MyI2C_SDA_W(uint8_t x)//数据线写
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,(BitAction) x);
	Delay_us(10);//I2C多慢都可以 但是快的话要注意手册的时序要求，所以有些主板芯片这里需要加延时
}

uint8_t MyI2C_SDA_R(void)
{
	uint8_t BitValue;  //用作表示返回的电平
	BitValue=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7);
	Delay_us(10);
	return BitValue;
	//函数	                           功能	                                        	返回值	                	核心特点
//GPIO_ReadInputData	           读取整个端口的实际引脚电平（读 IDR 寄存器）	    16 位整数	                批量读取，反映真实物理电平，输出模式下也可回读校验
//GPIO_ReadInputDataBit	    	   读取单个引脚的实际电平（读 IDR 的某一位）	    Bit_SET / Bit_RESET			单引脚读取，灵活直观
//GPIO_ReadOutputData			   读取整个端口的输出设定值（读 ODR 寄存器）	    16 位整数					读取的是软件写入的输出状态，而非引脚真实电平
//GPIO_ReadOutputDataBit	   	   读取单个引脚的输出设定值							Bit_SET / Bit_RESET	  	    单引脚读取输出配置值
}

void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;			
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7; //这里PA6是数据线，PA7是时钟线
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//需要默认至高电平，释放总线
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);//（此时高阻态因上拉电阻所以是高电平I2c总线此时处于空闲状态）
	
}

void MyI2C_Start(void)//开始（起始条件）
{
	MyI2C_SDA_W(1);//最好先拉高SDA 因为这里SCL为高电平时SDA产生上升沿会被误判为终止信号
	MyI2C_SCL_W(1);//确保SDA为高电平后再拉高SCL
	MyI2C_SDA_W(0);//触发起始条件
	MyI2C_SCL_W(0);//数据在SCL为低电平时才能传输，所以为数据传输做准备拉低SCL
}

void MyI2C_Stop(void)//终止条件
{	
	MyI2C_SDA_W(0);//为了确保输出上升沿，先拉低数据线
	MyI2C_SCL_W(1);//随后让SDA在SCL高电平期间产生上升沿触发起始条件
	MyI2C_SDA_W(1);
}

void MyI2C_SendByte(uint8_t Byte)//发送
{
	uint8_t i;
	for(i=0;i<8;i++)	//发送8个Bit，一个字节
	{
		MyI2C_SDA_W(Byte & (0x80>>i));	//在这个函数的判断条件中，参数是非0即1的所以这样写没有问题
		MyI2C_SCL_W(1);
		MyI2C_SCL_W(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)//收到
{
	uint8_t i,Byte=0x00;//默认未收到数据的情况下此位为0
	MyI2C_SDA_W(1);//主机这里要把数据线释放交给从机		
	for(i=0;i<8;i++)
	{
		MyI2C_SCL_W(1);//这里已经把SDA控制权交给从机了，所以直接拉高SCL读取数据
		if(MyI2C_SDA_R()==1){Byte |= (0x80>>i);}//每或一位去改变下一位
		MyI2C_SCL_W(0);//时钟线低电平时从机才可以改变数据 
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)//发送应答 Ack:Acknowledge (确认、承认、在这里表示已收到)
{
	MyI2C_SDA_W(AckBit);//在这里SCL为低电平时应答，主机将AckBit放在SDA上，非0即1则表示应答
	MyI2C_SCL_W(1);	//从机读取应答
	MyI2C_SCL_W(0); //进入下一个时序单元
}

uint8_t MyI2C_ReceiveAck(void)//接收应答
{
	uint8_t AckBit;
	MyI2C_SDA_W(1);//主机释放SDA防止干扰从机  这里释放后，从机会将应答位放在SDA上	
	MyI2C_SCL_W(1);
	AckBit=MyI2C_SDA_R();//拉高SCL读取应答位
	MyI2C_SCL_W(0);//进入下一个时序单元 	
	return AckBit;
}
