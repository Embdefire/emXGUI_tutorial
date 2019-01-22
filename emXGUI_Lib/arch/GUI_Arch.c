
#include <string.h>
#include "gui_drv.h"

/*===================================================================================*/


/*===================================================================================*/

GL_OP GL_MEM_8PP;
GL_OP GL_MEM_16PP;
GL_OP GL_MEM_32PP;

/*===================================================================================*/

void* GUI_memset(void * s, int c, int n)
{
	return memset(s,c,n);
}

void* GUI_memcpy(void *s1, const void *s2, int n)
{
	return	memcpy(s1,s2,n);
}


/*=========================================================================================*/

void	GUI_memset8(U8 *dst,U8 dat,int count)
{
	memset(dst,dat,count);
}

void	GUI_memset8_step(U8 *dst,U8 dat,int count,int step)
{
	switch(count&0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break ;
	}

	while(count > 0)
	{
		count -= 8;

//		out8: 
      *dst =dat; dst+= step;
		out7: *dst =dat; dst+= step;
		out6: *dst =dat; dst+= step;
		out5: *dst =dat; dst+= step;
		
		out4: *dst =dat; dst+= step;
		out3: *dst =dat; dst+= step;
		out2: *dst =dat; dst+= step;
		out1: *dst =dat; dst+= step;
	}

}

void	GUI_memset16(U16 *dst,U16 dat,int count)
{
	
	switch(count & 0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break;
	}
	
	while(count > 0)
	{
		count -= 8;

//		out8:
      *dst++ =dat;
		out7: *dst++ =dat;
		out6: *dst++ =dat;
		out5: *dst++ =dat;
		
		out4: *dst++ =dat;
		out3: *dst++ =dat;
		out2: *dst++ =dat;
		out1: *dst++ =dat;
	}
}


void	GUI_memset16_step(U16 *dst,U16 dat,int count,int step)
{
	switch(count&0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break;
	}
	
	while(count > 0)
	{
		count -= 8;

//		out8: 
      *dst =dat; dst+= step;
		out7: *dst =dat; dst+= step;
		out6: *dst =dat; dst+= step;
		out5: *dst =dat; dst+= step;
		
		out4: *dst =dat; dst+= step;
		out3: *dst =dat; dst+= step;
		out2: *dst =dat; dst+= step;
		out1: *dst =dat; dst+= step;
	}

}

void	GUI_memset32(U32 *dst,U32 dat,int count)
{
	switch(count&0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break;
	}
	
	while(count > 0)
	{
		count -= 8;

//		out8: 
      *dst++ =dat;
		out7: *dst++ =dat;
		out6: *dst++ =dat;
		out5: *dst++ =dat;
		
		out4: *dst++ =dat;
		out3: *dst++ =dat;
		out2: *dst++ =dat;
		out1: *dst++ =dat;
	}
}

void	GUI_memset32_step(U32 *dst,U32 dat,int count,int step)
{
	switch(count&0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break;
	}
	
	while(count > 0)
	{
		count -= 8;

//		out8: 
      *dst =dat; dst+= step;
		out7: *dst =dat; dst+= step;
		out6: *dst =dat; dst+= step;
		out5: *dst =dat; dst+= step;
		
		out4: *dst =dat; dst+= step;
		out3: *dst =dat; dst+= step;
		out2: *dst =dat; dst+= step;
		out1: *dst =dat; dst+= step;
	}
}

/*===================================================================================*/
#if 0
U8	alpha_blend_value(U8 dst,U8 src,U8 src_a)
{
//	return	( src * src_a + dst * (255-src_a))>>8;
	return	(((src-dst)*src_a)>>8) + dst; 
}
#endif

U8 GUI_AlphaBlendValue(U8 dst,U8 src,U8 src_a)
{
	return	(((src-dst)*src_a)>>8) + dst;
}

/*============================================================================*/

COLOR_RGB32 GUI_AlphaBlendColor(COLOR_RGB32 bk_c,COLOR_RGB32 fr_c,U8 alpha)
{
	U8 bk_r,bk_g,bk_b;
	U8 fr_r,fr_g,fr_b;
	
	bk_r =bk_c>>16;
	bk_g =bk_c>>8;
	bk_b =bk_c;
	
	fr_r =fr_c>>16;
	fr_g =fr_c>>8;
	fr_b =fr_c;

	
	fr_r = (((fr_r-bk_r)*alpha)>>8) + bk_r; 
	fr_g = (((fr_g-bk_g)*alpha)>>8) + bk_g; 
	fr_b = (((fr_b-bk_b)*alpha)>>8) + bk_b; 

/*
	fr_r = ( fr_r * alpha + bk_r * (255-alpha))>>8;
	fr_g = ( fr_g * alpha + bk_g * (255-alpha))>>8;	
	fr_b = ( fr_b * alpha + bk_b * (255-alpha))>>8;	
*/

	return	RGB888(fr_r,fr_g,fr_b);
	
}



