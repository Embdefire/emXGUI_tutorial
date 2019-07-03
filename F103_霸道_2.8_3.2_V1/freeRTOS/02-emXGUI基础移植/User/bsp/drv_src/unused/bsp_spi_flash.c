
#include <string.h>
#include "Include.h"


/*
	STM32F4XX ʱ�Ӽ���.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2��SPI3 �� PCLK1, ʱ��42M
		SPI1       �� PCLK2, ʱ��84M

		STM32F4 ֧�ֵ����SPIʱ��Ϊ 37.5 Mbits/S, �����Ҫ��Ƶ��
		STM32Ӳ��SPI�ӿ� = SPI3 ���� SPI1
		����SPI1��ʱ��Դ��84M, SPI3��ʱ��Դ��42M��Ϊ�˻�ø�����ٶȣ������ѡ��SPI1��

*/

//#define SPI_FLASH			SPI3
#define SPI_FLASH			SPI1

//#define ENABLE_SPI_RCC() 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE)
#define ENABLE_SPI_RCC() 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)

/*
	��SPIʱ�������2��Ƶ����֧�ֲ���Ƶ��
	�����SPI1��2��ƵʱSCKʱ�� = 42M��4��ƵʱSCKʱ�� = 21M
	�����SPI3, 2��ƵʱSCKʱ�� = 21M
*/
#define SPI_BAUD			SPI_BaudRatePrescaler_2

/* ƬѡGPIO�˿�  */
#define SF_CS_GPIO			GPIOG
#define SF_CS_PIN			GPIO_Pin_6

#define SF_CS_L()      SF_CS_GPIO->BSRRH = SF_CS_PIN
#define SF_CS_H()      SF_CS_GPIO->BSRRL = SF_CS_PIN

/*=========================================================================================*/


//#define CMD_AAI       0xAD  	/* AAI �������ָ��(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_EWRSR	  0x50		/* ����д״̬�Ĵ��������� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_SE        0x20		/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define DUMMY_BYTE    0xA5		/* ���������Ϊ����ֵ�����ڶ����� */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */

/*=========================================================================================*/



static const SFLASH_T *pSF=NULL;

static const SFLASH_T flash_tbl[]={

	{ "MX25L1606E",		0xC22015,	4*KB,   2*MB },
	{ "W25Q16",			0xEF4015,	4*KB,   2*MB },
	{ "W25Q32",			0xEF4016,	4*KB,   4*MB },
	{ "W25Q64",			0xEF4017,	4*KB,   8*MB },
	{ "W25Q128",		0xEF4018,	4*KB,  16*MB },

	{ "Unknow",			0xFFFFFF,	4*KB,  1*MB },

};

#define	FLASH_TBL_COUNT	(sizeof(flash_tbl)/sizeof(flash_tbl[0]))
static mutex_obj *mutex_lock=NULL;

/*=========================================================================================*/


static u8 sf_SendByte(u8 _ucValue);
static void sf_WriteEnable(void);
static void sf_WriteStatus(u8 _ucValue);
static void sf_WaitForWriteEnd(void);
static u8 sf_NeedErase(u8 * _ucpOldBuf, u8 *_ucpNewBuf, u16 _uiLen);
static u8 sf_CmpData(u32 _uiSrcAddr, u8 *_ucpTar, u32 _uiSize);
static u8 sf_AutoWritePage(u8 *_ucpSrc, u32 _uiWrAddr, u16 _usWrLen);

static void bsp_CfgSPIForSFlash(void);

static u8 s_spiBuf[4096];	/* ����д�������ȶ�������page���޸Ļ�������������page��д */



/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSpiFlash
*	����˵��: ��ʼ������FlashӲ���ӿڣ�����STM32��SPIʱ�ӡ�GPIO)
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{
	int i,id;

	mutex_lock =SYS_mutex_create(NULL);

	/*
	 * STM32Ӳ��SPI�ӿ� = SPI1
		����Flash�ͺ�Ϊ W25Q128 (80MHz)
		SPI_SCK  - PB3
		SPI_MISO - PB4
		SPI_MOSI - PB5
		SF_CS    - PG6


	*/
	if(1)
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* ʹ��GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOG, ENABLE);

		/* ���� SCK, MISO �� MOSI Ϊ���ù��� */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* ����Ƭѡ����Ϊ�������ģʽ */
		SF_CS_H();		/* Ƭѡ�øߣ���ѡ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = SF_CS_PIN;
		GPIO_Init(SF_CS_GPIO, &GPIO_InitStructure);
	}


	/* ����SPIӲ���������ڷ��ʴ���Flash */
	bsp_CfgSPIForSFlash();

	id=sf_ReadID();
	for(i=0;i<FLASH_TBL_COUNT;i++)
	{
		if(id ==flash_tbl[i].ChipID)
		{
			break;
		}
	}
	pSF =&flash_tbl[i];

