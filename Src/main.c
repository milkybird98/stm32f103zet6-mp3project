/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"

/* USER CODE BEGIN Includes */
#include "lcd_ili9341.h"
#include "vs1053b.h"
#include "string.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
FATFS SDfat;
uint8_t keySta=0;
uint8_t sofSta = 0;
fileNumber numOfFile;
uint8_t needReprint = 0;
uint8_t fullSchedule=0;
GPIO_TypeDef *ports[] = {GPIOF, GPIOF, GPIOF, GPIOF};
uint16_t keys[] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
char *weekday[]={"MON","TUE","WED","THU","FRI","SAT","SUN"};
	uint8_t numofmon[]={31,28,31,30,31,30,31,31,30,31,30,31};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_RTC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void show_Sub_Menu(stack *now, uint16_t choosen, uint8_t type,
                   uint8_t refresh);
void show_Title(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void show_Schedule_Menu(uint8_t choosen) {
	for(int i=0;i<6;i++){
		drawRectangle(0, 42 + 20*i , 240, 18, RGB(0xaa, 0xaa, 0xaa));
	}
	drawRectangle(0, 42 + 20*choosen, 240, 18, RGB(0xdd, 0xdd, 0xdd));
	drawEnSentence(0,47,"Start Schedule",14,0x0000);
	drawEnSentence(0,67,"Set Time",8,0x0000);
	drawEnSentence(0,87,"View Rule",9,0x0000);
	drawEnSentence(0,107,"Add new Rule",12,0x0000);
	drawEnSentence(0,127,"Delete Rule",11,0x0000);
	drawEnSentence(0,147,"Clear Rule",10,0x0000);
}

void Set_Time(){
	uint8_t i=0;
	uint8_t delayTime=100;
	RTC_TimeTypeDef sTime;
	sTime.Hours=0;
	sTime.Minutes=0;
	sTime.Seconds=0;
  RTC_DateTypeDef sDate;
	sDate.Year=0;
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0;
	
	uint8_t needPrint=1;
	uint8_t step=0;
	char tempchar[20];

	while(1){
		
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
          if(i!=2 && keySta==i){
						delayTime=10;
					}else{
						delayTime=100;
					}
					break;
				}
      }
    }
		
		keySta=i;
		
		if(step==0){
			if(keySta == 0 && sDate.Year > 0) {
        sDate.Year--;
				needPrint=1;
      } else if (keySta == 1 && sDate.Year < 99) {
        sDate.Year++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==1){
			if(keySta == 0 && sDate.Month > 1) {
        sDate.Month--;
				needPrint=1;
      } else if (keySta == 1 && sDate.Month < 12) {
        sDate.Month++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==3){
			if(keySta == 0 && sDate.WeekDay > 1) {
        sDate.WeekDay--;
				needPrint=1;
      } else if (keySta == 1 && sDate.WeekDay < 7) {
        sDate.WeekDay++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==2){
			if(keySta == 0 && sDate.Date > 1) {
        sDate.Date--;
				needPrint=1;
      } else if (keySta == 1 && sDate.Date < numofmon[sDate.Month-1]) {
        sDate.Date++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==4){
			if(keySta == 0 && sTime.Hours > 0) {
        sTime.Hours--;
				needPrint=1;
      } else if (keySta == 1 && sTime.Hours < 23) {
        sTime.Hours++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==5){
			if(keySta == 0 && sTime.Minutes > 0) {
        sTime.Minutes--;
				needPrint=1;
      } else if (keySta == 1 && sTime.Minutes < 59) {
        sTime.Minutes++;
				needPrint=1;
      } else if (keySta == 2) {
				HAL_Delay(50);
				step++;
			} else if(keySta == 3){
				return;
			}
		}else if(step==6){
			drawEnSentence(120-9*strlen("Confirm[Y/N]")/2,200,"Confirm[Y/N]",strlen("Confirm[Y/N]"),0x0000);			
			if (keySta == 2) {

				  hrtc.Instance = RTC;
					hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
					hrtc.Init.AsynchPrediv = 127;
					hrtc.Init.SynchPrediv = 255;
					hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
					hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
					hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
				
				sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
				sTime.StoreOperation = RTC_STOREOPERATION_SET;
				HAL_RTC_Init(&hrtc);
				HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
				HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
				return;
			} else if(keySta == 3){
				return;
			}
		}
		if(needPrint){
			drawRectangle(0,160,240,60,0xffff);
			sprintf(tempchar,"20%02d.%d.%d %s",sDate.Year,sDate.Month,sDate.Date,weekday[sDate.WeekDay-1]);
			drawEnSentence(120-9*strlen(tempchar)/2,160,tempchar,strlen(tempchar),0x0000);
			sprintf(tempchar,"%02d:%02d",sTime.Hours,sTime.Minutes);
			drawEnSentence(120-9*strlen(tempchar)/2,180,tempchar,strlen(tempchar),0x0000);			
			needPrint=0;
		}
		HAL_Delay(delayTime);
	}
}

void create_Shfile(){
	FIL sdfile;
	f_open(&sdfile, "0:/Sch.lis", FA_CREATE_ALWAYS);
  HAL_Delay(1);
  f_close(&sdfile);
}

void write_Shfile(uint8_t *timedata,uint8_t * path){
	FIL sdfile;
	FRESULT res;
	uint32_t bw;
	f_open(&sdfile, "0:/Sch.lis", FA_WRITE);
  HAL_Delay(1);
	res = f_lseek(&sdfile, f_size(&sdfile));
  if (res != FR_OK) return;
  res = f_write(&sdfile, timedata, 4, &bw);
	res = f_write(&sdfile, path, 40, &bw);
  res = f_close(&sdfile);
}

void read_Shfile(uint8_t *timedata,uint8_t * path,uint8_t index){
	FIL sdfile;
	FRESULT res;
	uint32_t bw;
	f_open(&sdfile, "0:/Sch.lis", FA_READ);
  HAL_Delay(1);
	res = f_lseek(&sdfile, 44*index);
  if (res != FR_OK) return;
  res = f_read(&sdfile, timedata, 4, &bw);
	res = f_read(&sdfile, path, 40, &bw);
  res = f_close(&sdfile);
}


uint8_t read_Slot(uint8_t *path, uint8_t *timeData, uint8_t index){
	read_Shfile(timeData,path,index);
	return timeData[0];
}

void start_Schedule(){
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;
	FIL ScheduleFil;
	uint8_t numofSlot;
	f_open(&ScheduleFil,"0:/Sch.lis",FA_READ);
	numofSlot=f_size(&ScheduleFil)/44;
	f_close(&ScheduleFil);
	uint8_t nextSlot=0;
	uint8_t timeData[4];
	uint8_t path[40];
	uint32_t minTime=0xffffffff;
	uint32_t timetoNext=0;
	char tempchar[24];

	while(1){

		 if (0 == HAL_GPIO_ReadPin(ports[3], keys[3])) {
       HAL_Delay(2000);
        if (0 == HAL_GPIO_ReadPin(ports[3], keys[3])) {
					return;
				}
      }
		show_Title(); 
		timetoNext=0;
		HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);			 
		HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);		
			
		for(int i=0;i<numofSlot;i++){
			if(read_Slot(path,timeData,i)){
				if(timeData[1]>numofmon[sDate.Month-1]) continue;
				if(timeData[1]<sDate.Date){
					timetoNext=24*60*(numofmon[sDate.Month-1]+sDate.Date-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);		
				}else if(timeData[1]==sDate.Date){
					if(timeData[2]<sTime.Hours){
						timetoNext=24*60*(numofmon[sDate.Month-1]+sDate.Date-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);							
					}else if(timeData[2]==sTime.Hours){
						if(timeData[3]<sTime.Minutes){
							timetoNext=24*60*(numofmon[sDate.Month-1]+sDate.Date-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);			
						}else{
							timetoNext=timeData[3]-sTime.Minutes;
						}
					}else{
						timetoNext=(timeData[2]-sTime.Hours)*60+timeData[3]-sTime.Minutes;
					}
				}else{
					timetoNext=24*60*(timeData[1]-sDate.WeekDay-1)+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);					
				}
			}else{
				if(timeData[1]<sDate.WeekDay){
					timetoNext=24*60*(7+sDate.WeekDay-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);	
				}else if(timeData[1]==sDate.WeekDay){
					if(timeData[2]<sTime.Hours){
						timetoNext=24*60*(7+sDate.WeekDay-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);						
					}else if(timeData[2]==sTime.Hours){
						if(timeData[3]<sTime.Minutes){
							timetoNext=24*60*(7+sDate.WeekDay-1-timeData[1])+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);	
						}else{
							timetoNext=timeData[3]-sTime.Minutes;
						}
					}else{
						timetoNext=(timeData[2]-sTime.Hours)*60+timeData[3]-sTime.Minutes;
					}
				}else{
					timetoNext=24*60*(timeData[1]-sDate.WeekDay-1)+timeData[2]*60+timeData[3]+(23-sTime.Hours)*60+(59-sTime.Minutes);						
				}
			}
			if(timetoNext<minTime){
				minTime=timetoNext;
				nextSlot=i;
			}
		}
		
			sprintf(tempchar,"20%02d.%d.%d %s",sDate.Year,sDate.Month,sDate.Date,weekday[sDate.WeekDay-1]);
			drawEnSentence(120-9*strlen(tempchar)/2,100,tempchar,strlen(tempchar),0x0000);
			sprintf(tempchar,"%02d:%02d",sTime.Hours,sTime.Minutes);
			drawEnSentence(120-9*strlen(tempchar)/2,120,tempchar,strlen(tempchar),0x0000);			
		
			
		
		if(read_Slot(path,timeData,nextSlot)){
			sprintf(tempchar,"At %02d:%02d On %02d",timeData[2],timeData[3],timeData[1]);
		}else{
			sprintf(tempchar,"At %02d:%02d On %s",timeData[2],timeData[3],weekday[timeData[1]-1]);
		}
		
		drawEnSentence(120-9*strlen((char *)path)/2,160,(char *)path,strlen((char *)path),0x0000);
		drawEnSentence(120-9*strlen(tempchar)/2,180,tempchar,strlen(tempchar),0x0000);					
		
		if(minTime==0){
			vs1053b_play_music((char *)path);
			continue;
		}
		
		HAL_Delay(5000);
	}
}

