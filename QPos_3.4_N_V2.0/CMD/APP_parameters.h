#ifndef _APP_PARAMETER_H_
#define _APP_PARAMETER_H_

//------------��������------------------------------------
#define	FLASH_DEVICE_VERSION_INFO	0x803F001
#define PARAMETER_INIT_FLAG   		0x12345678
//------------�ṹ�嶨��---------------------------------
//  ע�� �����������Ѿ��������������ã���Ӳ���ֻ�����ں�������
typedef struct
{
	unsigned int paramInitFlag;//��ʼ����־

	unsigned char SecurityAuthSerial[6];//��ȫ��֤����
	unsigned char mPOSEntrySerial[5];//MPOS�������
	unsigned char DeviceType[8];//�豸�ͺ�
	unsigned char DeviceVersion[20];
	unsigned char FirmwareVersion[20];
	unsigned char SoftVersionSource[2];
	unsigned char	TermianalAPPVersion[6];
	unsigned char DeviceSerialNO[24];
	unsigned char BatchUploadFlag;
	unsigned char	DeviceModelState[8];	
}DEVICE_VERSION_INFO;

//----------------------------�ⲿ�ӿ�---------------------------
extern DEVICE_VERSION_INFO *pdevice_version_info;
extern const DEVICE_VERSION_INFO device_info_init;
extern const unsigned char g_cucbDevicePara[];

void Update_Parameters(unsigned char *psrc,unsigned char *pdest,unsigned char len);

void Input_DeviceSerialNO(void);

unsigned char GET_APPVersion(unsigned char *pIn);

unsigned char GET_DeviceSerialNO(unsigned char *pIn);

void Update_DeviceSerialNO(unsigned char *pIN);

void Update_APPVersion(void);

void Device_Init(void);

#endif
