

#include "BSP.h"

/*===============================================================================================*/

#if 1
#pragma	pack(4)


static u8 kmem_buf[32*KB];

#pragma	pack()
#endif

/*=========================================================================================*/

#if(GUI_VMEM_EN)    
/* VMEM�ڴ���� */
/* �����ź��� */
static GUI_MUTEX *mutex_vmem = NULL;

/* �ڴ�ѹ����� */
heap_t heap_vmem;

/* VMEM������ */
static uint8_t buff_vmem[VMEM_SIZE] __attribute__((at(VMEM_BASE)));//
#endif

static heap_t *heap_exsram=NULL;

/**
  * @brief  ����һ���ڴ��
  * @note  ʹ��vmallocǰ������ñ�������ʼ���ڴ�Ѿ��
  * @retval ��
  */
void GUI_VMEM_Init(void)
{
#if(GUI_VMEM_EN)      
	mutex_vmem = GUI_MutexCreate();
	x_heap_init(&heap_vmem,
                (void*)buff_vmem,
                  VMEM_SIZE,
                  VMEM_ALLOC_UNIT);	 /* ����һ���ڴ�� */
#endif
}

void*	vmalloc(u32 size)
{
	//return kmalloc(size);
//	return HeapAlloc(heap_exsram,size);
  
#if(GUI_VMEM_EN)      

	u8 *p;
	
	GUI_MutexLock(mutex_vmem,5000);
	p =x_heap_alloc(&heap_vmem,size);
	GUI_MutexUnlock(mutex_vmem);
  if(p==NULL)
	{
	    GUI_ERROR("GUI_VMEM_Alloc,no enough space(for %d byte)",size);
	}
  
	return p;
#endif
}

void	vfree(void *p)
{
	//return kfree(p);
//	HeapFree(heap_exsram,p);
#if(GUI_VMEM_EN)      
	GUI_MutexLock(mutex_vmem,5000);
	x_heap_free(&heap_vmem,p);
	GUI_MutexUnlock(mutex_vmem);
#endif
}

/*===============================================================================================*/


void*	stack_malloc(u32 size)
{
//	return kmalloc(size);

}

void	stack_free(void *p)
{
//	kfree(p);
}

/*===============================================================================================*/

//static heap_obj *heap_dma=NULL;

void*	dma_mem_alloc(u32 size)
{
	//STM32F103 ���ܽ��ⲿSRAM��DMA ?

	//return HeapAlloc(heap_vmem,size);
//	return kmalloc(size);

}

void	dma_mem_free(void *p)
{
	//HeapFree(heap_vmem,p);
//	kfree(p);
}

/*===============================================================================================*/

void Board_MemInit(void)
{
//	kmem_heap_init(kmem_buf,sizeof(kmem_buf),32);
//	heap_exsram  =HeapCreate((u8*)EXT_SRAM_BASE,EXT_SRAM_SIZE,64); //EXSRAM

#if(GUI_VMEM_EN)      
	mutex_vmem = GUI_MutexCreate();
	x_heap_init(&heap_vmem,
                (void*)buff_vmem,
                  VMEM_SIZE,
                  VMEM_ALLOC_UNIT);	 /* ����һ���ڴ�� */
#endif

}

/*===============================================================================================*/

u32	GetMemTotSize(void)
{
	u32 m1;

//	HeapInfo(heap_exsram,&m1,NULL,NULL);

	return (m1);
}

u32	GetMemCurSize(void)
{
	u32 m1;

//	HeapInfo(heap_exsram,NULL,&m1,NULL);
	return (m1);
}

