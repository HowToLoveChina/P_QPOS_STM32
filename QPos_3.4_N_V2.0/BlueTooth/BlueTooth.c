/*
    this file is a source file for BlueTooth.
*/
#include "stm32f10x.h"
#include "string.h"
#include "Bluetooth.h"
#include "SysTick_delay.h"
#include "Roc_LittleTool.h"
#include "IROM.h"

BLUETOOTH_STATUS g_eBluetoothRecvStatus = BLUETOOTH_RECV_NO_DATA;

BLUETOOTH_MESSAGE g_sBluetoothMessage;

unsigned int g_uiBluetoothRcvLen;

//unsigned char g_ucbBluetoothRcvBuf[1280];

//unsigned char g_ucbBluetoothSendBuf[1280];

unsigned char g_ucbBluetoothBuf[1280];

BLUETOOTH_STATE g_BluetoothState;

unsigned char g_BluetoothDateIN;

unsigned int g_uiTime100ms_BTRecvTimeout;

/*
�ú���������ʼ������ʹ�õĴ��ںͿ���IO��״̬IO
����ʹ�õ���UART2
���ڶ�Ӧ��IO����TX==PA2��RX==PA3
ʹ�õĿ���IO��״̬IO��
����ģ���IO        ����ģ������ź�   ����     STM32��IO��   ���ź�    ����
BT_EAN              18                  I         PC15         4         O
UART_TX_IND         9                   O         PC0          8         I
STATUS_IND          10                  O         PC1          9         I
P20                 16                  I         PC2          10        O
P24                 17                  I         PC3          11        O
WEAKUP              7                   I         PA0          14        O
RST                 19                  I         PA1          15        O
PAIRING             25                  I         PA5          21        O
RX_IND *            24                  I         PA7          23        O
LINK_DROP           23                  I         PA6          22        O

����˵����
BT_EAN��ѡ������ģ������пռ���ROM����FLASH��H��ROM L��FLASH
UART_TX_IND����ʾ����ģ��Ҫ��һ��ʱ��������ݡ�
STATUS_IND����UART_TX_INDһ���ʾ����ģ�������״̬��
P20��P24һ���������ģ��Ĺ���ģʽ HH Ӧ��ģʽ LL bootģʽ LH EEPROM����ģʽ
WEAKUP���������������ģʽ��������������ģ�顣
RST����λ����ģ�顣
PAIRING����ʾ����ģ���Ѿ���ԡ�
RX_IND����������ģ��ȴ�һ��ʱ���������ݡ�
LINK_DROP����������ģ��Ͽ����ӡ�
*/
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;    
    //������ʹ�õ�����GPIO��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD,ENABLE);      	
    //������������Tx��GPIO��
	GPIO_InitStructure.GPIO_Pin = BT_UART_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_UARTGPIO, &GPIO_InitStructure);    
    //������������Rx��GPIO��
	GPIO_InitStructure.GPIO_Pin = BT_UART_RxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_UARTGPIO, &GPIO_InitStructure);    
	
    //����������״̬IO��
	GPIO_InitStructure.GPIO_Pin = BT_PIN_StatusInd1|BT_PIN_StatusInd2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_StatusInd, &GPIO_InitStructure);       
    //��������������IO��
	GPIO_InitStructure.GPIO_Pin = BT_PIN_MoodSelect;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_MoodSelect, &GPIO_InitStructure);      
	//EAN �ܽ�
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);      
    	*/
    	//BT_UART_Rx_IND
    	GPIO_InitStructure.GPIO_Pin = BT_PIN_RxInd;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_RxInd, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = BT_PIN_RST;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_RST, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BT_PIN_WakeUP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_WakeUP, &GPIO_InitStructure);

	//BOOT�ܽ�
	GPIO_InitStructure.GPIO_Pin = BT_PIN_BOOT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_BOOT, &GPIO_InitStructure);
	GPIO_SetBits(BT_Port_BOOT,BT_PIN_BOOT); 

    GPIO_SetBits(BT_Port_MoodSelect,BT_PIN_MoodSelect);//P20,Ӧ��ģʽ    
    GPIO_SetBits(BT_Port_RxInd,BT_PIN_RxInd); //RX_IND
    GPIO_SetBits(BT_Port_WakeUP,BT_PIN_WakeUP); //WAKEUP
    GPIO_ResetBits(BT_Port_RST,BT_PIN_RST);//�õ͸�����
    Delay_ms(0x55);//	��Ҫһ��55ms�ĵ͵�ƽ
     GPIO_SetBits(BT_Port_RST,BT_PIN_RST); //RST
    Delay_ms(0x200);//	��Ҫһ��55ms�ĵ͵�ƽ
    
    // Enable the USARTX gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);	 
}

