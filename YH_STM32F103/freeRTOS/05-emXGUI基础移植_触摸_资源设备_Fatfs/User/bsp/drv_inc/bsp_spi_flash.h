/*
*********************************************************************************************************
*
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_flash.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#define SF_MAX_PAGE_SIZE	(4 * 1024)

/* ���崮��Flash ID */
/*
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q64BV_ID    = 0xEF4017
};
*/

typedef struct
{
	const char *ChipName;		/* оƬ�ͺ��ַ�������Ҫ������ʾ */
	u32 ChipID;		/* оƬID */
	u32 PageSize;		/* ҳ���С */
	u32 TotalSize;		/* ������ */

}SFLASH_T;

void bsp_InitSFlash(void);
u32 sf_ReadID(void);
void sf_EraseChip(void);
void sf_EraseSector(u32 _uiSectorAddr);
void sf_PageWrite(u8 * _pBuf, u32 _uiWriteAddr, u16 _usSize);
u8 sf_WriteBuffer(u8* _pBuf, u32 _uiWriteAddr, u16 _usWriteSize);
void sf_ReadBuffer(u8 * _pBuf, u32 _uiReadAddr, u32 _uiSize);

extern SFLASH_T g_tSF;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
