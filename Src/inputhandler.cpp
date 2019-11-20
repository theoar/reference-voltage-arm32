#include "inputhandler.hpp"

#include "lcd.h"

PinCofiguration::PinCofiguration()
{
	this->Gpio = 0;
	this->Pin = 0;	
}

PinCofiguration::PinCofiguration(GPIO_TypeDef *_Gpio, 
																uint32_t _Pin, 
																uint8_t ActiveState, 
																uint32_t Mode,
																uint32_t Pull,
																uint32_t Output, 
																uint32_t Speed
																)
{
	this->configure(_Gpio, _Pin, ActiveState, Mode, Pull, Output, Speed);
}
																

void PinCofiguration::configure(GPIO_TypeDef *_Gpio, 
																uint32_t _Pin, 
																uint8_t ActiveState, 
																uint32_t Mode,
																uint32_t Pull,
																uint32_t Output, 
																uint32_t Speed
																)
{
		this->Pin = _Pin;
		this->Gpio = _Gpio;
		this->ActiveState = ActiveState;
	
		enableGPIOClock(this->Gpio);
	
		LL_GPIO_InitTypeDef InitType;		
		InitType.Mode = Mode;
		InitType.OutputType = Output;
		InitType.Pin = _Pin;
		InitType.Pull = Pull;
		InitType.Speed = Speed;

		LL_GPIO_Init(_Gpio, &InitType);
		
}

Encoder::Encoder(PinCofiguration &_PinA, PinCofiguration &_PinB)
{
	this->PinA = _PinA;
	this->PinB = _PinB;
	this->Mask = 0;
}

EventsEnum Encoder::handle()
{
	EventsEnum ReturnedEvent = EventsEnum::NO_EVENT;
	
	this->Mask <<= 2;
	this->Mask &= 15;
	
	bool IsActive = false;
	
	
	
	if( LL_GPIO_IsInputPinSet(this->PinA.Gpio, this->PinA.Pin) )
	{
		if( this->PinA.ActiveState == ACTIVE_HIGH )
			IsActive = true;		
		else		
			IsActive = false;		
	}
	else
	{
		if( this->PinA.ActiveState == ACTIVE_LOW )
			IsActive = true;		
		else		
			IsActive = false;		
	}
	
	if(IsActive)
		this->Mask |= 0<<1;
	else
		this->Mask |= 1<<1;
		
	if( LL_GPIO_IsInputPinSet(this->PinB.Gpio, this->PinB.Pin) )
	{
		if( this->PinB.ActiveState == ACTIVE_HIGH )
			IsActive = true;		
		else		
			IsActive = false;		
	}
	else
	{
		if( this->PinB.ActiveState == ACTIVE_LOW )
			IsActive = true;		
		else		
			IsActive = false;		
	}
	
	if(IsActive)
		this->Mask |= 0<<0;
	else
		this->Mask |= 1<<0;
	
	if(this->Mask==14)	
		ReturnedEvent = EventsEnum::ROTATED_LEFT;	
	if(this->Mask==13)
		ReturnedEvent = EventsEnum::ROTATED_RIGHT;	
		
	return ReturnedEvent;
}

Button::Button(PinCofiguration &_Pin, uint32_t _TimeStep, uint32_t _HoldTime)
{
	this->Pin = _Pin;
	this->CurrentState = ButtonState::NONE;
	this->CurrentTime = 0;
	this->HoldTime = _HoldTime;
	this->TimeStep = _TimeStep;
}

