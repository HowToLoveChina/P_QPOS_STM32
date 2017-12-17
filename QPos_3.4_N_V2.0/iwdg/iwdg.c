#include"Iwdg.h"
#include "stm32f10x.h"


void vIWDG_Init(void)
{
	RCC_LSICmd(ENABLE);//��LSI
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //����֮ǰҪ����ʹ�ܼĴ���д
	IWDG_SetPrescaler(IWDG_Prescaler_256);//4//4khz 256��Ƶ6.4ms
	IWDG_SetReload(4000);// ���4095
	IWDG_ReloadCounter();
	IWDG_Enable();// Enable IWDG (the LSI oscillator will be enabled by hardware)
}

