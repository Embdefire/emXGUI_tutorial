/**
  *********************************************************************
  * @file    X_GUI_FreeRTOS.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI��FreeRTOS����ϵͳ�ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
/* �ڹ���ѡ���ж�������꣬������ʹ�õĲ���ϵͳ */  
#ifdef	X_GUI_USE_FREERTOS

#include	<stddef.h>
#include	"emXGUI_Arch.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/
GUI_MUTEX*	GUI_MutexCreate(void)
{
	return (GUI_MUTEX*)xSemaphoreCreateRecursiveMutex();	
}

/*===================================================================================*/
/*
��������: ��������
����: hMutex(��GUI_MutexCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/
BOOL	GUI_MutexLock(GUI_MUTEX *hMutex,U32 time)
{
	if(xSemaphoreTakeRecursive((SemaphoreHandle_t)hMutex,pdMS_TO_TICKS(time))==pdTRUE)
	{
		return TRUE;
	}
	return	FALSE;
}

/*===================================================================================*/
/*
��������: �������
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_MutexUnlock(GUI_MUTEX *hMutex)
{
	 xSemaphoreGiveRecursive((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
��������: ����ɾ��
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_MutexDelete(GUI_MUTEX *hMutex)
{
	vSemaphoreDelete((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
��������: ����һ���ź���
����: init: �ź�����ʼֵ; max: �ź������ֵ
����: �ź���������(Ψһ��ʶ)
˵��: �ź�����������ʵ��OS����,������ָ��,ID�ŵ�...
*/
GUI_SEM*	GUI_SemCreate(int init,int max)
{
	return (GUI_SEM*)xSemaphoreCreateCounting(max,init);
}

/*===================================================================================*/
/*
��������: �ź����ȴ�
����: hsem(��GUI_SemCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/
BOOL	GUI_SemWait(GUI_SEM *hsem,U32 time)
{

	if(xSemaphoreTake((SemaphoreHandle_t)hsem,pdMS_TO_TICKS(time))== pdTRUE)
	{
		return TRUE;
	}
	return FALSE;
}

/*===================================================================================*/
/*
��������: �ź�������
����: hsem(��GUI_SemCreate���صľ��);  
����: ��
˵��: .
*/
void	GUI_SemPost(GUI_SEM *hsem)
{
	xSemaphoreGive((SemaphoreHandle_t)hsem);
}

/*===================================================================================*/
/*
��������: �ź���ɾ��
����: hsem(��GUI_SemCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_SemDelete(GUI_SEM *hsem)
{
	vSemaphoreDelete((SemaphoreHandle_t)hsem);
}

/*===================================================================================*/
/*
��������: ��õ�ǰ�߳̾��(Ψһ��ʶ)
����: ��  
����: ��ǰ�߳�Ψһ��ʶ,��ʵ��OS����,������ָ��,ID�ŵ�...
˵��: .
*/
HANDLE	GUI_GetCurThreadHandle(void)
{
	return	(HANDLE)xTaskGetCurrentTaskHandle();
}


/*===================================================================================*/
/*
��������: ��õ�ǰϵͳʱ��(��λ:����)
����: ��  
����: ��ǰϵͳʱ��
˵��: .
*/
U32	GUI_GetTickCount(void)
{
	U32 i;
	
	i=xTaskGetTickCount();
	
	return (i*1000)/configTICK_RATE_HZ;

}

/*===================================================================================*/
/*
��������: ���ʱ�����ó�CPU
����: ��  
����: ��
˵��: ������OS�������,��򵥵ķ�����:OS Delay һ�� tick ����.
*/
void	GUI_Yield(void)
{
	vTaskDelay(2);
}

/*===================================================================================*/
/*
��������: ��ʱ����
����: ms: ��ʱʱ��(��λ:����) 
����: ��
˵��: 
*/
void	GUI_msleep(u32 ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}



#endif

/********************************END OF FILE****************************/

