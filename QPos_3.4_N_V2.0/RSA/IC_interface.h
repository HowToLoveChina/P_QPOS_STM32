#ifndef  _IC_INTERFACE_H_
#define  _IC_INTERFACE_H_

//AID_INFO
#define	FLASH_AID_ADDR	0x803b000
#define MAX_AID_COUNT	20

//RID_INFO
#define FLASH_CAPK_ADDR	0x803c000
#define MAX_CAPK_COUNT	20

typedef struct
{
	unsigned char m_AID[17];//��һ�ֽ�Ϊ����
	unsigned char m_ASI;//Ӧ��ѡ��ָʾ��
	unsigned char m_APPVer[2];//Ӧ�ð汾��
	unsigned char m_TAC_Default[5];
	unsigned char m_TAC_Onlie[5];
	unsigned char m_TAC_Denial[5];//
	unsigned char m_ThresholdAmount[4];//�ն�����޶�
	unsigned char m_ThresholdforBiasRandomSelection[4];//ƫ�����ѡ�����ֵ
	unsigned char m_MAXPercentforBiasRandomSelection;//ƫ�����ѡ���Ŀ��ٷ���
	unsigned char m_PercentforBiasRandomSelection;
	unsigned char	m_DDOL_Default[128];
	unsigned char m_OnlinePIN;
	unsigned char m_NFC_ThresholdAmount[6];
	unsigned char m_NFC_CVMm_NFCThresholdAmount[6];
	unsigned char m_NFC_OFFLineThresholdAmount[6];
	unsigned char m_ECTerminalTransLimit[6];//�ն˵����ֽ����޶�
}AID_STRUCT;

typedef struct
{
	AID_STRUCT AID_LIST[MAX_CAPK_COUNT];
}AID_LIST_STRUCT;

typedef struct
{
	unsigned char m_RID[5];
	unsigned char m_CapkIndex;
	unsigned char m_Validtime[4];
	unsigned char m_Hashalgorithm;
	unsigned char m_Publicflag;
	unsigned char m_Modules[249];//��������
	unsigned char m_Exponents[4];//��������
	unsigned char m_Checksum[20];
}CAPK;

typedef struct
{
	CAPK CAPK_LIST[MAX_CAPK_COUNT];
}CAPK_LIST_STRUCT;

unsigned char AID_GetCount(void);

void	AID_DeleteALL(void);

void	AID_DeleteAID(unsigned char *puchAID,unsigned char uchAIDLen);

unsigned char AID_Check(unsigned char *puchAID,unsigned char uchAIDLen);

AID_STRUCT* AID_GetOne(unsigned char *puchAID,unsigned char uchAIDLen);

void AID_GetInfo(unsigned char *pucOut,unsigned char *uiLen);


unsigned char CAPK_GetCount(void);

void	CAPK_DeleteALL(void);

void	CAPK_DeleteRID(unsigned char ucRID[5]);

unsigned char CAPK_Check(unsigned char uchRID[5],unsigned char uchpki);

void	CAPK_DeleteRIDandPKI(unsigned char ucRID[5],unsigned char pki);

unsigned char CAPK_ADD(unsigned char *pucIn,unsigned int uiLen);

CAPK * CAPK_GetOneKey(unsigned char ucRID[5],unsigned char pki);

void CAPK_GetInfo(unsigned char *pucOut,unsigned int *uiLen);

#endif

