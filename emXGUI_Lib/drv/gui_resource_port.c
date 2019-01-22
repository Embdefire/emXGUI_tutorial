/**
  *********************************************************************
  * @file    gui_resource_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �����ⲿ��Դ�Ľӿ�
  *          �������������ⲿFLASH�е���Դ
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include <string.h>
#include "board.h" 
#include	"emXGUI.h"
#include	"emXGUI_Arch.h"
#include "x_libc.h" 
#include "gui_resource_port.h"
#include "gui_mem_port.h"



/*=========================================================================================*/
/*������Դ�豸�Ļ����ź���*/
static GUI_MUTEX *mutex_lock=NULL;

/**
  * @brief  ��ʼ����Դ�豸���ⲿFLASH��
  * @param  ��
  * @retval �Ƿ��ʼ������
  */
BOOL RES_DevInit(void)
{  
	mutex_lock=GUI_MutexCreate();

	if(SPI_FLASH_Init() == 0)
    return TRUE;
  else
    return FALSE;

//	RES_DevTest();
}

/**
  * @brief  ��ȡ�豸ID
  * @param  ��
  * @retval �豸ID
  */
U32 RES_DevGetID(void)
{
	U32 id;
	GUI_MutexLock(mutex_lock,5000);

	id =SPI_FLASH_ReadID();
	GUI_MutexUnlock(mutex_lock);
	return id;
}

/**
  * @brief  ���豸д������
  * @param  buf Ҫд�������
  * @param  addr д���Ŀ���ַ
  * @param  size д�����������size��Ӧ����BLOCK��С��
  * @retval �Ƿ�д������
  */
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
	SPI_FLASH_BufferWrite(buf,addr,size);
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ���豸�ж�ȡ����
  * @param  buf �洢��ȡ��������
  * @param  addr ��ȡ��Ŀ���ַ
  * @param  size ��ȡ��������
  * @retval �Ƿ��ȡ����
  */
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);

	SPI_FLASH_BufferRead(buf,addr,size);
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ��������
  * @param  addr Ҫ������������ַ
  * @retval �������ֽ���
  */
int RES_DevEraseSector(u32 addr)
{
	GUI_MutexLock(mutex_lock,5000);
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
	GUI_MutexUnlock(mutex_lock);
	return SPI_FLASH_SectorSize;
}

/*=========================================================================================*/
#if 0
/**
  * @brief  ��Դ�豸����
  * @param  ��
  * @retval ��
  */
void RES_DevTest(void)
{
	char *buf;
	volatile u32 id;

	buf =GUI_VMEM_Alloc(4096);
	RES_DevInit();
	id =RES_DevGetID();
  
//  GUI_Printf("0x%x",id);

#if 0  
	while(1)
	{	
    int i = 0;

		RES_DevEraseSector(i);

		memset(buf,i>>12,4096);
		RES_DevWrite(buf,i,4096);

		memset(buf,0,4096);
		RES_DevRead((u8 *)buf,i,4096);

		i += 4096;
		GUI_msleep(100);

	}
#endif  
	GUI_VMEM_Free(buf);
}
#endif



/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ����Դλ��
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
s32 RES_GetOffset(const char *res_name)
{
	int i,len;
	CatalogTypeDef dir;

	len =x_strlen(res_name);
	for(i=0;i<GUI_CATALOG_SIZE;i+=32)
	{
		RES_DevRead((u8*)&dir,GUI_RES_BASE+i,32);

		if(x_strncasecmp(dir.name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
			return dir.offset + GUI_RES_BASE;
		}
	}

	return -1;
}

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ��Դ����Ϣ
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @param  dir[out] Ҫ���ҵ���Դ����
  * @note   �˴�dir.offset�ᱻ��ֵΪ��Դ�ľ��Ե�ַ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	int i,len;
  
	len =x_strlen(res_name);
  /* �������ֱ���Ŀ¼ */
	for(i=0;i<GUI_CATALOG_SIZE;i+=32)
	{
		RES_DevRead((u8*)dir,GUI_RES_BASE+i,32);

		if(x_strncasecmp(dir->name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
      dir->offset += GUI_RES_BASE;
			return dir->offset ;
		}
	}
	return -1;
}

/********************************END OF FILE****************************/