void create_Slot(){
	uint8_t choosen=0;
	uint8_t delayTime=100;
	uint8_t i=0;
	uint8_t step=0;
	uint8_t refresh=0;
	stack now;
	uint8_t isfirst=1;
	uint8_t tempMp3[40];
	uint8_t timeData[4];
	uint8_t tempchar[10];
	
	while(1){
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
          if(i!=2 && keySta==i){
						delayTime=10;
					}else{
						delayTime=120;
					}
					break;
				}
      }
    }
			
		keySta=i;
		
		if(step==0){
			if(isfirst){
				show_Title();
				now.nums = numOfFile.mp3;
        show_Sub_Menu(&now, 0, 0, 1);
				isfirst=0;
			}
      if (keySta == 0 && choosen > 0) {
        choosen--;
        if (choosen % 14 == 13) refresh = 1;
				now.nums = numOfFile.mp3;
				show_Sub_Menu(&now, choosen, 0, refresh);
				refresh = 0;
      } else if (keySta == 1 && choosen < numOfFile.mp3 - 1) {
        choosen++;
        if (choosen % 14 == 0) refresh = 1;
				now.nums = numOfFile.mp3;
				show_Sub_Menu(&now, choosen, 0, refresh);
				refresh = 0;
      } else if (keySta == 2) {
				now.nums = fullSchedule - choosen;
				getData(&now,0);
				strcpy((char *)tempMp3,now.path);
				step++;
				isfirst=1;
				HAL_Delay(50);
			} else if(keySta == 3){
				return;
			}
		}else if(step==1){
			if(isfirst){
				show_Title();
				choosen=1;
				isfirst=0;
				drawRectangle(0, 42 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47,"By Date",strlen("By Date"),0x0000);
				drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0,47+20,"By Weekday",strlen("By Weekday"),0x0000);
			}
			if(keySta == 0 && choosen == 0){
				drawRectangle(0, 42 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47,"By Date",strlen("By Date"),0x0000);
				drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0,47+20,"By Weekday",strlen("By Weekday"),0x0000);
				choosen=1;
			}else if (keySta == 1 && choosen ==1){
				drawRectangle(0, 42 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0,47,"By Date",strlen("By Date"),0x0000);
				drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+20,"By Weekday",strlen("By Weekday"),0x0000);
				choosen=0;
			}else if (keySta == 2){
				isfirst=1;
				step++;
				timeData[0]=choosen;
				HAL_Delay(50);
			}else if(keySta == 3){
				return;
			}
		}else if(step==2){
			if(timeData[0]==1){
				if(isfirst){
					//show_Title();
					choosen=0;
					isfirst=0;
					drawRectangle(0, 42 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47,"By Date",strlen("By Date"),0x0000);
					sprintf((char *)tempchar,"%d",choosen);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,(char *)tempchar,strlen((char *)tempchar),0x0000);
				}			
				if(keySta == 0 && choosen > 1){
					choosen--;
					sprintf((char *)tempchar,"%d",choosen);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,(char *)tempchar,strlen((char *)tempchar),0x0000);
				}else if(keySta == 1 && choosen <31){
					choosen++;
					sprintf((char *)tempchar,"%d",choosen);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,(char *)tempchar,strlen((char *)tempchar),0x0000);
				}else if(keySta == 2){
					isfirst=1;
					step++;
					timeData[1]=choosen;
					HAL_Delay(50);
				}else if(keySta == 3){
					return;
				}
			}else	if(timeData[0]==0){
				if(isfirst){
					//show_Title();
					choosen=1;
					isfirst=0;
					drawRectangle(0, 42 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47,"By Weekday",strlen("By Weekday"),0x0000);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,weekday[choosen-1],4,0x0000);
				}			
				if(keySta == 0 && choosen > 1){
					choosen--;
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,weekday[choosen-1],4,0x0000);
				}else if(keySta == 1 && choosen <7){
					choosen++;
					drawRectangle(0, 42+20 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
					drawEnSentence(0,47+20,weekday[choosen-1],4,0x0000);
				}else if(keySta == 2){
					isfirst=1;
					step++;
					timeData[1]=choosen;
					HAL_Delay(50);
				}else if(keySta == 3){
					return;
				}
			}
		}else if(step==3){
			if(isfirst){
				//show_Title();
				choosen=0;
				isfirst=0;
				drawRectangle(0, 42 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0,47,"By Weekday",strlen("By Weekday"),0x0000);
				if(timeData[0]==1){
					sprintf((char *)tempchar,"%d",timeData[1]);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47+20,(char *)tempchar,strlen((char *)tempchar),0x0000);
				}else{
					drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47+20,weekday[choosen],4,0x0000);
				}
				sprintf((char *)tempchar,"%02d:",choosen);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}
			if(keySta == 0 && choosen > 0){
				choosen--;
				sprintf((char *)tempchar,"%02d:",choosen);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}else if(keySta == 1 && choosen <23){
				choosen++;
				sprintf((char *)tempchar,"%02d:",choosen);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}else if(keySta == 2){
				isfirst=1;
				step++;
				timeData[2]=choosen;
				HAL_Delay(50);
			}else if(keySta == 3){
					return;
			}
		}else if(step==4){
			if(isfirst){
				//show_Title();
				choosen=0;
				isfirst=0;
				drawRectangle(0, 42 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0,47,"By Weekday",strlen("By Weekday"),0x0000);
				if(timeData[0]==1){
					sprintf((char *)tempchar,"%d",timeData[1]);
					drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47+20,(char *)tempchar,strlen((char *)tempchar),0x0000);
				}else{
					drawRectangle(0, 42+20 , 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0,47+20,weekday[choosen],4,0x0000);
				}
				sprintf((char *)tempchar,"%02d:%02d",timeData[2],0);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}
			if(keySta == 0 && choosen > 0){
				choosen--;
				sprintf((char *)tempchar,"%02d:%02d",timeData[2],choosen);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}else if(keySta == 1 && choosen <59){
				choosen++;
				sprintf((char *)tempchar,"%02d:%02d",timeData[2],choosen);
				drawRectangle(0, 42+40 , 240, 18, RGB(0xdd, 0xdd, 0xdd));
				drawEnSentence(0,47+40,(char *)tempchar,strlen((char *)tempchar),0x0000);
			}else if(keySta == 2){
				isfirst=1;
				step++;
				timeData[3]=choosen;
				HAL_Delay(50);
			}else if(keySta == 3){
					return;
			}
		}else if(step==5){
			write_Shfile(timeData,tempMp3);
			return;
		}
		HAL_Delay(delayTime);
	}
}

