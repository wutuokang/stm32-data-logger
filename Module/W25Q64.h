#ifndef __W25Q64_H
#define __W25Q64_H

	void W25Q64_Init(void);
	/*
	*
	*	手册对应指令
	*/
	void W25Q64_ReadID(uint8_t *MID,uint16_t *DID);
	//写使能
	void W25Q64_WriteEnable(void);
	//页编程
	void W25Q64_PageProgram(uint32_t Address,uint8_t *Data,uint16_t len);
	//扇区擦除
	void W25Q64_Erase(uint32_t Address);
	//读取
	void W25Q64_ReadData(uint32_t Address,uint8_t *Data,uint32_t len);

#endif
