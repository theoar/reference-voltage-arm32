#ifndef MENU_HPP
#define MENU_HPP

#include <string.h>
#include <stdio.h>

#include "fixedpoint.hpp"
#include "lcd.h"
#include "ditherpwm.hpp"
#include "functions.h"


enum class MenuEvents
{
	Enter,
	Escape,
	LeftPos,
	RightPos,
	LeftVal,
	RightVal
};

enum class ExternalEvent
{
	None,
	EditFinished,
	Incremented,
	Decremented,
	Entered
};

class MenuElement
{
	protected:
		MenuElement *Parent = 0;
	public:
		virtual void display(void) = 0;
		virtual void handleEvent(const MenuEvents &Event) = 0;
	
		virtual bool isNeedToHandleEvent(const MenuEvents &Event) 
		{
			return false;
		}
		
		void setParent(MenuElement *_Parent)
		{
			this->Parent=_Parent;
		}
		
		MenuElement * getParent()
		{
			return this->Parent;
		}
};

class ExternalEventHandler
{
	public:
		virtual void handleExternalEvent(MenuElement *Source, ExternalEvent Evnt) = 0;
};

class MenuElementVariable : public MenuElement
{
	private:
		FixedPoint CurrentValue;
		FixedPoint MinValue;
		FixedPoint MaxValue;
		FixedPoint Step;
	
		ExternalEventHandler * ExternalEventObject = 0;
	
		uint8_t DecPoints = 2;
		uint8_t FracPoints = 4;
		char Title[20];
		char Prefix[8];
		char Sufix[8];
	
		uint8_t TitleLen;
		uint8_t PrefixLen;
		uint8_t SufixLen;		
		uint8_t VarLen;
	
		bool ReadOnly = false;	
		int CurrentPos = -1;
		
		uint8_t CurrentX = 0;
		
		bool isNeedToHandleEvent(const MenuEvents &Event);
	
	public:		
		MenuElementVariable() {};
		MenuElementVariable(const FixedPoint &Min,
												const FixedPoint &Max,
												const FixedPoint &Step,												
												const char * _Title,
												const char * _Prefix,
												const char * _Sufix,
												bool _ReadOnly,
												ExternalEventHandler * ExtEvntObj	);
	
		void display(void);
		void handleEvent(const MenuEvents &Event);
												
		void setValue(const FixedPoint & Value);
												
		const FixedPoint getCurrentValue() const;
												
												
	private:
		void displayTitle(void);
		void displayValue(void);
	
		bool isActiveEdit(void) { return this->CurrentPos != - 1; }
		
		void activeEdit(void);
		void setPosition(void);
		void deActiveEdit(void);
		
		uint8_t nextLeftPos(void);
		uint8_t nextRightPos(void);
		
		void increment(void);
		void decrement(void);
		
};

#define MENU_MAXELEMENTS 6
class Menu : public MenuElement
{
	private:
		MenuElement *Elements[MENU_MAXELEMENTS];
	
		uint32_t CurrentElement;
		uint32_t NumOfElements;
	
		void nextLeft(void);
		void nextRight(void);
	
	public:
		Menu(void) { this->CurrentElement = 0; this->NumOfElements = 0; }
		
		void addElement(MenuElement * Element);
		
		void display(void) { this->Elements[this->CurrentElement]->display(); }
		void handleEvent(const MenuEvents &Event);
				
};

void Menu::addElement(MenuElement * Element)
{
	if(NumOfElements+1>MENU_MAXELEMENTS)
		return;
	else
	{
		Element->setParent(this);
		this->Elements[this->NumOfElements] = Element;
		this->NumOfElements++;
	}
	

}

void Menu::nextLeft(void)
{
	if(this->CurrentElement==0)
		this->CurrentElement = this->NumOfElements-1;
	else
		this->CurrentElement--;
	
	this->display();
}

void Menu::nextRight(void)
{
	this->CurrentElement++;
	if(this->CurrentElement>=this->NumOfElements)
		this->CurrentElement=0;
	
	this->display();
}

void Menu::handleEvent(const MenuEvents &Event)
{
	if(this->Elements[this->CurrentElement]->isNeedToHandleEvent(Event))
				this->Elements[this->CurrentElement]->handleEvent(Event);
	else
	{
		switch(Event)
		{
			case MenuEvents::LeftPos:
			{
				this->nextLeft();
			}
			break;
				
			case MenuEvents::RightPos:			
			{
				this->nextRight();
			}
			break;
				
			case MenuEvents::LeftVal:			
			{				
				
			}
			break;
				
			case MenuEvents::RightVal:			
			{			
				
			}
			break;	
			
			case MenuEvents::Enter:
			{
					
			}
			break;
			
			case MenuEvents::Escape:
			{	
				
			}
			break;
		}
	}
}
#define MENUELEMENTOPTIONLIST_MAXOPTIONS 4
class MenuElementOptionList : public MenuElement
{
	private:
		char Title[16];
		uint8_t TitleLen;
	
