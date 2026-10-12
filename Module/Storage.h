#ifndef __STORAGE_H
#define __STORAGE_H

#define STORAGE_DATA_ADDR    0x000000      //数据区起始
#define STORAGE_DATA_END     0x7FEFFF      //数据区结束
#define STORAGE_META_ADDR    0x7FF000      //元数据扇区
#define SECTOR_SIZE          4096
#define PAGE_SIZE            256

//记录存入用的结构体
typedef struct {
	uint8_t  guard;
	uint8_t  dht_temp;
	uint8_t  dht_humi;
	uint8_t  fan_speed;
	uint16_t adc0;
	uint16_t adc1;
	int16_t  temp_mpu;
	uint32_t tick;
} Record_t;


typedef struct {
	uint32_t magic;//判断这个芯片是否写过，写过的话就找上次写的地址，否则从头开始写
	uint32_t w_addr;//上次断电时的写指针地址
	uint16_t count;//记录的条数
	uint16_t checksum;//校验数据可靠性
} Meta_t;


void Storage_Write(Record_t *Rec);
//n代表读的位置
uint8_t Storage_Read(uint16_t n, Record_t *Rec);
uint32_t Storage_Count(void);
uint32_t Storage_Addr(void);
//存储数据到元数据区域
void Storage_SaveMeta(void);
//读取上次断电后的元数据，初始化为这次的初始数据
void Storage_Init(void);
#endif