void show_Schedule(void){
	uint8_t i=0;
	uint8_t numofSlot=0;
	FIL ScheduleFil;
	uint8_t timeData[4];
	uint8_t path[40];
	uint8_t tempchar[20];
	uint8_t page=0;
	uint8_t nowpage=0;
	
	f_open(&ScheduleFil,"0:/Sch.lis",FA_READ);
	numofSlot=f_size(&ScheduleFil)/44;
	for(int n=0;;n++){
		if(numofSlot<=7*(n+1)){
			page=n;
			break;
		}
	}

	f_close(&ScheduleFil);
	needReprint=1;
	while(1){
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
					break;
				}
      }
    }
			
		keySta=i;
		
		if(keySta==0 && nowpage>0){
			nowpage--;
			needReprint=1;
			show_Title();
		}else if(keySta==1 && nowpage<page){
			nowpage++;
			needReprint=1;
			show_Title();
		}else if(keySta==3){
			return;
		}
		
		if(needReprint==1){
			for(int n=0;n<7 && n<numofSlot-nowpage*7;n++){
				if(read_Slot(path,timeData,n+nowpage*7)){
					drawRectangle(0, 42 + 40 * n, 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0, 47 + 40 * n, (char *)path, strlen((char *)path),
												 RGB(0, 0, 0));
					sprintf((char *)tempchar,"At %02d:%02d On %02d",timeData[2],timeData[3],timeData[1]);
					drawRectangle(0, 42 + 40 * n+20, 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0, 47 + 40 * n+20, (char *)tempchar, strlen((char *)tempchar),
												 RGB(0, 0, 0));			
				}else{
					drawRectangle(0, 42 + 40 * n, 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0, 47 + 40 * n, (char *)path, strlen((char *)path),
												 RGB(0, 0, 0));
					sprintf((char *)tempchar,"At %02d:%02d On %s",timeData[2],timeData[3],weekday[timeData[1]-1]);
					drawRectangle(0, 42 + 40 * n+20, 240, 18, RGB(0xaa, 0xaa, 0xaa));
					drawEnSentence(0, 47 + 40 * n+20, (char *)tempchar, strlen((char *)tempchar),
												 RGB(0, 0, 0));			
				}
			}
			needReprint=0;
		}
	}
}