		char OptionTabNames[MENUELEMENTOPTIONLIST_MAXOPTIONS][16];
		uint8_t OptionLenght[MENUELEMENTOPTIONLIST_MAXOPTIONS];
	
		ExternalEventHandler * ExternalHandlerTab[MENUELEMENTOPTIONLIST_MAXOPTIONS];
	
		uint32_t NumOfOptions;
		uint8_t CurrentOption;
	
		void nextLeft(void);
		void nextRight(void);
		
	
	public:
		MenuElementOptionList(const char * _Title);
	
		void addOption(const char *Option, ExternalEventHandler *Handler);
	
		void display(void);
		void handleEvent(const MenuEvents &Event);
	
		bool isNeedToHandleEvent(const MenuEvents &Event) {
			return Event==MenuEvents::LeftVal || Event==MenuEvents::RightVal || Event==MenuEvents::Enter;
		};
		
};

MenuElementOptionList::MenuElementOptionList(const char * _Title)
{
	TitleLen =  stringcopy(this->Title, _Title);
	NumOfOptions = 0;	
	CurrentOption = 0;
}

void MenuElementOptionList::nextLeft(void)
{
	if(this->CurrentOption==0)
		this->CurrentOption = this->NumOfOptions-1;
	else
		this->CurrentOption--;
	
	this->display();
}

void MenuElementOptionList::nextRight(void)
{
	this->CurrentOption++;
	if(this->CurrentOption>=this->NumOfOptions)
		this->CurrentOption = 0;
	
	this->display();
}

void MenuElementOptionList::addOption(const char *Option, ExternalEventHandler *Handler)
{
	if(this->NumOfOptions>=MENUELEMENTOPTIONLIST_MAXOPTIONS)
		return;
	else
	{
		this->OptionLenght[this->NumOfOptions] = stringcopy(this->OptionTabNames[this->NumOfOptions], Option);
		this->ExternalHandlerTab[this->NumOfOptions] = Handler;
		this->NumOfOptions++;
	}
	
	this->display();
}

void MenuElementOptionList::handleEvent(const MenuEvents &Event)
{
	ExternalEvent Evnt = ExternalEvent::None;
	
		switch(Event)
		{	
			case MenuEvents::LeftVal:			
			{				
				this->nextLeft();
			}
			break;
				
			case MenuEvents::RightVal:			
			{			
				this->nextRight();
			}
			break;	
			
			case MenuEvents::Enter:
			{
				Evnt = ExternalEvent::Entered;
				if(this->ExternalHandlerTab[this->CurrentOption]!=0)
					this->ExternalHandlerTab[this->CurrentOption]->handleExternalEvent(this, Evnt);
			}
			break;			
		
			
			default:
				return;
			break;
	}
}

void MenuElementOptionList::display(void)
{
	lcdClr();
	lcdGoTo((16 - this->TitleLen)/2,0);
	lcdPuts(this->Title);
	
	lcdGoTo((16-this->OptionLenght[this->CurrentOption]-2)/2, 1);
	lcdPutc('<');
	lcdPuts(this->OptionTabNames[this->CurrentOption]);
	lcdPutc('>');
}

class MainVoltageChangeHandler : public ExternalEventHandler
{
	private:
		DitherPwm *PwmObject;
	public:
		MainVoltageChangeHandler(DitherPwm * Obj)
		{
			this->PwmObject = Obj;
		};
		virtual void handleExternalEvent(MenuElement *Source, ExternalEvent Evnt)
		{
			MenuElementVariable* Src = dynamic_cast<MenuElementVariable*>(Source);	
			if(Evnt==ExternalEvent::EditFinished || Evnt==ExternalEvent::Decremented || Evnt==ExternalEvent::Incremented)
			{
				PwmObject->setVoltage(Src->getCurrentValue());
			}
		}
};

class NextMenuHandler : public ExternalEventHandler
{
	private:
		MenuElement *NextMenu;				
		MenuElement **CurrentMenu;	
	public:		
		NextMenuHandler(MenuElement *Next, MenuElement **MenuVar)
		{
			this->NextMenu = Next;		
			CurrentMenu = MenuVar;
		}
		
		void handleExternalEvent(MenuElement *Source, ExternalEvent Evnt)
		{
			*this->CurrentMenu = NextMenu;
		}	
};

class PrevMenuHandler : public ExternalEventHandler
{
	private:
		MenuElement **CurrentMenu;	
	
	public:		
		PrevMenuHandler(MenuElement **MenuVar)
		{				
			CurrentMenu = MenuVar;
		}
		
