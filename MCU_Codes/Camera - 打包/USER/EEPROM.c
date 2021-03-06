#include "EEPROM.h"
#include "Var_Def.h"
#include "stmflash.h"
#include "usart.h"

 extern __IO	int32_t Para_K[16];
 extern __IO	int32_t Para_B[16];
// extern __IO uint8_t Serial_Numb;
 extern __IO  uint8_t 	 NO_Buffer[];	             //NO:终端号
 extern __IO  uint8_t    Sensor_amount  ;          //采集器数量
 extern __IO  uint8_t 	 Sensor_ADR[40];	         //采集器地址（编号）
 extern __IO  uint8_t 	 Sensor_Sel[40];           //传感器类型
	
extern __IO  uint16_t 	 RTC_1S_Count;             //系统启停后的秒计时
extern __IO	int8_t       AUTO_Control_ON_OFF;      //泵站自动控制 0 停止 1 启动
extern __IO	int8_t       MUANUAL_Control_ON_OFF;   //泵站手动控制 0 停止 1 启动�
extern __IO	int8_t       ControlMode ;             
extern __IO	int8_t       Transducer_ControlMode ;  //变频器控制方式 0：恒压泵水 1：点频控制
extern __IO	int32_t      WaterSource_Level;        //水源地水位选择

extern __IO	int8_t       FreqConv_SET_OnOff ;      //控制变频器开关0：关 1开   存储地址？？？
extern __IO	int16_t      FreqConv_SET_Freq ;       //控制变频器频率**.*Hz  存储地址？？？
extern __IO	int32_t      WaterPressure_SET  ;      //管道压力设置值，******.*Kpa   存储地址？？？
extern __IO	int32_t      WaterSourceLevel_S;       //水源地水位上限   **.*
extern __IO	int32_t      WaterSourceLevel_X;       //水源地水位下限   **.*
extern __IO	int32_t	     CH_Volt_slope[50];
extern __IO	int32_t    	 CH_Volt_zero[50];

extern __IO int16_t CH_Type[16];
extern __IO int16_t CH_Gain[16];
	

uint8_t EEPROM_Buffer[16];	

void NO_ParaRead(void)
{
	uint32_t EEPROM_Address; //  TNS 0531-LC-5A01
	uint8_t  Temp_i;	
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0020;//终端号读取
  STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,12);	

	for(Temp_i=0; Temp_i<12; Temp_i++)
	{
	  NO_Buffer[Temp_i+3] = EEPROM_Buffer[Temp_i];
	}
}


