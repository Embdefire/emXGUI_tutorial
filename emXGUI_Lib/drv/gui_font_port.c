/**
  *********************************************************************
  * @file    gui_font_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �������ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "gui_drv_cfg.h"

#include "gui_mem_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"

/*===================================================================================*/
/*===================================================================================*/


#include <GUI_Font_XFT.h>

/**
  * @note  ��Ҫ��������뵽�ڲ�FLASH��
  * ����Ӧ�����������ļ���ӵ����̣�Ȼ���ڴ˴���������
  * ��ʹ��XFT_CreateFont��������
  */
extern const char GB2312_16_1BPP[];
extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_4BPP[];

extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];



/*===================================================================================*/
#if (GUI_USE_EXTERN_FONT && (!GUI_FONT_LOAD_TO_RAM))

/**
  * @brief  ����ý��������ݵĻص�����
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int font_read_data_exFlash(void *buf,int offset,int size,LONG lParam)
{
  /* ��������offset�Ǿ����ַ������������ļ��е�ƫ�� 
   * lParam �������ļ���FLASH�еĻ���ַ
  */
	offset += lParam;
  
  /* ��ȡ�������ģ�������� */
	RES_DevRead(buf,offset,size);
	return size;
}
#endif

/**
  * @brief  GUIĬ�������ʼ��
  * @param  ��
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Default_FontInit(void)
{

	HFONT hFont=NULL;

#if (GUI_FONT_LOAD_TO_RAM  )
  {  
    /* ���������ļ�������RAM */
    
    int font_base;
    
    /* ָ�򻺳�����ָ�� */
    static u8 *pFontData_XFT=NULL;
    CatalogTypeDef dir;
    
    /* RES_GetInfo��ȡ����dir.offset����Դ�ľ��Ե�ַ */
    font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);

    if(font_base > 0)
    {
    	pFontData_XFT =(u8*)GUI_VMEM_Alloc(dir.size);
      if(pFontData_XFT!=NULL)
      {
        RES_DevRead(pFontData_XFT, font_base, dir.size);

        hFont = XFT_CreateFont(pFontData_XFT);
      }
    }
  }
#elif (GUI_USE_EXTERN_FONT)   
  {
    /* ʹ�����豸�������壬����Ҫ��ȡ */
    if(hFont==NULL)
    { 
    	int font_base;
      CatalogTypeDef dir;

    	font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);
    	if(font_base > 0)
    	{
    		hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
    	}
    }
  }
#endif

    /* ��ǰ����������ʧ�ܣ�ʹ���ڲ�FLASH�е����ݣ������е�C�������飩
    *  �����������ʱ���������ļ���ӵ����̣��ڱ��ļ�ͷ�����Ӧ���������������
    *  Ȼ�����XFT_CreateFont�����������弴��
    */
    if(hFont==NULL)
    { 
      /* �ӱ��ؼ���(������������) */    	
      hFont =XFT_CreateFont(GUI_DEFAULT_FONT);  /*ASCii�ֿ�,20x20,4BPP�����*/
      
      /* �����ֿ�洢ռ�ÿռ�ǳ��󣬲��Ƽ������ڲ�FLASH */
    	//hFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312�ֿ�,16x16,2BPP�����*/
    	//hFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312�ֿ�,20x20,4BPP�����*/
    }
	return hFont;
}

/********************************END OF FILE****************************/