void Schedule_Mode(){
	uint8_t delayTime=100;
  uint8_t choosen = 0;
	uint8_t i;
	
	show_Title();
	show_Schedule_Menu(0);
	
	while(1){
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
          if(keySta==i){
						delayTime=10;
					}else{
						delayTime=100;
					}
					break;
				}
      }
    }
		keySta=i;
		
		if(keySta==0 && choosen >0){
			choosen--;
			//show_Title();
			show_Schedule_Menu(choosen);
		}else if(keySta==1 && choosen <5){
			choosen++;
			//show_Title();
			show_Schedule_Menu(choosen);
		}else if(keySta==2 ){
			switch(choosen){
				case 0:
					show_Title();
					start_Schedule();
					break;
				case 1:
					show_Title();
					Set_Time();
					break;				
				case 2:
					show_Title();
					show_Schedule();
					break;
				case 3:
					show_Title();
					create_Slot();
					break;
				case 4:
					break;
				case 5:
					show_Title();
					drawEnSentence(120-9*strlen("Confirm[Y/N]")/2,180,"Confirm[Y/N]",strlen("Confirm[Y/N]"),0x0000);
					while(1){
						for (i = 0; i < 4; i++) {
							if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
								HAL_Delay(10);
								if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
									if(keySta==i){
										delayTime=10;
									}else{
										delayTime=100;
									}
									break;
								}
							}
						}
						keySta=i;				
						if(keySta==2){create_Shfile();break;}
						else if(keySta==3) break;
					}	
					break;
				
			}
			show_Title();
			show_Schedule_Menu(choosen);
		}
		HAL_Delay(delayTime);
	}
	
}