#if 1
	//sf_ReadInfo();			/* �Զ�ʶ��оƬ�ͺ� */

	SF_CS_L();					/* �����ʽ��ʹ�ܴ���FlashƬѡ */
	sf_SendByte(CMD_DISWR);		/* ���ͽ�ֹд�������,��ʹ�����д���� */
	SF_CS_H();					/* �����ʽ�����ܴ���FlashƬѡ */

	sf_WaitForWriteEnd();		/* �ȴ�����Flash�ڲ�������� */
	sf_WriteStatus(0);			/* �������BLOCK��д���� */
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CfgSPIForSFlash
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ���ٶȵȲ��������ڷ���SPI�ӿڵĴ���Flash��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_CfgSPIForSFlash(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	/* ��SPIʱ�� */
	ENABLE_SPI_RCC();

	/* ����SPIӲ������ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* ���ݷ���2��ȫ˫�� */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32��SPI����ģʽ ������ģʽ */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* ����λ���� �� 8λ */
	/* SPI_CPOL��SPI_CPHA���ʹ�þ���ʱ�Ӻ����ݲ��������λ��ϵ��
	   ��������: ���߿����Ǹߵ�ƽ,��2�����أ������ز�������)
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* ʱ�������ز������� */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* ʱ�ӵĵ�2�����ز������� */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* Ƭѡ���Ʒ�ʽ��������� */

	/* ���ò�����Ԥ��Ƶϵ�� */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BAUD;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* ����λ������򣺸�λ�ȴ� */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC����ʽ�Ĵ�������λ��Ϊ7�������̲��� */
	SPI_Init(SPI_FLASH, &SPI_InitStructure);

	SPI_Cmd(SPI_FLASH, DISABLE);			/* �Ƚ�ֹSPI  */
	SPI_Cmd(SPI_FLASH, ENABLE);				/* ʹ��SPI  */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_EraseSector
*	����˵��: ����ָ��������
*	��    ��:  _uiSectorAddr : ������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_EraseSector(u32 _uiSectorAddr)
{
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_SE);								/* ���Ͳ������� */
	sf_SendByte((_uiSectorAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiSectorAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiSectorAddr & 0xFF);				/* ����������ַ��8bit */
	SF_CS_H();									/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_EraseChip
*	����˵��: ��������оƬ
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_EraseChip(void)
{
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_BE);							/* ������Ƭ�������� */
	SF_CS_H();									/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_PageWrite
*	����˵��: ��һ��page��д�������ֽڡ��ֽڸ������ܳ���ҳ���С��4K)
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_PageWrite(u8 * _pBuf, u32 _uiWriteAddr, u16 _usSize)
{
	u32 i, j;


	for (j = 0; j < _usSize / 256; j++)
	{
		sf_WriteEnable();								/* ����дʹ������ */

		SF_CS_L();									/* ʹ��Ƭѡ */
		sf_SendByte(0x02);								/* ����AAI����(��ַ�Զ����ӱ��) */
		sf_SendByte((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
		sf_SendByte((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
		sf_SendByte(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */

		for (i = 0; i < 256; i++)
		{
			sf_SendByte(*_pBuf++);					/* �������� */
		}

		SF_CS_H();								/* ��ֹƬѡ */

		sf_WaitForWriteEnd();						/* �ȴ�����Flash�ڲ�д������� */

		_uiWriteAddr += 256;
	}

	/* ����д����״̬ */
	SF_CS_L();
	sf_SendByte(CMD_DISWR);
	SF_CS_H();

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */

}

/*
*********************************************************************************************************
*	�� �� ��: sf_ReadBuffer
*	����˵��: ������ȡ�����ֽڡ��ֽڸ������ܳ���оƬ������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiReadAddr ���׵�ַ
*				_usSize �����ݸ���, ���Դ���PAGE_SIZE,���ǲ��ܳ���оƬ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_ReadBuffer(u8 * _pBuf, u32 _uiReadAddr, u32 _uiSize)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > pSF->TotalSize)
	{
		return;
	}

	/* ������������ */
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_READ);							/* ���Ͷ����� */
	sf_SendByte((_uiReadAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiReadAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiReadAddr & 0xFF);				/* ����������ַ��8bit */
	while (_uiSize--)
	{
		*_pBuf++ = sf_SendByte(DUMMY_BYTE);			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	}
	SF_CS_H();									/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_CmpData
*	����˵��: �Ƚ�Flash������.
*	��    ��:  	_ucpTar : ���ݻ�����
*				_uiSrcAddr ��Flash��ַ
*				_uiSize �����ݸ���, ���Դ���PAGE_SIZE,���ǲ��ܳ���оƬ������
*	�� �� ֵ: 0 = ���, 1 = ����
*********************************************************************************************************
*/
static u8 sf_CmpData(u32 _uiSrcAddr, u8 *_ucpTar, u32 _uiSize)
{
	u8 ucValue;

	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSrcAddr + _uiSize) > pSF->TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_READ);							/* ���Ͷ����� */
	sf_SendByte((_uiSrcAddr & 0xFF0000) >> 16);		/* ����������ַ�ĸ�8bit */
	sf_SendByte((_uiSrcAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	sf_SendByte(_uiSrcAddr & 0xFF);					/* ����������ַ��8bit */
	while (_uiSize--)
	{
		/* ��һ���ֽ� */
		ucValue = sf_SendByte(DUMMY_BYTE);
		if (*_ucpTar++ != ucValue)
		{
			SF_CS_H();
			return 1;
		}
	}
	SF_CS_H();
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_NeedErase
*	����˵��: �ж�дPAGEǰ�Ƿ���Ҫ�Ȳ�����
*	��    ��:   _ucpOldBuf �� ������
*			   _ucpNewBuf �� ������
*			   _uiLen �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 0 : ����Ҫ������ 1 ����Ҫ����
*********************************************************************************************************
*/
static u8 sf_NeedErase(u8 * _ucpOldBuf, u8 *_ucpNewBuf, u16 _usLen)
{
	u16 i;
	u8 ucOld;

	/*
	�㷨��1����old ��, new ����
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

	�㷨��2��: old �󷴵Ľ���� new λ��
		  0010   old
	&	  0101   new
		 =0000

	�㷨��3��: ���Ϊ0,���ʾ�������. �����ʾ��Ҫ����
	*/

	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;

		/* ע������д��: if (ucOld & (*_ucpNewBuf++) != 0) */
		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_AutoWritePage
*	����˵��: д1��PAGE��У��,�������ȷ������д���Ρ��������Զ���ɲ���������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 0 : ���� 1 �� �ɹ�
*********************************************************************************************************
*/
static u8 sf_AutoWritePage(u8 *_ucpSrc, u32 _uiWrAddr, u16 _usWrLen)
{
	u16 i;
	volatile	u16 j;					/* ������ʱ */
	u32 uiFirstAddr;		/* ������ַ */
	u8 ucNeedErase;		/* 1��ʾ��Ҫ���� */
	u8 cRet;

	/* ����Ϊ0ʱ����������,ֱ����Ϊ�ɹ� */
	if (_usWrLen == 0)
	{
		return 1;
	}

	/* ���ƫ�Ƶ�ַ����оƬ�������˳� */
	if (_uiWrAddr >= pSF->TotalSize)
	{
		return 0;
	}

	/* ������ݳ��ȴ����������������˳� */
	if (_usWrLen > pSF->PageSize)
	{
		return 0;
	}

	/* ���FLASH�е�����û�б仯,��дFLASH */
	sf_ReadBuffer(s_spiBuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spiBuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}

	/* �ж��Ƿ���Ҫ�Ȳ������� */
	/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
	ucNeedErase = 0;
	if (sf_NeedErase(s_spiBuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(pSF->PageSize - 1));

	if (_usWrLen == pSF->PageSize)		/* ������������д */
	{
		for	(i = 0; i < pSF->PageSize; i++)
		{
			s_spiBuf[i] = _ucpSrc[i];
		}
	}
	else						/* ��д�������� */
	{
		/* �Ƚ��������������ݶ��� */
		sf_ReadBuffer(s_spiBuf, uiFirstAddr, pSF->PageSize);

		/* ���������ݸ��� */
		i = _uiWrAddr & (pSF->PageSize - 1);
		memcpy(&s_spiBuf[i], _ucpSrc, _usWrLen);
	}

	/* д��֮�����У�飬�������ȷ����д�����3�� */
	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
		if (ucNeedErase == 1)
		{
			sf_EraseSector(uiFirstAddr);		/* ����1������ */
		}

		/* ���һ��PAGE */
		sf_PageWrite(s_spiBuf, uiFirstAddr, pSF->PageSize);

		if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
		{
			cRet = 1;
			break;
		}
		else
		{
			if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
			{
				cRet = 1;
				break;
			}

			/* ʧ�ܺ��ӳ�һ��ʱ�������� */
			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_WriteBuffer
*	����˵��: д1��������У��,�������ȷ������д���Ρ��������Զ���ɲ���������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWrAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 1 : �ɹ��� 0 �� ʧ��
*********************************************************************************************************
*/
u8 sf_WriteBuffer(u8* _pBuf, u32 _uiWriteAddr, u16 _usWriteSize)
{
	u16 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % pSF->PageSize;
	count = pSF->PageSize - Addr;
	NumOfPage =  _usWriteSize / pSF->PageSize;
	NumOfSingle = _usWriteSize % pSF->PageSize;

	if (Addr == 0) /* ��ʼ��ַ��ҳ���׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��ҳ���С */
		{
			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else 	/* ���ݳ��ȴ��ڵ���ҳ���С */
		{
			while (NumOfPage--)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, pSF->PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  pSF->PageSize;
				_pBuf += pSF->PageSize;
			}
			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
			{
				return 0;
			}
		}
	}
	else  /* ��ʼ��ַ����ҳ���׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��ҳ���С */
		{
			if (NumOfSingle > count) /* (_usWriteSize + _uiWriteAddr) > SPI_FLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
				{
					return 0;
				}

				_uiWriteAddr +=  count;
				_pBuf += count;

				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, temp) == 0)
				{
					return 0;
				}
			}
			else
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
				{
					return 0;
				}
			}
		}
		else	/* ���ݳ��ȴ��ڵ���ҳ���С */
		{
			_usWriteSize -= count;
			NumOfPage =  _usWriteSize / pSF->PageSize;
			NumOfSingle = _usWriteSize % pSF->PageSize;

			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
			{
				return 0;
			}

			_uiWriteAddr +=  count;
			_pBuf += count;

			while (NumOfPage--)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, pSF->PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  pSF->PageSize;
				_pBuf += pSF->PageSize;
			}

			if (NumOfSingle != 0)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
				{
					return 0;
				}
			}
		}
	}
	return 1;	/* �ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_ReadID
*	����˵��: ��ȡ����ID
*	��    ��:  ��
*	�� �� ֵ: 32bit������ID (���8bit��0����ЧIDλ��Ϊ24bit��
*********************************************************************************************************
*/
u32 sf_ReadID(void)
{
	u32 uiID;
	u8 id1, id2, id3;

	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_RDID);						/* ���Ͷ�ID���� */
	id1 = sf_SendByte(DUMMY_BYTE);				/* ��ID�ĵ�1���ֽ� */
	id2 = sf_SendByte(DUMMY_BYTE);				/* ��ID�ĵ�2���ֽ� */
	id3 = sf_SendByte(DUMMY_BYTE);				/* ��ID�ĵ�3���ֽ� */
	SF_CS_H();									/* ����Ƭѡ */

	uiID = ((u32)id1 << 16) | ((u32)id2 << 8) | id3;

	return uiID;
}


/*
*********************************************************************************************************
*	�� �� ��: sf_SendByte
*	����˵��: ����������һ���ֽڣ�ͬʱ��MISO���߲����������ص�����
*	��    ��:  _ucByte : ���͵��ֽ�ֵ
*	�� �� ֵ: ��MISO���߲����������ص�����
*********************************************************************************************************
*/
static u8 sf_SendByte(u8 _ucValue)
{
	/* �ȴ��ϸ�����δ������� */
	while (SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_TXE) == RESET);

	/* ͨ��SPIӲ������1���ֽ� */
	SPI_I2S_SendData(SPI_FLASH, _ucValue);

	/* �ȴ�����һ���ֽ�������� */
	while (SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_RXNE) == RESET);

	/* ���ش�SPI���߶��������� */
	return SPI_I2S_ReceiveData(SPI_FLASH);
}

/*
*********************************************************************************************************
*	�� �� ��: sf_WriteEnable
*	����˵��: ����������дʹ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_WREN);								/* �������� */
	SF_CS_H();									/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_WriteStatus
*	����˵��: д״̬�Ĵ���
*	��    ��:  _ucValue : ״̬�Ĵ�����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WriteStatus(u8 _ucValue)
{
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_WRSR);						/* ������� д״̬�Ĵ��� */
	sf_SendByte(_ucValue);						/* �������ݣ�״̬�Ĵ�����ֵ */
	SF_CS_H();									/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_WaitForWriteEnd
*	����˵��: ����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	SF_CS_L();									/* ʹ��Ƭѡ */
	sf_SendByte(CMD_RDSR);							/* ������� ��״̬�Ĵ��� */
	while((sf_SendByte(DUMMY_BYTE) & WIP_FLAG) == SET);	/* �ж�״̬�Ĵ�����æ��־λ */
	SF_CS_H();									/* ����Ƭѡ */
}

/*=========================================================================================*/
/*=========================================================================================*/

BOOL SF_Write(u8 *buf,u32 addr,u32 size)
{
	SYS_mutex_lock(mutex_lock,5000);

	sf_WriteBuffer(buf,addr,size);

	SYS_mutex_unlock(mutex_lock);
	return TRUE;
}

BOOL SF_Read(u8 *buf,u32 addr,u32 size)
{
	SYS_mutex_lock(mutex_lock,5000);

	sf_ReadBuffer(buf,addr,size);

	SYS_mutex_unlock(mutex_lock);
	return TRUE;
}

/*=========================================================================================*/