EventsEnum Button::handle()
{
	EventsEnum ReturedEvent = EventsEnum::NO_EVENT;	
	
	
	bool IsActive;
	if(LL_GPIO_IsInputPinSet(this->Pin.Gpio, this->Pin.Pin))
	{
		if(this->Pin.ActiveState == ACTIVE_HIGH)
			IsActive = true;
		else
			IsActive = false;
	}
	
	else
	{
		if(this->Pin.ActiveState == ACTIVE_LOW)
			IsActive = true;
		else
			IsActive = false;
	}
	
	
	switch(this->CurrentState)
	{
		case ButtonState::NONE:
		{
			if(IsActive)
			{
				this->CurrentState = ButtonState::CLICKED;
				ReturedEvent = EventsEnum::CLICKED;
				this->CurrentTime = this->HoldTime;
			}
		}			
		break;
		
		case ButtonState::CLICKED:
		{
			if(IsActive)
			{
				if(this->CurrentTime==0)
				{
					this->CurrentTime = this->HoldTime;
					this->CurrentState = ButtonState::HOLDING;
					ReturedEvent = EventsEnum::HOLDING;
				}
				else
				{
					if(this->CurrentTime!=0)
						this->CurrentTime -= this->TimeStep;
				}
			}
			else
			{
				ReturedEvent = EventsEnum::PULLED;
				this->CurrentState = ButtonState::NONE;
				this->CurrentTime = 0;
			}
		}			
		break;
		
		case ButtonState::HOLDING:
		{
			if(IsActive)
			{
				if(this->CurrentTime==0)
				{
					this->CurrentTime = this->HoldTime;
					this->CurrentState = ButtonState::HOLDING;
					ReturedEvent = EventsEnum::HOLDING;
				}
				else
				{
					if(this->CurrentTime!=0)
						this->CurrentTime -= this->TimeStep;
				}
			}
			else
			{
				ReturedEvent = EventsEnum::PULLED;
				this->CurrentState = ButtonState::NONE;
				this->CurrentTime = 0;
			}
		}			
		break;		
		
	}

	return ReturedEvent;
}

InputHandler::InputHandler()
{
	for(uint32_t X = 0; X<MAX_DEVICES; ++X)
	{
		this->InputDevicesTable[X] = 0;	
		this->OutputEventTable[X].Device = 0;
		this->OutputEventTable[X].EventType = EventsEnum::NO_EVENT;
	}
}

void InputHandler::addDevice(InputDevice *Device)
{
	NumOfDevices++;
	this->InputDevicesTable[NumOfDevices-1] = Device;
}

void InputHandler::removeDevice(InputDevice *Device)
{
	NumOfDevices--;
	
	int RemoveIndex = -1;
	for(uint32_t X = 0; X<MAX_DEVICES; ++X)
	{
		if(this->InputDevicesTable[X] == Device)
		{
			RemoveIndex = X;
			break;
		}
	}
	
	for(uint32_t X = RemoveIndex; X<NumOfDevices; ++X)
		InputDevicesTable[X] = InputDevicesTable[X+1];
	
	InputDevicesTable[NumOfDevices] = 0;
}

void InputHandler::pollDevices(void)
{	
	for(uint32_t X = 0; X<this->NumOfDevices; ++X)
	{		
		EventsEnum Event = this->InputDevicesTable[X]->handle();
		if(Event != EventsEnum::NO_EVENT)
		{			
			if(this->EventTableStatus!=STATUS_FULL)
			{
				this->EventTableStatus = STATUS_DATA;
				
				this->OutputEventTable[this->CurrentEventWriteIndex].EventType = Event;
				this->OutputEventTable[this->CurrentEventWriteIndex].Device = this->InputDevicesTable[X];				
				
				this->CurrentEventWriteIndex++;
				this->CurrentEventWriteIndex %= MAX_DEVICES;
				
				if(this->CurrentEventWriteIndex ==this->CurrentEventReadIndex)				
					this->EventTableStatus = STATUS_FULL;				
			}
			
		}
	}
	
}

bool InputHandler::hasPendingEvent(void)
{
	return this->EventTableStatus != STATUS_EMPTY;
}

InputHandler::EventStruct InputHandler::readEvent(void)
{
	EventStruct RetVal;
	
	if(this->EventTableStatus==STATUS_EMPTY)
	{		
		RetVal.Device = 0;
		RetVal.EventType = EventsEnum::NO_EVENT;		
	}
	else
	{
		RetVal = this->OutputEventTable[this->CurrentEventReadIndex];	
		
		this->CurrentEventReadIndex++;
		this->CurrentEventReadIndex %= MAX_DEVICES;
		
		if(this->CurrentEventReadIndex==this->CurrentEventWriteIndex)
			this->EventTableStatus=STATUS_EMPTY;
		
					
	}
	
	return RetVal;
	
}

bool InputHandler::isEmpty(void)
{
	return this->EventTableStatus == STATUS_EMPTY;
}

bool InputHandler::isFull(void)
{
	return this->EventTableStatus == STATUS_FULL;
}
