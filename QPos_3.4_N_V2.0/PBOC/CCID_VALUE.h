#ifndef __CCID_VALUE_H
#define __CCID_VALUE_H

//#define 	POS_ENCPKT_CLA 							0x7f
//#define		POS_PLAINPKT_CLA					    0x7e

#define     DOUBLE_PACKET							0x01
#define 	DOUBLE_PACKET_OFF						0x00

/*�ɹ���ɲ����سɹ�
*/
	//����ɹ�
	#define SW_OPERATION_SUCCESS	0x9000
	
	//��Ҫ��������
	#define SW_OPERATION_CONTINUE	0x6100

/*������ɲ����ر�����EEPROM�������κα仯
*/
	//����Le����ǰ�ļ����¼�ѽ���	
	#define SW_DATA_LESSTHANLE		0x6282
	
	//ѡ����ļ���Ч
	#define SW_SELECTFILE_INVALID	0x6283

	#define SW_DIS_TIMEOUT			0x6284
	
/*������ɲ����ر�����EEPROM�����仯
*/
	//�����֤���������ؼ���ֵ�����仯
	#define SW_AUTH_ERROR			0X63C0

	
/*ִ�в��ָ����ִ�й����У���������ĳ����������г����쳣���⣬EEPROM�������κα仯
*/
	//û��׼ȷ���
	#define SW_UNKOWN_ERROR			0X6400
	
/*ִ�в��ָ����ִ�й����У�������д�ļ������г����쳣���⣬EEPROM�����仯
*/
	//�洢������
	#define SW_MEMORY_ERROR			0X6581

	//�洢��Խ��
	#define SW_MEMORY_BEYOND		0X6582

/*�������ڽ��ܵ�ָ������ָ��ĺϷ��ԣ�����ָ������Դ��ƥ��
*/
/*���ȴ���
*/
	//Lc��Le���ȴ���
	#define SW_LCLE_ERROR			0X6700
	
/*0x68xx CLA�Ĺ��ܲ�֧��
*/
	//��֧���߼��ŵ�
	#define SW_CHANNEL_NONSUPPORT	0X6881
	
	//��֧�ְ�ȫ����
	#define SW_SM_NONSUPPORT		0X6882

	//CLA����
	#define SW_CLA_ERROR			0X6883

/*0x69xx �������INS
*/
	//�������ļ���ƥ�䣨�����ö�д�������ļ���ָ��ȥ��д��¼�ļ���
	#define	SW_INS_FILE_NOTMATCH	0X6981
	
	//��ȫ״̬������	
	#define SW_SECURITY_DISSATISFY	0X6982
	
	//��֤��ʽ������
	#define SW_AUTH_LOCK			0x6983
	
	//���õ�������Ч������Ҫʹ����������������������)
	#define SW_REFERENCEDATA_INVALID 0x6984
	
	//ʹ�õ�������������
	#define SW_CONDITION_DISSATISFY	0X6985
	
	//���������ǰû��ѡ�е�EF�����ж�EF�Ĳ�����	
	#define SW_INS_FORBID			0X6986
	
	//��ȫ�������ݶ���ʧ	
	#define SW_SMOBJ_LOST			0X6987
	
	//��ȫ�������ݶ�����ȷ	
	#define SW_SMOBJ_ERROR			0X6988

	//��Ƭ������
	#define SW_CARD_LOCK			0X6989

	//Ӧ�ñ���ʱ����
	#define SW_APPLICATION_LOCK		0X6990

	//Ӧ�ñ���������
	#define SW_APPLICATION_LOCK_FOREVER		0X6991

	
//P1 P2����
	//�����ֶ������������֤����	
	#define SW_DATAFIELD_ERROR		0X6A80
	
	//���ܲ�֧��	
	#define SW_FUNCTION_NOTSUPPORT	0X6A81
	
	//Ŀ¼���ļ�δ�ҵ�
	#define SW_FILE_NOTFOUND		0X6A82
	
	//��¼δ�ҵ�
	#define SW_RECORD_NOTFOUND		0X6A83
	
	//�ļ���û���㹻�ռ�	
	#define SW_FILESPACE_NOTENOUGH	0X6A84

	//P1 P2����ȷ
	#define SW_P1P2_ERROR			0x6a86

	//P1 P2��LC��ƥ��
	#define SW_P1P2_LC_NOTMATCH		0x6a87

	//���õ�����δ�ҵ�
	#define SW_KEY_NOTFOUND			0x6a88

	#define SW_CAPABILITY_LENGTH	0x6c00

	#define SW_COMMAND_UNVALID		0x6d00

//�Զ��������
	//��Ƭ�ϵ�ʧ��
	#define CARD_POWERON_FAIL		0x1111
	//PIN���ܴ���
	#define KEY_ENCRYPT_ERROR		0x2222



#define     CREATE_FILE			    0x80
#define     DELETE_FILE			    0x81
#define     SELECT_FILE			    0x82
#define     UPDATE_BINARY			0x83
#define     READ_BINARY			    0x84
#define     GET_FILEINFO	        0x85
#define     VERIFY_PIN			    0x86
#define     CHANGE_PIN  			0x87
#define     PIN_UNBLOCK_KEY			0x88
#define     EXTERNAL_AUTHENTICATION	0x89
#define     INTERNAL_AUTHENTICATION	0x8a
#define     WRITE_KEY   			0x8b
#define     GET_CHALLENGE			0x8c
#define     GENERATE_SECKEY			0x8d
#define     ENCRYPT_DECRYPT         0x8e
#define     GENERATE_RSAKEY			0x8f
#define     RSA_SIGN			    0x90
#define     RSA_VERIFY			    0x91

#define 	ACTIVE_DF				0x92
#define     APPLICATION_BLOCK		0x93
#define     APPLICATION_UNBLOCK		0x94
#define     CARD_BLOCK			    0x95

#define     GET_RESPONSE			0x96
#define     GET_INFO			    0x97
#define     SET_DATA				0x98


#endif 
