#include "string.h"
#include	"APP_parameters.h"
#include "UP_MPOS_CMD.h"
#include "Bluetooth.h"
#include "IROM.h"
#include "Sercrity.h"
#include "Key.h"
#include "Buzzer.h"
#include "sercrity.h"
#include	"IC_interface.h"
#include	"lcd_gui.h"

DEVICE_VERSION_INFO *pdevice_version_info=(DEVICE_VERSION_INFO*)FLASH_DEVICE_VERSION_INFO;

const DEVICE_VERSION_INFO device_info_init=
{
	0x12345678,	
	//��ȫ��֤���	6 PP3342	
	"PP3342",
	//����������ɺ�  5	MP003
	"MP013",
	//�豸�ͺ�	8  PSC-043-A									
	"PSC043A ",
	//Ӳ���汾��   20	V000_000_001		
 	"V03                 ",
	//�̼��汾��   20	V000_000_024
	"1.1                 ",
	//�ն�Ӧ�ð汾��Դ��  2	  32
	"32",	
	//�ն�Ӧ�ð汾��  6	 410201											
	"410201",					

	"013000000000000000000000",
	//������ָʾ	1
	0, 
	//Ӳ��ģ��״̬	8 	
	{0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00}
};
const unsigned char g_cucbDevicePara[FLASH_ADDR_DEVICE_PARA_MAX]= 
{
	//����оƬ���߿��	0x00��8λ.0x01��16λ.0x02��32λ.0x03��64λ
	0xDF, 0x60, 0x01, 0x02,
	//����оƬƬ�Ϸ���ʧ�Դ洢��С 
	0xDF, 0x61, 0x04,0x00,0x00, 0x01, 0x00, 
	//����оƬƬ����ʧ�Դ洢��С
	0xDF, 0x62, 0x04, 0x00,0x00, 0x00, 0x30,  
	//�洢����С
//	0xDF, 0x63, 0x04, 0x00, 0x01, 0x00, 0x00,   
	//�����汾
	0xDF, 0x64, 0x01, 0x02,  
	//֧�ֵ�ͨѶ��ʽ
	0xDF, 0x65, 0x01, 0x03, 
	//�Ӵ�ʽIC��֧�ֵĿ�Ƭ��ѹ
	0xDF, 0x66, 0x01, 0x03, 
	//�Ӵ�ʽIC��֧�����
	0xDF, 0x67, 0x02, 0x00,0x01,
	//�ǽӴ�ʽIC��֧�����
	0xDF, 0x68, 0x01, 0x03, 
	//��дǩ��֧�����
	0xDF, 0x69, 0x01, 0x00, 
	//��ӡ��֧�����
	0xDF, 0x70, 0x01, 0x00, 
	//������
	//0xDF, 0x71, 0x01, 0x0f,
	//��Ļ����
	0xDF, 0x72, 0x01, 0x00,
	//��Ļ�ֱ���
	0xDF, 0x73, 0x04, 0x01, 0x28, 0x00, 0x64,
	//��ʾ����
	0xDF, 0x74, 0x02, 0x05, 0x20, 
	//�ֿ�֧��
	0xDF, 0x75, 0x01, 0x01, 
	//��ȫоƬ֧�ֵ��㷨
	0xDF, 0x76, 0x02, 0x07, 0x00,
	//RSA�㷨��Կ����
	0xDF, 0x77, 0x02, 0x10,0x24,
	//SHA-1�㷨��Կ����
	0xDF, 0x78, 0x02, 0x10,0x24,
	
};
//****************��psrc������д��pdestָ����ڴ�
void Update_Parameters(unsigned char *psrc,unsigned char *pdest,unsigned char len)
{
	IROM_Write((unsigned int)pdest,psrc,len);
}
unsigned char GET_APPVersion(unsigned char *pIn)
{
	unsigned char len;
	len=sizeof(pdevice_version_info->TermianalAPPVersion);
	memcpy(pIn,pdevice_version_info->TermianalAPPVersion,len);
	return len;
}

unsigned char GET_DeviceSerialNO(unsigned char *pIn)
{
	unsigned char len;
	len=sizeof(pdevice_version_info->DeviceSerialNO);
	memcpy(pIn,pdevice_version_info->DeviceSerialNO,len);
	return len;
}

void Update_DeviceSerialNO(unsigned char *pIN)
{	
	unsigned char ucbBuf[16] = "UP-013-00000000";
	
	Update_Parameters(pIN,pdevice_version_info->DeviceSerialNO,24);
	memcpy(ucbBuf + 8,pdevice_version_info->DeviceSerialNO+16, 8);	
	iBluetooth_ChangeBTName(ucbBuf, 16);
	
}
void Update_APPVersion(void)
{
	if(strcmp(device_info_init.TermianalAPPVersion,pdevice_version_info->TermianalAPPVersion)!=0)
	{
		Update_Parameters((unsigned char *)device_info_init.TermianalAPPVersion,pdevice_version_info->TermianalAPPVersion,6);
	}
}

void Input_DeviceSerialNO(void)
{
	unsigned char buf[24];
	unsigned char ucKey;
	unsigned char num;

	num=0;
	vLCD_ClearWorkPlace2345();

	vLCD_GUI_ShowMid12X12("�������к�(���Ի�)",2);
	while(1)
	{
		ucKey=ucKey_Scan(1);

		if((ucKey >= '0') && (ucKey <= '9')&&(num<24))
		{
			buf[num]=ucKey;
			if(num<20)
				vLCD_GUI_Show12X12_Ex(&ucKey,1,3,num*6);
			else
				vLCD_GUI_Show12X12_Ex(&ucKey,1,4,(num-20)*6);
			num++;
		}
		else if(ucKey=='E')
		{
			vLCD_GUI_ShowMid12X12("ȡ������",5);
			return ;
		}
		else if(ucKey=='C')
		{	
			if(num>0)
			{
				num--;
				if(num<20)
				{
					vLCD_GUI_Show12X12_Ex(" ",1,3,num*6);					
				}
				else
				{
					vLCD_GUI_Show12X12_Ex(" ",1,4,(num-20)*6);
				}				
			}						
		}
		else if(ucKey=='O')
		{
			if(num==24)
			{
				vLCD_GUI_ShowMid12X12("������",5);
				Update_DeviceSerialNO(buf);				
				vLCD_ClearWorkPage(5);
				vLCD_GUI_ShowMid12X12("���óɹ�",5);
				return;
			}
			else
			{
				vLCD_GUI_ShowMid12X12("���ȴ���",5);
			}
		}

	}
}
void Device_Init(void)
{
	unsigned char ucbTMK[20] = 
	{
		0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x82
	};
	unsigned char ucbBuf[16] = "UP-013-00000000";
	unsigned char ucRet;
	
	if(pdevice_version_info->paramInitFlag!=PARAMETER_INIT_FLAG)	//δ��ʼ��
	{
		IROM_Write(FLASH_DEVICE_VERSION_INFO,(unsigned char *)&device_info_init.paramInitFlag,sizeof(DEVICE_VERSION_INFO));

		QPOSFileStruct();
		createPublicFile(0x0006);

		ucRet = seleteBinaryFile(0x0006);	
		if(ucRet)
		{
			return ;
		}
		else{}

		ucRet = updateBinaryFile(UN_TMK_ADDR, ucbTMK, 0x10);	
		if( ucRet)
		{
			return ;
		}
		memcpy(ucbBuf + 8,pdevice_version_info->DeviceSerialNO+16, 8);	
		iBluetooth_ChangeBTName(ucbBuf, 16);	
	}
	Update_APPVersion();

	return ;
	
}

