
#include "Kernel.h"
#include "BSP.h"

/*=========================================================================================*/
#define	SCL_PIN	GPIOD,GPIO_Pin_7
#define	SDA_PIN	GPIOD,GPIO_Pin_3

//IO��������
#define IIC_SCL(x)		GPIO_Pin_Set(SCL_PIN,x)		//SCL
#define IIC_SDA(x)		GPIO_Pin_Set(SDA_PIN,x)		//SDA
#define READ_SDA		GPIO_Pin_Get(SDA_PIN)   	//��ȡSDA

/*=========================================================================================*/

static void SDA_IN(void)
{	//PD3 ����ģʽ
	volatile int i;

	GPIOD->MODER &= ~(3<<(3*2));
	for(i=0;i<5;i++);
}

static	void SDA_OUT(void)
{	//PD3 ���ģʽ
	volatile int i;

	GPIOD->MODER &= ~(3<<(3*2));
	GPIOD->MODER |=  (1<<(3*2));
	for(i=0;i<5;i++);
}

/*=========================================================================================*/

static void iic_delay(int loop)
{
	volatile int a;

	while(loop-- > 0)
	{
		for(a=0;a<50;a++);
	}
}

static void IIC_Stop(void);

static	mutex_obj *mutex_lock=NULL;

//��ʼ��IIC
static BOOL IIC_Init(void)
{
	GPIO_InitTypeDef gpio_init;

	mutex_lock =SYS_mutex_create(NULL);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //��GPIOʱ��

	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
	gpio_init.GPIO_OType = GPIO_OType_PP; 	//PPģʽ��ODģʽ�и��ߵ��ٶ�,��ǿ�����������Ϳ���������...
	gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;	//��������ʹ��
	gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_3;
	GPIO_Init(GPIOD, &gpio_init);
	
	//��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ
	IIC_Stop();
	return TRUE;
}

static BOOL IIC_Open(void)
{
	SYS_mutex_lock(mutex_lock,5000);
	return TRUE;
}

static void IIC_Close(void)
{
	SYS_mutex_unlock(mutex_lock);
}

/*=========================================================================================*/

//����IIC��ʼ�ź�
static void IIC_Start(void)
{//START:when CLK is high,DATA change form high to low

	SDA_OUT();	//sda�����
	IIC_SDA(1);
	IIC_SCL(1);
	iic_delay(2);
 	IIC_SDA(0);
	iic_delay(2);
	IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
	iic_delay(2);
}	  

//����IICֹͣ�ź�
static void IIC_Stop(void)
{//STOP:when CLK is high DATA change form low to high

	SDA_OUT();//sda�����
	IIC_SCL(0);
	IIC_SDA(0);
 	iic_delay(2);
	IIC_SCL(1); 
	iic_delay(2);
	IIC_SDA(1);//����I2C���߽����ź�
	iic_delay(2);
}


//�ȴ�Ӧ���źŵ���
static BOOL IIC_Wait_Ack(void)
{
	u16 timeout=0;

	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA(1);
	iic_delay(1);
	IIC_SCL(1);    //CPU����SCL = 1, ��ʱ�����᷵��ACKӦ��
	iic_delay(1);
	while(READ_SDA)
	{
		if(timeout++ > 1000)
		{
			return FALSE;
		}
	}
	IIC_SCL(0);//ʱ�����0
	iic_delay(1);
	return TRUE;
} 

//����ACKӦ��
static void IIC_Ack(void)
{
	SDA_OUT();
	IIC_SDA(0);   //CPU����SDA = 0
	iic_delay(1);
	IIC_SCL(1);   //CPU����1��ʱ��
	iic_delay(1);
	IIC_SCL(0);
	iic_delay(1);

	IIC_SDA(1);   //CPU�ͷ�SDA����
}

//������ACKӦ��		    
static void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SDA(1);   //CPU����SDA = 1
	iic_delay(1);
	IIC_SCL(1);   //CPU����1��ʱ��
	iic_delay(1);
	IIC_SCL(0);
	iic_delay(1);

}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
static BOOL IIC_Send_Byte(u8 txd,u8 ack)
{                        
	int i;

	SDA_OUT(); 	    
    for(i=0;i<8;i++)
    {
        IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
        iic_delay(1);

    	if(txd&0x80)
    	{
    		IIC_SDA(1);
    	}
    	else
    	{
    		IIC_SDA(0);
    	}

        txd <<= 1;
		iic_delay(1);
		IIC_SCL(1);
		iic_delay(1);

    }
    IIC_SCL(0);
    IIC_SDA(1); //free SDA
    if(ack == IIC_NoACK)
    {
    	return TRUE;
    }

	if(IIC_Wait_Ack())
	{
		return TRUE;
	}

	return FALSE;
} 	    

//��1���ֽ�.
static u8 IIC_Read_Byte(u8 ack)
{
	u8 i,dat=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
		IIC_SCL(1);
		iic_delay(1);
		dat<<=1;
        if(READ_SDA)
        {
        	dat |= 0x01;
        }
        IIC_SCL(0);
        iic_delay(1);
    }

    if(ack==IIC_ACK)
    {
    	IIC_Ack(); //����ACK
    }
    else
    {
    	IIC_NAck();//����nACK
    }
    return dat;
}

/*=========================================================================================*/

const struct	i2c_ops I2C_GPD7_GPD3={

	IIC_Init,
	IIC_Open,
	IIC_Close,
	IIC_Start,
	IIC_Stop,
	IIC_Send_Byte,
	IIC_Read_Byte,
};

/*=========================================================================================*/

