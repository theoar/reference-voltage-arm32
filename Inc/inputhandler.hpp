#ifndef INPUTHANLDER_H
#define INPUTHANLDER_H

#include "stm32f407xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"

#include "functions.h"

#define ACTIVE_HIGH 1
#define ACTIVE_LOW 0


enum class EventsEnum
{
	CLICKED = 0,
	HOLDING = 1,
	PULLED = 2,
	ROTATED_LEFT = 3,
	ROTATED_RIGHT = 4,
	NO_EVENT
};

struct PinCofiguration
{
	GPIO_TypeDef *Gpio;
	uint32_t Pin;
	uint8_t ActiveState;	
	
	PinCofiguration();
	
	PinCofiguration(GPIO_TypeDef *_Gpio, 
																uint32_t _Pin, 
																uint8_t ActiveState, 
																uint32_t Mode,
																uint32_t Pull = LL_GPIO_PULL_UP,
																uint32_t Output = LL_GPIO_OUTPUT_PUSHPULL, 
																uint32_t Speed = LL_GPIO_SPEED_FREQ_HIGH
																);
	
	void configure(GPIO_TypeDef *_Gpio, 
																uint32_t _Pin, 
																uint8_t ActiveState, 
																uint32_t Mode,
																uint32_t Pull = LL_GPIO_PULL_UP,
																uint32_t Output = LL_GPIO_OUTPUT_PUSHPULL, 
																uint32_t Speed = LL_GPIO_SPEED_FREQ_HIGH
																);
		
};




class InputDevice
{
	public:
		virtual EventsEnum handle() = 0;
	
};

class Encoder : public InputDevice
{
	
	private:
		PinCofiguration PinA;
		PinCofiguration PinB;
		uint8_t Mask;
		
		
	public:
		Encoder(PinCofiguration &_PinA, PinCofiguration &_PinB);
	
		EventsEnum handle();
};

enum class ButtonState
{
	NONE = 0,	
	CLICKED = 1,
	HOLDING = 2
};


class Button : public InputDevice
{
	private:
		PinCofiguration Pin;
		ButtonState CurrentState;
		uint32_t CurrentTime;
		uint32_t HoldTime;
		uint32_t TimeStep;
	
	public:
		Button(PinCofiguration &_Pin, uint32_t _TimeStep, uint32_t _HoldTime);
	
		EventsEnum handle(); 
};


#define MAX_DEVICES 8
#define STATUS_EMPTY 0
#define STATUS_FULL 1
#define STATUS_DATA 2

class InputHandler
{
	
	public:
		
		struct EventStruct
		{
			InputDevice* Device;
			EventsEnum EventType;
		};
		
	private:			
			uint32_t NumOfDevices = 0;
	
			uint8_t CurrentEventReadIndex = 0;
			uint8_t CurrentEventWriteIndex = 0;
			uint8_t EventTableStatus = STATUS_EMPTY;
	
			InputDevice* InputDevicesTable[MAX_DEVICES];
			EventStruct OutputEventTable[MAX_DEVICES];	
	
	public:		
		InputHandler();		

		void addDevice(InputDevice *Device);
		void removeDevice(InputDevice *Device);
		void pollDevices(void);
	
		bool hasPendingEvent(void);
		bool isEmpty(void);
		bool isFull(void);
	
		EventStruct readEvent(void);	
		
};




#endif