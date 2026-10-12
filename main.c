#include "stm32f10x.h"    
#include "Delay.h"   // Device header
#include "OLED.h"
#include "usart.h"
#include "MPU6050.h"
#include "AD.h"
#include "Timer.h"
#include "FAN.h"
#include "W25Q64.h"
#include "DHT11.h"
#include "Button.h"
#include "led.h"
#include "Buzzer.h"
#include "Storage.h"

#define EXPORT_MODE    0        //！！！！运行模式，0代表正常运行，1代表读取数据！！！！//
#define LOG_MODE       1        //！！！！运行模式，0代表波形（VOFA+），1代表文字日志！！！！//
/*
*
*		ADC
*
*/
#define Filte 10
uint16_t adc0_Filte[Filte] = {0},adc1_Filte[Filte] = {0};//滤波后的原始数据
uint8_t adc_idx[2] = {0};	//写指针
uint16_t adc0,adc1;
uint16_t adc0_raw,adc1_raw;
float v0,v1;
//服务于串口打印的adc滤波前后波动
uint16_t r0_max = 0, r0_min = 4095;      //热敏：原始值的最大/最小
uint16_t f0_max = 0, f0_min = 4095;      //热敏：滤波后的最大/最小
uint16_t r1_max = 0, r1_min = 4095;      //光敏：原始
uint16_t f1_max = 0, f1_min = 4095;      //光敏：滤波后
uint8_t fan_adc_Temp;		//服务于后续的Task_fan
//调用adc，每5ms执行一次，


uint16_t Task_adc_Filte(uint16_t *adc_Filte,uint8_t idx,uint16_t adc_arw)
{
	uint32_t sum=0;
	uint8_t  i;
	adc_Filte[adc_idx[idx]]=adc_arw;
	adc_idx[idx]++;
	if(adc_idx[idx] >= Filte)adc_idx[idx] = 0;
	for(i = 0;i < Filte;i++)
	{
		sum = sum + adc_Filte[i];
	}
	return sum/Filte;
}

void Task_adc(void)
{
	adc0_raw=AD_Value[0];
	adc1_raw=AD_Value[1];
	
	adc0=Task_adc_Filte(adc0_Filte,0,AD_Value[0]);
	adc1=Task_adc_Filte(adc1_Filte,1,AD_Value[1]);

	if(adc0_raw > r0_max) r0_max = adc0_raw;
	if(adc0_raw < r0_min) r0_min = adc0_raw;
	if(adc0     > f0_max) f0_max = adc0;
	if(adc0     < f0_min) f0_min = adc0;

	if(adc1_raw > r1_max) r1_max = adc1_raw;
	if(adc1_raw < r1_min) r1_min = adc1_raw;
	if(adc1     > f1_max) f1_max = adc1;
	if(adc1     < f1_min) f1_min = adc1;
	
	v0=(float)adc0/4096*3.3;
	v1=(float)adc1/4096*3.3;
	if(adc0 < 1900) fan_adc_Temp = 25;
	else fan_adc_Temp = 0;
	
}

void Task_adc_oled(void)
{
	OLED_ShowString(1,1," Temp_Raw:");
	OLED_ShowString(2,1,"light_Raw:");
	OLED_ShowString(3,1," Temp_v:");
	OLED_ShowString(4,1,"light_v:");
	OLED_ShowString(3,14,"V");
	OLED_ShowString(4,14,"V");
	OLED_ShowNum(1,11,adc0,4);
	OLED_ShowNum(2,11,adc1,4);
	OLED_ShowSignedFloat(3,9,v0,1,2);
	OLED_ShowSignedFloat(4,9,v1,1,2);
}	


/*
*
*		补光led
*
*/

void Task_led(void)
{	
	uint8_t i;
	for(i=0;i<4;i++)
	{
		if(adc1 > (i + 1) * 1000) ON_led_PA_1234(i);
		else OFF_led_PA_1234(i); 	
	}
}


