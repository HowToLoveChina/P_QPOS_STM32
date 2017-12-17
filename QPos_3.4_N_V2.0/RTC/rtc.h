#ifndef __RTC_H
#define __RTC_H	    
//Mini STM32������
//RTCʵʱʱ�� ��������			 
//����ԭ��@ALIENTEK
//2010/6/6

//ʱ��ṹ��
typedef struct 
{
	unsigned short w_year;
	unsigned char  w_month;
	unsigned char  w_date;
 	unsigned char hour;
	unsigned char min;
	unsigned char sec;			
	//������������
	unsigned char  week;		 
}CALENDAR_OBJ;
					 
extern CALENDAR_OBJ g_sCalendar;	//�����ṹ��

extern unsigned char const mon_table[12];	//�·��������ݱ�

unsigned char RTC_Init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;

unsigned char Is_Leap_Year(unsigned short year);//ƽ��,�����ж�
unsigned char RTC_Get(void);         //����ʱ��   
unsigned char RTC_Get_Week(unsigned short year,unsigned char month,unsigned char day);
unsigned char RTC_Set(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);//����ʱ��			 
#endif


