#include "functions.h"

#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_bus.h"


#define CHANNEL_INDEX 0
#define STREAM_INDEX 1

#define PIN_INDEX 0
#define AF_INDEX 1


//GPIO
GPIO_TypeDef* AllGPIOTable[] = 
{
	#ifdef GPIOA 
	GPIOA, 
	#endif
	
	#ifdef GPIOB
	GPIOB, 
	#endif
	
	#ifdef GPIOC
	GPIOC, 
	#endif
	
	#ifdef GPIOD
	GPIOD, 
	#endif
	
	#ifdef GPIOE
	GPIOE, 
	#endif
	
	#ifdef GPIOF
	GPIOF, 
	#endif
	
	#ifdef GPIOG
	GPIOG, 
	#endif
	
	#ifdef GPIOH
	GPIOH, 
	#endif
};	

const uint32_t NumberOfGPIO = sizeof(AllGPIOTable)/sizeof(GPIO_TypeDef*);
uint32_t AllGOIORccTable [] =
{
	#ifdef GPIOA 
	LL_AHB1_GRP1_PERIPH_GPIOA, 
	#endif
	
	#ifdef GPIOB
	LL_AHB1_GRP1_PERIPH_GPIOB, 
	#endif
	
	#ifdef GPIOC
	LL_AHB1_GRP1_PERIPH_GPIOC, 
	#endif
	
	#ifdef GPIOD
	LL_AHB1_GRP1_PERIPH_GPIOD, 
	#endif
	
	#ifdef GPIOE
	LL_AHB1_GRP1_PERIPH_GPIOE, 
	#endif
	
	#ifdef GPIOF
	LL_AHB1_GRP1_PERIPH_GPIOF, 
	#endif
	
	#ifdef GPIOG
	LL_AHB1_GRP1_PERIPH_GPIOG, 
	#endif
	
	#ifdef GPIOH
	LL_AHB1_GRP1_PERIPH_GPIOH, 
	#endif
};	


//GPIO




//TIMER
TIM_TypeDef* AllTimersTable[] = 
{

	#ifdef TIM1 
	TIM1, 
	#endif
	
	#ifdef TIM2
	TIM2, 
	#endif
	
	#ifdef TIM3
	TIM3, 
	#endif
	
	#ifdef TIM4
	TIM4, 
	#endif
	
	#ifdef TIM5
	TIM5, 
	#endif
	
	#ifdef TIM6
	TIM6, 
	#endif
	
	#ifdef TIM7
	TIM7, 
	#endif
	
	#ifdef TIM8
	TIM8, 
	#endif
	#ifdef TIM9
	TIM9, 
	#endif
	
	#ifdef TIM10
	TIM10, 
	#endif
	#ifdef TIM11
	TIM11, 
	#endif
	
	#ifdef TIM12
	TIM12, 
	#endif
	
	#ifdef TIM13
	TIM13, 
	#endif
	
	#ifdef TIM14
	TIM14, 
	#endif
};
const uint32_t NumberOfTimer = sizeof(AllTimersTable)/sizeof(TIM_TypeDef*);				
void (*AllTimersRccAxBxTable[]) (uint32_t) =
{
	#ifdef TIM1 
	&LL_APB2_GRP1_EnableClock,
	#endif
	
	#ifdef TIM2
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM3
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM4
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM5
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM6
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM7
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM8
	&LL_APB2_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM9
	&LL_APB2_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM10
	&LL_APB2_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM11
	&LL_APB2_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM12
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM13
	&LL_APB1_GRP1_EnableClock, 
	#endif
	
	#ifdef TIM14
	&LL_APB1_GRP1_EnableClock, 
	#endif	
};