void CSV_ParaRead(void)
{
	uint32_t EEPROM_Address;
	int32_t  Para_Slope,Para_Zero,Middle_Tmp;
	uint8_t i = 0;	
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0100;//通道配置
	for(i=0; i<5; i++)     //4个通道
	{
		STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,5);
		if(EEPROM_Buffer[0]==0x55)
		{
			Para_Slope = EEPROM_Buffer[2];
			Middle_Tmp = EEPROM_Buffer[3];
			Para_Slope += (Middle_Tmp<<8);
			Middle_Tmp = EEPROM_Buffer[4];
			Para_Slope += (Middle_Tmp<<16);
			Middle_Tmp = EEPROM_Buffer[5];
			Para_Slope += (Middle_Tmp<<24);
			CH_Volt_slope[i] = Para_Slope;
				
			Para_Zero = EEPROM_Buffer[6];
			Middle_Tmp = EEPROM_Buffer[7];
			Para_Zero += (Middle_Tmp<<8);
			Middle_Tmp = EEPROM_Buffer[8];
			Para_Zero += (Middle_Tmp<<16);
			Middle_Tmp = EEPROM_Buffer[9];
			Para_Zero += (Middle_Tmp<<24);
			CH_Volt_zero[i] = Para_Zero;
		}
		else
		{
        switch(i)
        {
					case 0:
						CH_Volt_slope[0] = 10000;	//电压校准K:±**.****
						CH_Volt_zero[0] = 0;//电压校准B：±*******uV						
						break;
					case 1:
						CH_Volt_slope[1] = 10000;	//电流校准K:±**.****
						CH_Volt_zero[1] = 0;//电流校准B：±*******uA							
						break;
					case 2:
						CH_Volt_slope[2] = 10000;	//电流校准K:±**.****
						CH_Volt_zero[2] = 0;//电流校准B：±*******uA								
						break;
					case 3:
						CH_Volt_slope[3] = 10000;	//电压校准K:±**.****
						CH_Volt_zero[3] = 0;//电压校准B：±*******uV							
						break;	
					case 4:
						CH_Volt_slope[4] = 10000;	//电流校准K:±**.****
						CH_Volt_zero[4] = 0;//电流校准B：±*******uA						
						break;
					default:				
						break;	
				}
			}
		EEPROM_Address += 0x10;
	}
	Delayms(20);
}

 void CSP_ParaRead(void)    //
{
 	uint32_t EEPROM_Address;
 	int32_t  Para_JKB[3],Middle_Tmp;
 	uint8_t Pos_i,Pos_j;
 	
 	EEPROM_Address = FLASH_SAVE_ADDR + 0x0300;//通道配置
 	for(Pos_i=0; Pos_i<5; Pos_i++)
 	{
 		STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,5);
 		if(EEPROM_Buffer[8]==0x55)
		{
			for(Pos_j=0; Pos_j<2; Pos_j++)
			{
				Para_JKB[Pos_j] = EEPROM_Buffer[4*Pos_j];
				Middle_Tmp = EEPROM_Buffer[4*Pos_j+1];
				Para_JKB[Pos_j] += (Middle_Tmp<<8);
				Middle_Tmp = EEPROM_Buffer[4*Pos_j+2];
				Para_JKB[Pos_j] += (Middle_Tmp<<16);
				Middle_Tmp = EEPROM_Buffer[4*Pos_j+3];
				Para_JKB[Pos_j] += (Middle_Tmp<<24);
			}
			//Para_J[Pos_i] = Para_JKB[0];             //更新参数
			Para_K[Pos_i] = Para_JKB[0];
			Para_B[Pos_i] = Para_JKB[1];
		}
		else
		{
       switch(Pos_i)
        {
					case 0://电池电压		            
								Para_K[0] = 100000;
								Para_B[0] = 0;					
								break;
					case 1://水压传感器
								Para_K[1] = 6250;
								Para_B[1] = -250000;							
								break;
					case 2://流速
								Para_K[2] = 18750;
								Para_B[2] = -750000;									
								break;
					case 3://水位
								Para_K[3] = 10;
								Para_B[3] = 0;								
								break;	
					case 4://备用
								Para_K[4] = 100000;
								Para_B[4] = 0;						
								break;
					default:				
						break;	
				}
	  }
 		EEPROM_Address += 0x10;
	}
}