/*============================================================================*/



/*===================================================================================*/

/*===================================================================================*/


/**
  * @brief  GUI�ں˾���������Ҫ���ڴ���Z��
  */

/* �Ƿ��ӡ��ǰʹ�õľ��������� */
//#define	RGN_DEBUG

/** 
 �ھ�̬�ڴ�������RECT_LL ��������
 ʹ�þ�̬�ڴ�������Լ����ٵķ��䣬������ϵͳ����
 ��������̬ RECT_LL �������꣬�ͻ���� GUI_MEM_Alloc��
 �Զ�̬�ڴ淽ʽ���� RECT_LL 
 �ⲿ����GUI_RectLL_Alloc����ʵ��

 ����û����ڲ��࣬����ϵͳ��̬ RAM�ֱȽ���ʱ��
 �Ϳ��԰��������Сһ��
 
 һ��RECT_LLԪ��20�ֽڣ��ܵľ�̬�ռ�ռ��Ϊ 20*RECT_LL_SLOT_NUM ���ֽ�
*/
#ifndef	RECT_LL_SLOT_NUM
#define	RECT_LL_SLOT_NUM	512
#endif

static	RECT_LL *rgn_free=NULL;
static	RECT_LL  rgn_slot[RECT_LL_SLOT_NUM];
static	GUI_MUTEX *rgn_lock=NULL;

#ifdef	RGN_DEBUG
static	int	rgn_use=0;
static	int rgn_use_max=0;
#endif

/**
  * @brief  ��ʼ��GUI�ں˾�������
  */
static void	_RectLL_Init(void)
{
	int i;

#ifdef	RGN_DEBUG
	rgn_use 	=0;
	rgn_use_max =0;
#endif

	rgn_lock =GUI_MutexCreate();


	for(i=0;i<RECT_LL_SLOT_NUM-1;i++)
	{
		rgn_slot[i].next   = &rgn_slot[i+1];
		rgn_slot[i+1].next = NULL;
	}
	rgn_slot[RECT_LL_SLOT_NUM-1].next =NULL;

	rgn_free =rgn_slot;

}

/**
  * @brief  ����GUI�ں˾�������
  * @note   ����ǰʹ�õ�С��RECT_LL_SLOT_NUM��ֱ�ӴӾ�̬�ڴ�rgn_slot�з��䣬
  *         ����ͨ��GUI_MEM_Alloc�Ӷ�̬�ռ����
  */
RECT_LL*	GUI_RectLL_Alloc(void)
{
	RECT_LL *rgn;

	GUI_MutexLock(rgn_lock,0xFFFFFFFF);
	rgn	=rgn_free;
	if(rgn!=NULL)
	{
		rgn_free =rgn->next;
#ifdef	RGN_DEBUG
		rgn_use++;
		if(rgn_use > rgn_use_max)
		{
			rgn_use_max =rgn_use;
			GUI_DEBUG("rgn_use_max: %d\r\n",rgn_use_max);
		}
#endif
	}
	GUI_MutexUnlock(rgn_lock);

	if(rgn==NULL)
	{
		rgn =GUI_MEM_Alloc(sizeof(RECT_LL));
		rgn->next=NULL;
	}
	return	rgn;
}

/**
  * @brief  �ͷ�GUI�ں˾�������
  */
void	GUI_RectLL_Free(RECT_LL* rgn)
{
	RECT_LL *next;

	GUI_MutexLock(rgn_lock,0xFFFFFFFF);
	while(rgn != NULL)
	{
		next = rgn->next;

		if(rgn >= &rgn_slot[0])
		if(rgn <= &rgn_slot[RECT_LL_SLOT_NUM-1])
		{
			rgn->next = rgn_free;
			rgn_free  = rgn;
		}
		else
		{
			GUI_MEM_Free(rgn);
		}
		rgn = next;

#ifdef	RGN_DEBUG
		if(rgn_use>0)
		{
			rgn_use--;
		}
#endif
	}
	GUI_MutexUnlock(rgn_lock);


}


/*============================================================================*/
/**
  * @brief  GUI�ܹ�������ʼ��.
  */
BOOL	GUI_Arch_Init(void)
{
	_RectLL_Init();                    //��ʼ��GUI�ں˾����������ڴ��ڵ���ʱ��Z�����

	GL_MEM_8BPP_Init(&GL_MEM_8PP);   //��ʼ��8λ�ڴ��ͻ�ͼ����.
	GL_MEM_16BPP_Init(&GL_MEM_16PP); //��ʼ��16λ�ڴ��ͻ�ͼ����.
	GL_MEM_32BPP_Init(&GL_MEM_32PP); //��ʼ��32λ�ڴ��ͻ�ͼ����.

	return TRUE;

}


/*============================================================================*/