uint32_t AllTimersRccTable[] = 
{
	#ifdef TIM1 
	LL_APB2_GRP1_PERIPH_TIM1,
	#endif
	
	#ifdef TIM2
	LL_APB1_GRP1_PERIPH_TIM2, 
	#endif
	
	#ifdef TIM3
	LL_APB1_GRP1_PERIPH_TIM3, 
	#endif
	
	#ifdef TIM4
	LL_APB1_GRP1_PERIPH_TIM4, 
	#endif
	
	#ifdef TIM5
	LL_APB1_GRP1_PERIPH_TIM5, 
	#endif
	
	#ifdef TIM6
	LL_APB1_GRP1_PERIPH_TIM6, 
	#endif
	
	#ifdef TIM7
	LL_APB1_GRP1_PERIPH_TIM7, 
	#endif
	
	#ifdef TIM8
	LL_APB2_GRP1_PERIPH_TIM8, 
	#endif
	
	#ifdef TIM9
	LL_APB2_GRP1_PERIPH_TIM9, 
	#endif
	
	#ifdef TIM10
	LL_APB2_GRP1_PERIPH_TIM10, 
	#endif
	
	#ifdef TIM11
	LL_APB2_GRP1_PERIPH_TIM11, 
	#endif
	
	#ifdef TIM12
	LL_APB1_GRP1_PERIPH_TIM12, 
	#endif
	
	#ifdef TIM13
	LL_APB1_GRP1_PERIPH_TIM13, 
	#endif
	
	#ifdef TIM14
	LL_APB1_GRP1_PERIPH_TIM14, 
	#endif	
};



TIM_TypeDef* AllTimersWithDma[] = 
{
	#ifdef TIM1 
	TIM1, 
	#endif
	
	#ifdef TIM2
	TIM2, 
	#endif
	
	#ifdef TIM3
	TIM3, 
	#endif
	
	#ifdef TIM4
	TIM4, 
	#endif
	
	#ifdef TIM5
	TIM5, 
	#endif	
	
	#ifdef TIM8
	TIM8, 
	#endif
	
};



const uint32_t NumberOfTimersWithDma = sizeof(AllTimersWithDma)/sizeof(TIM_TypeDef*);
uint32_t AllTimerDmaChStr[][2] =
{
	#ifdef TIM1 
	{LL_DMA_CHANNEL_6, LL_DMA_STREAM_5}, 
	#endif
	
	#ifdef TIM2
	{LL_DMA_CHANNEL_3, LL_DMA_STREAM_1}, 
	#endif
	
	#ifdef TIM3
	{LL_DMA_CHANNEL_5, LL_DMA_STREAM_2},  
	#endif
	
	#ifdef TIM4
	{LL_DMA_CHANNEL_2, LL_DMA_STREAM_6},  
	#endif
	
	#ifdef TIM5
	{LL_DMA_CHANNEL_6, LL_DMA_STREAM_6}, 
	#endif	
	
	#ifdef TIM8
	{LL_DMA_CHANNEL_7, LL_DMA_STREAM_1}, 
	#endif
};




DMA_TypeDef * AllTimersDmaPtr[] =
{
	#ifdef TIM1 
	DMA2, 
	#endif
	
	#ifdef TIM2
	DMA1, 
	#endif
	
	#ifdef TIM3
	DMA1,  
	#endif
	
	#ifdef TIM4
	DMA1,  
	#endif
	
	#ifdef TIM5
	DMA1, 
	#endif	
	
	#ifdef TIM8
	DMA2, 
	#endif
};







uint32_t TimerWithDmaPinConf[][2] =
{
	#ifdef TIM1 
	{LL_GPIO_PIN_8, LL_GPIO_AF_1}, 
	#endif
	
	#ifdef TIM2
	{LL_GPIO_PIN_0, LL_GPIO_AF_1}, 
	#endif
	
	#ifdef TIM3
	{LL_GPIO_PIN_6, LL_GPIO_AF_2},  
	#endif
	
	#ifdef TIM4
	{LL_GPIO_PIN_6, LL_GPIO_AF_2},  
	#endif
	
	#ifdef TIM5
	{LL_GPIO_PIN_10, LL_GPIO_AF_2}, 
	#endif	
	
	#ifdef TIM8
	{LL_GPIO_PIN_6, LL_GPIO_AF_3}, 
	#endif
};