void vBluetooth_IVT_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;    
    //������ʹ�õ�����GPIO��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD,ENABLE);      	
    //������������Tx��GPIO��
	GPIO_InitStructure.GPIO_Pin = BT_UART_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_UARTGPIO, &GPIO_InitStructure);    
    //������������Rx��GPIO��
	GPIO_InitStructure.GPIO_Pin = BT_UART_RxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_UARTGPIO, &GPIO_InitStructure);    

	//����������״̬IO��
	GPIO_InitStructure.GPIO_Pin = BT_PIN_StatusInd1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BT_Port_StatusInd, &GPIO_InitStructure); 
	
    //����������״̬IO��
	//GPIO_InitStructure.GPIO_Pin = BT_PIN_StatusInd1|BT_PIN_StatusInd2;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(BT_Port_StatusInd, &GPIO_InitStructure);       
    //��������������IO��
	//GPIO_InitStructure.GPIO_Pin = BT_PIN_MoodSelect;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	//GPIO_Init(BT_Port_MoodSelect, &GPIO_InitStructure);      
	//EAN �ܽ�
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);      
    	*/
    	//BT_UART_Rx_IND
    	//GPIO_InitStructure.GPIO_Pin = BT_PIN_RxInd;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	//GPIO_Init(BT_Port_RxInd, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = BT_PIN_RST;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_RST, &GPIO_InitStructure);
	GPIO_SetBits(BT_Port_RST,BT_PIN_RST);

	//Connect�л��ܽ�
	GPIO_InitStructure.GPIO_Pin = BT_PIN_ConnectRev;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_ConnectRev, &GPIO_InitStructure);
	GPIO_ResetBits(BT_Port_ConnectRev,BT_PIN_ConnectRev); 
	
	//BOOT�ܽ�
	GPIO_InitStructure.GPIO_Pin = BT_PIN_BOOT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BT_Port_BOOT, &GPIO_InitStructure);
	GPIO_ResetBits(BT_Port_BOOT,BT_PIN_BOOT); 
	//GPIO_InitStructure.GPIO_Pin = BT_PIN_WakeUP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	//GPIO_Init(BT_Port_WakeUP, &GPIO_InitStructure);
    
    //GPIO_SetBits(BT_Port_MoodSelect,BT_PIN_MoodSelect);//P20,Ӧ��ģʽ    
    //GPIO_SetBits(BT_Port_RxInd,BT_PIN_RxInd); //RX_IND
    //GPIO_SetBits(BT_Port_WakeUP,BT_PIN_WakeUP); //WAKEUP
    //GPIO_ResetBits(BT_Port_RST,BT_PIN_RST);//�õ͸�����
    //Delay_ms(0x55);//	��Ҫһ��55ms�ĵ͵�ƽ
    // GPIO_SetBits(BT_Port_RST,BT_PIN_RST); //RST
    //Delay_ms(0x200);//	��Ҫһ��55ms�ĵ͵�ƽ
    
    // Enable the USARTX gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);	 
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_USART_Config(void)
{
  USART_InitTypeDef	 USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

  USART_DeInit(BT_Port_UART);
  /* ---------------------------------------------------------------
    USART Configuration: 
    FPCLK = 36 MHz, Baud rate =36000,MODE: RX & TX
  --------------------------------------------------------------- */
  USART_InitStructure.USART_BaudRate            = 115200;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
 
  USART_Init(BT_Port_UART, &USART_InitStructure);
  USART_ClearFlag(BT_Port_UART,USART_FLAG_RXNE); 
  /* Enable the USART Receive interrupt: this interrupt is generated when the 
   USART1 receive data register is not empty */
  USART_ITConfig(BT_Port_UART, USART_IT_RXNE, ENABLE);	  //�����ж�
  /* Enable USART1 */
  USART_Cmd(BT_Port_UART, ENABLE);
}
void vBluetooth_EXTILine_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
	
	/* Configure EXTI Line1 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EXTI Line1 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_LinkDrop(void)
{
	if(Bluetooth_BM77==Bluetooth_Type())
	{
#if 0
	GPIO_ResetBits(GPIOA,GPIO_Pin_6); //RX_IND
	Delay_ms(300);
    GPIO_SetBits(GPIOA,GPIO_Pin_6); //RX_IND
	Delay_ms(300);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LINK_DROP
	Delay_ms(300);
	GPIO_SetBits(GPIOA,GPIO_Pin_7); //LINK_DROP
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//�õ͸�����
    Delay_ms(0x55);//	��Ҫһ��55ms�ĵ͵�ƽ
    GPIO_SetBits(GPIOA,GPIO_Pin_1);//�ø߸�����    
    Delay_ms(0x200);//	��Ҫһ��55ms�ĵ͵�ƽ
#endif
	}
	else
	{
		BLUETOOTH_STATE BTstate;

		BTstate=eBluetooth_GetBluetoothState();
		if(BTstate==BLUETOOTH_LINK_WITH)
		{	
			vBluetooth_Reset();
			/*
			GPIO_SetBits(BT_Port_ConnectRev,BT_PIN_ConnectRev); //
			Delay_ms(5);
			GPIO_ResetBits(BT_Port_ConnectRev,BT_PIN_ConnectRev); //
			while(eBluetooth_GetBluetoothState()!=BLUETOOTH_SHUT_DOWN){};
			*/
		}
	}
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************