void show_Welcome(void) {
  drawRectangle(0, 0, 240, 320, 0x0000);
  drawPictu(60, 120, "0:/welcome");
  drawCnSentence(75, 300, "少女祈祷中", strlen("少女祈祷中"), 0xffff,0x0000);
}

void show_Title(void) {
  drawRectangle(0, 0, 240, 40, RGB(0x01, 0xff, 0x66));
  drawEnSentence(62, 16, "LITTLE PLAYER", strlen("LITTLE PLAYER"), 0x0000);
  drawRectangle(0, 40, 240, 280, RGB(0xff, 0xff, 0xff));
}

void show_Main_Menu(uint16_t choosen) {
  drawPictu(88, 62, "0:/music");
  drawPictu(88, 148, "0:/file");
  drawPictu(88, 234, "0:/photos");

  drawEnSentence(75, 130, "MP3 PLAYER", strlen("MP3 Player"), 0x0000);
  drawEnSentence(75, 216, "TXT VIEWER", strlen("TXT Viewer"), 0x0000);
  drawEnSentence(75, 302, "BMP VIEWER", strlen("BMP Viewer"), 0x0000);

  drawRectangle(55, 89, 11, 11, RGB(0xff, 0xff, 0xff));
  drawRectangle(55, 175, 11, 11, RGB(0xff, 0xff, 0xff));
  drawRectangle(55, 261, 11, 11, RGB(0xff, 0xff, 0xff));
  drawCircul(60, 94 + 86 * choosen, 5, 0x0000);
}

