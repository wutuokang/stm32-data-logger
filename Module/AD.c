#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[2];

//配置了时钟、GPIO、ADC通道、ADC结构体、（中断和看门狗可以配置）ADC上电开始复位校准、开始校准
void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);
	//ADC工作于独立模式，右对齐、用软件作为触发源不使用外部触发源，单次转换的非扫描模式
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;  								 //模式只选用这一种，其他的都是双ADC需要配置的
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;							 //选择数据对齐方式，选择右对齐
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;  //选择外部触发源，这里选择不使用外部触发
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;									 //连续转换模式，选择单次转换还是连续转换
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;												 //扫描转换，选择扫描转换模式或是非扫描转换模式
	ADC_InitStructure.ADC_NbrOfChannel=2;															 //通道数目，对应在扫描模式下的通道数目
//	(这里注意转换和扫描的搭配，根据数据手册与规则后续代码需做调整)
	ADC_Init(ADC1,&ADC_InitStructure);
	


	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_BufferSize=2; 																		 //传输计数器的值(0~65535)（传输的次数）
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;                           //外设站点作为SRC（起点）还是DST（终点）
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;																	 //是否是软件触发（寄存器转运到寄存器） 
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)AD_Value;          											 //存储器站点的首地址
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;					 		 //存储器站点指定数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					 	  	     //存储器点地址是否自增
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;															 //是否自动重装 自动重装和软件触发不能同时使用 不然DMA会停不下来)
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; 												 //外设站点的首地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		 //外设站点指定数据宽度（这里是uint16_t的长度）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						 //外设站点地址是否自增
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	 										 //所选通道的软件优先级
	
	//这里注意！！！看看要转运的数据在DMA的哪个通道上面就用哪个通道
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);			
	
	DMA_Cmd(DMA1_Channel1,ENABLE);																						
	
	//这里应该还有中断和看门狗可以配置，但这个单纯的测电压不需要
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);												//复位校准（软件置标志位为1）
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);	//等待校准完成（标志位为1随后开始校准，结束后硬件使标志位为0）
	
	ADC_StartCalibration(ADC1);												//开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);				//等待校准完成（原理同上）

//`Software`：软件 `Start`：启动 `Conv` = Conversion，缩写，转换  这里是触发ADC的作用
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	
	 
}



