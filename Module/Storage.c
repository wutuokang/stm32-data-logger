#include "stm32f10x.h"                  // Device header
#include "W25Q64.h"
#include "Storage.h"

static uint32_t w_addr    = STORAGE_DATA_ADDR;     //写指针
static uint32_t rec_count = 0;                     //已记录条数

void Storage_Write(Record_t *Rec)
{
	//如果当前地址剩余量不足以支持下一次数据写入
	if((w_addr % PAGE_SIZE)+sizeof(Record_t) > PAGE_SIZE)
	{
		//就计算当前是第几页，并且跳到下一页
		w_addr =( w_addr / PAGE_SIZE + 1) * PAGE_SIZE;
	}
	
	//当前是新的一个扇区的话就先擦除
	if(w_addr % SECTOR_SIZE == 0)
	{
		Storage_SaveMeta();
		W25Q64_Erase(w_addr);
	}
	
	Rec->guard = 0xff;//判断是否写完的标志位
	W25Q64_PageProgram(w_addr,(uint8_t *)Rec,sizeof(Record_t));
	Rec->guard = 0xaa;
	W25Q64_PageProgram(w_addr,&Rec->guard, 1);
	
	w_addr += sizeof(Record_t);  //写指针前进
	rec_count++;//写入次数增加
	
	if (w_addr > STORAGE_DATA_END)//应该用不上，写完后回到首地址覆盖旧地址
	{
		w_addr = STORAGE_DATA_ADDR;
	}
}



//n代表读的位置
uint8_t Storage_Read(uint16_t n, Record_t *Rec)
{
	//计算这次读的位置
	uint32_t addr = STORAGE_DATA_ADDR + (uint32_t)n * sizeof(Record_t);

	W25Q64_ReadData(addr, (uint8_t *)Rec, sizeof(Record_t));
	//如果读到未完全存入的数据则返回相应的信号
	if (Rec->guard != 0xAA) return 1;
	return 0;
}

uint32_t Storage_Count(void)
{
	return rec_count;
}

uint32_t Storage_Addr(void)
{
	return w_addr;
}

//存储数据到元数据区域
void Storage_SaveMeta(void)
{
	Meta_t meta;

	meta.magic    = 0xA5A5A5A5;  //随便写的一个数据，用法见Storage.h
	meta.w_addr   = w_addr;			 //现在写到的地址
	meta.count    = rec_count;   //现在写了多少条
	meta.checksum = meta.magic ^ meta.w_addr ^ meta.count;   //校验方法

	W25Q64_Erase(STORAGE_META_ADDR);
	W25Q64_PageProgram(STORAGE_META_ADDR,(uint8_t *)&meta,sizeof(Meta_t));
}



//读取上次断电后的元数据，初始化为这次的初始数据
void Storage_Init(void)
{
	W25Q64_Init();
	Meta_t meta;
	uint16_t chk;

	W25Q64_ReadData(STORAGE_META_ADDR, (uint8_t *)&meta, sizeof(Meta_t));

	chk = meta.magic ^ meta.w_addr ^ meta.count;

	if (meta.magic == 0xA5A5A5A5 && chk == meta.checksum)  //校验
	{
		w_addr    = meta.w_addr;
		rec_count = meta.count;
	}
	else		//校验失败的话，为了安全，从头写，并清空记录条数
	{
		w_addr    = STORAGE_DATA_ADDR;
		rec_count = 0;
	}
}


