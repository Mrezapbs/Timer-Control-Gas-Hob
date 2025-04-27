/* 
  Date: February 1st, 2025
  Designer:	MrezaPBS(Written From Scratch) 
  Special thanks to AMD_71
*/
#include "BS66F340C.h"
/**************************************************************/
/**********************Software Overview***********************/
/**************************************************************/
/*This software is Controlling 4 Touchkeys, a 15 Pin Led module,a 
Buzzer and a Relay. Through touching keys, Some flags will be set 
or reset, after that these flags are usually checked at Display_
Handler and Display will be set. */
/**************************************************************/
/*************************Preprocessors************************/
/**************************************************************/
#define SET		1
#define RESET		0

#define	CATHODE_1	 _pb7
#define	CATHODE_2  	 _pb2
#define	CATHODE_3	 _pb3
#define	CATHODE_4	 _pb1
#define	ANODE_1		 _pb0
#define	ANODE_2		 _pa0
#define	ANODE_3		 _pa2
#define	ANODE_4		 _pe1
#define	ANODE_5		 _pc3
#define	ANODE_6		 _pa6
#define	ANODE_7		 _pa7
#define	ANODE_8		 _pc0
#define	ANODE_9		 _pc1
#define	ANODE_10	 _pe0
#define	ANODE_11	 _pc2

#define RELAY_PIN	 _pe4

#define FLAG_GLOBAL_BLINK 		FlagByte0.Bit0
#define FLAG_BLINK 			FlagByte0.Bit1
#define FLAG_COLON 			FlagByte0.Bit2
#define FLAG_COLON_BLINK		FlagByte0.Bit3
#define FLAG_LOCK_ICON 			FlagByte0.Bit4
#define FLAG_LOCK_ICON_BLINK	  	FlagByte0.Bit5
#define FLAG_COOKTIME_ICON 		FlagByte0.Bit6
#define FLAG_COOKTIME_ICON_BLINK	FlagByte0.Bit7

#define FLAG_AM_ICON			FlagByte1.Bit0
#define FLAG_AM_ICON_BLINK		FlagByte1.Bit1
#define FLAG_CLOCK_ICON			FlagByte1.Bit2
#define FLAG_CLOCK_ICON_BLINK		FlagByte1.Bit3
#define FLAG_COOKENDTIME_ICON		FlagByte1.Bit4
#define FLAG_COOKENDTIME_ICON_BLINK 	FlagByte1.Bit5
#define FLAG_PM_ICON			FlagByte1.Bit6
#define FLAG_PM_ICON_BLINK		FlagByte1.Bit7

#define FLAG_SPEAKER_ICON		FlagByte2.Bit0
#define FLAG_SPEAKER_ICON_BLINK		FlagByte2.Bit1
#define FLAG_ALARM_ICON			FlagByte2.Bit2
#define FLAG_ALARM_ICON_BLINK		FlagByte2.Bit3
#define FLAG_LIGHT_ICON			FlagByte2.Bit4
#define FLAG_LIGHT_ICON_BLINK		FlagByte2.Bit5
#define FLAG_DONT_CALIBRATE		FlagByte2.Bit6
#define FLAG_SHORT_TOUCH		FlagByte2.Bit7

#define FLAG_HOLD_TOUCH			FlagByte3.Bit0
#define	FLAG_TOUCH_DIAGNOSE		FlagByte3.Bit1
#define FLAG_BUZZER_EN			FlagByte3.Bit2	
#define FLAG_AMPM_CLOCK			FlagByte3.Bit3
#define FLAG_HALF_SECOND		FlagByte3.Bit4
#define FLAG_LOCK_STATE			FlagByte3.Bit5
#define FLAG_COOKTIME_COUNT_START 	FlagByte3.Bit6
#define FLAG_ALARMTIME_COUNT_START 	FlagByte3.Bit7

#define FLAG_COOKTIME_WORKING	   	FlagByte4.Bit0
#define FLAG_ALARMTIME_WORKING 		FlagByte4.Bit1
#define FLAG_ALARMTIME_ENDED		FlagByte4.Bit2
#define FLAG_COOKTIME_EDIT		FlagByte4.Bit3
#define FLAG_COOKENDTIME_WORKING	FlagByte4.Bit4
#define FLAG_NO_BEEP			FlagByte4.Bit5
#define FLAG_MANUALLY_ENDED		FlagByte4.Bit6
#define FLAG_E2P_WRITE			FlagByte4.Bit7

#define FLAG_ALARMTIME_EDIT 		FlagByte5.Bit0
#define FLAG_DECIDE_WHICH_MENU		FlagByte5.Bit1
#define FLAG_COOKENDTIME_EDIT		FlagByte5.Bit2
#define FLAG_SETTINGS_CHANGED		FlagByte5.Bit3
#define FLAG_CLOCK_CHANGED		FlagByte5.Bit4

#define UP_TCH01	30		//setting
#define UP_TCH02	30		//play
#define UP_TCH03	40		//Down
#define UP_TCH04	40		//Up

#define DOWN_TCH01	20
#define DOWN_TCH02	20
#define DOWN_TCH03	30
#define DOWN_TCH04	30

#define	TCH_COUNTER_CONST 5

#define _DF_DISPLAY_E		10
#define _DF_DISPLAY_F		11
#define _DF_DISPLAY_n		12
#define _DF_DISPLAY_d		13
#define _DF_DISPLAY_DASH	14
#define _DF_DISPLAY_b		15
#define _DF_DISPLAY_NOTHING	99

#define BUZZER_PLAY		_pas02=_pas12

/************Menus**********/
#define START_UP			1
#define NORMAL				2
#define LOCK_SETTING		    	3
#define CLOCK_SETTING		    	4
#define BUZZER_SETTING		  	5
#define LIGHT_SETTING			6
#define COOKTIME_SETTING		7
#define COOKENDTIME_SETTING 		8
#define ALARMTIME_SETTING		9
#define COOK_TIME_DOWNCOUNTER  		10
#define ALARM_TIME_DOWNCOUNTER 		11
#define END_MENU			12

#define TEST_MENU			99

#define _7MINUTES			420
#define _8MINUTES			480
/**************************************************************/
/*************************FLAGS********************************/
/**************************************************************/
typedef struct OneBitVariable
{
	unsigned char Bit0	: 1 ;
	unsigned char Bit1	: 1 ;
	unsigned char Bit2	: 1 ;
	unsigned char Bit3	: 1 ;
	unsigned char Bit4	: 1 ;
	unsigned char Bit5	: 1 ;
	unsigned char Bit6	: 1 ;
	unsigned char Bit7	: 1 ;
	
}OneBit;


OneBit FlagByte0;
OneBit FlagByte1;
OneBit FlagByte2;
OneBit FlagByte3;
OneBit FlagByte4;
OneBit FlagByte5;

/**************************************************************/
/*************************Variables****************************/
/**************************************************************/
unsigned char DisplayMenu = START_UP;
unsigned char _1stDigit,_2ndDigit,_3rdDigit,_4thDigit;
unsigned char DivRem = 0;
unsigned char DivQuot = 0;
unsigned int BuzzerTime=0;
unsigned char SoundType = 0;
unsigned char StartingDelay = 0;

