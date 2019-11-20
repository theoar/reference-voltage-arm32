#ifndef DITHERPWM_H
#define DITHERPWM_H

#include <inttypes.h>
#include <stdlib.h>

#include "stm32f4xx_ll_tim.h"
#include "stm32f407xx.h"
#include "stm32f4xx_ll_dma.h"

#include "fixedpoint.hpp"


class DitherPwm
{
	private:
		uint16_t N;
		uint16_t ExtraDuty;
		uint16_t Duty;
		uint16_t Period;
		uint16_t Prescaller;
		TIM_TypeDef * Timer;	
		
		uint16_t *DitherTable = 0;
		
		FixedPoint VoltageReference;
		FixedPoint BasicStep;
		FixedPoint PreciseStep;
		FixedPoint CurrentVoltage;
	
		uint8_t OcPolarity;	
	public:
		
		DitherPwm(TIM_TypeDef * _Timer);
		DitherPwm();
	
		~DitherPwm();
	
		void init(TIM_TypeDef * _Timer);
		bool start(void);
		bool stop(void);
		
		FixedPoint setupByBasicStepN(FixedPoint _VoltageReference, FixedPoint _BasicStep, uint16_t _N, uint16_t _Prescaller, uint8_t _OcPolarity);
		FixedPoint setupByPreciseStepN(FixedPoint _VoltageReference, FixedPoint _PreciseStep, uint16_t _N, uint16_t _Prescaller, uint8_t _OcPolarity);		
		FixedPoint setupBySteps(FixedPoint _VoltageReference, FixedPoint _BasicStep, FixedPoint _PreciseStep, uint16_t _Prescaller, uint8_t _OcPolarity);			

		FixedPoint setVoltage(FixedPoint Voltage);
		FixedPoint getVoltage() const;
		
		uint16_t getN(void) const;
		uint16_t getDuty(void) const;
		uint16_t GetExtraDuty(void) const;
		uint16_t getPeriod(void) const;
		uint16_t getPrescaller(void) const;	
		FixedPoint getPwmFrequency(void) const;
	
		const uint16_t * getDitherTable(void) const;		
	
	private:
		void generateDitherTable(void);		
		void setupDMA(void);
		void setupTimer(void);
		void allocateDitherTable(void);
};

#endif