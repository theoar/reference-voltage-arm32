#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
 extern "C"{
#endif



#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_dma.h"

#include <inttypes.h>


void enableGPIOClock(GPIO_TypeDef* Gpio);
void enableTIMClock(TIM_TypeDef * Timer);
void enableDMAClock(DMA_TypeDef * Dma);
void enableTIMGPIOClock(TIM_TypeDef * Timer);

uint32_t getTimerDmaChannel(TIM_TypeDef *Timer);
uint32_t getTimerDmaStream(TIM_TypeDef *Timer);
DMA_TypeDef * getTimerDma(TIM_TypeDef *Timer);
	 
uint32_t getTimerCh1Pin(TIM_TypeDef *Timer);
uint32_t getTimerCh1Af(TIM_TypeDef *Timer);

uint32_t stringcopy(char * Dst, const char * Src);

	 
#ifdef __cplusplus
}
#endif



#endif