		void handleExternalEvent(MenuElement *Source, ExternalEvent Evnt)
		{
			*this->CurrentMenu = Source->getParent()->getParent();
		}	
};


MenuElementVariable::MenuElementVariable
												(const FixedPoint &_Min,
												const FixedPoint &_Max,
												const FixedPoint &_Step,												
												const char * _Title,
												const char * _Prefix,
												const char * _Sufix,
												bool _ReadOnly,
												ExternalEventHandler * ExtEvntObj)
{
	this->MinValue = _Min;
	this->MaxValue = _Max;
	this->Step = _Step;
	this->CurrentValue = _Min;
	
	this->ExternalEventObject = ExtEvntObj;
	
	char Index;
	if(_Title!=0)	
	{		
		Index = 0;
		while(_Title[Index]!=0)
		{	
			this->Title[Index] = _Title[Index];
			Index++;
		}
		this->Title[Index] = 0;
	}
	
	if(_Prefix!=0)
	{		
		Index = 0;
		while(_Prefix[Index]!=0)
		{	
			this->Prefix[Index] = _Prefix[Index];
			Index++;
		}
		this->Prefix[Index] = 0;
	}
	
	if(_Sufix!=0)
	{		
		Index = 0;
		while(_Sufix[Index]!=0)
		{	
			this->Sufix[Index] = _Sufix[Index];
			Index++;
		}
		this->Sufix[Index] = 0;
	}
	
	this->TitleLen = strlen(this->Title);
	this->PrefixLen = strlen(this->Prefix);
	this->SufixLen = strlen(this->Sufix);
	
	this->ReadOnly = _ReadOnly;
	
	this->DecPoints = this->MaxValue.getDecLen();
	this->FracPoints = this->Step.getFracLen();
	this->VarLen = this->DecPoints + (this->FracPoints > 0 ? this->FracPoints+1 : 0);
	this->CurrentPos = -1;
}

const FixedPoint MenuElementVariable::getCurrentValue(void) const
{
	return this->CurrentValue;
}

void MenuElementVariable::displayTitle(void)
{
	uint8_t XPos = (16-this->TitleLen)/2;
	lcdGoTo(XPos, 0);
	lcdPuts(this->Title);	
}

bool MenuElementVariable::isNeedToHandleEvent(const MenuEvents &Event)
{		
	switch(Event)
	{
		case MenuEvents::LeftPos:
		{
				if(this->isActiveEdit())
					return true;
		}
		break;
			
		case MenuEvents::RightPos:			
		{
			if(this->isActiveEdit())
				return true;
		}
		break;
			
		case MenuEvents::LeftVal:			
		{				
			return true;
		}
		break;
			
		case MenuEvents::RightVal:			
		{			
			return true;
		}
		break;	
		
		case MenuEvents::Enter:
		{
			return true;	
		}
		break;
		
		case MenuEvents::Escape:
		{	
			return true;
		}
		break;
	}
	
	return false;
}

void MenuElementVariable::displayValue(void)
{
	lcdGoTo(0,1);
	lcdPuts(this->Prefix);
	
	uint8_t CurrentDecDigits = this->CurrentValue.getDecLen();
	for(uint8_t X = CurrentDecDigits; X<this->DecPoints; ++X)
		lcdPutc('0');
	
	if(CurrentDecDigits!=0)
		lcdPrintf("%u", this->CurrentValue.getDecima());

	if(this->FracPoints>0)
	{
		lcdPutc('.');
		if(this->CurrentValue==FixedPoint(0,0))
		{
			for(uint8_t X = 0; X<this->FracPoints; ++X)
				lcdPutc('0');			
		}
		else
		{
			char Buff[16];	
			sprintf(Buff, "%u", (this->CurrentValue.getFraction()+this->CurrentValue.Diver));
			Buff[this->FracPoints+1] = 0;
			lcdPuts(&Buff[1]);
		}
	}
	lcdPuts(this->Sufix);
	lcdGoTo(this->CurrentX,1);
}

void MenuElementVariable::display()
{
	lcdClr();
	this->displayTitle();
	this->displayValue();
	
}
void MenuElementVariable::handleEvent(const MenuEvents &Event)
{
	if(this->ReadOnly)
		return;
	
	ExternalEvent Evnt = ExternalEvent::None;
	
	switch(Event)
	{
		case MenuEvents::LeftPos:
		{
				if(this->isActiveEdit())
					this->nextLeftPos();
		}
		break;
			
		case MenuEvents::RightPos:			
		{
			if(this->isActiveEdit())
				this->nextRightPos();
		}
		break;
			
		case MenuEvents::LeftVal:			
		{				
			this->decrement();
			if(!this->isActiveEdit())
			{
				Evnt = ExternalEvent::Decremented;
				if(this->ExternalEventObject!=0)
					this->ExternalEventObject->handleExternalEvent(this,Evnt);
			}
		}
		break;
			
		case MenuEvents::RightVal:			
		{			
			this->increment();	
			
			if(!this->isActiveEdit() )
			{	
				Evnt = ExternalEvent::Incremented;
				if(this->ExternalEventObject!=0)
					this->ExternalEventObject->handleExternalEvent(this,Evnt);		
			}				
		}
		break;	
		
		case MenuEvents::Enter:
		{
			this->activeEdit();			
		}
		break;
		
		case MenuEvents::Escape:
		{	
			this->deActiveEdit();
			
			if(!this->isActiveEdit())
			{
				Evnt = ExternalEvent::EditFinished;
				if( this->ExternalEventObject!=0 )
					this->ExternalEventObject->handleExternalEvent(this,Evnt);
			}
		}
		break;
	}
}