void ADS_ParaRead(void)
{
	uint32_t EEPROM_Address;
	uint8_t  i;	
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0600;//通道配置   
		for(i=0; i<40;i++)//30个通道
		{
			STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,2);
      if(EEPROM_Buffer[0]==0x55)
			{				
				Sensor_ADR[i]=EEPROM_Buffer[3];	//传感器编号
				Sensor_Sel[i]=EEPROM_Buffer[2]; //传感器类型
			}
			else
			{
        switch(i)
        {
					case 0:
						Sensor_ADR[0]= 100;	//传感器地址
						Sensor_Sel[0]='B';//传感器类型						
						break;
					case 1:
						Sensor_ADR[1]= 101;	//传感器地址
						Sensor_Sel[1]='M';//传感器类型						
						break;
					case 2:
						Sensor_ADR[2]= 102;	//传感器地址
						Sensor_Sel[2]='2';//传感器类型						
						break;
					case 3:
						Sensor_ADR[3]= 103;	//传感器地址
						Sensor_Sel[3]='1';//传感器类型						
						break;	
					case 4:
						Sensor_ADR[4]= 0;	//传感器地址
						Sensor_Sel[4]= 0;//传感器类型						
						break;
					case 5:
						Sensor_ADR[5]= 0;	//传感器地址
						Sensor_Sel[5]= 0;//传感器类型						
						break;	
					case 6:
						Sensor_ADR[6]= 0;	//传感器地址
						Sensor_Sel[6]= 0;//传感器类型						
						break;
					case 7:
						Sensor_ADR[7]= 107;	//传感器地址
						Sensor_Sel[7]= '8';//传感器类型						
						break;	
					case 8:
						Sensor_ADR[8]= 108;	//传感器地址
						Sensor_Sel[8]= 'L';//传感器类型						
						break;
					case 9:
						Sensor_ADR[9]= 109;	//传感器地址
						Sensor_Sel[9]= 'N';//传感器类型						
						break;	
					case 10:
						Sensor_ADR[10]= 0;	//传感器地址
						Sensor_Sel[10]= 0;//传感器类型						
						break;
					case 11:
						Sensor_ADR[11]= 0;	//传感器地址
						Sensor_Sel[11]= 0;//传感器类型						
						break;
					case 12:
						Sensor_ADR[12]= 0;	//传感器地址
						Sensor_Sel[12]=0;//传感器类型						
						break;
					case 13:
						Sensor_ADR[13]= 0;	//传感器地址
						Sensor_Sel[13]=0;//传感器类型						
						break;	
					case 14:
						Sensor_ADR[14]= 0;	//传感器地址
						Sensor_Sel[14]= 0;//传感器类型						
						break;
					case 15:
						Sensor_ADR[15]= 0;	//传感器地址
						Sensor_Sel[15]= 0;//传感器类型						
						break;	
					case 16:
						Sensor_ADR[16]= 0;	//传感器地址
						Sensor_Sel[16]= 0;//传感器类型						
						break;
					case 17:
						Sensor_ADR[17]= 0;	//传感器地址
						Sensor_Sel[17]= 0;//传感器类型						
						break;	
					case 18:
						Sensor_ADR[18]= 0;	//传感器地址
						Sensor_Sel[18]= 0;//传感器类型						
						break;
					case 19:
						Sensor_ADR[19]= 0;	//传感器地址
						Sensor_Sel[19]= 0;//传感器类型						
						break;	
					case 20:
						Sensor_ADR[20]= 0;	//传感器地址
						Sensor_Sel[20]= 0;//传感器类型						
						break;
					case 21:
						Sensor_ADR[21]= 2;	//传感器地址
						Sensor_Sel[21]= '2';//传感器类型						
						break;
					case 22:
						Sensor_ADR[22]= 8;	//传感器地址
						Sensor_Sel[22]='1';//传感器类型						
						break;
					case 23:
						Sensor_ADR[23]= 3;	//传感器地址
						Sensor_Sel[23]='3';//传感器类型						
						break;	
					case 24:
						Sensor_ADR[24]= 4;	//传感器地址
						Sensor_Sel[24]='4';//传感器类型						
						break;
					case 25:
						Sensor_ADR[25]= 0;	//传感器地址
						Sensor_Sel[25]= 0;//传感器类型						
						break;	
					case 26:
						Sensor_ADR[26]= 0;	//传感器地址
						Sensor_Sel[26]= 0;//传感器类型						
						break;
					case 27:
						Sensor_ADR[27]= 0;	//传感器地址
						Sensor_Sel[27]= 0;//传感器类型						
						break;	
					case 28:
						Sensor_ADR[28]= 0;	//传感器地址
						Sensor_Sel[28]= 0;//传感器类型						
						break;
					case 29:
						Sensor_ADR[29]= 0;	//传感器地址
						Sensor_Sel[29]= 0;//传感器类型						
						break;						
					case 30:
						Sensor_ADR[30]= 0;	//传感器地址
						Sensor_Sel[30]= 0;//传感器类型						
						break;
					case 31:
						Sensor_ADR[31]= 0;	//传感器地址
						Sensor_Sel[31]= 0;//传感器类型						
						break;
					case 32:
						Sensor_ADR[32]= 0;	//传感器地址
						Sensor_Sel[32]= 0;//传感器类型						
						break;
					case 33:
						Sensor_ADR[33]= 0;	//传感器地址
						Sensor_Sel[33]= 0;//传感器类型						
						break;	
					case 34:
						Sensor_ADR[34]= 0;	//传感器地址
						Sensor_Sel[34]= 0;//传感器类型						
						break;
					case 35:
						Sensor_ADR[35]= 0;	//传感器地址
						Sensor_Sel[35]= 0;//传感器类型						
						break;	
					case 36:
						Sensor_ADR[36]= 0;	//传感器地址
						Sensor_Sel[36]= 0;//传感器类型						
						break;
					case 37:
						Sensor_ADR[37]= 0;	//传感器地址
						Sensor_Sel[37]= 0;//传感器类型						
						break;	
					case 38:
						Sensor_ADR[38]= 0;	//传感器地址
						Sensor_Sel[38]= 0;//传感器类型						
						break;
					case 39:
						Sensor_ADR[39]= 0;	//传感器地址
						Sensor_Sel[39]= 0;//传感器类型						
						break;						
					default:break;
				}
      }
			EEPROM_Address += 0x10;
		}			
}