unsigned char KeyState1 = 0;	
unsigned char KeyState2 = 0;
unsigned int TchHoldTime = 0;
unsigned char TouchRefresh = 0;
unsigned char TouchBit1 = 0;	
unsigned int TchModule0 = 0;
unsigned int TchModule2 = 0;
unsigned int TchReference01 = 0;
unsigned int TchReference02 = 0;
unsigned int TchReference03 = 0;
unsigned int TchReference04 = 0;
unsigned char TchCounter01 = 0;		
unsigned char TchCounter02 = 0;	
unsigned char TchCounter03 = 0;		
unsigned char TchCounter04 = 0;	
unsigned char TchCalibrTime01 = 0;	
unsigned char TchCalibrTime02 = 0;	
unsigned char TchCalibrTime03 = 0;
unsigned char TchCalibrTime04 = 0;
char ClockHour 	 	 = 12;
char ClockMinute 	 = 0;
char ClockSecond  	 = 0;
char CookTimeHour 	 = 0;
char CookTimeMinute  = 0;
char CookTimeSecond  = 0;
char AlarmTimeHour 	 = 0;
char AlarmTimeMinute = 0;
char AlarmTimeSecond = 0;
char CookEndTimeHour   = 12;
char CookEndTimeMinute = 0;
char CookEndTimeSecond = 0;
char CookEndTimeHourStart;
char CookEndTimeMinuteStart;
unsigned int AlarmEndSeconds,CookEndSeconds;//The alarm goes off after 7 minutes if no keys are touched
unsigned char BuzzerSoundType = 1;
unsigned char BuzzerSoundLevel = 180;
unsigned char BacklightType= 3;
unsigned char DimLevel = 8;
unsigned char SettingTimerSec = 0;
unsigned char Initializer = RESET;
unsigned char LastDisplayState = START_UP;
unsigned int BuzzerDelay;
/**************************************************************/
/*************************Functions****************************/
/**************************************************************/
void Decide_Which_Menu(void)
{
	/////
	if(FLAG_COOKENDTIME_WORKING)
	{
		if(FLAG_ALARMTIME_WORKING)	DisplayMenu = ALARM_TIME_DOWNCOUNTER;
		else				DisplayMenu = NORMAL;
	}
	////
	else if(!FLAG_COOKTIME_WORKING && !FLAG_ALARMTIME_WORKING)
	{
		DisplayMenu = NORMAL;	
	}
	else if (FLAG_COOKTIME_WORKING && FLAG_ALARMTIME_WORKING)
	{
		if(CookTimeHour < AlarmTimeHour)
		{
			DisplayMenu = COOK_TIME_DOWNCOUNTER;	
		}	
		else if (AlarmTimeHour < CookTimeHour)
		{
			DisplayMenu = ALARM_TIME_DOWNCOUNTER;
		}
		else // ==
		{
			if(CookTimeMinute<AlarmTimeMinute)
			{
				DisplayMenu = COOK_TIME_DOWNCOUNTER;
			}
			else if(AlarmTimeMinute<CookTimeMinute)
			{
				DisplayMenu = ALARM_TIME_DOWNCOUNTER;
			}
			else
			{
				if(CookTimeSecond<AlarmTimeSecond)
				{
					DisplayMenu = COOK_TIME_DOWNCOUNTER;
				}
				else
					DisplayMenu = ALARM_TIME_DOWNCOUNTER;
			}
		}	
	}
	else if(FLAG_COOKTIME_WORKING)
	{
		DisplayMenu = COOK_TIME_DOWNCOUNTER;
	}
	else if(FLAG_ALARMTIME_WORKING)
	{
		DisplayMenu = ALARM_TIME_DOWNCOUNTER;	
	}	
}
void Division( volatile unsigned char Num , volatile unsigned char Denom )
{
	DivQuot	= Num / Denom ;
	DivRem	= Num % Denom ;
}
void Anode_Setter(unsigned char digit_number)
{
	switch(digit_number)
	{
		case 0: ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_5=ANODE_6=1;  	 	break;
		case 1: ANODE_2=ANODE_3=1;					        break;
		case 2: ANODE_1=ANODE_2=ANODE_4=ANODE_5=ANODE_7=1;		 	break;
		case 3: ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_7=1;			break;
		case 4: ANODE_2=ANODE_3=ANODE_6=ANODE_7=1;				break;
		case 5: ANODE_1=ANODE_3=ANODE_4=ANODE_6=ANODE_7=1;			break;
		case 6: ANODE_1=ANODE_3=ANODE_4=ANODE_5=ANODE_6=ANODE_7=1;		break;			
		case 7: ANODE_1=ANODE_2=ANODE_3=1;					break;	
		case 8: ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_5=ANODE_6=ANODE_7=1;	break;	
		case 9: ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_6=ANODE_7=1;		break;
		case 10:ANODE_1=ANODE_4=ANODE_5=ANODE_6=ANODE_7=1;			break;//E
		case 11:ANODE_1=ANODE_5=ANODE_6=ANODE_7=1;				break;//F
		case 12:ANODE_3=ANODE_5=ANODE_7=1;  					break;//n
		case 13:ANODE_2=ANODE_3=ANODE_4=ANODE_5=ANODE_7=1; 			break;//d
		case 14:ANODE_7=1; 				    			break;//-
		case 15:ANODE_3=ANODE_4=ANODE_5=ANODE_6=ANODE_7=1;			break;//b
		case 99:ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_5=ANODE_6=ANODE_7=0; 	break;//show nothing
	}	
}
void Display_Scan(void)
{
	static unsigned char DisplayIteration = 1;
	DisplayIteration++;
	if(DisplayIteration > DimLevel) DisplayIteration =1;
	switch(DisplayIteration)
	{
		case 1:
		CATHODE_2=1;CATHODE_3=1;CATHODE_4=1;CATHODE_1=0;
		Anode_Setter(_1stDigit);
		if(FLAG_COLON)				ANODE_8  = SET;
		else					ANODE_8  = RESET;
		if(FLAG_LOCK_ICON)			ANODE_9  = SET;
		else					ANODE_9  = RESET;
		if(FLAG_COOKTIME_ICON)			ANODE_10 = SET;
		else					ANODE_10 = RESET;
		break;
		
		case 3:
		CATHODE_1=1;CATHODE_3=1;CATHODE_4=1;CATHODE_2=0;
		Anode_Setter(_2ndDigit);
		if(FLAG_AM_ICON)			ANODE_8  = SET;
		else					ANODE_8  = RESET;
		if(FLAG_CLOCK_ICON)			ANODE_9  = SET;
		else					ANODE_9  = RESET;
		if(FLAG_COOKENDTIME_ICON)		ANODE_10 = SET;
		else					ANODE_10 = RESET;
		break;
		
		case 5:
		CATHODE_1=1;CATHODE_2=1;CATHODE_4=1;CATHODE_3=0;
		Anode_Setter(_3rdDigit);
		if(FLAG_PM_ICON)			ANODE_8  = SET;
		else					ANODE_8  = RESET;
		if(FLAG_SPEAKER_ICON)			ANODE_9  = SET;
		else					ANODE_9  = RESET;
		if(FLAG_ALARM_ICON)			ANODE_10 = SET;
		else					ANODE_10 = RESET;
		break;
		
		case 7:
		CATHODE_1=1;CATHODE_2=1;CATHODE_3=1;CATHODE_4=0;
		Anode_Setter(_4thDigit);
		if(FLAG_LIGHT_ICON)			ANODE_9  = SET;
		else					ANODE_9  = RESET;
		break;
		
		case 2:case 4:case 6:case 8:
		CATHODE_1=1;CATHODE_2=1;CATHODE_3=1;CATHODE_4=1;
		ANODE_1=ANODE_2=ANODE_3=ANODE_4=ANODE_5=ANODE_6=ANODE_7=ANODE_8=ANODE_9=ANODE_10=ANODE_11=0;
		break;
	}	
}
/**************************************************************/
/************************Interrupt Routines********************/
/**************************************************************/
void __attribute((interrupt(0x18))) ISR_CTM1( void )//DEFINE_ISR (Interrupt_CTM1, 0x18)	//timer
{
	static unsigned char _64usCounter=0;
	static unsigned int _1MsecCounter=0;
	_ctm1pf = 0;	//about 64us

	_64usCounter++;
	if(_64usCounter > 15)		//about 1ms
	{
		_64usCounter=0;
		if(BuzzerDelay<1000) BuzzerDelay++;
		Display_Scan();
		_1MsecCounter++;
		if(FLAG_BUZZER_EN)
		{
			if(BuzzerTime > 0)  BuzzerTime--;
			else 		     {FLAG_BUZZER_EN = RESET;}
		}
		if(_1MsecCounter>=10)
		{
			_1MsecCounter = 0;
			if(TchHoldTime<500)	TchHoldTime++;
		}		
	}
}
void __attribute((interrupt(0x28))) ISR_TmBase1( void )//0.5second
{
	///////////Half Second Routine
	{
		FLAG_HALF_SECOND = !FLAG_HALF_SECOND;
		if(FLAG_GLOBAL_BLINK)		
		{
			FLAG_BLINK  = !FLAG_BLINK;
		}			
		
		if(StartingDelay<250)	StartingDelay++;
		
		if(!FLAG_DONT_CALIBRATE)
		{
			if(TchCalibrTime01<250)	TchCalibrTime01++;
			if(TchCalibrTime02<250)	TchCalibrTime02++;
			if(TchCalibrTime03<250)	TchCalibrTime03++;
			if(TchCalibrTime04<250)	TchCalibrTime04++;	
		}
	}
	///////////One Second Routine
	if(FLAG_HALF_SECOND)
	{		
		if(DisplayMenu==END_MENU) 
		{if(CookEndSeconds<_8MINUTES) CookEndSeconds++;}
		else	CookEndSeconds=0;
		if(CookEndSeconds>= _7MINUTES)	FLAG_NO_BEEP = SET;
		if(FLAG_ALARMTIME_ENDED)
		{if(AlarmEndSeconds<_8MINUTES)AlarmEndSeconds++;}
		else  AlarmEndSeconds=0;
		if(AlarmEndSeconds>= _7MINUTES) FLAG_ALARMTIME_ENDED = RESET;
		
		if(SettingTimerSec>=1) 		SettingTimerSec--;		
		
		ClockSecond++;
		if(ClockSecond >=60)
		{
			ClockSecond = 0;
			ClockMinute++;
			if(ClockMinute >= 60)
			{
				ClockMinute = 0;
				ClockHour++;
				if(ClockHour > 23)
					ClockHour = 0;
			}
			
		}	
		
		if(FLAG_COOKTIME_COUNT_START && FLAG_COOKTIME_WORKING)
		{
			if(CookTimeHour>0)
			{
				if(CookTimeMinute>0)
				{
					if(CookTimeSecond>0)
					{
						CookTimeSecond--;
					}
					else//CookTimeSecond==0
					{
						CookTimeSecond = 59;
						CookTimeMinute--;
					}					
				}
				else//CookTimeMinute==0
				{
					if(CookTimeSecond>0)
					{
						CookTimeSecond--;
					}
					else//CookTimeSecond==0
					{
						CookTimeHour--;
						CookTimeMinute = 59;
						CookTimeSecond = 59;
					}
				}
			}
			else//CookTimeHour==0
			{
				if(CookTimeMinute>0)
				{
					if(CookTimeSecond>0)
					{
						CookTimeSecond--;
					}
					else//CookTimeSecond<0
					{
						CookTimeMinute--;
						CookTimeSecond = 59;
					}					
				}
				else//CookTimeMinute ==0
				{
					if(CookTimeSecond>0)
					{
						CookTimeSecond--;
					}
					else//CookTimeSecond==0
					{
						if(DisplayMenu != COOKTIME_SETTING)
						{
							DisplayMenu = END_MENU;
							FLAG_COOKTIME_WORKING =RESET;
							CookEndSeconds=0;
						}						
					}
				}	
			}
		}
		if(FLAG_ALARMTIME_COUNT_START && FLAG_ALARMTIME_WORKING)
		{
			if(AlarmTimeHour>0)
			{
				if(AlarmTimeMinute>0)
				{
					if(AlarmTimeSecond>0)
					{
						AlarmTimeSecond--;
					}
					else//AlarmTimeSecond==0
					{
						AlarmTimeSecond = 59;
						AlarmTimeMinute--;
					}					
				}
				else//AlarmTimeMinute==0
				{
					if(AlarmTimeSecond>0)
					{
						AlarmTimeSecond--;
					}
					else//CookTimeSecond==0
					{
						AlarmTimeHour--;
						AlarmTimeMinute=59;
						AlarmTimeSecond=59;
					}
				}
			}
			else//AlarmTimeHour==0
			{
				if(AlarmTimeMinute>0)
				{
					if(AlarmTimeSecond>0)
					{
						AlarmTimeSecond--;
					}
					else//AlarmTimeSecond==0
					{
						AlarmTimeMinute--;
						AlarmTimeSecond = 59;
					}					
				}
				else//AlarmTimeMinute ==0
				{
					if(AlarmTimeSecond>0)
					{
						AlarmTimeSecond--;
					}
					else//AlarmTimeSecond==0
					{
						if(!FLAG_COOKTIME_WORKING)	
						{
							if(DisplayMenu != END_MENU) DisplayMenu = NORMAL;
							else						DisplayMenu = END_MENU;						
						}
						else						DisplayMenu = COOK_TIME_DOWNCOUNTER;
						FLAG_ALARMTIME_ENDED = SET;
						AlarmEndSeconds=0;
						FLAG_ALARMTIME_WORKING = RESET;
					}
				}	
			}
		}
		if(FLAG_COOKENDTIME_WORKING && !FLAG_COOKENDTIME_EDIT)
		{
			if((ClockHour == CookEndTimeHourStart)&&(ClockMinute == CookEndTimeMinuteStart))
			{
				FLAG_COOKTIME_WORKING 	  = SET;
				FLAG_COOKTIME_COUNT_START = SET;
				FLAG_COOKENDTIME_WORKING  = RESET;
				FLAG_DECIDE_WHICH_MENU 	  = SET;
				////
//				CookTimeSecond=0;
//				CookTimeMinute++;		
				//// 
			}
		}
	}
}
/**************************************************************/
/*************************Functions****************************/
/**************************************************************/
void E2p_Write(unsigned char E2pAdrs,unsigned char E2pData)
{
	_eea=E2pAdrs;
	_eed=E2pData;
	_mp1l=0x40;
	_mp1h=0x01;
	_emi=0;
	_iar1&=0xFD;
	_iar1|=0x08;
	_iar1|=0x04;
	_emi=1;
	while((_iar1 & 0x04)>0)	_nop();
	_iar1=0x00;
	_mp1h=0x00;
}
unsigned char E2p_Read(unsigned char E2pAdrs)
{
	_eea=E2pAdrs;
	_mp1l=0x40;
	_mp1h=0x01;
	_iar1&=0xF7;
	_iar1|=0x02;
	_iar1|=0x01;
	while((_iar1 & 0x01)>0)	_nop();
	_iar1=0x00;
	_mp1h=0x00;
	return(_eed);
}
void Set_Buzzer_SoundType(void)
{
	switch(BuzzerSoundType)
	{
		case 1:BuzzerSoundLevel=180;
		break;
		case 2:BuzzerSoundLevel =80;
		break;
		case 3:BuzzerSoundLevel =50;
		break;	
	}
	_ctm0al = BuzzerSoundLevel ;
	_ctm0ah = 0;
}
void Set_Backlight_Level(void)
{
	if(BacklightType == 3)	    DimLevel = 8;
	else if(BacklightType == 2) DimLevel = 12;
	else if(BacklightType == 1) DimLevel = 16;
}
void Sound(char type)
{
	if(FLAG_BUZZER_EN==0)
	{
		switch(type)
		{
			case 1:SoundType=1; BuzzerTime=200;
			break;
			
			case 2:SoundType=2; BuzzerTime=1000;
			break;
			
			case 3:SoundType=3; BuzzerTime=500;
			break;	
		}
		FLAG_BUZZER_EN=1;
	}
	else
	{}
}
void Set_Cookend_Time(void)
{
	if(CookEndTimeHour>=CookTimeHour)
	{
		CookEndTimeHourStart=CookEndTimeHour-CookTimeHour;								
	}
	else
	{
		CookEndTimeHourStart=24+CookEndTimeHour-CookTimeHour;	
	}
	
	if(CookEndTimeMinute>=CookTimeMinute)
	{
		CookEndTimeMinuteStart=CookEndTimeMinute-CookTimeMinute;
	}
	else
	{
		CookEndTimeMinuteStart=60+CookEndTimeMinute-CookTimeMinute;
		CookEndTimeHourStart--;
	}	
}
void Buzzer_Handler(void)
{
	switch(SoundType)
	{
		case 1:
		if(BuzzerTime>0)      BUZZER_PLAY = SET;
		else 		      BUZZER_PLAY = RESET;
		break;

		case 2:
		if((BuzzerTime >500 && BuzzerTime<700) ||(BuzzerTime >800 && BuzzerTime<1000)) BUZZER_PLAY = SET;
		else 									       BUZZER_PLAY = RESET;
		break;
		
		case 3:
		if(BuzzerTime>0)      BUZZER_PLAY = SET;
		else 		      BUZZER_PLAY = RESET;
		break;
	}
}
void Cookend_Time_DownKey_Function(void)
{
	if(FLAG_COOKENDTIME_WORKING)
	{
		if(CookEndTimeHour > ClockHour+CookTimeHour)
		{
			if(CookEndTimeMinute>0) CookEndTimeMinute--;
			else
			{
				CookEndTimeMinute = 59;
				if(CookEndTimeHour>0) CookEndTimeHour--;
				else		      CookEndTimeHour = 23;
			}
		}
		else if(CookEndTimeHour==(ClockHour+CookTimeHour))
		{
			if(CookEndTimeMinute > ClockMinute+CookTimeMinute+1)
			{
				if(CookEndTimeMinute>0) CookEndTimeMinute--;
			}
		}
	}	
}
void Cookend_time_UpKey_Fucntion(void)
{
	Set_Cookend_Time();
	FLAG_COOKENDTIME_WORKING=SET;
	FLAG_COOKENDTIME_EDIT	= SET;						
	CookEndTimeMinute++;
	if(CookEndTimeMinute >=60)
	{
		CookEndTimeMinute = 0;
		CookEndTimeHour++;
		if(CookEndTimeHour >= 24)
			CookEndTimeHour=0;
	}	
}
void Cooktime_Upkey_Function(void)
{
	FLAG_COOKTIME_EDIT = SET;
	///
	FLAG_COOKTIME_WORKING = SET;
	FLAG_COOKTIME_COUNT_START = RESET;
	Set_Cookend_Time();
	///
	if(!FLAG_COOKENDTIME_WORKING) 	        CookTimeMinute++;
	else if(CookEndTimeHourStart>ClockHour) CookTimeMinute++;
	else if(CookEndTimeHourStart==ClockHour){if(CookEndTimeMinuteStart>ClockMinute+1) CookTimeMinute++;}
	if(CookTimeMinute > 59)
	{
		CookTimeMinute = 0;
		CookTimeHour++;
		if(CookTimeHour > 10)
			CookTimeHour = 10;
	}	
	//CookTimeSecond = 0;	
}
void AlarmTime_Setting_PlayKey_Function(void)
{
	if(AlarmTimeHour || AlarmTimeMinute || AlarmTimeSecond)
	{
	    FLAG_ALARMTIME_WORKING = SET;
	    FLAG_ALARMTIME_COUNT_START = SET;	
	}
	if(FLAG_ALARMTIME_EDIT &&(!AlarmTimeHour && !AlarmTimeMinute))
	{
	    FLAG_ALARMTIME_WORKING = RESET;
	    FLAG_ALARMTIME_COUNT_START = RESET;	
	}
	Decide_Which_Menu();
	if(FLAG_ALARMTIME_EDIT)
	{
	    FLAG_ALARMTIME_EDIT = RESET;
	    AlarmTimeSecond = 0;
	}	
}
void CookTime_Setting_Downkey_Function(void)
{
	FLAG_COOKTIME_EDIT = SET;			
	CookTimeMinute--;
	if(CookTimeMinute < 0)
	{
		if(CookTimeHour >0)
		{	
			CookTimeMinute = 59;
			CookTimeHour--;
			if(CookTimeHour < 0)
				CookTimeHour = 0;
		}
		else
		{
			CookTimeMinute = 0;
		}
	}	
	if(!CookTimeMinute && !CookTimeHour)
	{
		FLAG_COOKENDTIME_WORKING = RESET;
		CookTimeSecond =0;
	}	
}
void Setting_Key_Function(void)
{
	SettingTimerSec = 5;
	FLAG_SHORT_TOUCH=1;	
	Sound(1);
	if(!FLAG_TOUCH_DIAGNOSE)
	{
		if(!FLAG_LOCK_STATE)
		{
			if(FLAG_ALARMTIME_ENDED)	FLAG_ALARMTIME_ENDED = RESET;
			else
			{
				switch(DisplayMenu)
				{
					case START_UP:
					{
						DisplayMenu = CLOCK_SETTING;			
					}
					break;
					
					case NORMAL:
					case LOCK_SETTING:
					case CLOCK_SETTING:
					case BUZZER_SETTING:
					case LIGHT_SETTING:
					{
						if(DisplayMenu == NORMAL) LastDisplayState = DisplayMenu;
						if(!FLAG_SETTINGS_CHANGED)
						{
							if(!FLAG_COOKTIME_WORKING && !FLAG_ALARMTIME_WORKING)
							{
								DisplayMenu++; 
								if((DisplayMenu==CLOCK_SETTING)&&(FLAG_COOKENDTIME_WORKING))
								{
									DisplayMenu = BUZZER_SETTING;		
								}
								if(DisplayMenu==LIGHT_SETTING) {FLAG_BUZZER_EN=0;Sound(1);}
								if(DisplayMenu > LIGHT_SETTING)	DisplayMenu = NORMAL;
							}
							else
							{
								if(DisplayMenu == LOCK_SETTING)	
								{	
									if(FLAG_ALARMTIME_WORKING && !FLAG_COOKTIME_WORKING &&!FLAG_COOKENDTIME_WORKING)
										DisplayMenu = CLOCK_SETTING;
									else
										DisplayMenu = BUZZER_SETTING;
								}
								else if(DisplayMenu==CLOCK_SETTING)
									DisplayMenu = BUZZER_SETTING;
								else if(DisplayMenu == BUZZER_SETTING)	{DisplayMenu = LIGHT_SETTING;FLAG_BUZZER_EN=0;Sound(1);}
								else if(DisplayMenu == LIGHT_SETTING)	DisplayMenu = LastDisplayState;
							}	
						}
						else
						{
							FLAG_SETTINGS_CHANGED = RESET;
							DisplayMenu = LastDisplayState;
						}
						if(DisplayMenu==BUZZER_SETTING)	BuzzerDelay=0;
						if(FLAG_CLOCK_CHANGED){FLAG_CLOCK_CHANGED = RESET;ClockSecond=0;}
					}
					break;
					
					case COOKTIME_SETTING:
					case COOKENDTIME_SETTING:
					case ALARMTIME_SETTING:
					case COOK_TIME_DOWNCOUNTER:
					case ALARM_TIME_DOWNCOUNTER:
					{
						LastDisplayState = DisplayMenu;
						DisplayMenu = LOCK_SETTING;	
					}
					break;
					
					case END_MENU:
					{
						FLAG_NO_BEEP = SET;
					}
					break;				
				}
			}
		}
		else 
		{
			switch(DisplayMenu)
			{
				case START_UP:
				DisplayMenu = NORMAL;
				break;
				
				case LOCK_SETTING:
				DisplayMenu = LastDisplayState;
				break;
				
				case NORMAL:				
				case COOK_TIME_DOWNCOUNTER:
				case ALARM_TIME_DOWNCOUNTER:
				LastDisplayState = DisplayMenu;
				DisplayMenu = LOCK_SETTING;
				break;
				
				case END_MENU:
				FLAG_NO_BEEP = SET;
				if(FLAG_ALARMTIME_WORKING)	DisplayMenu = ALARM_TIME_DOWNCOUNTER;
				else 				DisplayMenu = NORMAL;
				break;
			}	
		}
	}
}
void Play_Key_Function(void)
{
	FLAG_SHORT_TOUCH=1;	
	if(!FLAG_LOCK_STATE)
	{
		SettingTimerSec = 5;	
		if(!FLAG_TOUCH_DIAGNOSE)
		{
			if(FLAG_ALARMTIME_ENDED)	FLAG_ALARMTIME_ENDED = RESET;
			else
			{
				Sound(1);
				switch(DisplayMenu)
				{
					case TEST_MENU:
					break;
					
					case START_UP:
					{
						DisplayMenu = CLOCK_SETTING;
					}
					break;
					
					case LOCK_SETTING:
					{
						if(!FLAG_COOKTIME_WORKING && !FLAG_ALARMTIME_WORKING && !FLAG_COOKENDTIME_WORKING) DisplayMenu = NORMAL;
						else
						{	
							if(FLAG_COOKTIME_WORKING || FLAG_ALARMTIME_WORKING)
								Decide_Which_Menu();
							else 
								DisplayMenu = LastDisplayState;
						}
					}
					break;
					
					case NORMAL:
					case CLOCK_SETTING:
					case BUZZER_SETTING:
					case LIGHT_SETTING:
					{
						DisplayMenu = COOKTIME_SETTING;
						if(FLAG_CLOCK_CHANGED){FLAG_CLOCK_CHANGED = RESET;ClockSecond=0;}
					}
					break;
					
					case COOKTIME_SETTING:
					{
						if(CookTimeHour || CookTimeMinute || CookTimeSecond)
						{
							if(!FLAG_COOKENDTIME_WORKING)
							{
								FLAG_COOKTIME_WORKING =SET; 
								FLAG_COOKTIME_COUNT_START = SET;
							}
						}		
						if(FLAG_COOKTIME_WORKING)
						{
							if(FLAG_COOKTIME_EDIT)
							{
								if(!CookTimeHour && !CookTimeMinute )
									{DisplayMenu = END_MENU; FLAG_MANUALLY_ENDED = SET;FLAG_NO_BEEP = SET;   FLAG_COOKTIME_WORKING = RESET;FLAG_ALARMTIME_ENDED = SET;}
								else
									Decide_Which_Menu();							
							}
							else
							{
								DisplayMenu = COOKENDTIME_SETTING;
							}
						}
						else
						{
							if(!FLAG_COOKTIME_EDIT)
							{
								if(!FLAG_COOKENDTIME_WORKING) DisplayMenu = ALARMTIME_SETTING;
								else if (FLAG_COOKENDTIME_WORKING) DisplayMenu = COOKENDTIME_SETTING;
							}
							else if (!CookTimeHour && !CookTimeMinute && FLAG_COOKTIME_EDIT)
							{
								DisplayMenu = END_MENU;
								FLAG_MANUALLY_ENDED=SET;FLAG_ALARMTIME_ENDED = SET;
								FLAG_NO_BEEP = SET;
							}
							else  DisplayMenu = COOKENDTIME_SETTING;	
						}
						if(FLAG_COOKTIME_EDIT)
						{
							FLAG_COOKTIME_EDIT = RESET;
							CookTimeSecond = 0;
						}					
					}
					break;
					
					case COOKENDTIME_SETTING:
					{
						if(FLAG_COOKENDTIME_WORKING)
						{
							if(FLAG_COOKENDTIME_EDIT)
							{
								if(FLAG_ALARMTIME_WORKING)	DisplayMenu = ALARM_TIME_DOWNCOUNTER;
								else						DisplayMenu = NORMAL;
								FLAG_COOKTIME_COUNT_START = RESET;
								FLAG_COOKTIME_WORKING = RESET;	
								if(CookTimeSecond>10)
								{
									CookTimeSecond=0;
									if(CookTimeMinute<59)CookTimeMinute++;
								}
								Set_Cookend_Time();
							}
							else
							{
								DisplayMenu = ALARMTIME_SETTING;
							}	
						}
						else
						{	
							DisplayMenu = ALARMTIME_SETTING;						
						}
						FLAG_COOKENDTIME_EDIT = RESET;					
					}
					break;
					 
					case ALARMTIME_SETTING:
					{
						AlarmTime_Setting_PlayKey_Function();
					}
					break;
					
					case COOK_TIME_DOWNCOUNTER:
					{
						DisplayMenu = COOKTIME_SETTING;		
					}
					break;
					
					case ALARM_TIME_DOWNCOUNTER:
					{
						DisplayMenu = COOKTIME_SETTING;
					}
					break;
					
					case END_MENU:
					{
						FLAG_NO_BEEP = SET;	
						if(!FLAG_ALARMTIME_WORKING)	DisplayMenu = NORMAL;
						else				DisplayMenu = ALARM_TIME_DOWNCOUNTER;
						FLAG_MANUALLY_ENDED   = RESET;
					}
					break;
				}
			}
		}
	}
	else 
	{
		if(FLAG_ALARMTIME_ENDED)	FLAG_ALARMTIME_ENDED = RESET;	
	}
}
void Down_Key_Funciton(void)
{
	FLAG_SHORT_TOUCH=1;
	if(!FLAG_LOCK_STATE || DisplayMenu==LOCK_SETTING)
	{
		SettingTimerSec = 5;
		if(!FLAG_TOUCH_DIAGNOSE)
		{
			if(FLAG_ALARMTIME_ENDED)	FLAG_ALARMTIME_ENDED = RESET;
			else
			{
				Sound(1);
				switch(DisplayMenu)
				{
					case TEST_MENU:
					break;			
					
					case START_UP:
					{
						Sound(1);
						DisplayMenu = CLOCK_SETTING;	
					}
					break;
					case CLOCK_SETTING:
					{
						ClockMinute--;
						if(ClockMinute < 0)
						{
							ClockMinute = 59;
							ClockHour--;
							if(ClockHour < 0)
								ClockHour = 23;
						}		
						FLAG_SETTINGS_CHANGED=SET;
						FLAG_CLOCK_CHANGED = SET;
					}
					break;
					
					case LOCK_SETTING:
					{
						FLAG_LOCK_STATE = RESET;
						FLAG_E2P_WRITE	= SET;	
						FLAG_SETTINGS_CHANGED = SET;
					}
					break;
					
					case BUZZER_SETTING:
					{
						if(BuzzerSoundType > 1)
						{
							BuzzerSoundType--;
							FLAG_E2P_WRITE = SET;
						}
						else if(BuzzerSoundType > 3)
						{
							BuzzerSoundType = 1;
							FLAG_E2P_WRITE = SET;	
						}
						Set_Buzzer_SoundType();	
						FLAG_SETTINGS_CHANGED = SET;				
					}
					break;
					
					case LIGHT_SETTING:
					{
						if(BacklightType > 1) 
						{
							BacklightType--;
							FLAG_E2P_WRITE = SET; 
						}
						else if(BacklightType>3)
						{
							BacklightType = 1;
							FLAG_E2P_WRITE = SET;
						}
						Set_Backlight_Level();
					}
					break;
					
					case COOKTIME_SETTING:
					{
						CookTime_Setting_Downkey_Function();
					}
					break;
					
					case ALARMTIME_SETTING:
					{
						FLAG_ALARMTIME_EDIT = SET;
						AlarmTimeMinute--;
						if(AlarmTimeMinute < 0)
						{
							if(AlarmTimeHour >0)
							{	
								AlarmTimeMinute = 59;
								AlarmTimeHour--;
								if(AlarmTimeHour < 0)
									AlarmTimeHour = 0;
							}
							else
							{
								AlarmTimeMinute = 0;
							}
						}	
					}
					break;
					
					case COOKENDTIME_SETTING:
					{
						Cookend_Time_DownKey_Function();
					}
					break;
	
					case END_MENU:
					{
						FLAG_NO_BEEP = SET;
					}
					break;		
					
					default:
					break;
					
				}
			}
		}
	}
	else
	{		
		if(FLAG_ALARMTIME_ENDED)  FLAG_ALARMTIME_ENDED = RESET;	
	}
}
void Down_Key_Hold_Function(void)
{
	SettingTimerSec = 5;
	switch(DisplayMenu)
	{
		case CLOCK_SETTING:
		{
			if(TchHoldTime>50)
			{
				ClockMinute--;
				if(ClockMinute < 0)
				{
					ClockMinute = 59;
					ClockHour--;
					if(ClockHour < 0)
						ClockHour = 23;
				}
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}	
		}
		break;	
		
		case COOKTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				CookTime_Setting_Downkey_Function();
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break;
		
		case ALARMTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				AlarmTimeMinute--;
				if(AlarmTimeMinute < 0)
				{
					if(AlarmTimeHour >0)
					{	
						AlarmTimeMinute = 59;
						AlarmTimeHour--;
						if(AlarmTimeHour < 0)
							AlarmTimeHour = 0;
					}
					else
					{
						AlarmTimeMinute = 0;
					}
				}	
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break;
		
		case COOKENDTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				Cookend_Time_DownKey_Function();
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break; 
	}
}
void Up_Key_Funciton(void)
{
	FLAG_SHORT_TOUCH=1;	
	if(!FLAG_LOCK_STATE || DisplayMenu==LOCK_SETTING)
	{
		SettingTimerSec = 5;	
		if(!FLAG_TOUCH_DIAGNOSE) 
		{
			if(FLAG_ALARMTIME_ENDED)	FLAG_ALARMTIME_ENDED = RESET;
			else
			{
				Sound(1);
				switch(DisplayMenu)
				{
					case TEST_MENU:
					break;
					
					case START_UP:
					{
						DisplayMenu = CLOCK_SETTING;	
					}
					break;
	
					case CLOCK_SETTING:
					{
						ClockMinute++;
						if(ClockMinute > 59)
						{
							ClockMinute = 0;
							ClockHour++;
							if(ClockHour > 23)
								ClockHour = 0;
						}
						FLAG_SETTINGS_CHANGED=SET;
						FLAG_CLOCK_CHANGED = SET;
					}
					break;
					
					case LOCK_SETTING:
					{
						FLAG_LOCK_STATE = SET;
						FLAG_E2P_WRITE  = SET;
						FLAG_SETTINGS_CHANGED = SET;
					}
					break;
					
					case BUZZER_SETTING:
					{
						if(BuzzerSoundType<3)
						{ 
							BuzzerSoundType++; 
							FLAG_E2P_WRITE = SET;
						}
						else if(BuzzerSoundType>3)
						{
							BuzzerSoundType=3;
							FLAG_E2P_WRITE = SET;
						}
						Set_Buzzer_SoundType();
						FLAG_SETTINGS_CHANGED = SET;
					}
					break;
					
					case LIGHT_SETTING:
					{
						if(BacklightType<3)
						{ 
							BacklightType++;
							FLAG_E2P_WRITE = SET;
						}
						else if (BacklightType>3)
						{
							BacklightType=3;
							FLAG_E2P_WRITE = SET;
						}
						Set_Backlight_Level();
					}
					break;
					
					case COOKTIME_SETTING:
					{
						Cooktime_Upkey_Function();
					}
					break;
					
					case COOKENDTIME_SETTING:
					{
						
						if(!FLAG_COOKENDTIME_WORKING)
						{
							if(CookTimeHour+ClockHour<24)
								CookEndTimeHour=CookTimeHour+ClockHour;
							else
								CookEndTimeHour=CookTimeHour+ClockHour-24;
								
							if(CookTimeMinute+ClockMinute<60)
								CookEndTimeMinute=CookTimeMinute+ClockMinute;
							else
								{CookEndTimeMinute=CookTimeMinute+ClockMinute-60;CookEndTimeHour++;}
						}
						Cookend_time_UpKey_Fucntion();
					}
					break;
					
					case ALARMTIME_SETTING:
					{
						FLAG_ALARMTIME_EDIT = SET;
						AlarmTimeMinute++;
						if(AlarmTimeMinute>59)
						{
							if(AlarmTimeHour == 23)
							{
								AlarmTimeMinute = 59;
							}
							else
							{
								AlarmTimeMinute = 0;
								AlarmTimeHour++;
								if(AlarmTimeHour > 23)
									AlarmTimeHour = 23;
							}
						}
					}
					break;
					
					case END_MENU:
					{
						FLAG_NO_BEEP = SET;
					}
					break;
					
					default:
					break;
					
				}
			}
		}
	}
	else
	{		
		if(FLAG_ALARMTIME_ENDED)  FLAG_ALARMTIME_ENDED = RESET;	
	}
}
void Up_Key_Hold_Function(void)
{
	SettingTimerSec = 5;
	switch(DisplayMenu)
	{
		case CLOCK_SETTING:
		{
			if(TchHoldTime>50)
			{
				ClockMinute++;
				if(ClockMinute > 59)
				{
					ClockMinute = 0;
					ClockHour++;
					if(ClockHour > 23)
						ClockHour = 0;
				}			
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break;	
		
		case COOKTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				Cooktime_Upkey_Function();
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}			
		}
		break;
		
		case NORMAL:
		{
			if(TchHoldTime>399)//5 Sec
			{
				FLAG_SHORT_TOUCH = 1;
				FLAG_HOLD_TOUCH  = 1;	
				FLAG_DONT_CALIBRATE = SET;
				FLAG_AMPM_CLOCK   = !FLAG_AMPM_CLOCK;
				Sound(1);
			}
		}
		break;
		
		case ALARMTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				AlarmTimeMinute++;
				if(AlarmTimeMinute > 59)
				{
					if(AlarmTimeHour == 23)
					{
						AlarmTimeMinute = 59;
					}
					else
					{
						AlarmTimeMinute = 0;
						AlarmTimeHour++;
						if(AlarmTimeHour > 23)
							AlarmTimeHour = 23;
					}
				}	
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break;
		
		case COOKENDTIME_SETTING:
		{
			if(TchHoldTime>50)
			{
				Cookend_time_UpKey_Fucntion();
				FLAG_SHORT_TOUCH =1;
				TchHoldTime-=5;
				FLAG_DONT_CALIBRATE = SET;
			}
		}
		break;
	}
}
void Key_Check(void)
{
	if(!FLAG_SHORT_TOUCH)
	{
		if(KeyState1==0)
		{
			if(KeyState2==0x01)	Setting_Key_Function();
			if(KeyState2==0x02)	Play_Key_Function();
		}
		if(KeyState1==0x04)	Down_Key_Funciton();
		if(KeyState1==0x08)	Up_Key_Funciton();		
	} 
	if(!FLAG_HOLD_TOUCH)
	{
		if(!FLAG_TOUCH_DIAGNOSE)
		{
			if(KeyState1==0x01 && !FLAG_LOCK_STATE)
			{
				if(TchHoldTime>150)
				{
					{_1stDigit--;if(_1stDigit<1) _1stDigit=9;}
					FLAG_SHORT_TOUCH =1;
					TchHoldTime-=20;
					FLAG_DONT_CALIBRATE = SET;
				}
				
			}
			else if(KeyState1==0x02 && !FLAG_LOCK_STATE)
			{
				if(DisplayMenu== COOKTIME_SETTING)
				{
					if(TchHoldTime>150)
					{
						DisplayMenu = END_MENU;
						Sound(1);
						CookTimeHour = CookTimeMinute = CookTimeSecond = 0;
						FLAG_MANUALLY_ENDED = SET;FLAG_ALARMTIME_ENDED = SET;
						FLAG_COOKTIME_WORKING = RESET;
						FLAG_COOKTIME_COUNT_START = RESET;
						FLAG_NO_BEEP = SET;
						
						FLAG_HOLD_TOUCH =1;
						FLAG_SHORT_TOUCH =1;
						FLAG_DONT_CALIBRATE = SET;
					}
				}
			}
			else if(KeyState1==0x04 && !FLAG_LOCK_STATE) Down_Key_Hold_Function();
			else if(KeyState1==0x08 && !FLAG_LOCK_STATE) Up_Key_Hold_Function();
		}
	}
	if((KeyState1 & 0x0F)==0)
	{
		KeyState1=0;
		TchHoldTime=0;
		FLAG_SHORT_TOUCH=0;	FLAG_HOLD_TOUCH=0;
		FLAG_TOUCH_DIAGNOSE=0;	FLAG_DONT_CALIBRATE = RESET;
	}
	KeyState2 = KeyState1;
}
void Show_Clock(void)
{
	unsigned char ShowingHour;
	if(FLAG_AMPM_CLOCK)
	{
		ShowingHour = ClockHour;
		if(ClockHour < 12)
		{
			if(ClockHour== 0)
			{
				_1stDigit = 1;
				_2ndDigit = 2;
			}
			else
			{
				Division( ShowingHour , 10 ) ;
				_1stDigit = DivQuot;
				_2ndDigit = DivRem ;
			}
			FLAG_AM_ICON = SET;
			FLAG_PM_ICON = RESET;
		}
		else//12-24
		{
			if(ClockHour == 12)
			{
				_1stDigit = 1;
				_2ndDigit = 2;
			}
			else
			{
				ShowingHour = ShowingHour - 12;
				Division( ShowingHour , 10 ) ;
				_1stDigit = DivQuot;
				_2ndDigit = DivRem ;
			}
			FLAG_AM_ICON = RESET;
			FLAG_PM_ICON = SET;
		}
		
	}
	else
	{
		Division(ClockHour,10);
		_1stDigit = DivQuot;
		_2ndDigit = DivRem;
		FLAG_AM_ICON = RESET;
		FLAG_PM_ICON = RESET;	
	}	
	Division(ClockMinute,10);
	_3rdDigit = DivQuot;
	_4thDigit = DivRem;	
}
void Touch_Check(void)
{
	if(_tkmf)
	{
		TchModule0=_tkm016dh;	TchModule0=TchModule0<<8;	TchModule0+=_tkm016dl;
		TchModule2=_tkm216dh;	TchModule2=TchModule2<<8;	TchModule2+=_tkm216dl;
		_tkmf=0;
		
		if(TouchRefresh==0)
		{
			if(StartingDelay<5)	
			{
				TchReference01=TchModule0;
			}
			else
			{
				if((TchModule0<(TchReference01+10)) && (TchModule0>(TchReference01-10)))	TchCalibrTime01=0;		
				else				{if(TchCalibrTime01>19)	{TchCalibrTime01=0;	TchReference01=TchModule0;}}
				if(TchCalibrTime01>19)	{TchCalibrTime01=0;	TchReference01=TchModule0;}
				
				if(TchReference01>UP_TCH01)
				{
					if(TchModule0>(TchReference01-DOWN_TCH01))		{if(TchCounter01>0)			TchCounter01--;}
					else if(TchModule0<(TchReference01-UP_TCH01))		{if(TchCounter01<TCH_COUNTER_CONST)	TchCounter01++;}
					
					if(TchCounter01==TCH_COUNTER_CONST)			KeyState1|=0x01;
					if(TchCounter01==0)					KeyState1&=0xFE;
				}
			}
		}
		else if(TouchRefresh==1)
		{
			if(StartingDelay<5)	
			{
				TchReference02=TchModule0;
				TchReference03=TchModule2;
			}
			else
			{
				if((TchModule0<(TchReference02+10)) && (TchModule0>(TchReference02-10)))	TchCalibrTime02=0;		
				else			{if(TchCalibrTime02>19)	{TchCalibrTime02=0;	TchReference02=TchModule0;}}
				if(TchCalibrTime02>19)	{TchCalibrTime02=0;	TchReference02=TchModule0;}
				
				if(TchReference02>UP_TCH02)
				{
					if(TchModule0>(TchReference02-DOWN_TCH02))		{if(TchCounter02>0)			TchCounter02--;}
					else if(TchModule0<(TchReference02-UP_TCH02))		{if(TchCounter02<TCH_COUNTER_CONST)	TchCounter02++;}
					
					if(TchCounter02==TCH_COUNTER_CONST)			KeyState1|=0x02;
					if(TchCounter02==0)					KeyState1&=0xFD;
				}
				
				if((TchModule2<(TchReference03+10)) && (TchModule2>(TchReference03-10)))	TchCalibrTime03=0;		
				else				{if(TchCalibrTime03>19)	{TchCalibrTime03=0;	TchReference03=TchModule2;}}
				if(TchCalibrTime03>19)	{TchCalibrTime03=0;	TchReference03=TchModule2;}
				
				if(TchReference03>UP_TCH03)
				{
					if(TchModule2>(TchReference03-DOWN_TCH03))		{if(TchCounter03>0)			TchCounter03--;}
					else if(TchModule2<(TchReference03-UP_TCH03))		{if(TchCounter03<TCH_COUNTER_CONST)	TchCounter03++;}
					
					if(TchCounter03==TCH_COUNTER_CONST)			KeyState1|=0x04;
					if(TchCounter03==0)					KeyState1&=0xFB;
				}
			}
		}
		else if(TouchRefresh==2)
		{
			if(StartingDelay<5)	
			{
				TchReference04=TchModule2;
			}
			else
			{
				if((TchModule2<(TchReference04+10)) && (TchModule2>(TchReference04-10)))	TchCalibrTime04=0;		
				else				{if(TchCalibrTime04>19)	{TchCalibrTime04=0;	TchReference04=TchModule2;}}
				if(TchCalibrTime04>19)		{TchCalibrTime04=0;	TchReference04=TchModule2;}
				
				if(TchReference04>UP_TCH04)
				{
					if(TchModule2>(TchReference04-DOWN_TCH04))	{if(TchCounter04>0)			TchCounter04--;}
					else if(TchModule2<(TchReference04-UP_TCH04))	{if(TchCounter04<TCH_COUNTER_CONST)	TchCounter04++;}
					
					if(TchCounter04==TCH_COUNTER_CONST)			KeyState1|=0x08;
					if(TchCounter04==0)					KeyState1&=0xF7;
				}
			}
		}
		
		if(TouchRefresh==0)
		{
			_pbs1 &= 0b11001111;//key3
			
			_pes0 |= 0b10000000;//key11
			_pbs1 |= 0b00001000;//key2
			TouchRefresh=1;
		}
		else if(TouchRefresh==1)
		{
			_pes0 &= 0b00111111;//key11
			_pbs1 &= 0b11110011;//key2
			
			_pes0 |= 0b00100000;//key12
			TouchRefresh=2;
		}
		else
		{
			_pes0 &= 0b11001111;//key12
			_pbs1 |= 0b00100000;//key3
			TouchRefresh=0;		
		}
		_tkcfov=0;
		_tk16ov=0;
		_tkrcov=0;
		_tkbusy=0;
		_tkst=0;
		TouchBit1=0;
	}
	else if(!TouchBit1)
	{
		_tkm0c1=0b00110110;
		_tkm2c1=0b00111100;

		if(TouchRefresh==0)			{_tkm0c2=0x02;}
		else if(TouchRefresh==1)	{_tkm0c2=0x01; _tkm2c2=0x03;}
		else 						{_tkm2c2=0x02;}

		_tkst=1;
		_tkbusy=1;
		TouchBit1=1;
	}
}
void Display_Handler(void)
{
	if(FLAG_DECIDE_WHICH_MENU)
	{
		Decide_Which_Menu();
		FLAG_COOKENDTIME_ICON = RESET;
		FLAG_DECIDE_WHICH_MENU = RESET;
	}
	switch(DisplayMenu)
	{
		case START_UP:
		{
			if(FLAG_BLINK)
			{
				_1stDigit =_DF_DISPLAY_NOTHING;
				_2ndDigit =0;
				_3rdDigit =_DF_DISPLAY_F;
				_4thDigit =_DF_DISPLAY_F;				
			}
			else
			{
				_1stDigit=_2ndDigit=_3rdDigit=_4thDigit=_DF_DISPLAY_NOTHING;
			}
			FLAG_CLOCK_ICON = SET;
			
			if(FLAG_LOCK_STATE)		FLAG_LOCK_ICON = SET;
			else					FLAG_LOCK_ICON = RESET;
		}
		break;
		
		case CLOCK_SETTING:
		{
			FLAG_LOCK_ICON_BLINK  = RESET;
			FLAG_LOCK_ICON		  = RESET;
			FLAG_CLOCK_ICON_BLINK = SET;
			FLAG_COLON = SET;
			Show_Clock();
			
			if(!SettingTimerSec)
			{
				if(FLAG_CLOCK_CHANGED){FLAG_CLOCK_CHANGED = RESET;ClockSecond=0;}
				DisplayMenu = NORMAL;
			}
		}
		break;
		
		case NORMAL:
		{
			if(FLAG_LOCK_STATE) FLAG_LOCK_ICON = SET;
			else				FLAG_LOCK_ICON = RESET;
			FLAG_LOCK_ICON_BLINK 		= RESET;
			FLAG_CLOCK_ICON 	  		= RESET;
			FLAG_CLOCK_ICON_BLINK 		= RESET;
			FLAG_SPEAKER_ICON			= RESET;
			FLAG_SPEAKER_ICON_BLINK 	= RESET;
			FLAG_LIGHT_ICON				= RESET;
			FLAG_LIGHT_ICON_BLINK 		= RESET;
			FLAG_COOKTIME_ICON_BLINK	= RESET;
			FLAG_COOKENDTIME_ICON_BLINK = RESET;
			FLAG_NO_BEEP   				= RESET;
			if(FLAG_COOKENDTIME_WORKING)
			{
				FLAG_COOKTIME_ICON		= SET;
				FLAG_COOKENDTIME_ICON	= SET;
			}
			else
			{
				FLAG_COOKTIME_ICON		= RESET;
				FLAG_COOKENDTIME_ICON	= RESET;
			}
			FLAG_COLON_BLINK = SET;
			Show_Clock();					
			if(FLAG_ALARMTIME_ENDED) 
			{
				FLAG_ALARM_ICON_BLINK = SET;
			}
			else
			{
				FLAG_ALARM_ICON_BLINK = RESET;
				FLAG_ALARM_ICON		  = RESET;
			}
		}
		break;
		
		case LOCK_SETTING:
		{
			FLAG_COOKTIME_ICON_BLINK = RESET;
			FLAG_ALARM_ICON_BLINK	 = RESET;
			FLAG_COOKENDTIME_ICON_BLINK = RESET;			
			if(FLAG_COOKTIME_WORKING ||FLAG_COOKENDTIME_WORKING)	FLAG_COOKTIME_ICON = SET;
			else							FLAG_COOKTIME_ICON = RESET;	
			if(FLAG_COOKENDTIME_WORKING)FLAG_COOKENDTIME_ICON = SET;
			else							FLAG_COOKENDTIME_ICON = RESET;
			if(FLAG_ALARMTIME_WORKING)				FLAG_ALARM_ICON	=SET;
			else							FLAG_ALARM_ICON	= RESET;
			FLAG_CLOCK_ICON  = RESET;
			FLAG_COLON = RESET;
			FLAG_COLON_BLINK = RESET;
			if(FLAG_LOCK_STATE)					{FLAG_LOCK_ICON_BLINK = RESET;FLAG_LOCK_ICON = SET;SettingTimerSec=5;}
			else							FLAG_LOCK_ICON_BLINK = SET;
			if(FLAG_LOCK_STATE)	
			{
				_1stDigit = _DF_DISPLAY_NOTHING;
				_2ndDigit = _DF_DISPLAY_NOTHING;
				_3rdDigit = 0;
				_4thDigit = _DF_DISPLAY_n;	
			}
			else
			{
				_1stDigit = _DF_DISPLAY_NOTHING;
				_2ndDigit = 0;
				_3rdDigit = _DF_DISPLAY_F;
				_4thDigit = _DF_DISPLAY_F;				
			}	
			if(FLAG_COOKENDTIME_WORKING)		{FLAG_COOKTIME_ICON = SET;}
			else if(!FLAG_COOKTIME_WORKING)		{FLAG_COOKTIME_ICON_BLINK = RESET;FLAG_COOKTIME_ICON = RESET;}
			if(!FLAG_ALARMTIME_WORKING)		{FLAG_ALARM_ICON_BLINK = RESET;FLAG_ALARM_ICON = RESET;}
			if(!SettingTimerSec)
			{
				FLAG_SETTINGS_CHANGED = RESET;
				if(FLAG_COOKTIME_WORKING || FLAG_ALARMTIME_WORKING)
				{
					DisplayMenu = LastDisplayState;
				}
				else
					DisplayMenu = NORMAL;
			}
		}
		break;
		
		case BUZZER_SETTING:
		{
			FLAG_CLOCK_ICON_BLINK   = RESET;
			FLAG_CLOCK_ICON		= RESET;
			FLAG_COLON		= RESET;
			FLAG_COLON_BLINK	= RESET;
			FLAG_SPEAKER_ICON_BLINK = SET;
			FLAG_LOCK_ICON_BLINK    = RESET;
			FLAG_LOCK_ICON		= RESET;
			
			_1stDigit = _DF_DISPLAY_b;
			_2ndDigit = _DF_DISPLAY_DASH;
			_3rdDigit = 0;
			_4thDigit = BuzzerSoundType;
			if(BuzzerDelay>500)
				Sound(2);			
			if(!SettingTimerSec)
			{
				FLAG_SETTINGS_CHANGED = RESET;
				if(FLAG_COOKTIME_WORKING || FLAG_ALARMTIME_WORKING)
				{
					DisplayMenu = LastDisplayState;
				}
				else
					DisplayMenu = NORMAL;
			}	
		}
		break;
		
		case LIGHT_SETTING:
		{
			FLAG_SPEAKER_ICON_BLINK = RESET;
			FLAG_SPEAKER_ICON 	= RESET;
			FLAG_LIGHT_ICON_BLINK   = SET;
			_1stDigit = _DF_DISPLAY_d;
			_2ndDigit = _DF_DISPLAY_DASH;
			_3rdDigit = 0;
			_4thDigit = BacklightType;
			Set_Backlight_Level();
			if(!SettingTimerSec)
			{
				if(FLAG_COOKTIME_WORKING || FLAG_ALARMTIME_WORKING)
				{
					DisplayMenu = LastDisplayState;
				}
				else
					DisplayMenu = NORMAL;
			}			
		}
		break;
		
		case COOKTIME_SETTING:
		{
			Initializer 	         = SET;			
			FLAG_LOCK_ICON_BLINK     = RESET;
			FLAG_CLOCK_ICON_BLINK    = RESET;
			FLAG_CLOCK_ICON	         = RESET;
			FLAG_SPEAKER_ICON_BLINK  = RESET;
			FLAG_SPEAKER_ICON        = RESET;
			FLAG_LIGHT_ICON_BLINK    = RESET;
			FLAG_LIGHT_ICON	         = RESET;
			FLAG_COOKTIME_ICON_BLINK = SET;
			FLAG_COLON_BLINK	 = RESET;
			FLAG_COLON		 = SET;
			if(!FLAG_COOKENDTIME_WORKING)	FLAG_COOKENDTIME_ICON = RESET;
			else				FLAG_COOKENDTIME_ICON = SET;
			if(FLAG_COOKTIME_EDIT)
			{
				Division(CookTimeHour,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(CookTimeMinute,10);
				_3rdDigit =	DivQuot;
				_4thDigit = DivRem;
			}
			else 
			{
				if(!CookTimeHour)
				{
					Division(CookTimeMinute,10);
					_1stDigit = DivQuot;
					_2ndDigit = DivRem;
					Division(CookTimeSecond,10);
					_3rdDigit =	DivQuot;
					_4thDigit = DivRem;
				}
				else
				{
					Division(CookTimeHour,10);
					_1stDigit = DivQuot;
					_2ndDigit = DivRem;
					Division(CookTimeMinute,10);
					_3rdDigit =	DivQuot;
					_4thDigit = DivRem;
				}	
			}
			if(!SettingTimerSec)
			{
				if(!CookTimeHour && !CookTimeMinute && FLAG_COOKTIME_EDIT)
				{
					CookTimeSecond = 0; 
					FLAG_COOKTIME_WORKING = RESET;
					FLAG_COOKTIME_COUNT_START = RESET;
				}
				else if(CookTimeHour || CookTimeMinute || CookTimeSecond)
				{
					FLAG_COOKTIME_WORKING = SET;
					FLAG_COOKTIME_COUNT_START = SET;	
				}
				Decide_Which_Menu();	
				if(FLAG_COOKTIME_EDIT)
				{			
					FLAG_COOKTIME_EDIT = RESET;
					CookTimeSecond = 0;
				}
			}	
		}
		break;
		
		case COOKENDTIME_SETTING:
		{
			if(Initializer)
			{
				Initializer = RESET;
				if(!FLAG_COOKENDTIME_WORKING)
				{
					CookEndTimeHour = ClockHour + CookTimeHour;
					CookEndTimeMinute = ClockMinute + CookTimeMinute;
					if(CookEndTimeMinute >= 60)
					{
						CookEndTimeMinute = CookEndTimeMinute -60;
						CookEndTimeHour++;	
					}
					if(CookEndTimeHour >= 24)
					{
						CookEndTimeHour = CookEndTimeHour -24;
					}			
				}	
			}
			FLAG_COOKENDTIME_ICON_BLINK = SET;
			FLAG_COLON_BLINK	    = RESET;
			FLAG_COLON		    = SET;
			FLAG_COOKTIME_ICON_BLINK    = RESET;
			FLAG_COOKTIME_ICON	    = SET;
					
			FLAG_SPEAKER_ICON_BLINK= RESET;
			FLAG_SPEAKER_ICON      = RESET;
			FLAG_LIGHT_ICON_BLINK  = RESET;
			FLAG_LIGHT_ICON	       = RESET;
			FLAG_LOCK_ICON_BLINK   = RESET;
			if(FLAG_LOCK_STATE)	FLAG_LOCK_ICON=SET;
			else			FLAG_LOCK_ICON=RESET;		
			Division(CookEndTimeHour,10);
			_1stDigit = DivQuot;
			_2ndDigit = DivRem;
			Division(CookEndTimeMinute,10);
			_3rdDigit = DivQuot;
			_4thDigit = DivRem;	
			
			if(!SettingTimerSec)
			{
				FLAG_COOKENDTIME_EDIT	= RESET;
				if(!FLAG_COOKENDTIME_WORKING)
					Decide_Which_Menu();
				else
				{
					if(FLAG_ALARMTIME_WORKING)	DisplayMenu=ALARM_TIME_DOWNCOUNTER;
					else				DisplayMenu = NORMAL;
					
					FLAG_COOKTIME_COUNT_START = RESET;
					FLAG_COOKTIME_WORKING = RESET;	
					CookTimeSecond = 0;
					Set_Cookend_Time();

				}	
			}
				
		}
		break;
		
		case ALARMTIME_SETTING:
		{
			FLAG_COOKTIME_ICON_BLINK = RESET;
			if(FLAG_COOKTIME_WORKING || FLAG_COOKENDTIME_WORKING)	FLAG_COOKTIME_ICON = SET;
			else 							FLAG_COOKTIME_ICON = RESET;
			if(FLAG_COOKENDTIME_WORKING)				FLAG_COOKENDTIME_ICON = SET;
			else 							FLAG_COOKENDTIME_ICON = RESET;
			
			FLAG_SPEAKER_ICON_BLINK= RESET;
			FLAG_SPEAKER_ICON      = RESET;
			FLAG_LIGHT_ICON_BLINK  = RESET;
			FLAG_LIGHT_ICON	       = RESET;
			FLAG_LOCK_ICON_BLINK   = RESET;
			if(FLAG_LOCK_STATE)	FLAG_LOCK_ICON=SET;
			else			FLAG_LOCK_ICON=RESET;				
			
			FLAG_COOKENDTIME_ICON_BLINK	= RESET;
			FLAG_ALARM_ICON_BLINK	 	= SET;
			FLAG_COLON_BLINK 		= RESET;
			FLAG_COLON			= SET;
			
			
			if(FLAG_ALARMTIME_EDIT)
			{
				Division(AlarmTimeHour,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(AlarmTimeMinute,10);
				_3rdDigit =	DivQuot;
				_4thDigit = DivRem;	
			}
			else
			{
				if(!AlarmTimeHour)
				{
					Division(AlarmTimeMinute,10);
					_1stDigit = DivQuot;
					_2ndDigit = DivRem;
					Division(AlarmTimeSecond,10);
					_3rdDigit =	DivQuot;
					_4thDigit = DivRem;	
				}
				else
				{
					Division(AlarmTimeHour,10);
					_1stDigit = DivQuot;
					_2ndDigit = DivRem;
					Division(AlarmTimeMinute,10);
					_3rdDigit =	DivQuot;
					_4thDigit = DivRem;						
				}	
			}
			if(!SettingTimerSec)
			{
				AlarmTime_Setting_PlayKey_Function();			
			}	
		}
		break;
		
		case COOK_TIME_DOWNCOUNTER:
		{
			FLAG_COOKTIME_ICON_BLINK = RESET;
			FLAG_COOKTIME_ICON 	 = SET;
			FLAG_COLON_BLINK   	 = SET;
			FLAG_SPEAKER_ICON_BLINK  = RESET;
			FLAG_SPEAKER_ICON        = RESET;
			FLAG_LIGHT_ICON_BLINK    = RESET;
			FLAG_LIGHT_ICON	         = RESET;
			FLAG_LOCK_ICON_BLINK     = RESET;
			if(FLAG_LOCK_STATE)	FLAG_LOCK_ICON=SET;
			else			FLAG_LOCK_ICON=RESET;	
			
			FLAG_COOKENDTIME_ICON_BLINK = RESET;
			if(FLAG_COOKENDTIME_WORKING)	FLAG_COOKENDTIME_ICON = SET;
			else				FLAG_COOKENDTIME_ICON = RESET;
			
			if(FLAG_ALARMTIME_ENDED) FLAG_ALARM_ICON_BLINK = SET;
			else
			{
				if(FLAG_ALARMTIME_WORKING)
				{
					FLAG_ALARM_ICON_BLINK = RESET;
					FLAG_ALARM_ICON = SET;
				}
				else
				{
					FLAG_ALARM_ICON_BLINK = RESET;
					FLAG_ALARM_ICON = RESET;
				}			
			}
			
			if(!CookTimeHour)
			{
				Division(CookTimeMinute,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(CookTimeSecond,10);
				_3rdDigit =	DivQuot;
				_4thDigit = DivRem;
			}
			else
			{
				Division(CookTimeHour,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(CookTimeMinute,10);
				_3rdDigit =	DivQuot;
				_4thDigit = DivRem;
			}			
		}
		break;
				
		case ALARM_TIME_DOWNCOUNTER:
		{
			FLAG_SPEAKER_ICON_BLINK= RESET;
			FLAG_SPEAKER_ICON      = RESET;
			FLAG_LIGHT_ICON_BLINK  = RESET;
			FLAG_LIGHT_ICON	       = RESET;
			FLAG_LOCK_ICON_BLINK   = RESET;
			if(FLAG_LOCK_STATE)	FLAG_LOCK_ICON=SET;
			else			FLAG_LOCK_ICON=RESET;	

			FLAG_ALARM_ICON 	    = SET;
			FLAG_ALARM_ICON_BLINK       = RESET;
			FLAG_COLON_BLINK 	    = SET;
			FLAG_COOKTIME_ICON_BLINK    = RESET;
			FLAG_COOKENDTIME_ICON_BLINK = RESET;
			if(FLAG_COOKENDTIME_WORKING)	FLAG_COOKENDTIME_ICON = SET;
			else				FLAG_COOKENDTIME_ICON = RESET;
			if(FLAG_COOKTIME_WORKING || FLAG_COOKENDTIME_WORKING)	FLAG_COOKTIME_ICON	= SET;
			else							FLAG_COOKTIME_ICON	= RESET;
			if(FLAG_COOKTIME_WORKING)
			{
				Decide_Which_Menu();
			}
			if(!AlarmTimeHour)
			{
				Division(AlarmTimeMinute,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(AlarmTimeSecond,10);
				_3rdDigit =	DivQuot;
				_4thDigit = DivRem;
			}
			else
			{
				Division(AlarmTimeHour,10);
				_1stDigit = DivQuot;
				_2ndDigit = DivRem;
				Division(AlarmTimeMinute,10);
				_3rdDigit = DivQuot;
				_4thDigit = DivRem;
			}
		}
		break;
		
		case END_MENU:
		{
			if(FLAG_MANUALLY_ENDED) {FLAG_COOKTIME_ICON_BLINK = RESET; FLAG_COOKTIME_ICON = SET;}
			else			{FLAG_COOKTIME_ICON_BLINK = SET;}
			if(!FLAG_NO_BEEP)
				Sound(2);			
			FLAG_COOKENDTIME_WORKING	= RESET;
			FLAG_COOKENDTIME_ICON		= RESET;
			FLAG_COOKENDTIME_ICON_BLINK 	= RESET;
			FLAG_SPEAKER_ICON_BLINK		= RESET;
			FLAG_LIGHT_ICON_BLINK		= RESET;
			FLAG_SPEAKER_ICON		= RESET;
			FLAG_LIGHT_ICON			= RESET;
			FLAG_LOCK_ICON_BLINK		= RESET;
			if(FLAG_LOCK_STATE)		FLAG_LOCK_ICON = SET;
			else				FLAG_LOCK_ICON = RESET;
			FLAG_COLON_BLINK 		= RESET;
			FLAG_COLON		 	= RESET;
			if(FLAG_ALARMTIME_ENDED) 
			{
				if(!FLAG_MANUALLY_ENDED)
					FLAG_ALARM_ICON_BLINK = SET;
				else 
				{
					FLAG_ALARM_ICON 	= RESET;
					FLAG_ALARMTIME_ENDED 	= RESET;
					FLAG_ALARMTIME_WORKING  = RESET;
					AlarmTimeMinute = AlarmTimeHour = AlarmTimeSecond=0;	
				}
			}
			else
			{
				FLAG_ALARM_ICON_BLINK = RESET;
				if(!FLAG_ALARMTIME_WORKING) FLAG_ALARM_ICON = RESET;
			}
			_1stDigit = _DF_DISPLAY_NOTHING;
			_2ndDigit = _DF_DISPLAY_E;
			_3rdDigit = _DF_DISPLAY_n;
			_4thDigit = _DF_DISPLAY_d;
		}
		break;

		default:
		break;
	}
	if((DisplayMenu!=NORMAL)&&(DisplayMenu!=CLOCK_SETTING))	
	{
		FLAG_AM_ICON=FLAG_PM_ICON=RESET;
	}
	{
		if(FLAG_COLON_BLINK)			{if(FLAG_HALF_SECOND) FLAG_COLON = SET; 	   	else	 FLAG_COLON = RESET;	       }
		if(FLAG_LOCK_ICON_BLINK)		{if(FLAG_HALF_SECOND) FLAG_LOCK_ICON = SET; 		else	 FLAG_LOCK_ICON = RESET;       } 
		if(FLAG_COOKTIME_ICON_BLINK)		{if(FLAG_HALF_SECOND) FLAG_COOKTIME_ICON = SET;  	else	 FLAG_COOKTIME_ICON = RESET;   } 
		if(FLAG_CLOCK_ICON_BLINK)		{if(FLAG_HALF_SECOND) FLAG_CLOCK_ICON = SET;  		else	 FLAG_CLOCK_ICON = RESET;      }
		if(FLAG_COOKENDTIME_ICON_BLINK) 	{if(FLAG_HALF_SECOND) FLAG_COOKENDTIME_ICON = SET;  	else	 FLAG_COOKENDTIME_ICON = RESET;}
		if(FLAG_SPEAKER_ICON_BLINK)		{if(FLAG_HALF_SECOND) FLAG_SPEAKER_ICON = SET;  	else	 FLAG_SPEAKER_ICON = RESET;    }
		if(FLAG_LIGHT_ICON_BLINK)		{if(FLAG_HALF_SECOND) FLAG_LIGHT_ICON = SET;  		else	 FLAG_LIGHT_ICON = RESET;      }
		
		if(FLAG_ALARM_ICON_BLINK)
		{
			if(FLAG_ALARMTIME_ENDED)
			{
				if(FLAG_HALF_SECOND) 
				{
					Sound(2);
				}
				if((BuzzerTime>500 && BuzzerTime<700) ||(BuzzerTime>800 && BuzzerTime<1000)) FLAG_ALARM_ICON = SET;
				else 																		 FLAG_ALARM_ICON = RESET;
			}
			else
			{
				if(FLAG_HALF_SECOND) FLAG_ALARM_ICON = SET; else FLAG_ALARM_ICON = RESET;		
			}
		}
	}
}
void Relay_Handler(void)
{
	if(DisplayMenu == START_UP || DisplayMenu == END_MENU || FLAG_COOKENDTIME_WORKING)  RELAY_PIN = RESET;//open
	else RELAY_PIN = SET;//close
}
void E2p_Check(void)
{
	if(FLAG_E2P_WRITE)
	{
		E2p_Write(0,0xAA);
		E2p_Write(1,FLAG_LOCK_STATE);	
		E2p_Write(2,BuzzerSoundType);	
		E2p_Write(3,BacklightType);		
		FLAG_E2P_WRITE=RESET;
	}
}
void Variables_Init(void)
{
	FLAG_LOCK_STATE      	  	= RESET;
	FLAG_COLON 	     	  	= RESET;
	FLAG_LOCK_ICON 	     	  	= RESET;
	FLAG_COOKTIME_ICON    	  	= RESET;
	FLAG_AM_ICON 		  	= RESET; 
	FLAG_CLOCK_ICON		  	= RESET;
	FLAG_COOKENDTIME_ICON 	  	= RESET;
	FLAG_PM_ICON		 	= RESET;
	FLAG_SPEAKER_ICON	  	= RESET;
	FLAG_ALARM_ICON		  	= RESET;
	FLAG_LIGHT_ICON		  	= RESET;
	
	FLAG_COLON_BLINK		= RESET;
	FLAG_LOCK_ICON_BLINK		= RESET;
	FLAG_COOKTIME_ICON_BLINK	= RESET;
	FLAG_AM_ICON_BLINK		= RESET;
	FLAG_CLOCK_ICON_BLINK		= RESET;
	FLAG_COOKENDTIME_ICON_BLINK	= RESET;
	FLAG_PM_ICON_BLINK		= RESET;
	FLAG_SPEAKER_ICON_BLINK		= RESET;
	FLAG_ALARM_ICON_BLINK		= RESET;
	FLAG_LIGHT_ICON_BLINK		= RESET;
	FLAG_AMPM_CLOCK			= RESET;	
	
	FLAG_COOKTIME_COUNT_START  	= RESET;
 	FLAG_ALARMTIME_COUNT_START	= RESET; 
	FLAG_COOKTIME_WORKING	 	= RESET;
	FLAG_ALARMTIME_WORKING 		= RESET;
	FLAG_ALARMTIME_ENDED		= RESET;
	FLAG_COOKTIME_EDIT		= RESET;
	
	DisplayMenu 		 	= START_UP;
	FLAG_COOKENDTIME_WORKING 	= RESET;
	FLAG_NO_BEEP 			= RESET;
	FLAG_MANUALLY_ENDED 	 	= RESET;
	FLAG_DECIDE_WHICH_MENU 	 	= RESET;
	FLAG_COOKENDTIME_EDIT    	= RESET;
	FLAG_SETTINGS_CHANGED    	= RESET;
	
	if(E2p_Read(0)==0xAA)
	{
		FLAG_LOCK_STATE = E2p_Read(1);
		BuzzerSoundType = E2p_Read(2);
		BacklightType   = E2p_Read(3);
	}
	else FLAG_E2P_WRITE = SET;
	Set_Backlight_Level();
	Set_Buzzer_SoundType();
}
void System_Init(void)
{
	//System Clock
	{
		_hircc=0b00001011;
		_hxtc=0b00000000;
		_lxtc=0b00000101;
		_scc=0b00000100;
		_wdtc=0b10101111;
		while( ! _hircf ) ; // Wait For Internal High Speed Clock Stabilize
	}

	//Pin shared Config
	{
		_pas0	= 0B10000000 ; // Xtal , Port , CTP0 , Port
		_pas1	= 0B00000010 ; // Port , Port , CTP0B , Xtal
		_pbs0	= 0B00000000 ; // Port , Port , Port , Port
		_pbs1	= 0B00101011 ; // Port , Touch , Touch , Analoge		
		_pcs0	= 0B00000000 ; // Port , Port , Port , Port		
		_pes0	= 0B10100000 ; // Touch , Touch , Port , Port
		_pes1	= 0B00000000 ; // - , - , Port , Port
	}
	
	_ifs0	= 0B00000000 ;
	
	//======	Fsub Config
	_fss	= SET ; // External Xtal Selected.
	_lxten	= SET ; // External Xtal Enabled.
	_lxtsp	= SET;	//Faster Startup
	
	while( ! _lxtf ) ; // Wait For External Low Speed Clock Stabilize.
	
	//Pin I/O Config
	{	
		_pa	= 0B00000000 ;
		_papu	= 0B00000000 ;
		_pac	= 0B00011000 ; // AND6 , AND7 , BUZZER2 , XT2 , XT1 , AND3 , BUZZER1 , AND2
		
		_pb	= 0B10001110 ;
		_pbpu	= 0B00000000 ;
		_pbc	= 0B01110000 ; // CTD1 , KY1 , KY2 , TEMP_SEN , CTD3 , CTD2 , CTD4 , AND1
		
		_pc	= 0B00000000 ;
		_pcpu	= 0B00000000 ;
		_pcc	= 0B11110000 ; // - , - , - , - , AND5 , AND11/Pulse1 , AND9 , AND8
		
		_pe	= 0B00010000 ;
		_pepu	= 0B00000000 ;
		_pec	= 0B11101100 ; // - , - , Zero_Cross , REL1OUT , KY4 , KY3 , AND4 , AND10/Pulse2
	}
	//Reset Voltage Control
	_lvdc	= 0B00010110 ; // LVDR Enabled, 3.6 Volt
	
	//======	Port Sink/Source Current Control
	_sledc	= 0B00000000 ;
	
	//Touch config
	{
		_tkm2roh = 0x00;
		_tkm2rol = 0x14;
		_tktmr   = 0;
		_tkc0    = 0b10000010;
		_tkc1	 = 0b00001101;
		
		_tkm2c0  = 0x0C;
		_tkm2c1  = 0b00111111;	
		_tkm2c2  = 0x00;
		_tkst    =0;	

		_tkm0roh = 0x00;	
		_tkm0rol = 0x14;
		_tktmr   = 0;
		_tkc0	 = 0b10000010;
		_tkc1	 = 0b00001101;
		
		_tkm0c0  = 0x0C;
		_tkm0c1  = 0b00111111;
		_tkm0c2  = 0x00;
		_tkst=0;
		
		_pes0 &= 0b00111111;//key11
		_pes0 &= 0b11001111;//key12
		_pbs1 &= 0b11110011;//key2
		_pbs1 |= 0b00100000;//key3
	}
	
	//Timer Config
	{
		_psc1r	= 0B00000011 ;			// Switched To Fsub Frequency
		_tb1c	= 0B10000110 ;			// TimeBase 1 Enabled And Switched To Fsub Clock.(2^14)/32768=0.5s
		
		//CTM0 PWM
		_ctm0c0	= 0B00111000 ;
		_ctm0c1	= 0B00110001 ;
		_ctm0al = 0 + BuzzerSoundLevel ;
		_ctm0ah = 0 ;
		_pas02 = _pas12 = RESET;
		
		//CTM1 Timer****************
		_ctm1c0=0b00001010;
		_ctm1c1=0b00000000;
	}
	//======	Touch Configuration	
	//interrupt Config
	{
		_ctm1pe = SET;
		_mf1e	= SET;
		_tb1e	= SET;
		_emi	= SET;
	}
}
void main(void)
{
	System_Init();
	Variables_Init();
	FLAG_BUZZER_EN = RESET;
	FLAG_GLOBAL_BLINK = SET;	
	Sound(3);
	while(1)
	{
		asm("clr wdt");
		Key_Check();	
		Touch_Check();
		Display_Handler();
		Buzzer_Handler();
		Relay_Handler();
		E2p_Check();
	}
}
