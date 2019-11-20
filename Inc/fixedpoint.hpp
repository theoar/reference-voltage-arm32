#include <inttypes.h>

#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

class FixedPoint
{
	private:
		uint32_t Decima;
		uint32_t Fraction;		
	
	public:				
		const static uint32_t Diver = 100000;
	
		FixedPoint(const uint32_t Dec, const uint32_t Frac)
		{
			uint32_t Temp = Frac;
			this->Decima = Dec;			
			
			while(Temp*10<this->Diver && Temp!=0)
				Temp*=10;
	
			this->Fraction = Temp;
		};
		
		FixedPoint(void)			
		{
			this->Decima = 0;
			this->Fraction = 0;
		};
	
		template<typename T> FixedPoint(const T & Val)
		{
			double Tmp = (double)Val;
			this->Decima = (uint32_t)Val;
			this->Fraction = Val*this->Diver - ((uint32_t)Val)*this->Diver;
		};

		template<typename FixedPoint> FixedPoint(const FixedPoint & Val)
		{
			this->Decima = Val.Decima;
			this->Fraction = Val.Fraction;
		};
				
				
			
		template<typename T>
		FixedPoint operator + (const T & Second) const
		{	
			FixedPoint RetVal(Second);			
			uint64_t Tmp = (this->Decima + RetVal.Decima)*this->Diver+this->Fraction+RetVal.Fraction;
			RetVal.Decima = (uint16_t)(Tmp/this->Diver);
			RetVal.Fraction = (Tmp - (uint64_t)(RetVal.Decima*this->Diver));
			
			return RetVal;
		};

		template<typename T>
		FixedPoint  & operator += (const T & Second)
		{	
			FixedPoint Part2(Second);
			*this = *this + Part2;	
			return *this;	
		};
		
		template<typename T>
		FixedPoint operator - (const T & Second) const
		{			
			FixedPoint RetVal(Second);
			uint64_t Tmp;
			if((this->Decima*this->Diver+this->Fraction) - (RetVal.Decima*this->Diver+RetVal.Fraction)<0)
				Tmp = 0;
			else
				Tmp = ((uint64_t)(this->Decima*this->Diver+this->Fraction)) - (uint64_t)((RetVal.Decima*this->Diver+RetVal.Fraction));
			
			RetVal.Decima = Tmp/this->Diver;
			RetVal.Fraction = Tmp - RetVal.Decima*this->Diver;
			
			return RetVal;
		};

		template<typename T>
		FixedPoint  & operator -= (const T & Second)	
		{
			FixedPoint Part2(Second);
			*this = *this - Part2;
			return *this;
		};
		
		template<typename T>
		FixedPoint operator / (const T & Second) const
		{			
			FixedPoint RetVal(Second);
			uint64_t Tmp;
			Tmp = ((uint64_t)this->Decima*(uint64_t)this->Diver+(uint64_t)this->Fraction)*(uint64_t)this->Diver;
			Tmp /= (uint64_t)RetVal.Decima*(uint64_t)this->Diver+(uint64_t)RetVal.Fraction;
			
			RetVal.Decima = Tmp/this->Diver;
			RetVal.Fraction = Tmp - RetVal.Decima*this->Diver;
			
			return RetVal;
		}

		template<typename T>
		FixedPoint  & operator /= (const T & Second)
		{
			FixedPoint Part2(Second);
			*this = *this/Part2;
			return *this;
		};

		template<typename T>	
		FixedPoint operator * (const T & Second) const
		{
			FixedPoint RetVal(Second);
			uint64_t Tmp;
			Tmp = (uint64_t)this->Decima*(uint64_t)this->Diver+(uint64_t)this->Fraction;	
			Tmp*=(uint64_t)RetVal.Decima*(uint64_t)this->Diver+(uint64_t)RetVal.Fraction;		
		
			Tmp/=this->Diver;
			
			RetVal.Decima = Tmp/this->Diver;
			RetVal.Fraction = Tmp - (uint64_t)RetVal.Decima*this->Diver;
			
			return RetVal;
		};	

		template<typename T>	
		FixedPoint  & operator *= (const T & Second)
		{
			FixedPoint Part2(Second);
			*this = *this * Part2;
			return *this;
		};
		
		operator uint16_t() const
		{
			return this->Decima;
		}

		operator double() const
		{
			return this->Decima+((double)this->Fraction/(double)this->Diver);
		}

		operator float() const
		{
			return this->Decima+((float)this->Fraction/(float)this->Diver);
		}

		bool operator < (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) < ( Second.Decima*Second.Diver+Second.Fraction);
		}

		bool operator > (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) > ( Second.Decima*Second.Diver+Second.Fraction);
		}

		bool operator == (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) == ( Second.Decima*Second.Diver+Second.Fraction);
		}

		bool operator <= (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) <= ( Second.Decima*Second.Diver+Second.Fraction);
		}

		bool operator >= (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) >= ( Second.Decima*Second.Diver+Second.Fraction);
		}

		bool operator != (const FixedPoint & Second) const
		{
			return (this->Decima*this->Diver+this->Fraction) != ( Second.Decima*Second.Diver+Second.Fraction);	}
			

		uint32_t getDecLen(void)
		{
			uint32_t Counter = 0;
			uint32_t Dec = this->Decima;
			while(Dec!=0)
			{
				Counter++;
				Dec/=10;
			}
			
			return Counter;
		}
		
		uint32_t getFracLen(void)
		{
			uint32_t Counter = 0;
			uint32_t Frac = this->Fraction;
			
			while((Frac%this->Diver)!=0)
			{
				Counter++;
				Frac*=10;
			}
			
			return Counter;
		}
		
		uint32_t getDecima(void)
		{
			return this->Decima;			
		}
		
		uint32_t getFraction(void)
		{
			return this->Fraction;
		}
		
		uint64_t getRawValue(void)
		{
			return this->Decima*this->Diver+this->Fraction;
		}
	
};




#endif