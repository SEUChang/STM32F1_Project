#include "main.h"
#define BUF_LEN 8

#define autoModeButton PAin(0)
#define emerStopButton PAin(1)
#define wirelessStopButton PCin(0)
#define cheatBrakingButton PBin(12)
#define offsetButton PGin(1)
#define stateLED1  PAout(4)
#define stateLED2  PAout(5)

#define noBraking 1500
#define maxBraking 2200
#define cheatBrakingVal 26 // waiting to be tested
u8 brakingVal = 0;
u8 isDriverless = 0; //2019.11.10

void write(USART_TypeDef* USARTx, uint8_t *Data,uint16_t len)
{
	int i=0;
	for(;i<len;i++)
	{
		USART_SendData(USARTx,*(Data+i));
		while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));
	}
}

uint8_t generate_check_sum(uint8_t *buf,int len)
{
	uint8_t sum=0;
	int i=2;
	for(;i<len;i++)
	{
		sum += *(buf+i);
	}
	return sum;
}

int main(void)
{		
	u8 sendBuf[BUF_LEN]={0x66,0xcc,0x00,0x04,0x01,0x00,0x00};

	system_init();
	delay_ms(100);
	TIM3->CCR1 = 1750;
	while(1)
	{
		sendBuf[5] = 0x00;
		
		if (emerStopButton==1||wirelessStopButton==1)//��ͣ���ذ���
		{
			sendBuf[5] |= 0x02;
			LED1=1;
			TIM_SetCompare1(TIM3,maxBraking);
		}
		
		
		if (autoModeButton!=1&&(emerStopButton!=1&& wirelessStopButton != 1))//�Զ����ء���ͣ��δ����,�ֶ�ģʽ�������ɲ
		{
			//sendBuf[5] |= 0x00;
			LED1=1;
			TIM_SetCompare1(TIM3,noBraking);
		}
		
		
		if (autoModeButton==1)//
		{	
			//if autoModeButton==1&&emerStopButton!=1&&wirelessStopButton != 0�Զ�ģʽ���ذ��£���ͣ��δ���£�����λ�������ɲ
			sendBuf[5] |= 0x01;
			LED1=0;
			if(emerStopButton!=1&&wirelessStopButton != 1)
			TIM_SetCompare1(TIM3,(u16)noBraking+(maxBraking-noBraking)/1000*brakingVal);
		}
		
		
		if (cheatBrakingButton == 1)
		{
			sendBuf[5] |= 0x04;
			if(brakingVal > cheatBrakingVal)
				TIM_SetCompare1(TIM3,(u16)noBraking+(maxBraking-noBraking)*brakingVal/100);
			else
				TIM_SetCompare1(TIM3,(u16)noBraking+(maxBraking-noBraking)*cheatBrakingVal/100);
		}

		
		
		if (offsetButton == 1)
			sendBuf[5]|= 0x08;

		if(isDriverless == 0)
		{
			stateLED1 = 1;
			stateLED2 = 0;
			
		}
		else
		{
			stateLED1 = 0;
			stateLED2 = 1;
		}
		sendBuf[BUF_LEN-1] = generate_check_sum(sendBuf,BUF_LEN-1);
		write(USART1,sendBuf,BUF_LEN);
		//printf("brakingVal : %d \r\n",brakingVal);
		delay_ms(15);
	}
}	 
