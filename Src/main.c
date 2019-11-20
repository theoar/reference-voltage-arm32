
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes -----------------------------------------------------------------*/

#include "main.h"

#include "ditherpwm.hpp"
#include "fixedpoint.hpp"
#include "inputhandler.hpp"
#include "menu.hpp"
#include "leds.h"
#include "lcd.h"

	 
static void initSystem();

void initHandlerTimer();
void initConfigLcd();

InputHandler Handler;

#ifdef __cplusplus
extern "C" {
#endif
void TIM5_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM5))
	{	
		LL_TIM_ClearFlag_UPDATE(TIM5);
		Handler.pollDevices();
	}
	return;
}
	
#ifdef __cplusplus
}
#endif


char bufftext[32];
int main(void)
{
	initSystem();	
	
	PinCofiguration Enc1A(GPIOC, LL_GPIO_PIN_1, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	PinCofiguration Enc1B(GPIOC, LL_GPIO_PIN_3, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	PinCofiguration Enc1Button(GPIOC, LL_GPIO_PIN_0, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	
	PinCofiguration Enc2A(GPIOE, LL_GPIO_PIN_11, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	PinCofiguration Enc2B(GPIOE, LL_GPIO_PIN_13, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	PinCofiguration Enc2Button(GPIOE, LL_GPIO_PIN_15, ACTIVE_LOW, LL_GPIO_MODE_INPUT);
	
	Encoder Enc1(Enc1A,Enc1B);
	Encoder Enc2(Enc2A, Enc2B);
	
	Button Button1(Enc1Button,10,500);
	Button Button2(Enc2Button,10,500);
	
	Handler.addDevice(&Enc1);
	Handler.addDevice(&Enc2);
	Handler.addDevice(&Button1);
	Handler.addDevice(&Button2);
			
	initConfigLcd();
	lcdInit();		
	initLeds();
			
	initHandlerTimer();
	
	ledOn(ORAGNE);

	DitherPwm Pwm1(TIM1);	
	Pwm1.setupBySteps(3.0, 0.1, 0.01, 16800, LL_TIM_OCPOLARITY_HIGH);	
	Pwm1.start();	
			
	MenuElement * CurrentElement = 0;
	MenuElement * PrevElement = 0;
		
	MainVoltageChangeHandler VoltageVariableHandler(&Pwm1);
	MenuElementVariable VoltageVariable(0.0001,	3,	0.01, "Nap. wyj.",	"U = ", " V", false, &VoltageVariableHandler);	
	VoltageVariable.setValue(2.5);
	Pwm1.setVoltage(2.5);
	
	MenuElementVariable RefVoltageVariable(0.0001, 3, 0.0001, "Nap. ref.", "U = ", " V", false, 0);
	MenuElementVariable NVariable(1, 1024, 1, "Liczba N", "N = ", " ", false, 0);
	MenuElementVariable StepVariable(0.0001, 3, 0.0001, "Krok zm. U", "U = ", " V", false, 0);
	MenuElementVariable PrecStepVariable(0.0001, 3, 0.0001, "Prec krok zm. U", "U = ", " V", false, 0);
	MenuElementVariable PrescallerVariable(1, UINT16_MAX, 1, "Prescaller", "P = ", " ", false, 0);
	
	Menu MainMenu;			
	Menu ConfigMenu;
	
	Menu ByStepsMenu;
	Menu ByPreciseStepNMenu;
	Menu ByBasicStepNMenu;
	
	ConfigMenu.setParent(&MainMenu);
	
	ByStepsMenu.setParent(&ConfigMenu);
	ByPreciseStepNMenu.setParent(&ConfigMenu);
	ByBasicStepNMenu.setParent(&ConfigMenu);

	MenuElementOptionList MainMenuOptionsList("Opcje");						
	
	MenuElementOptionList ConfigMenuOptionsList("Konfigruacja"); 		

	MenuElementOptionList ByStepsMenuOptionsList("PreStepN");		
	MenuElementOptionList ByPreciseStepNMenuOptionsList("PreStepN");		
	MenuElementOptionList ByBasicStepNMenuOptionsList("BasStepN");			
	
	MainMenu.addElement(&VoltageVariable);
	MainMenu.addElement(&MainMenuOptionsList);
	
	ConfigMenu.addElement(&ConfigMenuOptionsList);
	
	ByStepsMenu.addElement(&RefVoltageVariable);
	ByStepsMenu.addElement(&StepVariable);
	ByStepsMenu.addElement(&PrecStepVariable);
	ByStepsMenu.addElement(&PrescallerVariable);
	
	ByPreciseStepNMenu.addElement(&RefVoltageVariable);
	ByPreciseStepNMenu.addElement(&StepVariable);
	ByPreciseStepNMenu.addElement(&NVariable);
	ByPreciseStepNMenu.addElement(&PrescallerVariable);	
	
	ByBasicStepNMenu.addElement(&RefVoltageVariable);
	ByBasicStepNMenu.addElement(&PrecStepVariable);
	ByBasicStepNMenu.addElement(&NVariable);
	ByBasicStepNMenu.addElement(&PrescallerVariable);
	
	NextMenuHandler OptionsConfigNext(&ConfigMenu, &CurrentElement);
	
	MainMenuOptionsList.addOption("Konfiguracja", &OptionsConfigNext);
	MainMenuOptionsList.addOption("Parametry", 0);
	MainMenuOptionsList.addOption("Informacje", 0);
	
	NextMenuHandler OptionsByStepsNext(&ByStepsMenu, &CurrentElement);
	NextMenuHandler OptionsByPreciseStepNext(&ByPreciseStepNMenu, &CurrentElement);
	NextMenuHandler OptionsByBasicStepNext(&ByBasicStepNMenu, &CurrentElement);
	PrevMenuHandler PrevMenu(&CurrentElement);
	
	ConfigMenuOptionsList.addOption("Steps", &OptionsByStepsNext);
	ConfigMenuOptionsList.addOption("PreStepN", &OptionsByPreciseStepNext);
	ConfigMenuOptionsList.addOption("BasStepN", &OptionsByBasicStepNext);
	ConfigMenuOptionsList.addOption("Wyjdz", &PrevMenu);
	
	ByStepsMenuOptionsList.addOption("Zapisz", 0);
	ByStepsMenuOptionsList.addOption("Wyjdz", &PrevMenu);
	ByStepsMenu.addElement(&ByStepsMenuOptionsList);
	
	ByPreciseStepNMenuOptionsList.addOption("Zapisz", 0);
	ByPreciseStepNMenuOptionsList.addOption("Wyjdz", &PrevMenu);
	ByPreciseStepNMenu.addElement(&ByPreciseStepNMenuOptionsList);
	
	ByBasicStepNMenuOptionsList.addOption("Zapisz", 0);
	ByBasicStepNMenuOptionsList.addOption("Wyjdz", &PrevMenu);
	ByBasicStepNMenu.addElement(&ByBasicStepNMenuOptionsList);
	

				
	CurrentElement = &MainMenu;
	PrevElement = &MainMenu;
	
	ledOn(GREEN);
		
	CurrentElement->display();

  while (1)
  {	
		if(Handler.hasPendingEvent())
		{		
				InputHandler::EventStruct Event = Handler.readEvent();
				EventsEnum E = Event.EventType;
				InputDevice * D = Event.Device;				
		
				if(E==EventsEnum::CLICKED && D==&Button1)
				{
					CurrentElement->handleEvent(MenuEvents::Enter);
					ledToggle(BLUE);
				}
				
				if(E==EventsEnum::CLICKED && D==&Button2)
				{		
					CurrentElement->handleEvent(MenuEvents::Escape);
					ledToggle(BLUE);
				}
				
				if(E==EventsEnum::ROTATED_LEFT && D==&Enc1)
				{	
					CurrentElement->handleEvent(MenuEvents::LeftPos);
					ledToggle(BLUE);
				}
				
				if(E==EventsEnum::ROTATED_RIGHT && D==&Enc1)					
				{
					CurrentElement->handleEvent(MenuEvents::RightPos);
					ledToggle(BLUE);
				}
				
				if(E==EventsEnum::ROTATED_LEFT && D==&Enc2)
				{
					CurrentElement->handleEvent(MenuEvents::LeftVal);	
					ledToggle(BLUE);					
				}
				
				if(E==EventsEnum::ROTATED_RIGHT && D==&Enc2)
				{	
					CurrentElement->handleEvent(MenuEvents::RightVal);	
					ledToggle(BLUE);					
				}		
		}
		
		if(CurrentElement!=PrevElement)
		{
			PrevElement = CurrentElement;
			CurrentElement->display();
		}
	}		
  	
}

void initHandlerTimer()
{
	NVIC_SetPriority(TIM5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));	
	NVIC_EnableIRQ(TIM5_IRQn);
	
	enableTIMClock(TIM5);
	
	LL_TIM_InitTypeDef Timdef;
	Timdef.Prescaler = 16800-1;
	Timdef.Autoreload  = 100-1;
	Timdef.ClockDivision = 0;
	Timdef.CounterMode = LL_TIM_COUNTERMODE_UP;
	Timdef.RepetitionCounter = 0;
	
	LL_TIM_Init(TIM5, &Timdef);	
	LL_TIM_EnableIT_UPDATE(TIM5);			
	LL_TIM_ClearFlag_UPDATE(TIM5);	
	
	LL_TIM_EnableCounter(TIM5);
}

void initConfigLcd()
{	
	lcdSetupDataPin(LCD_D7,  LL_GPIO_PIN_0, GPIOD);
	lcdSetupDataPin(LCD_D6,  LL_GPIO_PIN_1, GPIOD);	
	lcdSetupDataPin(LCD_D5,  LL_GPIO_PIN_2, GPIOD);	
	lcdSetupDataPin(LCD_D4,  LL_GPIO_PIN_3, GPIOD);		
	lcdSetupControlPin(LCD_E,  LL_GPIO_PIN_6, GPIOD);
	lcdSetupControlPin(LCD_RW,  LL_GPIO_PIN_7, GPIOB);
	lcdSetupControlPin(LCD_RS,  LL_GPIO_PIN_8, GPIOB);
	
	lcdSetupBusWidth(LCD_BUS_4);
	lcdSetupRwLine(LCD_RW_ENABLE);
	lcdSetupWaitMode(LCD_WAIT_MODE_FLAG);		
}

static void initSystem(void)
{  
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
	
  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* BusFault_IRQn interrupt configuration */
  NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* UsageFault_IRQn interrupt configuration */
  NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* SVCall_IRQn interrupt configuration */
  NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* DebugMonitor_IRQn interrupt configuration */
  NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* PendSV_IRQn interrupt configuration */
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)  
		Error_Handler();  
  
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	
  LL_RCC_HSE_Enable();
   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1);
	
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_2);
	LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_8, 192, LL_RCC_PLLI2SR_DIV_2);		
	
	LL_RCC_PLL_Enable();
   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1); 
	
	LL_RCC_PLLI2S_Enable();
	/* Wait till PLL is ready */
  while(LL_RCC_PLLI2S_IsReady() != 1);  
  
  LL_RCC_LSI_Enable();
   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1);      
  
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  LL_RCC_EnableRTC();
	
  
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);	
	    
  LL_Init1msTick(168000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(168000000);	
	LL_SYSTICK_EnableIT();		
	
	LL_RCC_SetI2SClockSource(LL_RCC_I2S1_CLKSOURCE_PLLI2S);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));	
	
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
 /*
static void MX_RTC_Init(void)
{

  LL_RTC_InitTypeDef RTC_InitStruct;
  LL_RTC_TimeTypeDef RTC_TimeStruct;
  LL_RTC_DateTypeDef RTC_DateStruct;

  LL_RCC_EnableRTC();


  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
  RTC_InitStruct.AsynchPrescaler = 127;
  RTC_InitStruct.SynchPrescaler = 255;
  LL_RTC_Init(RTC, &RTC_InitStruct);

  LL_RTC_SetAsynchPrescaler(RTC, 127);

  LL_RTC_SetSynchPrescaler(RTC, 255);


  if(LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2){
  
  RTC_TimeStruct.Hours = 0;
  RTC_TimeStruct.Minutes = 0;
  RTC_TimeStruct.Seconds = 0;
  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);

  RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY;
  RTC_DateStruct.Month = LL_RTC_MONTH_JANUARY;
  RTC_DateStruct.Year = 0;
  LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_DateStruct);

    LL_RTC_BAK_SetRegister(RTC,LL_RTC_BKP_DR0,0x32F2);
  }

}
*/

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