void SNS_ParaRead(void)
{
	uint32_t EEPROM_Address;
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0500;
	
	STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,2);
	if(EEPROM_Buffer[3]==0x55) Sensor_amount=EEPROM_Buffer[0]*100+	EEPROM_Buffer[1]*10+EEPROM_Buffer[2];
  else 	Sensor_amount=4;  		
}


void FPS_ParaRead(void)                     //控制变频器开关
{
	uint32_t EEPROM_Address;
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0550;//通道配置
	
	STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,1);
//  FreqConv_SET_OnOff=EEPROM_Buffer[0];
//  RS485_Repeat_Operation=0;	
}

void WSL_ParaRead(void)   //水源地水位选择
{
  uint32_t EEPROM_Address;	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0580;//通道配置

  STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,1);
	
	WaterSource_Level=EEPROM_Buffer[0];
}

void WLS_ParaRead(void)   //水源地水位上限设置
{
   uint32_t EEPROM_Address;
	 int32_t  Middle_Tmp;
	
   EEPROM_Address = FLASH_SAVE_ADDR + 0x0590;//通道配置
	 STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,2);
	
	 WaterSourceLevel_S=EEPROM_Buffer[0];
	 Middle_Tmp = EEPROM_Buffer[1];
	 WaterSourceLevel_S+=(Middle_Tmp<<8);
	 Middle_Tmp = EEPROM_Buffer[2];
	 WaterSourceLevel_S += (Middle_Tmp<<16);
	 Middle_Tmp = EEPROM_Buffer[3];
	 WaterSourceLevel_S += (Middle_Tmp<<24);	
}

void WLl_ParaRead(void)  //水源地水位下限
{
   uint32_t EEPROM_Address;
	 int32_t  Middle_Tmp;
	
   EEPROM_Address = FLASH_SAVE_ADDR + 0x05A0;//通道配置
	 STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,2);
	
	 WaterSourceLevel_X=EEPROM_Buffer[0];
	 Middle_Tmp = EEPROM_Buffer[1];
	 WaterSourceLevel_X+=(Middle_Tmp<<8);
	 Middle_Tmp = EEPROM_Buffer[2];
	 WaterSourceLevel_X += (Middle_Tmp<<16);
	 Middle_Tmp = EEPROM_Buffer[3];
	 WaterSourceLevel_X += (Middle_Tmp<<24);
}


void FCM_ParaRead(void)                     //泵站控制方式 0：恒压泵水 1：点频控制  
{
	uint32_t EEPROM_Address;
	
	EEPROM_Address = FLASH_SAVE_ADDR + 0x0560;//通道配置
	
	STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,1);
  Transducer_ControlMode = EEPROM_Buffer[0];

}

void WPS_ParaRead(void)                      //恒压供水-水压设置
{
  uint32_t EEPROM_Address;
	int32_t  Middle_Tmp;
	
  EEPROM_Address = FLASH_SAVE_ADDR + 0x0520;;//通道配置
	STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,2);
	
	 WaterPressure_SET=EEPROM_Buffer[0];
	 Middle_Tmp = EEPROM_Buffer[1];
	 WaterPressure_SET+=(Middle_Tmp<<8);
	 Middle_Tmp = EEPROM_Buffer[2];
	 WaterPressure_SET += (Middle_Tmp<<16);
	 Middle_Tmp = EEPROM_Buffer[3];
	 WaterPressure_SET += (Middle_Tmp<<24);
	
//	RS485_Repeat_Operation=0;
}

void FQS_ParaRead(void)                     //控制变频器频率
{
  uint32_t EEPROM_Address;
	int32_t  Middle_Tmp;
	
  EEPROM_Address = FLASH_SAVE_ADDR + 0x0540;;//通道配置
	STMFLASH_Read(EEPROM_Address,(u16*)EEPROM_Buffer,1);
	
	FreqConv_SET_Freq=EEPROM_Buffer[0];
	Middle_Tmp = EEPROM_Buffer[1];
	FreqConv_SET_Freq+=(Middle_Tmp<<8);

	
}	




