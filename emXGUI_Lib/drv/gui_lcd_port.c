/**
  *********************************************************************
  * @file    gui_lcd_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   Һ�������ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include "board.h"
#include "gui_drv.h"
#include "gui_drv_cfg.h"
#include "gui_lcd_port.h"
/*============================================================================*/




/**
  * @brief  Һ������ʼ���ӿ�
  * @param  fb_addr Ҫʹ�õ��Դ��ַ
  */
void LCD_HardInit(u32 fb_addr)
{	
  /* ��ʼ��Һ���� */  
#if	(LCD_FORMAT == COLOR_FORMAT_RGB565)
  LCD_Init(fb_addr, 33, LTDC_Pixelformat_RGB565);
#endif
  
#if	(LCD_FORMAT == COLOR_FORMAT_XRGB8888)
  LCD_Init(fb_addr, 21, LTDC_Pixelformat_ARGB8888);
#endif

}

/**
  * @brief  Һ��������ƽӿ�
  * @param  on 1Ϊ��������ֵΪ��
  */
void LCD_BkLight(int on)
{
  LCD_BackLed_Control(on);
}


/**
  * @brief  ��ʼ����ʾ�豸�ӿڣ�������SURFACE����
  * @param  ��
  * @retval ��ʾ�豸Surface����ָ�룬�����õ��Ļ�ͼ����
  */
SURFACE* GUI_DisplayInit(void)
{
  /* ��ͼ���� */
	SURFACE *pSurf;

#if	(LCD_FORMAT == COLOR_FORMAT_RGB565)

  //������ͼ����
  /* ��̬����ķ�ʽ  */
// pSurf = GUI_CreateSurface(SURF_RGB565, 
//                              LCD_XSIZE,LCD_YSIZE,
//                              LCD_XSIZE*2,
//                              NULL);  
                              
  /* ֱ��ָ����ַ�ķ�ʽ�� �Դ��ַ��*/
 pSurf = GUI_CreateSurface(SURF_RGB565, 
                              LCD_XSIZE,LCD_YSIZE,
                              LCD_XSIZE*2,
                              (void*)LCD_FRAME_BUFFER);    

#endif

#if	(LCD_FORMAT == COLOR_FORMAT_XRGB8888)
	//��̬����ķ�ʽ����ʼ��LCD Surface�ṹ����(XRGB8888��ʽ)
	//lcd_buffer =(u8*)GUI_GRAM_Alloc(LCD_XSIZE,LCD_YSIZE*4);
  
  /* ֱ��ָ����ַ�ķ�ʽ�� �Դ��ַ��*/
  /* ��̬����ķ�ʽ  */
// pSurf = GUI_CreateSurface(SURF_XRGB8888, 
//                              LCD_XSIZE,LCD_YSIZE,
//                              LCD_XSIZE*4,
//                              NULL);  
                          
  pSurf = GUI_CreateSurface(SURF_XRGB8888, 
                              LCD_XSIZE,LCD_YSIZE,
                              LCD_XSIZE*4,
                              (void*)LCD_FRAME_BUFFER);                             

#endif
  
  if(pSurf == NULL)
  {
    GUI_Printf("#Error: GUI_CreateSurface Failed.\r\n");
  }

  //LCDӲ����ʼ��
 	LCD_HardInit((u32)pSurf->Bits); 

  //����
	pSurf->GL->FillArea(pSurf,0,0,LCD_XSIZE,LCD_YSIZE,pSurf->CC->MapRGB(0,0,0)); 
	//�򿪱���
  LCD_BkLight(TRUE);

	while(0)
	{ //����

		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,255,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,0,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,255,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,0));
		GUI_msleep(200);
		break;
	}

	return pSurf;
}


/**
  * @brief  ����SURFACE����
  * @param  Format ��ͼ�����ʽ
  * @param  Width Height ��ͼ������
  * @param  LineBytes ��ͼ����ÿ������ռ�����ֽ�
  * @param  bits �Դ��ַ����ΪNULL�����ʹ��GUI_GMEM_Alloc���붯̬�Դ�
  * @retval ��ʾ�豸Surface����ָ�룬�����õ��Ļ�ͼ����
  */
