/**
  *********************************************************************
  * @file    gui_picture_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ͼƬ��ʾ�ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "GUI_Drv_Cfg.h"
#include  "gui_resource_port.h"
#include  "gui_picture_port.h"

/*===================================================================================*/
/**
  * @brief  ����ý��������ݵĻص�����
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int bmp_read_data_exFlash(void *buf,int offset,int size,LPARAM lParam)
{
	offset += lParam;
	RES_DevRead(buf,offset,size);
	return size;
}

 /**
  * @brief  ��ʾ�ļ�ϵͳ�е�BMPͼƬ
  * @param  hdc�����룩����ͼ������
  * @param  x��y�����룩: ���Ƶ�Ŀ�������
  * @param 	lprc�����룩:Ҫ���Ƶ�BMPͼ���������������øò���ΪNULL�����������BMPͼ������
  * @retval FALSE:ʧ��; TRUE:�ɹ�
  */
BOOL _ShowBMPEx_rawFlash(HDC hdc, int x, int y, const RECT *lprc, char *file_name) 
{	 
    int offset;
    GUI_GET_DATA  get_data;
  
    offset =RES_GetOffset(file_name);
    if(offset > 0)
    {    
      get_data.lParam = offset;
      get_data.pfReadData = bmp_read_data_exFlash;

      return BMP_DrawEx(hdc,x,y,&get_data,lprc);
    }
    
    return FALSE;
}




/********************************END OF FILE****************************/