void show_Sub_Menu(stack *now, uint16_t choosen, uint8_t type,
                   uint8_t refresh) {
  uint16_t i = 0;
  if (!now->nums) {
    drawEnSentence(0, 65, "NO DATA", strlen("NO DATA"), RGB(0, 0, 0));
  } else if (refresh) {
    drawRectangle(0, 40, 240, 280, RGB(0xff, 0xff, 0xff));
    now->nums -= (choosen / 14) * 14;
    choosen %= 14;
    while (i < 14 && !isempty(now)) {
      getData(now, type);
      if (i == choosen)
        drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xdd, 0xdd, 0xdd));
      else
        drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xaa, 0xaa, 0xaa));
      drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
                     RGB(0, 0, 0));
      i++;
      pop(now);
    }
  } else if(!refresh){
    now->nums -= (choosen / 14) * 14;
    choosen %= 14;
    if (choosen == 0) {
      getData(now, type);
      drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xdd, 0xdd, 0xdd));
      drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
                     RGB(0, 0, 0));
      pop(now);
      i++;
			if(!isempty(now)){
				getData(now, type);
				drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xaa, 0xaa, 0xaa));
				drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
											RGB(0, 0, 0));
			}
    } else if (choosen == 13) {
      now->nums -= 12;
      i = 12;
      getData(now, type);
      drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xaa, 0xaa, 0xaa));
      drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
                     RGB(0, 0, 0));
      pop(now);
      i++;
      getData(now, type);
      drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xdd, 0xdd, 0xdd));
      drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
                     RGB(0, 0, 0));
    } else if(choosen >0 && choosen <13) {
      now->nums -= choosen - 1;
      i = choosen - 1;
      while (i <= choosen + 1 && !isempty(now)) {
        getData(now, type);
        if (i == choosen)
          drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xdd, 0xdd, 0xdd));
        else
          drawRectangle(0, 42 + 20 * i, 240, 18, RGB(0xaa, 0xaa, 0xaa));
        drawEnSentence(0, 47 + 20 * i, now->name, strlen(now->name) - 4,
                       RGB(0, 0, 0));
        i++;
        pop(now);
      }
    }
  }
}