GPIO_TypeDef *AllTimerGpioPtr[] = 
{
	#ifdef TIM1 
	GPIOA,
	#endif
	
	#ifdef TIM2
	GPIOA,
	#endif
	
	#ifdef TIM3
	GPIOA,  
	#endif
	
	#ifdef TIM4
	GPIOB,  
	#endif
	
	#ifdef TIM5
	GPIOH, 
	#endif	
	
	#ifdef TIM8
	GPIOC, 
	#endif
};
//TIMER


//DMA
DMA_TypeDef * AllDmaPtr[] =
{
	#ifdef DMA1
	DMA1,
	#endif
	
	#ifdef DMA2
	DMA2,
	#endif
};
const uint32_t NumberOfDma = sizeof(AllDmaPtr)/sizeof(DMA_TypeDef*);
uint32_t AllDmaRccTable[] =
{
	#ifdef DMA1
	LL_AHB1_GRP1_PERIPH_DMA1,
	#endif
	
	#ifdef DMA2
	LL_AHB1_GRP1_PERIPH_DMA2,
	#endif
};



//DMA





//FUNCTIONS
void enableGPIOClock(GPIO_TypeDef* Gpio)
{
	for(uint32_t X = 0; X<NumberOfGPIO; ++X)
	{
		if(AllGPIOTable[X]==Gpio)
		{			
			LL_AHB1_GRP1_EnableClock(AllGOIORccTable[X]);			
			break;			
		}
	}	
	return;
}

void enableTIMClock(TIM_TypeDef * Timer)
{
	for(uint32_t X = 0; X<NumberOfTimer; ++X)
	{
		if(AllTimersTable[X]==Timer)
		{			
			AllTimersRccAxBxTable[X](AllTimersRccTable[X]);			
			break;
		}
	}
	
	return;
}

void enableDMAClock(DMA_TypeDef * Dma)
{
	for(uint32_t X = 0; X<NumberOfDma; ++X)
	{
		if(AllDmaPtr[X]==Dma)
		{			
			LL_AHB1_GRP1_EnableClock(AllDmaRccTable[X]);			
			break;
		}
	}
	
	return;	
}

void enableTIMGPIOClock(TIM_TypeDef * Timer)
{
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X] == Timer)
		{
			enableGPIOClock(AllTimerGpioPtr[X]);
			return;
		}
	}
}

uint32_t getTimerDmaChannel(TIM_TypeDef *Timer)
{
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X]==Timer)
		{			
			return AllTimerDmaChStr[X][CHANNEL_INDEX];
		}
	}
	
	return 0-1;
}

uint32_t getTimerDmaStream(TIM_TypeDef *Timer)
{
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X]==Timer)
		{			
			return AllTimerDmaChStr[X][STREAM_INDEX];
		}
	}
	
	return 0-1;
}

DMA_TypeDef * getTimerDma(TIM_TypeDef *Timer)
{
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X]==Timer)
		{			
			return AllTimersDmaPtr[X];
		}
	}
	
	return 0;
}

	 
uint32_t getTimerCh1Pin(TIM_TypeDef *Timer)
{
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X] == Timer)
			return TimerWithDmaPinConf[X][PIN_INDEX];
	}
	return 0-1;
}

uint32_t getTimerCh1Af(TIM_TypeDef *Timer)
{	
	for(uint32_t X = 0; X<NumberOfTimersWithDma; ++X)
	{
		if(AllTimersWithDma[X] == Timer)
			return TimerWithDmaPinConf[X][AF_INDEX];
	}
	return 0-1;
}


uint32_t stringcopy(char * Dst, const char * Src)
{
	uint32_t Len = 0;
	while(Src[Len]!=0)
	{	
		Dst[Len] = Src[Len];
		Len++;
	}
	
	Dst[Len] = 0;
	
	return Len;
}
