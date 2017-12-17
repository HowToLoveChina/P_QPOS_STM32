
#ifndef _SERCRITY_H_
#define _SERCRITY_H_

#include "Z8D256.h"

typedef union 
{
    unsigned short length;
    unsigned char lenbuffer[2];
}DataLength,*pDataLength;
typedef struct 
{
    unsigned char commandFlag;
    unsigned char commandHead;
    DataLength dataLen;
    unsigned char cla;
    unsigned char ins;
    unsigned char pp1;
    unsigned char pp2;
    unsigned char lc;
    unsigned char databuf[255];    
}CommandData,*pCommandData;

typedef struct
{  	
    unsigned char answerFlag; //Э��ͷ
	unsigned char answerHead; //Э��ͷ
	DataLength   dataLen;	
	unsigned char dataBuf[255]; //���ͽ��յ����ݻ�����
}AnswerData,*pAnswerData;
typedef struct
{
    unsigned char recvbuff[139];//���������е�����
    unsigned char restorebuff[132];//�����ȡpublicfile�е�n e
}PbulickeySt;
typedef struct
{
    unsigned char recvbuff[139];//����des������������
    unsigned char desbuffer[16];//����des���ܺ�Ľ������
}DesSt;
typedef union 
{
	  CommandData  Psamcommand;
	  unsigned char  psamcommandbuff[270];
}psamComBuffer;
typedef union 
{
	  AnswerData  Psamanswer;
	  unsigned char  psamanswerbuff[260];
}psamAnsBuffer;
typedef union
{
    PbulickeySt publicstruct;
    unsigned char bublicbuffer[271];
}PublickeyBuffer;
typedef union
{
    DesSt desStruct;
    unsigned char desDataBuffer[271];
}DesBuffer;
typedef union
{
	 psamComBuffer psamcombuf;
     psamAnsBuffer psamansBuf;
     PublickeyBuffer publickbuf;
     DesBuffer desbuf;
}psamAPDU;

typedef union
{
	unsigned short tempcrc;
	unsigned char crcbuf[2];
}CRCst;
typedef struct 
{
    unsigned char kid;
    unsigned char ktype;//��Կ����
    unsigned char kversion;//��Կ�汾
    unsigned char kArigth;//��Կ�㷨��ʶ;0x00 3DES 0x04 DES
    unsigned char kuseAc;//ʹ��Ȩ��
    unsigned char kstate;//��Կ�ĺ���״̬
    unsigned char kchangeAc;//��Կ���޸�Ȩ��
    unsigned char times;//���Դ���   
    unsigned char keybuf[16];
}keySt;
typedef union
{
    keySt keyStruct;
    unsigned char buffer[24];
}keyUion;
typedef struct 
{
    unsigned char btype;//�ļ�����
    unsigned char bFidh;//�ļ�ID��
    unsigned char bFidl;//�ļ�ID��
    unsigned char bFsizeh;//�ļ���С��
    unsigned char bFsizel;//�ļ���С��
    unsigned char bRef;//����
    unsigned char bReadAtr;//������
    unsigned char bWriteAtr;//д����  
    unsigned char bReadAc;//��Ȩ��
    unsigned char bWriteAc;//дȨ��
    unsigned char bUseAc;//ʹ��Ȩ��
    unsigned char bReadKid;//����Կ��ʶ
    unsigned char bWriteKid;//д��Կ��ʶ
    unsigned char bUseKid; //ʹ����Կ��ʶ   
}BinarySt;
typedef union
{
    BinarySt BinaryStruct;
    unsigned char buffer[24];
}BinaryUion;
typedef union
{
    keyUion keyui;
    BinaryUion binaryui;
}OptionSt;

extern psamAPDU psamapdu;
extern CRCst crcValue;

Z8D256_RECV_STATUS Z8D256_RecvString_XIEYI(unsigned char* buffer);

void LitteToBigStr(unsigned char buffer[],unsigned short len);
void sercrityTest(void);
unsigned char EraseMF(void);
unsigned char CreateMF(void);
unsigned char createKeyFile(unsigned int fileSize,unsigned char reloadAt,unsigned char reloadAc,unsigned char reloadKeyID);
unsigned char loadMainkey(void);
unsigned char loadDESkey(keyUion *pkeyui);
unsigned char createBinaryFile(BinaryUion *pbui);
void LittleToBig_short(const unsigned short in,unsigned short *pout);
//mode 1: �豸id�� 0��PSAMid
unsigned char getDeviceInfo(unsigned char*pbuff,unsigned char mode);
//�����豸��Ϣ
unsigned char setDeviceInfo(unsigned char*pbuff);
unsigned char setPSAMInfo(unsigned char*pbuff);
//����Ӧ����Կ
unsigned char setApplykey(unsigned char *pbuffer,unsigned char kid);
unsigned char setTrackkey(unsigned char *pbuffer,unsigned char kid);

//����˽Կ�ļ�
unsigned char createPrivateFile(unsigned short privateFileID);
//������Կ�ļ�
unsigned char createPublicFile(unsigned short publicFileID);
//���¹�Կ�ļ�
unsigned char updatePublickeyFile(unsigned short publicFileID);
//��ȡmacֵ
unsigned char ToGetMAC(const unsigned char buffer[],unsigned short len,unsigned char *macBuff);
//��ȡ8���ֽں�8���ֽڵ�XORֵ
unsigned char XORArigth(const unsigned char *buffer,unsigned short len,unsigned char *XORBuff);

//ѡ��������ļ�
unsigned char seleteBinaryFile(unsigned int fileID);
//���¶������ļ�
unsigned char updateBinaryFile(unsigned short addr,unsigned char *pbuffer,unsigned char len);

unsigned char ReadBinaryFile(unsigned short addr,unsigned char *pbuffer,unsigned char len);
//������Կ
unsigned char changeDESkey(unsigned char buffer[],unsigned char KID,unsigned char len);
//��DES�ӽ���
unsigned char SingleDesEntryOrDisEntry(unsigned char srcbuffer[],unsigned char len,unsigned char KID,unsigned char mode);
unsigned char SingleDesBykey(unsigned char srcbuffer[],unsigned char len,unsigned char *pkey,unsigned char mode);
unsigned char ThreeDesBykey(unsigned char srcbuffer[],unsigned short len,unsigned char *pkey,unsigned char mode);
unsigned char threeDesEntryOrDisEntry(unsigned char srcbuffer[],unsigned char len,unsigned char KID,unsigned char mode);


//����RSA��Կ��
unsigned char genaryRsaKey(unsigned short publicFileID,unsigned short privateFileID);
unsigned char ReadPublicFile(unsigned short publicFileID);
unsigned char RsaDisEncrty(unsigned short privateFileID,unsigned char buffer[]);
unsigned char RsaEncrty(unsigned short publicFileID,unsigned char buffer[]);
//PSAMUID
unsigned char getPSAMUID(unsigned char *pbuffer,unsigned char len);
unsigned char getPSAMInfo(unsigned char*pbuff);
unsigned char GetChallenge(unsigned char buffer[],unsigned char len);
//SHA1
unsigned char SHA1Arigth(unsigned char *pbuf,unsigned int len);

//to boot
unsigned char TobootZ8(void);

unsigned char QPOSFileStruct(void);


#endif //_SERCRITY_H_