void MenuElementVariable::activeEdit(void)
{
	if(this->CurrentPos==-1)
	{
		this->CurrentPos=0;
		this->setPosition();
		lcdEnableCursor();
	}
}

void MenuElementVariable::setPosition(void)
{
	if(this->CurrentPos > this->DecPoints+this->FracPoints - 1)
		this->CurrentPos = 0;
	
	if(this->CurrentPos == -1)
		this->CurrentPos = this->DecPoints+this->FracPoints-1;
	
	if(this->CurrentPos < this->DecPoints)
		this->CurrentX = this->PrefixLen + this->CurrentPos;
	else
		this->CurrentX = this->PrefixLen + this->CurrentPos+1;
	
	lcdGoTo(this->CurrentX, 1);
}

uint8_t MenuElementVariable::nextLeftPos(void)
{
	this->CurrentPos--;
	this->setPosition();
	return this->CurrentX;
}

uint8_t MenuElementVariable::nextRightPos(void)
{
	this->CurrentPos++;
	this->setPosition();
	return this->CurrentX;
}

void MenuElementVariable::increment(void)
{
	FixedPoint Tmp;
	if(this->CurrentPos==-1)
	{
		Tmp = this->CurrentValue+this->Step;
		if(Tmp>this->MaxValue)
			this->CurrentValue=this->MaxValue;
		else
			this->CurrentValue+=this->Step;
	}
	else
	{
		uint32_t Multipler = 10;
		for(uint8_t X = 0; X< 4 - this->FracPoints + (this->DecPoints + this->FracPoints - 1 - this->CurrentPos); ++X)
			Multipler*=10;
		
		FixedPoint ObecnyStep((double)Multipler/(double)FixedPoint::Diver);
		
		Tmp = this->CurrentValue/ObecnyStep;
		
		if(Tmp.getDecima() % 10 == 9)
		{
			ObecnyStep*=9;
			this->CurrentValue -= ObecnyStep;
		}
		else			
			this->CurrentValue += ObecnyStep;
		
		if(this->CurrentValue > this->MaxValue)
			this->CurrentValue = this->MaxValue;
		else
			if(this->CurrentValue < this->MinValue)
				this->CurrentValue = this->MinValue;
	}

	
	this->displayValue();
}


void MenuElementVariable::decrement(void)
{
	FixedPoint Tmp;
	
	if(this->CurrentPos == -1)
	{
		Tmp = this->CurrentValue-this->Step;
		
		if(Tmp<this->MinValue)
			this->CurrentValue=this->MinValue;
		else
			this->CurrentValue-=this->Step;
	}
	else
	{
		uint32_t Multipler = 10;		
		for(uint8_t X = 0; X< 4 - this->FracPoints + (this->DecPoints + this->FracPoints - 1 - this->CurrentPos); ++X)
			Multipler*=10;
		
		FixedPoint ObecnyStep((double)Multipler/(double)FixedPoint::Diver);
		
		Tmp = this->CurrentValue/ObecnyStep;
		
		if(Tmp.getDecima() % 10 == 0)
		{			
			ObecnyStep*=9;
			this->CurrentValue+=ObecnyStep;
		}
		else			
			this->CurrentValue-=ObecnyStep;
		
		if(this->CurrentValue > this->MaxValue)
			this->CurrentValue = this->MaxValue;
		else
			if(this->CurrentValue < this->MinValue)
				this->CurrentValue = this->MinValue;
	}
	
	this->displayValue();
}
void MenuElementVariable::deActiveEdit(void)
{
	this->CurrentPos = -1;
	lcdDisableCursor();
}

void MenuElementVariable::setValue(const FixedPoint & Value)
{
	if(Value<this->MinValue)
		this->CurrentValue = this->MinValue;
	if(Value>this->MaxValue)
		this->CurrentValue = this->MaxValue;
	
	FixedPoint Tmp = Value/this->Step;
	
	this->CurrentValue = this->Step * Tmp.getDecima();
	
}

#endif