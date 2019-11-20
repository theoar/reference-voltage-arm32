#include <inttypes.h>
#include <stdlib.h>

#include "stm32f4xx_ll_tim.h"
#include "stm32f407xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_dma.h"

#include "ditherpwm.hpp"
#include "functions.h"

DitherPwm::DitherPwm(TIM_TypeDef * _Timer)
{
	this->init(_Timer);
}
DitherPwm::DitherPwm()
{
	
}

void DitherPwm::generateDitherTable(void)
{
	uint16_t Sum = 0;
	for(uint16_t X = 0; X<this->N; ++X)
	{
		Sum = Sum+this->ExtraDuty;
		if(Sum/N > 0)		
			this->DitherTable[X] = this->Duty+1;		
		else		
			this->DitherTable[X] = this->Duty;
		
		Sum = Sum % N;
	}
}

DitherPwm::~DitherPwm()
{
	if(this->DitherTable!=0)
		delete this->DitherTable;
}

void DitherPwm::setupDMA(void)
{
	LL_DMA_InitTypeDef DmaConf;
	
	DMA_TypeDef * Dma = getTimerDma(this->Timer);
	uint32_t Stream = getTimerDmaStream(this->Timer);
	uint32_t Channel = getTimerDmaChannel(this->Timer);
	
	enableDMAClock(Dma);
	
	LL_DMA_DisableStream(Dma, Stream);
	LL_DMA_DeInit(Dma, Stream);	
	
	LL_DMA_StructInit(&DmaConf);
	
	DmaConf.Channel = Channel;	
	
	DmaConf.Priority = LL_DMA_PRIORITY_HIGH;
	DmaConf.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DmaConf.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	DmaConf.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
	
	DmaConf.MemBurst = LL_DMA_MBURST_SINGLE;		
	DmaConf.MemoryOrM2MDstAddress = (uint32_t)(this->DitherTable);
	DmaConf.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
	DmaConf.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	
	DmaConf.Mode = LL_DMA_MODE_CIRCULAR;	
	DmaConf.NbData = this->N;
	
	DmaConf.PeriphBurst = LL_DMA_PBURST_SINGLE;
	DmaConf.PeriphOrM2MSrcAddress = (uint32_t)&(this->Timer->CCR1);
	DmaConf.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
	DmaConf.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;		
	
	LL_DMA_Init(Dma, Stream, &DmaConf);	
	LL_DMA_EnableStream(Dma, Stream);				
	
	LL_TIM_EnableDMAReq_UPDATE(this->Timer);
}

void DitherPwm::init(TIM_TypeDef * _Timer)
{
	this->Timer = _Timer;	
	LL_TIM_DisableCounter(this->Timer);
	LL_TIM_DeInit(this->Timer);	
	
	LL_GPIO_InitTypeDef GpioDef;
	GpioDef.Mode = LL_GPIO_MODE_ALTERNATE;
	GpioDef.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GpioDef.Pull = LL_GPIO_PULL_NO;
	GpioDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	
		
	GpioDef.Pin = getTimerCh1Pin(this->Timer);
	GpioDef.Alternate = getTimerCh1Af(this->Timer);
	enableTIMGPIOClock(this->Timer);	
	LL_GPIO_Init(GPIOA, &GpioDef);
	enableTIMClock(this->Timer);
}