void show_Picture(uint16_t choosen) {
  stack now;
  now.nums = numOfFile.bmp - choosen;
  getData(&now, 2);
  drawRectangle(0, 0, 240, 320, 0x0001);
  drawPictu(0, 0, now.path);
}

uint32_t show_Txet_Page(uint16_t choosen, uint32_t position) {
  FIL txtFile;
  FRESULT res;
  stack now;
  uint32_t br;
  uint16_t bufferSended = 0;
  uint16_t x = 0;
  uint16_t y = 0;
  uint8_t oneLineNums = 0;
  uint16_t realWidth = 0;
  char txtBuffer[600];

  drawRectangle(0, 0, 240, 320, 0xffff);
  now.nums = numOfFile.txt - choosen;
  getData(&now, 1);
  res = f_open(&txtFile, now.path, FA_READ);
  res = f_lseek(&txtFile, position);
  if (res != FR_OK) return 0;
  res = f_read(&txtFile, txtBuffer, 600, &br);
  if (br == 0) {
    f_close(&txtFile);
    return 0;
  }
  for (uint16_t i = 0; i < br && y <= 300;) {
    while (1) {
      if (txtBuffer[i] == 0x0D) {
        if (i + 1 > br) break;
        if (txtBuffer[i + 1] == 0x0A) {
          i += 2;
        } else {
          i++;
        }
        break;
      } else if (txtBuffer[i] <= 0x7f && txtBuffer[i] >= ' ') {
        if (realWidth + 9 > 240) break;
        realWidth += 9;
        if (i + 1 > br) break;
        oneLineNums++;
        i++;
      } else {
        if (realWidth + 18 > 240) break;
        realWidth += 18;
        if (i + 1 > br) break;
        oneLineNums += 2;
        i += 2;
      }
    }

    drawCnSentence(x, y, txtBuffer + bufferSended, oneLineNums, 0x0000, 0xffff);
    realWidth = 0;
    oneLineNums = 0;
    bufferSended = i;
    y += 20;
  }
  f_close(&txtFile);
  return bufferSended;
}