void vBluetooth_UART_SendData(unsigned char* pucBuf, unsigned int uiLen)
{	 
	unsigned int uiLoop;
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	for(uiLoop = 0; uiLoop < uiLen; uiLoop++)
	{
		USART_SendData(BT_Port_UART, pucBuf[uiLoop]);
  		while(!(BT_Port_UART->SR & USART_FLAG_TXE)){};
		//Delay_ms(0x03);//	��Ҫһ��2ms�ĵ͵�ƽ		
	}
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_RecvData(unsigned char* pucBuf, unsigned int uiLen)
{
 	unsigned int uiTimeout = 0x1000000;	
	unsigned int uiLoop = 0;	
    for(uiLoop = 0; uiLoop < uiLen; uiLoop++)
    {
        while (!(BT_Port_UART->SR & USART_FLAG_RXNE))
        {
            uiTimeout--;
            if(uiTimeout == 0)
            {
                return (1);
            }
            else{}    
        }
        pucBuf[uiLoop] = (unsigned char)USART_ReceiveData(BT_Port_UART);	
	  uiTimeout = 0x1000000;	
    }    
    return (0);
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ ��ȡ��ǰ����ģ���״̬
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
BLUETOOTH_STATE eBluetooth_GetBluetoothState(void)
{
    unsigned char ucSta;
	BLUETOOTH_STATE BTstate;	//��ʾ��ǰ����������״̬\

	if(Bluetooth_BM77==Bluetooth_Type())
	{
		 ucSta = (GPIO_ReadInputDataBit(BT_Port_StatusInd,BT_PIN_StatusInd1) << 4) | (GPIO_ReadInputDataBit(BT_Port_StatusInd,BT_PIN_StatusInd2));
	    switch(ucSta)
	    {
	        case 0x11:
	            //shutdown״̬
	            BTstate = BLUETOOTH_SHUT_DOWN;            
	            break;
	        case 0x10:
	            BTstate = BLUETOOTH_ACCESS;
	            break;
	        case 0x01:
	            BTstate = BLUETOOTH_LINK_WITH;
	            break;
	        case 0x00:
	            BTstate = BLUETOOTH_LINK_WO;
	            break;                
	    }
	}
   	else
   	{
   		ucSta = GPIO_ReadInputDataBit(BT_Port_StatusInd,BT_PIN_StatusInd1);
		switch(ucSta)
		{
		case 1:
		   
		    BTstate = BLUETOOTH_SHUT_DOWN;            
		    break;
		case 0:
		    BTstate = BLUETOOTH_LINK_WITH;
		    break;         
		}
   	}
    return BTstate;
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ ���õ�ǰ����ģ���ģʽ
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_SetBluetoothMode(BLUETOOTH_MODE eMode)
{
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		switch(eMode)
		{
		    case BLUETOOTH_APP:
		        GPIO_SetBits(BT_Port_MoodSelect,BT_PIN_MoodSelect);
		       // GPIO_SetBits(GPIOC,GPIO_Pin_3);
		        break;
		    case BLUETOOTH_EEPROM_CONFIG:
		        GPIO_ResetBits(BT_Port_MoodSelect,BT_PIN_MoodSelect); 
		       // GPIO_SetBits(GPIOC,GPIO_Pin_3);            
		        break;
		    case BLUETOOTH_BOOT:
		       // GPIO_ResetBits(GPIOC,GPIO_Pin_2); 
		       // GPIO_ResetBits(GPIOC,GPIO_Pin_3); 
		        break;                
		}
		Delay_ms(0x55);//	��Ҫһ��5ms�ĵ͵�ƽ	//BM57ģʽת����Ҫʱ����ʱ 	
		GPIO_ResetBits(BT_Port_RST,BT_PIN_RST);//�õ͸�����
		Delay_ms(0x55);//	��Ҫһ��5ms�ĵ͵�ƽ		
		GPIO_SetBits(BT_Port_RST,BT_PIN_RST);;//
		Delay_ms(0x280);//	��Ҫһ��60ms�ĵ͵�ƽ  //BM57ģʽת����Ҫʱ����ʱ   
	}
    else 
    {
    		switch(eMode)
		{
		    case BLUETOOTH_APP:	
			GPIO_ResetBits(BT_Port_BOOT,BT_PIN_BOOT); 
		        break;
		    case BLUETOOTH_EEPROM_CONFIG:		            
		        break;
		    case BLUETOOTH_BOOT:
		       GPIO_SetBits(BT_Port_BOOT,BT_PIN_BOOT);      
		        break;                
		}
		  
    }
	return;
    
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ ��ȡ��ǰ����ģ���ģʽ
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
BLUETOOTH_MODE eBluetooth_GetBluetoothMode(void)
{
    unsigned char ucMode;
	BLUETOOTH_MODE BTmode;
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		ucMode = (GPIO_ReadOutputDataBit(BT_Port_MoodSelect,BT_PIN_MoodSelect) << 4) | 0x01;
	    switch(ucMode)
	    {
	        case 0x11:
	            BTmode = BLUETOOTH_APP; 
	            break;
	        case 0x01:
	            BTmode = BLUETOOTH_EEPROM_CONFIG;            
	            break;
	        case 0x00:
	            BTmode = BLUETOOTH_BOOT;       
	            break;                
	    }
	}
       else
       {
       	 BTmode = BLUETOOTH_APP; 
       }
    return BTmode;
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ //��������ģ�顣
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
unsigned char vBluetooth_Force_BT_Into_WeakUp(void)
{
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		if(eBluetooth_GetBluetoothState() == BLUETOOTH_SHUT_DOWN)//�жϵ�ǰ״̬�Ƿ�Ϊshutdown
		{
	        GPIO_ResetBits(BT_Port_WakeUP,BT_PIN_WakeUP);//�õ͸�����
	        Delay_ms(0x05);//	��Ҫһ��5ms�ĵ͵�ƽ
	        GPIO_SetBits(BT_Port_WakeUP,BT_PIN_WakeUP);//�ø߸�����
			while(eBluetooth_GetBluetoothState() != BLUETOOTH_LINK_WO);				
		}
		else
		{
			if((eBluetooth_GetBluetoothState() != BLUETOOTH_LINK_WO)&&(eBluetooth_GetBluetoothState() != BLUETOOTH_LINK_WITH))
			{	
				return 1;
			}
			return 0;
		}
	}
	else
	{
		return 0;
	}
		
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_Init(void)
{
	if(Bluetooth_BM77==Bluetooth_Type())
    		vBluetooth_GPIO_Config();
	else
		vBluetooth_IVT_GPIO_Config();
	
    vBluetooth_USART_Config();
	vBluetooth_EXTILine_Config();
 //   vBluetooth_SetBluetoothMode(BLUETOOTH_APP);      
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
void vBluetooth_SendData(unsigned char* pucBuf,unsigned int uiLen)
{
	if(vBluetooth_Force_BT_Into_WeakUp())//�����������ӣ�ͨ��WEAKUP������ʵ�֣�
	{
		return;
	}
	//��֪������������RX_IND�õͣ��ڵ͵�ƽ�ڼ䷢�����ݣ����������ݺ��øߡ�
	GPIO_ResetBits(BT_Port_RxInd, BT_PIN_RxInd);//set GPIOB.3=1
    Delay_ms(0x03);//	��Ҫһ��2ms�ĵ͵�ƽ
    vBluetooth_UART_SendData(pucBuf, uiLen);	
	GPIO_SetBits(BT_Port_RxInd, BT_PIN_RxInd);//set GPIOB.3=1
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_Update_EEPROM(unsigned char *pucSendBuf, unsigned int uiSendLen,
							unsigned char *pucRevBuf, unsigned int uiRevLen)
{
    unsigned char ucSta = 0;
	//�����ж��Ƿ��Ѿ�����
	USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
    vBluetooth_UART_SendData(pucSendBuf, uiSendLen);

	ucSta = iBluetooth_RecvData(pucRevBuf, uiRevLen);

	USART_ITConfig(BT_Port_UART, USART_IT_RXNE, ENABLE);	

    return ucSta;	
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_Read_EEPROM(unsigned char *pucTexbuf, unsigned char *pucAddrbuf, unsigned char uclen)
{
     unsigned char ucBuffer_Send[32] = {0x00};
	 unsigned char ucSlen = 0;
     unsigned char ucRlen = 0;
     unsigned char ucRet = 0;
	 ucBuffer_Send[0] = 0x01;
	 ucBuffer_Send[1] = 0x29;
	 ucBuffer_Send[2] = 0xFC;
	 ucBuffer_Send[3] = 0x03;
	 ucBuffer_Send[4] = pucAddrbuf[0];
	 ucBuffer_Send[5] = pucAddrbuf[1];
	 ucBuffer_Send[6] = uclen;
	 ucSlen = 0x07;
     ucRlen = 0x0A + ucBuffer_Send[6]; 
	 ucRet = iBluetooth_Update_EEPROM(ucBuffer_Send, ucSlen, pucTexbuf, ucRlen);
     return ucRet;
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_Write_EEPROM(unsigned char *pucTexbuf, unsigned char *pucAddrbuf, unsigned char uclen)
{
	unsigned char ucBuffer_Send[264] = {0x00};
    unsigned char ucBuffer_Recv[32] = {0x00};
	unsigned int ucSlen = 0;
	unsigned char ucRlen = 0;
	unsigned char i = 0;
    unsigned char ucRet = 0;
	ucBuffer_Send[0] = 0x01;
	ucBuffer_Send[1] = 0x27;
	ucBuffer_Send[2] = 0xFC;
	ucBuffer_Send[3] = uclen+3;
	ucBuffer_Send[4] = pucAddrbuf[0];
	ucBuffer_Send[5] = pucAddrbuf[1];
	ucBuffer_Send[6] = uclen;
    for(i = 0;i < uclen;i++)
	{
		ucBuffer_Send[7+i] = pucTexbuf[i]; 
    }
	
	ucSlen = ucBuffer_Send[3] + 4;
	ucRlen = 0x07; 
	ucRet = iBluetooth_Update_EEPROM(ucBuffer_Send, ucSlen, ucBuffer_Recv, ucRlen);
    if(ucRet == 0)
    {
        if(ucBuffer_Recv[6] != 0)
		{
            ucRet = 1;
		}
		else{}
    }
	else{}        
    return ucRet;    
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_ChangeBTName(unsigned char *pucBuf, unsigned char ucLen)
{
    unsigned char ucRet = 0;
    unsigned char AADRbuffer[2] = {0x00,0x0b};
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		    vBluetooth_SetBluetoothMode(BLUETOOTH_EEPROM_CONFIG);
	    ucRet = iBluetooth_Write_EEPROM(pucBuf, AADRbuffer, ucLen);
	    if(ucRet != 0)
		{
	        return ucRet;
		}
	/*
		else{}
	    for(ucRet = 0;ucRet < ucLen; ucRet++)
	    {
	        pucBuf[ucRet] = 0;
	    }
	    ucRet = iBluetooth_Read_EEPROM(pucBuf, AADRbuffer, ucLen);
	    if(ucRet == 0)
	    {
	        for(ucRet = 0; ucRet < ucLen; ucRet++)
	        {
	            pucBuf[ucRet] = pucBuf[ucRet + 10];
	        }
	    }
	  */
	    vBluetooth_SetBluetoothMode(BLUETOOTH_APP);
	}
	else
	{
		unsigned char CMD[30];
		unsigned int CMDlen = 0;
		unsigned int iRet;
		
		memcpy(CMD, "AT+NAME=", 8);
		CMDlen += 8;
		
		memcpy(CMD+CMDlen, pucBuf, ucLen);
		CMDlen += ucLen;

		CMD[CMDlen] = 0x0d;
		CMDlen ++;
		vBluetooth_LinkDrop();
		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
		
		vBluetooth_UART_SendData(CMD, CMDlen);

		iRet = iBluetooth_RecvData(CMD, 6);

		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, ENABLE);	

		return iRet;
	}
    return ucRet;    
}

//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
int iBluetooth_GETBTVersion(unsigned char *buf)
{
    unsigned char ucRet = 0;
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		memcpy(buf,"BM77",4);
		 return 0;
	}
	else
	{
		unsigned char CMD[32];
		unsigned int CMDlen = 0;
		
		memcpy(CMD, "AT+GVER\r", 8);
		CMDlen += 8;
		
		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
		
		vBluetooth_UART_SendData(CMD, CMDlen);

		ucRet = iBluetooth_RecvData(CMD, 31);

		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, ENABLE);	
		memcpy(buf,CMD+11,16);
	}
    return ucRet;    
}
//***************************************************************************************
//* Function Name  : 
//* Description    : /*add by Roc.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
BLUETOOTH_STATUS eBluetooth_RecvAndAnalyze(unsigned char * pucBuf)
{
    unsigned char ucRet = 0;
    unsigned int uiLen = 0;
	static unsigned int len=0;
	static unsigned char index=0;
	g_sBluetoothMessage.pucInData = pucBuf + 9;
	    
    ucRet = iBluetooth_RecvData(g_sBluetoothMessage.ucbHead, 2);
    if(ucRet != 0) 
    {
        return BLUETOOTH_RECV_NO_DATA;           
    }
	
	if((g_sBluetoothMessage.ucbHead[0] != 0x55) || (g_sBluetoothMessage.ucbHead[1] != 0xAA))
	{
		
        return BLUETOOTH_RECV_HEADER_ERROR;	
	}
	
	ucRet = iBluetooth_RecvData(&(g_sBluetoothMessage.ucbFrameIndex), 1);
    if(ucRet != 0)
    {  
		    
        return BLUETOOTH_RECV_NO_DATA;
    } 

	ucRet = iBluetooth_RecvData(g_sBluetoothMessage.ucbFrameLen, 2);
    if(ucRet != 0)
    {  
		    
        return BLUETOOTH_RECV_NO_DATA;
    } 
	uiLen = ((g_sBluetoothMessage.ucbFrameLen[0] &0x7f)<< 8) | g_sBluetoothMessage.ucbFrameLen[1];
	if(uiLen > 1280)
	{		
        return BLUETOOTH_RECV_LENGTH_ERROR;		
	}

	 g_sBluetoothMessage.uiLen=uiLen;
	ucRet = iBluetooth_RecvData(&(g_sBluetoothMessage.ucType), 1);
    if(ucRet != 0)
    {  
		    
        return BLUETOOTH_RECV_NO_DATA;
    } 

	ucRet = iBluetooth_RecvData(&(g_sBluetoothMessage.ucIndex), 1);
    if(ucRet != 0)
    {
		      
        return BLUETOOTH_RECV_NO_DATA;
    } 

	ucRet = iBluetooth_RecvData(g_sBluetoothMessage.ucbLen, 2);
    if(ucRet != 0)
    {
		
        return BLUETOOTH_RECV_NO_DATA;          
    }
	
	g_sBluetoothMessage.uibaglen = (g_sBluetoothMessage.ucbLen[0] << 8) | g_sBluetoothMessage.ucbLen[1];
	if(uiLen > 3072)
	{
		
        return BLUETOOTH_RECV_LENGTH_ERROR;		
	}
	
	uiLen-=4;//��ȥ������/����
	ucRet = iBluetooth_RecvData(g_sBluetoothMessage.pucInData, uiLen);
    if(ucRet != 0)
    {
		
        return BLUETOOTH_RECV_NO_DATA;          
    }
	
	ucRet = iBluetooth_RecvData(&(g_sBluetoothMessage.ucXOR), 1);
    if(ucRet != 0)
    {
		      
        return BLUETOOTH_RECV_NO_DATA;
    } 
    else{}
	
	ucRet = iBluetooth_RecvData(g_sBluetoothMessage.ucbTail, 2);
    if(ucRet != 0)
    {
		
        return BLUETOOTH_RECV_NO_DATA;          
    }
    else{}
	
	if((g_sBluetoothMessage.ucbTail[0] != 0xCC) || (g_sBluetoothMessage.ucbTail[1] != 0x33))
	{
		
        return BLUETOOTH_RECV_TAIL_ERROR;	
	}
	else
	{
	}	
	//add index, len, cmd, lrc,
#if 1
	memcpy(pucBuf,&g_sBluetoothMessage.ucbHead,9);
	i1ByteXOR(pucBuf + 2, g_sBluetoothMessage.uiLen+3,&ucRet);
	if(ucRet != g_sBluetoothMessage.ucXOR)
	{
        return BLUETOOTH_RECV_XOR_ERROR;              
   	 }
#endif
	if(g_sBluetoothMessage.ucType== 0x51)
	{	
		if(g_sBluetoothMessage.ucbFrameIndex == 0)
		{
	
			len=0;	
			memcpy(g_sBluetoothMessage.ucallbagdata, g_sBluetoothMessage.pucInData, uiLen);
			len+=uiLen;			
		}
		else
		{
			if(index==g_sBluetoothMessage.ucbFrameIndex)//�ظ����İ�
			{
				len-=uiLen;
			}
			memcpy(g_sBluetoothMessage.ucallbagdata + len, g_sBluetoothMessage.pucInData, uiLen);
			len+= uiLen ;		
		}
		index=g_sBluetoothMessage.ucbFrameIndex;
		//recv all?
		if(g_sBluetoothMessage.uibaglen == len)
		{
			return BLUETOOTH_RECV_SUCEESCE;
		}
		else
		{
			return BLUETOOTH_RECV_FRAME_SUCCESS;
		}
	}
	else if(g_sBluetoothMessage.ucType== 0x58)
	{
		return BLUETOOTH_RECV_ACK_SUCCESS;
	}
	else if(g_sBluetoothMessage.ucType== 0x59)
	{
		return BLUETOOTH_RECV_NAK_SUCCESS;
	}	
	

	
	return BLUETOOTH_RECV_SUCEESCE;
}

int iBluetooth_Send3TimesForTimeout(unsigned char * pucData, unsigned int uiLen)
{
	unsigned int uiLoop;
	g_eBluetoothRecvStatus=BLUETOOTH_RECV_NO_DATA;
	for(uiLoop = 0; uiLoop < 1; uiLoop++)
	{
		vBluetooth_SendData(pucData, uiLen);
		g_uiTime100ms_BTRecvTimeout = 30;
		g_BluetoothDateIN=2;
		while(1)
		{	
			if(g_eBluetoothRecvStatus == BLUETOOTH_RECV_ACK_SUCCESS)
			{
				return g_eBluetoothRecvStatus;
			}
			else{}

			if(g_uiTime100ms_BTRecvTimeout == 0)
			{
				break;
			}
			if(g_BluetoothDateIN==1)//���������յ����ݣ�����һ����ACK
			{
				return 1;
			}
		}
	}
	return (1);	
}
int iBluetooth_AnalyzeAndSend()
{
	unsigned int uiOffset;
	int iRet;

	unsigned int uiLen;
	unsigned char pucBuf[1280];

	g_sBluetoothMessage.pucOutData = pucBuf + 9;
	uiLen=g_sBluetoothMessage.uibaglen;
	
	if(uiLen> 3072)
	{
		return (0x71);
	}	
	else{}

	if(g_sBluetoothMessage.ucbStatus== 0)
	{
		g_sBluetoothMessage.ucType = 0x41;
	}
	else
	{
		g_sBluetoothMessage.ucType = 0x45;
	   	g_sBluetoothMessage.uibaglen= 3;
	}
	
	g_sBluetoothMessage.ucbLen[0] = (g_sBluetoothMessage.uibaglen) / 256;	
	g_sBluetoothMessage.ucbLen[1] = (g_sBluetoothMessage.uibaglen) % 256;
	if(g_sBluetoothMessage.uibaglen > 1024)
	{		
		uiOffset = 0;
		g_sBluetoothMessage.ucbFrameIndex = 0;
		while(g_sBluetoothMessage.uibaglen)
		{
			if(g_sBluetoothMessage.uibaglen > 1024)
			{
				g_sBluetoothMessage.ucbFrameIndex++;
				g_sBluetoothMessage.ucbFrameLen[0] = ((1024 + 4) / 256) | 0x80;	
				g_sBluetoothMessage.ucbFrameLen[1] = (1024 + 4) % 256;
				memcpy(pucBuf, g_sBluetoothMessage.ucbHead, 9);
				memcpy(pucBuf + 9, g_sBluetoothMessage.ucallbagdata+ uiOffset, 1024);
				g_sBluetoothMessage.ucXOR = 0;
				i1ByteXOR(pucBuf + 2, 7 + 1024, &(g_sBluetoothMessage.ucXOR));	
				pucBuf[9 + 1024] = g_sBluetoothMessage.ucXOR;
				pucBuf[9 + 1024 + 1] = 0xCC;
				pucBuf[9 + 1024 + 2] = 0x33;
				uiLen = 9 + 1024 + 3;
				uiOffset += 1024;
				g_sBluetoothMessage.uibaglen -= 1024;
			}
			else
			{
				g_sBluetoothMessage.ucbFrameIndex++;
				g_sBluetoothMessage.ucbFrameLen[0] = ((g_sBluetoothMessage.uibaglen + 4) / 256);	
				g_sBluetoothMessage.ucbFrameLen[1] = (g_sBluetoothMessage.uibaglen + 4) % 256;
				memcpy(pucBuf, g_sBluetoothMessage.ucbHead, 9);
				memcpy(pucBuf + 9, g_sBluetoothMessage.ucallbagdata + uiOffset, g_sBluetoothMessage.uibaglen);
				g_sBluetoothMessage.ucXOR = 0;
				i1ByteXOR(pucBuf + 2, 7 + g_sBluetoothMessage.uibaglen, &(g_sBluetoothMessage.ucXOR));	
				pucBuf[9 + g_sBluetoothMessage.uibaglen] = g_sBluetoothMessage.ucXOR;
				pucBuf[9 + g_sBluetoothMessage.uibaglen + 1] = 0xCC;
				pucBuf[9 + g_sBluetoothMessage.uibaglen + 2] = 0x33;
				uiLen = 9 + g_sBluetoothMessage.uibaglen + 3;
				g_sBluetoothMessage.uibaglen = 0;	
			}
			//send data
			iRet = iBluetooth_Send3TimesForTimeout(pucBuf, uiLen);	
			if(iRet != 0)
			{
				return iRet;	
			}
			else{}
		}
	}
	else
	{
		g_sBluetoothMessage.ucbFrameIndex = 0;
		g_sBluetoothMessage.ucbFrameLen[0] = (g_sBluetoothMessage.uibaglen + 4) / 256;	
		g_sBluetoothMessage.ucbFrameLen[1] = (g_sBluetoothMessage.uibaglen + 4) % 256;
		g_sBluetoothMessage.ucXOR = 0;
		memcpy(pucBuf, g_sBluetoothMessage.ucbHead, 9);
		memcpy(pucBuf + 9,g_sBluetoothMessage.ucallbagdata,g_sBluetoothMessage.uibaglen);
		i1ByteXOR(pucBuf+2, g_sBluetoothMessage.uibaglen + 7, &(g_sBluetoothMessage.ucXOR));
		g_sBluetoothMessage.pucOutData[g_sBluetoothMessage.uibaglen] = g_sBluetoothMessage.ucXOR;
		g_sBluetoothMessage.pucOutData[g_sBluetoothMessage.uibaglen + 1] = 0xCC;
		g_sBluetoothMessage.pucOutData[g_sBluetoothMessage.uibaglen + 2] = 0x33;
		uiLen = 9 + g_sBluetoothMessage.uibaglen + 3;	
		//send data
		iRet = iBluetooth_Send3TimesForTimeout(pucBuf, uiLen);
		if(iRet != 0)
		{
			return iRet;	
		}
		else{}	
	}
	return (0);
}

#if 0
unsigned char UI_init[]={
0xD2,0x19,0x94,0x52,0xDE,0x8C,0x01,0x0c,0x02,0x58,0xBF,0x55,0x50,0x2D,0x6D,0x50,
0x4F,0x53,0x2D,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x10,0x00,0x21,0x00,0xAE,
0x01,0x03,0x0B,0xBF,0x06,0xF6,0x7E,0xC8,0x1D,0x19,0x87,0x02,0x04,0x03,0x04,0x07,
0x04,0x0D,0x04,0x18,0x04,0x1C,0x03,0x00,0x00,0x52,0x5A,0xBA,0xD8,0xE2,0xED,0xF0,
0x00,0x00,0x00,0x22,0x44,0x10,0x11,0x44,0x65,0x00,0x01,0x30,0x30,0x30,0x30,0x09,
0x75,0xBB,0x81,0xD4,0xDA,0x5C,0xF2,0x47,0xE4,0x87,0x9F,0x60,0xEB,0x31,0xB6,0x13,
0xFC,0x29,0x7F,0x6E,0x32,0xDF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
0x00,0x01,0x00,0x00,0x08,0x01,0x00,0x01,0x03,0x20,0x04,0x00,0x7D,0x00,0x0C,0xFF,
0xFF,0x01,0x00,0x2E,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x08,0x00,0x08,0x00,0x00,

0x01,0x02,0x1F,0x40,0x00,0x0A,0x04,0x01,0xBA,0xB0,0x00,0x08,0x01,0x90,0x00,0x02,
0x02,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0xB9,0x01,0xC7,0x18,0x10,0x00,0x22,0x05,
0x05,0x05,0x00,0x05,0x05,0x00,0x00,0x01,0x01,0x01,0x00,0x02,0x01,0x00,0x00,0x03,
0x02,0x03,0x00,0x04,0x02,0x00,0x00,0x01,0x03,0x02,0x00,0x03,0x01,0x00,0x00,0x3C,
0x1E,0x1E,0x00,0x1E,0x1E,0x00,0x00,0x0A,0x53,0x65,0x72,0x69,0x61,0x6C,0x50,0x6F,
0x72,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x43,0x36,0x50,0x36,0x34,0x4A,0x32,0x4D,
0x5A,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x42,0x54,0x35,0x30,0x35,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x43,0x44,0x45,0x35,0x32,0x39,0x34,0x31,0x39,0x44,0x32,0x00,0x00,
0x00,0x00,0x00,0x39,0x50,0x50,0x01,0x20,0x00,0x01,0x43,0x68,0x61,0x74,0x62,0x6F,
0x61,0x72,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,
0x49,0x53,0x53,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x42,0x4D,0x37,0x37,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x38,0x43,0x44,0x45,0x35,0x32,0x39,0x34,0x31,0x39,0x44,0x32,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0x00,0x06,0x01,0x03,0x01,0x00,0x04,0x09,0x06,0x07,0x08,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0x55,
0x03,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x06,0xAE,


0x04,0x18,0x00,0x15,0x11,0x11,0x11,0x11,0x40,0x00,0x28,0x9A,0x07,0x01,0x0A,0x30,
0x36,0x55,0x40,0x00,0x0F,0x08,0xB0,0xF0,0x1F,0x00,0x0F,0xFF,0x17,0x28,0xF7,0x66,
0xF5,0x44,0x56,0x44,0xE1,0x24,0xC0,0x32,0x58,0xC0,0x3E,0x80,0x00,0x04,0x3F,0xF4,
0x00,0x02,0x10,0x03,0x5F,0x10,0x7F,0x00,0x55,0x00,0x40,0x00,0x6B,0xA0,0x00,0x40,
0x24,0x44,0xF7,0x66,0x0C,0x90,0x0B,0x33,0x68,0x8C,0x0D,0xCF,0x0C,0x00,0xF5,0x44,
0x56,0x44,0x0C,0x03,0xF0,0x7A,0x42,0x10,0x9A,0xA1,0x24,0x44,0xB0,0xF0,0xB0,0xF0,
0xB0,0xF0,0x21,0x01,0x8F,0x80,0x00,0x00,0x00,0xA1,0x0B,0x01,0xD6,0x12,0xC5,0x44,
0x55,0x44,0x00,0x22,0x00,0x26,0x00,0x9C,0x88,0x00,0x00,0x00,0x4E,0xCD,0xCC,0x30,
0x44,0x40,0x30,0x00,0x38,0x10,0x21,0x24,0x28,0x42,0x52,0x5E,0x9D,0x4B,0x86,0x00,
0x9B,0x05,0x31,0xCB,0x0F,0x63,0x10,0x30,0x11,0x13,0x1B,0x11,0x2A,0x24,0x64,0x11,
0x20,0x0F,0x1E,0x46,0x87,0x20,0xA0,0x50,0x62,0x08,0x01,0x0D,0x10,0x80,0x01,0x72,
0x6D,0x02,0x58,0x00,0x02,0x80,0x46,0x03,0x60,0x02,0x30,0x01,0x00,0x0C,0x01,0xE1,
0x07,0xAC,0x0C,0x13,0x00,0x00,0x0B,0x27,0x02,0x03,0x02,0x02,0x05,0x00,0x04,0x04,


0xB5,0x99,0x01,0x69,0x96,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0x55,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x63,0x6F,0x6D,0x2E,0x69,0x73,0x73,0x63,0x2E,0x64,0x61,0x74,0x61,0x70,0x61,0x74,
0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x63,0x6F,0x6D,0x2E,0x69,0x73,0x73,0x63,0x2D,0x74,0x65,0x63,0x68,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x49,0x53,0x53,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x42,0x4D,0x37,0x37,0x10,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x04,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x42,0x4D,0x37,0x37,0x53,0x50,0x50,0x30,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
int vBluetooth_UI_Init(void)
{
    unsigned char ucRet = 0;
    unsigned short addr=0;
	unsigned char addrbuf[2];
	unsigned char i;
	unsigned int len,ucLen=1024;
	unsigned int offset=0;
	vBluetooth_SetBluetoothMode(BLUETOOTH_EEPROM_CONFIG);
	while(ucLen)
	{
		len=ucLen>252?252:ucLen;
		addrbuf[0]=addr>>8;
		addrbuf[1]=addr&0xff;
		ucRet = iBluetooth_Write_EEPROM(UI_init+offset,addrbuf, len);
	    if(ucRet != 0)
		{
	     		break;
		}
		addr+=len;
		ucLen-=len;
		offset+=len;
	} 

    vBluetooth_SetBluetoothMode(BLUETOOTH_APP);
       return ucRet;
}
#else

int vBluetooth_UI_Init(void)
{
	if(Bluetooth_BM77==Bluetooth_Type())
	{
		unsigned char ucRet = 0;
		unsigned char AADRbuffer[2] = {0x00,0x07};
		unsigned char type[3]={0x58,0x02,0x0c};
	//	unsigned char type[3]={0x24,0x04,0x04};
		unsigned char rate[8]={0x00,0x08,0x00,0x10,0x00,0x04,0x00,0x02};
		vBluetooth_SetBluetoothMode(BLUETOOTH_EEPROM_CONFIG);

		ucRet = iBluetooth_Write_EEPROM(type, AADRbuffer, 3);//�޸�����Class
		if(ucRet != 0)
		{
	        return ucRet;
		}
		AADRbuffer[1]=0xDA;
		ucRet = iBluetooth_Write_EEPROM(rate, AADRbuffer, 8);//�޸�BLEͨѶ�ٶ�
		if(ucRet != 0)
		{
	        return ucRet;
		}
		 vBluetooth_SetBluetoothMode(BLUETOOTH_APP);
		 return ucRet;
	}
	else
		return 0;
	
}

#endif
void vBluetooth_Reset(void)
{
	if(Bluetooth_BM77==Bluetooth_Type())
		return ;
	else
	{
		GPIO_SetBits(BT_Port_RST,BT_PIN_RST);
		Delay_ms(2);
		GPIO_ResetBits(BT_Port_RST,BT_PIN_RST);
		Delay_ms(10);
		GPIO_SetBits(BT_Port_RST,BT_PIN_RST);
		Delay_ms(10);
	}
}
//***************************************************************************************
//* Function Name  : vBluetooth_Update()
//* Description    : /*add by lihuan.*/ 
//*       
//* Input          : 
//* Output         : None
//* Return         : None
//***************************************************************************************
#define	IVT_ERR_GETADDR		1
#define	IVT_ERR_GETSETTINGS	2
#define	IVT_ERR_ENTERBOOT		3
#define	IVT_ERR_CMD_GET		4
#define	IVT_ERR_CMD_READOUTUNPROTECT		5
#define	IVT_ERR_CMD_EXTENDERASE		6
#define	IVT_ERR_CMD_EXTENDERASE1		7
#define	IVT_ERR_CMD_WRITE		8
#define	IVT_ERR_CMD_READOUTPROTECT		9

#define ST_INIT						0x7F
#define ST_GET						0x00
#define ST_WRITE						0x31
#define ST_EXTENDERASE				0x44
#define ST_READOUT_PROTECT			0x82
#define ST_READOUT_UNPROTECT		0x92
#define ST_ACK		0x79

#define ST_MAX_LEN 0X100
void IVT_UPDATE_USART_Config(void)
{
  USART_InitTypeDef	 USART_InitStructure;
  //USART_Cmd(BT_Port_UART, DISABLE);	
  USART_DeInit(BT_Port_UART);
  /* ---------------------------------------------------------------
    USART Configuration: 
    FPCLK = 36 MHz, Baud rate =36000,MODE: RX & TX
  --------------------------------------------------------------- */
  USART_InitStructure.USART_BaudRate            = 115200;
  USART_InitStructure.USART_WordLength          = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_Even ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
 
  USART_Init(BT_Port_UART, &USART_InitStructure);
  USART_Cmd(BT_Port_UART, ENABLE);
}

char IVT_ExpectRec(unsigned char *buf,unsigned char len)
{
	if(iBluetooth_RecvData(buf,len))
	{
		return 1;
	}
	if(buf[0]!=ST_ACK)
	{
		return 1;
	}
	else
		return 0;
}
char ST_CheckSum(unsigned char *buf,unsigned short len)
{
	unsigned short i;
	unsigned char ret=0;
	for(i=0;i<len;i++)
	{
		ret^=buf[i];
	}
	return ret;
}
#if 0
char vBluetooth_IVT_Update(void)
{
	unsigned char CMD[300];
	unsigned int CMDlen = 0;
	unsigned int iRet;

	static unsigned char bt_addr[12];
	static unsigned char bt_settings[224];
	unsigned char *str_setting;

	unsigned int  loadaddr=0x8000000;
	unsigned char uchfilelen[4];
	unsigned int filelen,offset,len;
	
	
	/**********������*************/
	//USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
	__set_PRIMASK(1);
	#if 1
	memcpy(CMD,  "AT+LBDADDR?\r", 12);
	CMDlen = 12;	
	
	vBluetooth_UART_SendData(CMD, CMDlen);

 	if(iBluetooth_RecvData(CMD, 31))
 	{
 		return IVT_ERR_GETADDR;
 	}
	else
	{
		memcpy(bt_addr,CMD+11,12);
	}
	memcpy(CMD,  "AT+SETTINGS?\r", 13);
	CMDlen = 13;	
	
	vBluetooth_UART_SendData(CMD, CMDlen);
	iBluetooth_RecvData(CMD,(sizeof(struct IVT_Settings_t)*2)+18);
	str_setting = (unsigned char *)strstr(CMD, "+SETTINGS:");
	if(str_setting==NULL)
	{
		return IVT_ERR_GETSETTINGS;
	}
 	else
 	{
 		str_setting += strlen("+SETTINGS:");
		memcpy(bt_settings,str_setting,sizeof(struct IVT_Settings_t)*2);
 	}
	#endif
	/**********����ģ��*************/

	IVT_UPDATE_USART_Config();
	vBluetooth_SetBluetoothMode(BLUETOOTH_BOOT);
	vBluetooth_Reset();

	CMD[0]=ST_INIT;
	vBluetooth_UART_SendData(CMD,1);
	if(IVT_ExpectRec(CMD,1))
	{
		return IVT_ERR_ENTERBOOT;
	}

	CMD[0]=ST_GET;
	CMD[1]=0xff;
	vBluetooth_UART_SendData(CMD,2);
	if(IVT_ExpectRec(CMD,15))
	{
		return IVT_ERR_CMD_GET;
	}

	CMD[0]=ST_READOUT_UNPROTECT;
	CMD[1]=~ST_READOUT_UNPROTECT;
	vBluetooth_UART_SendData(CMD,2);
	if(IVT_ExpectRec(CMD,2))
	{
		return IVT_ERR_CMD_READOUTUNPROTECT;
	}

	vBluetooth_Reset();
	CMD[0]=ST_INIT;
	vBluetooth_UART_SendData(CMD,1);
	if(IVT_ExpectRec(CMD,1))
	{
		return IVT_ERR_ENTERBOOT;
	}
	//ERASE 
	CMD[0]=ST_EXTENDERASE;
	CMD[1]=~ST_EXTENDERASE;
	vBluetooth_UART_SendData(CMD,2);
	if(IVT_ExpectRec(CMD,1))
	{
		return IVT_ERR_CMD_EXTENDERASE;
	}

	CMD[0]=0xff;
	CMD[1]=0xff;
	CMD[2]=0xff^0xff;
	vBluetooth_UART_SendData(CMD,3);
	if(IVT_ExpectRec(CMD,1))
	{
		return IVT_ERR_CMD_EXTENDERASE1;
	}

	offset=0;
	len=0;
	IROM_Read(BOOTLOADER_LEN_OF_FILE_ADDR, uchfilelen, 4);
	filelen=(uchfilelen[0]<<24)+(uchfilelen[1]<<16)+(uchfilelen[2]<<8)+uchfilelen[3];;
	while(filelen)
	{
		len=ST_MAX_LEN<filelen?ST_MAX_LEN:filelen;

		CMD[0]=ST_WRITE;
		CMD[1]=~ST_WRITE;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}

		CMD[0]=loadaddr>>24;
		CMD[1]=loadaddr>>16;
		CMD[2]=loadaddr>>8;
		CMD[3]=loadaddr&0xff;
		CMD[4]=ST_CheckSum(CMD,4);
		vBluetooth_UART_SendData(CMD,5);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}

		CMD[0]=len-1;
		MSD_ReadBuffer(CMD+1,SPI_FLASH_FIRMWARE_ADDR+offset,len);
		CMD[len+1]=ST_CheckSum(CMD,len+1);
		vBluetooth_UART_SendData(CMD,len+2);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}
		loadaddr+=len;
		offset+=len;
		filelen-=len;
		
	}

	CMD[0]=ST_READOUT_PROTECT;
	CMD[1]=~ST_READOUT_PROTECT;
	vBluetooth_UART_SendData(CMD,2);
	if(IVT_ExpectRec(CMD,2))
	{
		return IVT_ERR_CMD_READOUTPROTECT;
	}
	
	vBluetooth_USART_Config();
	vBluetooth_SetBluetoothMode(BLUETOOTH_APP);
	vBluetooth_Reset();
	if(iBluetooth_RecvData(CMD, 12))//im ready
 	{
 		return IVT_ERR_GETADDR;
 	}
	/**********д����*************/
	#if 1
	memcpy(CMD,  "AT+LBDADDR=", 11);
	CMDlen = 11;	
	memcpy(CMD+CMDlen,bt_addr,12);
	CMDlen+=12;
	CMD[CMDlen]='\r';
	CMDlen++;
	vBluetooth_UART_SendData(CMD, CMDlen);

 	if(iBluetooth_RecvData(CMD, 6))
 	{
 		return IVT_ERR_GETADDR;
 	}

	memcpy(CMD,  "AT+SETTINGS=", 12);
	CMDlen = 12;	

	memcpy(CMD+CMDlen,bt_settings,(sizeof(struct IVT_Settings_t)*2));
	CMDlen+=sizeof(struct IVT_Settings_t)*2;
	CMD[CMDlen]='\r';
	CMDlen++;
	vBluetooth_UART_SendData(CMD, CMDlen);
	if(iBluetooth_RecvData(CMD, 6))
 	{
 		return IVT_ERR_GETADDR;
 	}
	#endif
	__set_PRIMASK(0);
}
#else
char IVT_Update(unsigned char *pIN,unsigned short ulen)
{
	unsigned char flag,CMD[300];
	unsigned int CMDlen = 0;

	static unsigned char bt_addr[12];
	static unsigned char bt_settings[224];
	unsigned char *str_setting;
	unsigned short len;

	if(Bluetooth_BM77==Bluetooth_Type())
	{
		return 0xff;
	}
	if(pIN[0]==1)//������
	{
		vLCD_GUI_ShowMid12X12("����������...",3);
		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
		//__set_PRIMASK(1);

		IROM_Read(BT_IVT_CONF_FLAG,&flag,1);

		if(flag==0x55)
		{
			memcpy(bt_addr,(unsigned char *)BT_IVT_ADDR,12);
			memcpy(bt_settings,(unsigned char *)BT_IVT_PARA,sizeof(struct IVT_Settings_t)*2);
		}
		else
		{
			memcpy(CMD,  "AT+LBDADDR?\r", 12);
			CMDlen = 12;	
			
			vBluetooth_UART_SendData(CMD, CMDlen);

		 	if(iBluetooth_RecvData(CMD, 31))
		 	{
		 		return IVT_ERR_GETADDR;
		 	}
			else
			{
				memcpy(bt_addr,CMD+11,12);
				IROM_Write(BT_IVT_ADDR, bt_addr, 12);
			}
			memcpy(CMD,  "AT+SETTINGS?\r", 13);
			CMDlen = 13;	
			
			vBluetooth_UART_SendData(CMD, CMDlen);
			iBluetooth_RecvData(CMD,(sizeof(struct IVT_Settings_t)*2)+18);
			str_setting = (unsigned char *)strstr(CMD, "+SETTINGS:");
			if(str_setting==NULL)
			{
				return IVT_ERR_GETSETTINGS;
			}
		 	else
		 	{
		 		str_setting += strlen("+SETTINGS:");
				memcpy(bt_settings,str_setting,sizeof(struct IVT_Settings_t)*2);
				IROM_Write(BT_IVT_PARA, bt_settings, sizeof(struct IVT_Settings_t)*2);
		 	}
			flag=0x55;
			IROM_Write(BT_IVT_CONF_FLAG,&flag,1);
		}
		IVT_UPDATE_USART_Config();
		vBluetooth_SetBluetoothMode(BLUETOOTH_BOOT);
		vBluetooth_Reset();

		CMD[0]=ST_INIT;
		vBluetooth_UART_SendData(CMD,1);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_ENTERBOOT;
		}

		CMD[0]=ST_GET;
		CMD[1]=0xff;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,15))
		{
			return IVT_ERR_CMD_GET;
		}

		CMD[0]=ST_READOUT_UNPROTECT;
		CMD[1]=~ST_READOUT_UNPROTECT;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,2))
		{
			return IVT_ERR_CMD_READOUTUNPROTECT;
		}

		vBluetooth_Reset();
		CMD[0]=ST_INIT;
		vBluetooth_UART_SendData(CMD,1);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_ENTERBOOT;
		}
		//ERASE 
		CMD[0]=ST_EXTENDERASE;
		CMD[1]=~ST_EXTENDERASE;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_EXTENDERASE;
		}

		CMD[0]=0xff;
		CMD[1]=0xff;
		CMD[2]=0xff^0xff;
		vBluetooth_UART_SendData(CMD,3);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_EXTENDERASE1;
		}
		return 0;

	}
	else if(pIN[0]==2)//����ģ��
	{
		len=ulen-5;
		
		CMD[0]=ST_WRITE;
		CMD[1]=~ST_WRITE;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}
		memcpy(CMD,pIN+1,4);//ƫ����		
		CMD[0]|=0x08;
		CMD[4]=ST_CheckSum(CMD,4);
		vBluetooth_UART_SendData(CMD,5);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}

		CMD[0]=len-1;
		memcpy(CMD+1,pIN+5,len);
		CMD[len+1]=ST_CheckSum(CMD,len+1);
		vBluetooth_UART_SendData(CMD,len+2);
		if(IVT_ExpectRec(CMD,1))
		{
			return IVT_ERR_CMD_WRITE;
		}
		return 0;
			
	}

	
	else if(pIN[0]==3)
	{
		CMD[0]=ST_READOUT_PROTECT;
		CMD[1]=~ST_READOUT_PROTECT;
		vBluetooth_UART_SendData(CMD,2);
		if(IVT_ExpectRec(CMD,2))
		{
			return IVT_ERR_CMD_READOUTPROTECT;
		}
		vBluetooth_USART_Config();
		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, DISABLE);
		vBluetooth_SetBluetoothMode(BLUETOOTH_APP);
		vBluetooth_Reset();
		if(iBluetooth_RecvData(CMD, 12))//im ready
	 	{
	 		return IVT_ERR_GETADDR;
	 	}
		/**********д����*************/
		#if 1
		memcpy(CMD,  "AT+LBDADDR=", 11);
		CMDlen = 11;	
		memcpy(CMD+CMDlen,bt_addr,12);
		CMDlen+=12;
		CMD[CMDlen]='\r';
		CMDlen++;
		vBluetooth_UART_SendData(CMD, CMDlen);

	 	if(iBluetooth_RecvData(CMD, 6))
	 	{
	 		return IVT_ERR_GETADDR;
	 	}

		memcpy(CMD,  "AT+SETTINGS=", 12);
		CMDlen = 12;	

		memcpy(CMD+CMDlen,bt_settings,(sizeof(struct IVT_Settings_t)*2));
		CMDlen+=sizeof(struct IVT_Settings_t)*2;
		CMD[CMDlen]='\r';
		CMDlen++;
		vBluetooth_UART_SendData(CMD, CMDlen);
		if(iBluetooth_RecvData(CMD, 6))
	 	{
	 		return IVT_ERR_GETADDR;
	 	}
		#endif
		USART_ITConfig(BT_Port_UART, USART_IT_RXNE, ENABLE);
		//__set_PRIMASK(0);

		flag=0xff;
		IROM_Write(FLASH_ADDR_FACTORY_INIT, &flag, 1);
		IROM_Write(BT_IVT_CONF_FLAG,&flag,1);
		vLCD_GUI_ShowMid12X12("������ɣ�������",3);

		return 0;
	}
	else
	{
		return 1;
	}
}
#endif
