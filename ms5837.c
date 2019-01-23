// MS5837 module library - version:1.0.0
#include "stm32f1xx_hal.h"
#include "ms5837.h"
#include "math.h"



extern I2C_HandleTypeDef hi2c2;

uint8_t data[2] , reset[1] , prom_ad[1] , crc[3] ,d1[1] , d2[1] , adc[1] ;;
uint32_t C[6], Ti = 0, offi = 0, sensi = 0, D2 ,D1 ;
float pressure,ctemp,fTemp;	


void prom(void)
{
 //first reset module 
	
		reset[0]=RESET_AD;

    HAL_I2C_Master_Transmit(&hi2c2,Address,reset,1,50);
    HAL_Delay(50);	
	
 //read prom	
	
for(int i = 0; i < 6; i++)
{	
		prom_ad[0] = 0xA2 + (2 * i);
    HAL_I2C_Master_Transmit(&hi2c2,Address,prom_ad,1,50);
    HAL_I2C_Master_Receive(&hi2c2,Address,data,2,50);
    C[i] = ((data[0] * 256) + data[1]);
}	 
  HAL_Delay(50);
}

void convert(void) 
{
	reset[0]=RESET_AD;
  d1[0]=0x4A;
	d2[0]=0x5A;
	adc[0]=0x00;
  

	
	HAL_I2C_Master_Transmit(&hi2c2,Address,reset,1,50);
    
  HAL_I2C_Master_Transmit(&hi2c2,Address,d1,1,50);
  HAL_Delay(50);
  HAL_I2C_Master_Transmit(&hi2c2,Address,adc,1,50);
  HAL_Delay(50);
  HAL_I2C_Master_Receive(&hi2c2,Address,crc,3,50);
   
	D1 = ((crc[0] * 65536.0) + (crc[1] * 256.0) + crc[2]);
	
	HAL_I2C_Master_Transmit(&hi2c2,Address,d2,1,50);
    HAL_Delay(50);
  
  HAL_I2C_Master_Transmit(&hi2c2,Address,adc,1,50);
    HAL_Delay(50);

  HAL_I2C_Master_Receive(&hi2c2,Address,crc,3,50);

  
  D2 = ((crc[0] * 65536.0) + (crc[1] * 256.0) + crc[2]);
  
  
  double dT = D2 - ((C[4] * 256));
  int_least32_t temp = 2000 + (dT * (C[5] / (pow(2, 23))));
  
  int_least64_t off = C[1] *  65536 + (C[3] * dT) / 128;
  int_least64_t sens = C[0] * 32768 + (C[2] * dT) / 256;

  if(temp >= 2000)
  {
     Ti = 2 * ((dT * dT) / (pow(2, 37)));
     offi = (pow((temp - 2000), 2)) / 16;
     sensi = 0;
  }
  else if(temp < 2000)
  {
    Ti = 3 * ((dT * dT) / (pow(2, 23)));
    offi = 3 * ((pow((temp - 2000), 2))) / 2;
    sensi = 5 * ((pow((temp - 2000),2))) / 8;
     if(temp < - 1500)
  {
     offi = offi + 7 * ((temp + 1500)*(temp + 1500));
     sensi = sensi + 4 * ((temp + 1500)*(temp + 1500)) ;
  }
  }
  
    
  temp -= Ti;
  off -= offi;
  sens -= sensi;

	
	D1 = (((D1 * sens) / 2097152) - off);
  D1 /= 8192;
  
  
  	pressure = D1 / 10.0;
   ctemp = temp / 100.0;
   fTemp = ctemp * 1.8 + 32.0;
	
 
}