void DitherPwm::setupTimer(void)
{	
	LL_TIM_DisableAllOutputs(this->Timer);
	LL_TIM_DisableCounter(this->Timer);
	
	LL_TIM_DeInit(this->Timer);
	
	LL_TIM_InitTypeDef InitTim;
	LL_TIM_StructInit(&InitTim);
	InitTim.ClockDivision = 0;
	InitTim.Prescaler = this->Prescaller-1;
	InitTim.RepetitionCounter = 0;	
	InitTim.Autoreload = 	this->Period-1;
	InitTim.CounterMode = LL_TIM_COUNTERMODE_UP;	
	LL_TIM_Init(this->Timer, &InitTim);
	
	LL_TIM_EnableARRPreload(this->Timer);	
	LL_TIM_SetTriggerOutput(this->Timer, LL_TIM_TRGO_UPDATE);
	LL_TIM_SetClockSource(this->Timer, LL_TIM_CLOCKSOURCE_INTERNAL);		
	
	LL_TIM_OC_InitTypeDef ChannelDef;	
	ChannelDef.OCPolarity = this->OcPolarity;
	ChannelDef.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	ChannelDef.OCMode = LL_TIM_OCMODE_PWM1;
	ChannelDef.OCState = LL_TIM_OCSTATE_ENABLE;
	ChannelDef.CompareValue = (this->Period / 2) - 1;	
	LL_TIM_OC_Init(this->Timer,LL_TIM_CHANNEL_CH1,&ChannelDef);
	
	LL_TIM_OC_EnablePreload(this->Timer, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableAllOutputs(this->Timer);	
}

void DitherPwm::allocateDitherTable(void)
{
	if(this->DitherTable!=0)
		free( this->DitherTable );
	this->DitherTable = (uint16_t*)malloc(sizeof(uint16_t)*this->N);	
	//this->DitherTable = new uint16_t[this->N];
}

FixedPoint DitherPwm::setupByBasicStepN(FixedPoint _VoltageReference, FixedPoint _BasicStep, uint16_t _N, uint16_t _Prescaller, uint8_t _OcPolarity)
{
	this->VoltageReference = _VoltageReference;
	this->BasicStep = _BasicStep;
	this->N = _N;
	
	
	this->Prescaller = _Prescaller;
	this->OcPolarity = _OcPolarity;		
	
	this->Period = (this->VoltageReference)/(this->BasicStep);
	this->PreciseStep = _BasicStep/N;		
	
	this->BasicStep  = 	(this->VoltageReference/this->Period);
	this->PreciseStep = this->BasicStep/this->N;
	this->setVoltage(this->VoltageReference*0.5);	
	
	this->allocateDitherTable();
	this->generateDitherTable();
	this->setupTimer();
	this->setupDMA();			
		
	return this->CurrentVoltage;
}

FixedPoint DitherPwm::setupByPreciseStepN(FixedPoint _VoltageReference, FixedPoint _PreciseStep, uint16_t _N, uint16_t _Prescaller, uint8_t _OcPolarity)
{
	this->VoltageReference = _VoltageReference;
	this->BasicStep = _PreciseStep*_N;
	this->N = _N;
	
	this->Prescaller = _Prescaller;
	this->OcPolarity = _OcPolarity;		
	
	this->Period = (this->VoltageReference)/(this->BasicStep);
	this->PreciseStep = _PreciseStep/N;		
	
	this->BasicStep  = 	(this->VoltageReference/this->Period);
	this->PreciseStep = this->BasicStep/this->N;
	this->setVoltage(this->VoltageReference*0.5);	
	
	this->allocateDitherTable();
	this->generateDitherTable();
	this->setupTimer();
	this->setupDMA();			
		
	return this->CurrentVoltage;
}

FixedPoint DitherPwm::setupBySteps(FixedPoint _VoltageReference, FixedPoint _BasicStep, FixedPoint _PreciseStep, uint16_t _Prescaller, uint8_t _OcPolarity)
{
	this->VoltageReference = _VoltageReference;
	this->BasicStep = _BasicStep;
	this->N = _BasicStep/_PreciseStep;
	
	
	this->Prescaller = _Prescaller;
	this->OcPolarity = _OcPolarity;		
	
	this->Period = (this->VoltageReference)/(this->BasicStep);
	this->PreciseStep = _BasicStep/N;		
	
	this->BasicStep  = 	(this->VoltageReference/this->Period);
	this->PreciseStep = this->BasicStep/this->N;
	this->setVoltage(this->VoltageReference*0.5);	
	
	this->allocateDitherTable();
	this->generateDitherTable();	
	this->setupTimer();
	this->setupDMA();		
	
	return this->CurrentVoltage;
}

FixedPoint DitherPwm::setVoltage(FixedPoint Voltage)
{
	this->Duty = ((Voltage*this->Period)/this->VoltageReference);
	this->ExtraDuty = (((Voltage* this->Period)/this->VoltageReference)- this->Duty)*this->N;
	this->CurrentVoltage = this->BasicStep*this->Duty + this->PreciseStep*this->ExtraDuty;
	this->generateDitherTable();
	
	return this->CurrentVoltage;
}

FixedPoint DitherPwm::getVoltage() const
{
	return FixedPoint(this->CurrentVoltage);
}

bool DitherPwm::start(void)
{
	LL_TIM_EnableCounter(this->Timer);
	return LL_TIM_IsEnabledCounter(this->Timer);
}

bool DitherPwm::stop(void)
{
	LL_TIM_DisableCounter(this->Timer);
	return LL_TIM_IsEnabledCounter(this->Timer) < 1;
}

uint16_t DitherPwm::getN(void) const
{
	return this->N;
}
uint16_t DitherPwm::getDuty(void) const
{
	return this->Duty;
}
uint16_t DitherPwm::getPeriod(void) const
{
	return this->Period;
}

uint16_t DitherPwm::getPrescaller(void) const
{
	return this->Prescaller;
}

uint16_t DitherPwm::GetExtraDuty(void) const
{
	return this->ExtraDuty;
}

const uint16_t * DitherPwm::getDitherTable(void) const
{
	return this->DitherTable;
}

FixedPoint DitherPwm::getPwmFrequency(void) const
{
	double Freq = (double)SystemCoreClock/(double)(this->Prescaller*this->Period);
	
	return FixedPoint(Freq);
}
