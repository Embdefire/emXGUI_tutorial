
#include	"BSP.h"

/*============================================================================================*/

void DebugUart_Init(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//����GPIO����USART1

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* �� UART ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


	/* �� PA9 ӳ��Ϊ USART1_TX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	/* �� PA10 ӳ��Ϊ USART1_RX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* ���� USART Tx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ���� USART Rx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



	// ����USART����
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ClearFlag(USART1,USART_IT_TXE);

	// USART1�����ж�ʹ��,����NVIC,ʹ��USART1
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//USART1_NVIC_Config();

	USART_Cmd(USART1, ENABLE);

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ,�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */

	//DebugPuts("\r\n");
	//DebugPuts("UART1 Init OK.\r\n");
#endif
}
/*============================================================================================*/


void	DbgUart_Send(const char *buf,int size)
{
#if 0
	int i;
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //�ȴ����ݷ������
		USART_SendData(USART1,buf[i]);
	}
#endif
}
/*============================================================================================*/

int	SysDrv_stdin(char *buf,int size)
{
 	return	-1;
}

int	SysDrv_stdout(const char *buf,int size)
{
  DbgUart_Send(buf,size);
  return TRUE;
}

int	SysDrv_stderr(const char *buf,int size)
{
  DbgUart_Send(buf,size);
  return TRUE;
}

/*============================================================================================*/