/*
*
*		DHT11
*
*/
uint8_t fan_speed;
uint8_t data[5]={0};
uint8_t dht11_Status = 1;
uint8_t fan_dht11_Temp,fan_dht11_humi;		//服务于后续的Task_fan
void Task_dht11(void)
{
	//这里因为dht11是读取数据时不能被打断，所以关闭中断，并且dht11性质特殊，所以底层逻辑使用延时的阻塞方式
	__disable_irq();
	dht11_Status=Dht11_ReadData(data);
	__enable_irq();
	if(data[0]>70) fan_dht11_humi = 25;
	else fan_dht11_humi = 0;
	if(data[2]>30) fan_dht11_Temp = 25;
	else fan_dht11_Temp = 0;
}

void Task_dht11_oled(void)
{
		OLED_ShowString(1,1,"Temp:");
		OLED_ShowString(2,1,"humi:");
		OLED_ShowString(3,1,"FanStatus:");
		OLED_ShowString(4,1,"Sbeed:");
	if(dht11_Status==0)
	{
		OLED_ShowNum(1,6,data[2],2);
		OLED_ShowNum(2,6,data[0],2);
	}
	else
	{
		OLED_ShowString(1,6,"--");
		OLED_ShowString(2,6,"--");
	}
	if(fan_speed)
	{
		OLED_ShowString(3,11,"ON ");
		OLED_ShowNum(4,7,fan_speed,3);
	}
	else
	{
		OLED_ShowString(3,11,"OFF");
		OLED_ShowString(4,7,"0  ");
	}
}




/*
*
*		MPU6050
*
*/
uint8_t ID_mpu6050;
int16_t AccX,AccY,AccZ,				//三轴加速度
				GyroX,GyroY,GyroZ,		//三轴角速度
				Temp_mpu_Raw;							//mpu6050内部温度
float accx_g, accy_g, accz_g;              
float gyrox_dps, gyroy_dps, gyroz_dps;     
float Temp_mpu;
void Task_mpu6050(void)
{
	MPU6050_GetData(&AccX,&AccY,&AccZ,		
									&GyroX,&GyroY,&GyroZ,		
									&Temp_mpu_Raw);
	Temp_mpu = Temp_mpu_Raw / 340.0f + 36.53f;
	accx_g =  AccX /2048.0f;
	accy_g =  AccY /2048.0f; 
	accz_g =  AccZ /2048.0f;
	gyrox_dps = GyroX / 16.4f;
	gyroy_dps = GyroY / 16.4f;
	gyroz_dps = GyroZ / 16.4f;
}

void Task_mpu6050_oled(void)
{
	OLED_ShowString(1,1,"ID:");
	OLED_ShowHexNum(1,4,ID_mpu6050,2);
	OLED_ShowString(1,7,"T:");
	OLED_ShowSignedFloat(1,9,Temp_mpu,2,1);

	OLED_ShowString(2,1,"AX");
	OLED_ShowSignedFloat(2,3,accx_g,1,2);
	OLED_ShowString(2,8,"g");
	OLED_ShowString(2,9,"GX");
	OLED_ShowSignedFloat(2,11,gyrox_dps,1,1);

	OLED_ShowString(3,1,"AY");
	OLED_ShowSignedFloat(3,3,accy_g,1,2);
	OLED_ShowString(3,8,"g");
	OLED_ShowString(3,9,"GY");
	OLED_ShowSignedFloat(3,11,gyroy_dps,1,1);

	OLED_ShowString(4,1,"AZ");
	OLED_ShowSignedFloat(4,3,accz_g,1,2);
	OLED_ShowString(4,8,"g");
	OLED_ShowString(4,9,"GZ");
	OLED_ShowSignedFloat(4,11,gyroz_dps,1,1);
}