SURFACE*	GUI_CreateSurface(SURF_FORMAT Format,int Width,int Height,int LineBytes,void *bits)
{

	SURFACE *pSurf;
   		
	switch(Format)
	{
		
		case	SURF_RGB332:
				pSurf = (SURFACE*)GUI_MEM_Alloc(sizeof(SURFACE));

				pSurf->Flags =0;

				if(LineBytes <= 0)
				{
					LineBytes = Width;
				}

				if(bits==NULL)
				{
					bits = (void*)GUI_GRAM_Alloc(Height*LineBytes);
					pSurf->Flags |= SURF_FLAG_GRAM;

				}
				SurfaceInit_RGB332(pSurf,&GL_MEM_8PP,Width,Height,LineBytes,bits);
				break;
					////
		
		case	SURF_RGB565:
				pSurf = (SURFACE*)GUI_MEM_Alloc(sizeof(SURFACE));
				pSurf->Flags =0;
				if(LineBytes <= 0)
				{
					LineBytes = Width*2;
				}
				if(bits==NULL)
				{
					bits = (void*)GUI_GRAM_Alloc(Height*LineBytes);
					pSurf->Flags |= SURF_FLAG_GRAM;

				}
				SurfaceInit_RGB565(pSurf,&GL_MEM_16PP,Width,Height,LineBytes,bits);
				break;
				////
		
		case	SURF_ARGB4444:
				pSurf = (SURFACE*)GUI_MEM_Alloc(sizeof(SURFACE));
				pSurf->Flags =0;
				if(LineBytes <= 0)
				{
					LineBytes = Width*2;
				}
				if(bits==NULL)
				{
					bits = (void*)GUI_GRAM_Alloc(Height*LineBytes);
					pSurf->Flags |= SURF_FLAG_GRAM;

				}
				SurfaceInit_ARGB4444(pSurf,&GL_MEM_16PP,Width,Height,LineBytes,bits);
				break;
				////
						
		case	SURF_XRGB8888:
				pSurf = (SURFACE*)GUI_MEM_Alloc(sizeof(SURFACE));
				pSurf->Flags =0;
				if(LineBytes <= 0)
				{
					LineBytes = Width*4;
				}
				if(bits==NULL)
				{
					bits = (void*)GUI_GRAM_Alloc(Height*LineBytes);
					pSurf->Flags |= SURF_FLAG_GRAM;

				}
				SurfaceInit_XRGB8888(pSurf,&GL_MEM_32PP,Width,Height,LineBytes,bits);
				break;
				////	
									
		case	SURF_ARGB8888:
				pSurf = (SURFACE*)GUI_MEM_Alloc(sizeof(SURFACE));
				pSurf->Flags =0;
				if(LineBytes <= 0)
				{
					LineBytes = Width*4;
				}
				if(bits==NULL)
				{
					bits = (void*)GUI_GRAM_Alloc(Height*LineBytes);
					pSurf->Flags |= SURF_FLAG_GRAM;

				}
				SurfaceInit_ARGB8888(pSurf,&GL_MEM_32PP,Width,Height,LineBytes,bits);
				break;
				////	
									
		default:
				pSurf = NULL;
				break;
				////
	
	}
		
	return	pSurf;
}


/**
  * @brief  �ͷŻ�ͼ����
  * @param  pSurf Ҫ�ͷŵĻ�ͼ����
  * @note  �����ͷ�pSurfʹ�õ��ڴ棬
  *         ���ñ��������˶�̬�ڴ���Ϊ�Դ�ռ䣬�����ͷ���Ӧ���Դ�ռ� 
  * @retval ��
  */
void	GUI_DeleteSurface(const SURFACE *pSurf)
{
	if(pSurf!=NULL)
	{
		if(pSurf->Flags&SURF_FLAG_GRAM)
		{
			GUI_GRAM_Free(pSurf->Bits);
		}
		GUI_MEM_Free((void*)pSurf);
	}
}

/********************************END OF FILE****************************/

