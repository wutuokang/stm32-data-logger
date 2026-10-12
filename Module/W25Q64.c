	#include "stm32f10x.h"                  // Device header
	#include "MYSPI.h"
	#include "W25Q64_INS.h"
	void W25Q64_Init(void)
	{
		MYSPI_Init();
	}

	/*
	*
	*	手册对应指令
	*/
		void W25Q64_WaitBusy()
	{
		uint16_t i=50000;
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
		while((MYSPI_SwapByte(W25Q64_DUMMY_BYTE)& 0x01) == 0x01)
		{
			i--;
			if(i==0){break;}
		}
		MYSPI_Stop();
	}

	
	void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
	{
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_JEDEC_ID);
		*MID=MYSPI_SwapByte(W25Q64_DUMMY_BYTE);
		*DID=MYSPI_SwapByte(W25Q64_DUMMY_BYTE);
		*DID <<= 8;
		*DID |= MYSPI_SwapByte(W25Q64_DUMMY_BYTE);
		MYSPI_Stop();
	}

	//写使能
	void W25Q64_WriteEnable(void)
	{
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_WRITE_ENABLE);
		MYSPI_Stop();
	}


	//页编程
	void W25Q64_PageProgram(uint32_t Address,uint8_t *Data,uint16_t len)
	{
		
		W25Q64_WriteEnable();

		uint16_t i;
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_PAGE_PROGRAM);
		MYSPI_SwapByte(Address>>16);
		MYSPI_SwapByte(Address>>8);
		MYSPI_SwapByte(Address);
		
		for(i=0;i<len;i++)
		{
			MYSPI_SwapByte(Data[i]);
		}
		MYSPI_Stop();
		W25Q64_WaitBusy();
	}

	//扇区擦除
	void W25Q64_Erase(uint32_t Address)
	{
		W25Q64_WriteEnable();
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
		MYSPI_SwapByte(Address>>16);
		MYSPI_SwapByte(Address>>8);
		MYSPI_SwapByte(Address);
		MYSPI_Stop();
		W25Q64_WaitBusy();
	}

	//读取
	void W25Q64_ReadData(uint32_t Address,uint8_t *Data,uint32_t len)
	{
		uint32_t i;
		MYSPI_Start();
		MYSPI_SwapByte(W25Q64_READ_DATA);
		MYSPI_SwapByte(Address>>16);
		MYSPI_SwapByte(Address>>8);
		MYSPI_SwapByte(Address);
		
		for(i=0;i<len;i++)
		{
			Data[i] = MYSPI_SwapByte(W25Q64_DUMMY_BYTE);
		}
		MYSPI_Stop();
	}
