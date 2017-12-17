//***************************************************************************************
//*  File Name		VectorTable_PBOC.h
//*  Author     	laughing       
//*	 Version		v1.0
//*  Date			05/20/2014 
//*  Description	
//*
//***************************************************************************************
#ifndef __VECTOR_TABLE_PBOC__
#define __VECTOR_TABLE_PBOC__

#define Time_LCD_BlackLightOFF	600    //�ر���,300*100ms,
#define Time_LCD_MainRresh		300	//ˢ������Ļ������ʱ��
#define Time_LCD_BackMainRresh		30	//��������Ļ������ʱ��
#define Time_PowerOFF			1200//�ػ�ʱ��

extern unsigned int g_uiTime100ms_LCDOff_PowerOff;

extern unsigned int g_uiTime100ms_LCDShow;

extern unsigned int g_uiTime100ms_Timeout;

extern unsigned int g_uiTime100ms_LCD_BackLightOff ;

extern unsigned char g_LCDShowInfo;

extern unsigned char g_LCDRefeshTimeFlag;

void vVectorTable_Device(void);

void vVectorTable_CMD(void);

unsigned char vVectorTable_Card(void);

void vVectorTable_Key(void);

unsigned char vVectorTableFactory_CMD(unsigned char * pucBuf);

void vFactory_Test(unsigned char flag);

void vFactory_Init(unsigned char flag);

#endif
//******************************end of file**********************************************
