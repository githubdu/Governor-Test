
#include "resource.h"



class Gonvernor
{
	private:
		TCHAR	Type[10];
		TCHAR	ID[10];
		double	Dia;
		double	RSpeed;
		double	ESpeed;
		double	MSpeed;
		int		Order;
		double	Velocity[count];
	public:
		void SetGonvernor()
		{
			wsprintf (Type, TEXT ("FF0001"));
			wsprintf (ID, TEXT ("000001"));
			Dia = 200.0;
			RSpeed = 1000.0;
			ESpeed = RSpeed*1.15;
			MSpeed = RSpeed*1.2;
			Order  = 10;
			int i = 0;
			for (i=0; i<count; i++)
			{
				if (i<=0.4*count)
				{
					Velocity[i] = i*RSpeed/(0.4*count);
				}
				if ((0.4*count<i)&&(i<=0.5*count))
				{
					Velocity[i] = RSpeed;
				}
				if ((0.5*count<i)&&(i<=0.65*count))
				{
					Velocity[i] = (i-0.5*count)*(ESpeed-RSpeed)/(0.65*count-0.5*count)+RSpeed;
				}
				if ((0.65*count<i)&&(i<=0.8*count))
				{
					Velocity[i] = (i-0.65*count)*(MSpeed-ESpeed)/(0.8*count-0.65*count)+ESpeed;
				}
				if ((0.8*count<i)&&(i<=1*count))
				{
					Velocity[i] = (i-0.8*count)*(0-MSpeed)/(1*count-0.8*count)+MSpeed;
				}
			}
		}
		void GetType(TCHAR * typebuf)
		{	int i = 0;
			for (i = 0; i<sizeof(Type);i++)
			{
				typebuf[i] = Type[i];
			}
			
		}
		void GetID(TCHAR * idbuf)
		{	
			int i = 0;
			for (i = 0; i<sizeof(ID);i++)
			{
				idbuf[i] = ID[i];
			}
		}

		void GetDia(double * diabuf)
		{
			*diabuf = Dia;
		}

		void GetRSpeed(double * rspeedbuf)
		{
			*rspeedbuf = RSpeed;
		}

		void GetESpeed(double * espeedbuf)
		{
			*espeedbuf = ESpeed;
		}

		void GetMSpeed(double* mspeedbuf)
		{
			*mspeedbuf = MSpeed;
		}

		void GetOrder(int* orderbuf)
		{
			*orderbuf = Order;
		}

		void GetVelocity(double* dwVelocity,int num)
		{
			int i = 0;
			for (i=0; i<num; i++)
			{
				dwVelocity[i]=Velocity[i];
			}
		}

};