/*
*
*		蜂鸣器，基于mpu6050内部温度报警
*
*/
void Task_Buzzer(void)
{
	uint16_t temp = (uint16_t)(Temp_mpu * 10);
	static uint32_t Buzzer_now = 0;
	if(temp > 300)
	{
		if((g_ms - Buzzer_now) % 10000 < 1000)  Buzzer_ON();
		else                                    Buzzer_OFF();
//		if(g_ms - Buzzer_now < 1000)	Buzzer_ON();
//		else	Buzzer_OFF();
//		if(g_ms - Buzzer_now >= 10000)		Buzzer_now = g_ms;
	}
	else 
	{
		Buzzer_OFF();
		Buzzer_now = g_ms;
	}
}


/*
*
*		按键BUTTON
*
*/
uint8_t page,page_Status;

void Task_Button(void)
{
	if( Button_Scan_Switch() )
	{
		if(page_Status == 0)
		{
			page++;
			if(page >= 4) page = 0;
		}
		
	}
	if( Button_Scan_Choose() )
	{
		page_Status = !page_Status;
	}
}



/*
*
*		W25Q64 存储
*
*/
Record_t rec;
void Task_Storage(void)
{
	rec.guard     = 0xAA;                 /* 会被 Storage_Write 覆盖，写这里只为完整 */
	rec.dht_temp  = data[2];              //DHT11温度
	rec.dht_humi  = data[0];              //DHT11湿度
	rec.fan_speed = fan_speed;            //风扇转速
	rec.adc0      = adc0;                 //热敏（滤波后）
	rec.adc1      = adc1;                 //光敏（滤波后）
	rec.temp_mpu  = (int16_t)(Temp_mpu * 10);   //MPU温度×10（防止小数丢失，其最后一位为小数）
	rec.tick      = g_ms;                 //记录时刻

	Storage_Write(&rec);
}




/*
*
*		FAN(风扇，用tb6612驱动)
*
*/

void Task_fan(void)
{
	fan_speed=fan_adc_Temp+fan_dht11_humi+fan_dht11_Temp;
	FAN_CCR4(fan_speed);
}



/*
*
*		系统信息页
*
*/
void Task_system_oled(void)
{
	OLED_ShowString(1,1,"SYSTEM");
	OLED_ShowString(2,1,"TIM:");
	OLED_ShowNum(2,5,g_ms / 1000 /3600,2);
	OLED_ShowString(2,7,"h");
	OLED_ShowNum(2,8,g_ms /1000 % 3600 /60,2);
	OLED_ShowString(2,10,"min");
	OLED_ShowNum(2,13,g_ms  / 1000 % 60,2);
	OLED_ShowString(2,15,"s");
	OLED_ShowString(3,1,"rec:");
	OLED_ShowNum(3,5,Storage_Count(),6);
	OLED_ShowString(4,1,"adr:");
	OLED_ShowHexNum(4,5,Storage_Addr(),6);
}



/*
*
*		菜单页
*
*/


void Task_menu_oled(void)
{
	uint8_t i;
	char *name[4]={"1.ENV","2.CLIMATE","3.MOTION","4.SYSTEM"};
	for(i=0;i<4;i++)
	{
		if(i == page)
		{
			OLED_ShowString(i+1,1,">");
			OLED_ShowString(i+1,2,name[i]);
		}
		else 
		{
			OLED_ShowString(i+1,1," ");
			OLED_ShowString(i+1,2,name[i]);
		}
	}
}





/*
*
*		OLED显示切换，各个外设产生的oled界面在这里进行切换显示
Task_adc_oled、Task_dht11_oled、Task_mpu6050_oled、Task_system_oled这些函数在当前模块中，自己的区域
*
*/
//uint8_t page=1; 

void Task_Display(void)
{
	static uint8_t last_page=255;
	static uint8_t last_page_Status=255;
	if(page != last_page || page_Status != last_page_Status)
	{
		OLED_Clear();
		last_page = page;
		last_page_Status = page_Status;
	}
	if(page_Status)
	{
		switch(page)
		{
			case 0:{Task_adc_oled();}break;
			case 1:{Task_dht11_oled();}break;
			case 2:{Task_mpu6050_oled();}break;
			case 3:{Task_system_oled();}break;
		}
	}
	else 
	{
		Task_menu_oled();
	}
}