void read_Text(uint16_t choosen) {
  uint32_t position[200];
  uint8_t page = 0;
	uint8_t delayTime=100;
  FIL txtFile;
  stack now;

  now.nums = numOfFile.txt - choosen;
  getData(&now, 1);
  f_open(&txtFile, now.path, FA_READ);
  uint32_t fullSize = f_size(&txtFile);
  f_close(&txtFile);

  position[0] = 0;

  position[1] = show_Txet_Page(choosen, position[0]);
  while (1) {
    uint8_t i;
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
          if(keySta==i){
						delayTime=10;
					}else{
						delayTime=100;
					}
					break;
				}
      }
    }
			
		keySta=i;
	
    if (keySta == 0 && page > 0) {
      page--;
      show_Txet_Page(choosen, position[page]);
     
    } else if (keySta == 1 && page < 200 && position[page + 1] < fullSize) {
      page++;
      position[page + 1] =
          show_Txet_Page(choosen, position[page]) + position[page];
     
    } else if (keySta == 3) {
     
      return;
    }
    HAL_Delay(delayTime);
  }
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_RTC_Init();

  /* USER CODE BEGIN 2 */
 
	uint8_t delayTime=100;
  uint8_t choosen = 0;
  uint8_t refresh = 0;
	
  f_mount(&SDfat, SDPath, 0);

  lcdRestart();
  HAL_Delay(5);
  lcdInit();

  show_Welcome();
  getFile(&numOfFile);
	vs1053b_reset();

	if (0 == HAL_GPIO_ReadPin(ports[3], keys[3])) {
		HAL_Delay(2000);
		if (0 == HAL_GPIO_ReadPin(ports[3], keys[3])) {
			Schedule_Mode();
		}
	}
	
  HAL_Delay(1500);
  show_Title();
  show_Main_Menu(0);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    uint8_t i;
    for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        HAL_Delay(10);
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
          if(keySta==i){
						delayTime=10;
					}else{
						delayTime=100;
					}
					break;
				}
      }
    }
			
		keySta=i;

    if (sofSta == 0) {
      if (keySta == 0 && choosen > 0) {
        choosen--;
        needReprint = 1;

      } else if (keySta == 1 && choosen < 2) {
        choosen++;
        needReprint = 1;

      } else if (keySta == 2) {
        sofSta = choosen + 1;
        choosen = 0;
        show_Title();
        refresh = 1;
        needReprint = 1;
      }
    } else if (sofSta == 1) {
      if (keySta == 0 && choosen > 0) {
        choosen--;
        if (choosen % 14 == 13) refresh = 1;
        needReprint = 1;
      } else if (keySta == 1 && choosen < numOfFile.mp3 - 1) {
        choosen++;
        if (choosen % 14 == 0) refresh = 1;
        needReprint = 1;
      } else if (keySta == 2) {
        sofSta = 4;
				show_Title();
				stack now;
				now.nums = numOfFile.mp3 - choosen;
				getData(&now, 0);
				vs1053b_play_music(now.path);
				show_Title();
				sofSta = 1;
				refresh=1;
				needReprint =1;
      } else if (keySta == 3) {
        sofSta = 0;
        choosen = 0;
        show_Title();
        needReprint = 1;
      }
    } else if (sofSta == 2) {
      if (keySta == 0) {
        if (choosen > 0) {
          choosen--;
          if (choosen % 14 == 13) refresh = 1;
          needReprint = 1;
        }
      } else if (keySta == 1) {
        if (choosen < numOfFile.txt - 1) {
          choosen++;
          if (choosen % 14 == 0) refresh = 1;
          needReprint = 1;
        }
      } else if (keySta == 2) {
        sofSta = 5;
        read_Text(choosen);
        sofSta = 2;
        show_Title();
        refresh = 1;
        needReprint = 1;
        
      } else if (keySta == 3) {
        sofSta = 0;
        choosen = 1;
        show_Title();
        needReprint = 1;
        
      }
    } else if (sofSta == 3) {
      if (keySta == 0) {
        if (choosen > 0) {
          choosen--;
          if (choosen % 14 == 13) refresh = 1;
          needReprint = 1;
          
        }
      } else if (keySta == 1) {
        if (choosen < numOfFile.bmp - 1) {
          choosen++;
          if (choosen % 14 == 0) refresh = 1;
          needReprint = 1;
          
        }
      } else if (keySta == 2) {
        sofSta = 6;
        needReprint = 1;
        
      } else if (keySta == 3) {
        sofSta = 0;
        choosen = 2;
        show_Title();
        needReprint = 1;
        
      }
    } else if (sofSta == 6) {
      if (keySta == 3) {
        sofSta = 3;
        show_Title();
        refresh = 1;
        needReprint = 1;
        
      }
    }

    stack now;
    if (sofSta == 0 && needReprint == 1) {
      show_Main_Menu(choosen);
      needReprint = 0;
    } else if (sofSta == 1 && needReprint == 1) {
      now.nums = numOfFile.mp3;
      show_Sub_Menu(&now, choosen, 0, refresh);
      refresh = 0;
      needReprint = 0;
    } else if (sofSta == 2 && needReprint == 1) {
      now.nums = numOfFile.txt;
      show_Sub_Menu(&now, choosen, 1, refresh);
      refresh = 0;
      needReprint = 0;
    } else if (sofSta == 3 && needReprint == 1) {
      now.nums = numOfFile.bmp;
      show_Sub_Menu(&now, choosen, 2, refresh);
      refresh = 0;
      needReprint = 0;
    } else if (sofSta == 6 && needReprint == 1) {
      show_Picture(choosen);
      needReprint = 0;
    }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	HAL_Delay(delayTime);
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2){
  sTime.Hours = 15;
  sTime.Minutes = 38;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_SET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  sDate.Month = RTC_MONTH_FEBRUARY;
  sDate.Date = 7;
  sDate.Year = 18;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
  }

}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 4;

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF4 PF5 PF6 PF7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