void Task_log(void)
{
#if LOG_MODE
	Serial_Printf("REC=%d\r\n", Storage_Count());
	Serial_Printf("ADR=%d\r\n", Storage_Addr());
	Serial_Printf("热敏 原始:%d-%d=%d 滤波:%d-%d=%d\r\n",
	              r0_max, r0_min, r0_max - r0_min,
	              f0_max, f0_min, f0_max - f0_min);
	Serial_Printf("光敏 原始:%d-%d=%d 滤波:%d-%d=%d\r\n",
	              r1_max, r1_min, r1_max - r1_min,
	              f1_max, f1_min, f1_max - f1_min);
#else
	
	Serial_Printf("%d,%d,%d,%d,%d,%d\n",
	              adc0, adc1, data[2], data[0],
	              (int)Temp_mpu, fan_speed);
#endif

	r0_max = 0; r0_min = 4095;
	f0_max = 0; f0_min = 4095;
	r1_max = 0; r1_min = 4095;
	f1_max = 0; f1_min = 4095;
}
uint8_t MID;
uint16_t DID;
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM_Init();
	AD_Init();
	led_Init();
	OLED_Init();
	MyUSART_Init();
	MPU6050_Init();
		ID_mpu6050=MPU6050_GetID();
	FAN_Init();
		FAN_Motor_Status(1);
	Dht11_Init();
	Button_Init();
	Storage_Init();
	Buzzer_Init();
	
	uint32_t tAdc = 0, tOled = 0, tLog = 0, tDht11 = 0, tMpu6050 = 0, tButton = 0, tFan = 0,
			 tLed = 0, tBuzzer = 0, tStorage =  0, tMeta = 0;
		
	
	
#if EXPORT_MODE

	/* ========== 导出模式 ========== */
	{
		Record_t r;
		uint32_t i, total;

		total = Storage_Count();

		Serial_Printf("total=%d\r\n", total);
		Serial_Printf("tick,adc0,adc1,dht_temp,dht_humi,temp_mpu,fan_speed\r\n");

		for (i = 0; i < total; i++)
		{
			if (Storage_Read(i, &r) == 0)
			{
				Serial_Printf("%d,%d,%d,%d,%d,%d,%d\r\n",
				              r.tick, r.adc0, r.adc1,
				              r.dht_temp, r.dht_humi,
				              r.temp_mpu, r.fan_speed);
			}
		}

		Serial_Printf("EXPORT DONE\r\n");
		while(1);                      /* 停住 */
	}

#else


	
	while(1)
	{
		uint32_t now = g_ms;
		if(now - tAdc >= 5)
		{
			tAdc = now;
			Task_adc();
		}
		if(now - tButton >= 20)
		{
			tButton = now;
			Task_Button();
		}
			if(now - tBuzzer >= 25)
		{
			tBuzzer = now;
			Task_Buzzer();
		}
		if(now - tMpu6050 >= 50)
		{
			tMpu6050 = now;
			Task_mpu6050();
		}
		if(now - tOled >= 100)
		{
			tOled = now;
			Task_Display();
		}
		if(now - tLed >= 200)
		{
			tLed = now;
			Task_led();
		}
		if(now - tFan >= 200)
		{
			tFan = now;
			Task_fan();
		}
		if(now - tStorage >= 500)
		{
			tStorage = now;
			Task_Storage();
		}
		if(now - tDht11 >= 2000)
		{
			tDht11 = now;
			Task_dht11();
		}
		#if LOG_MODE
		if(now - tLog >= 10000)
		#else
		if(now - tLog >= 100)
		#endif
		{
			tLog = now;
			Task_log();
		}
		if(now - tMeta >= 60000)//这里说明，保存元数据只有在写完一个扇区后，调用一次，所以，这里每60s额外保存一次
		{
			tMeta = now;
			Storage_SaveMeta();
		}
		
	}
	#endif 
}
