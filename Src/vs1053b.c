#include "vs1053b.h"
#include "lcd_ili9341.h"


FIL sdfile;
extern GPIO_TypeDef *ports[];
extern uint16_t keys[];

void vs1053b_set_vol(uint8_t volx) {
  uint16_t volt = 0;
  volt = 254 - volx;
  volt <<= 8;
  volt += 254 - volx;
  vs1053b_write_cmd(SPI_VOL, volt);
}
void vs1053b_soft_reset(void) {
  while (vs1053b_read_dreq() == 0) {
  }
  int try = 0;
  while (vs1053b_read_reg(SPI_MODE) != 0x0800) {
    vs1053b_write_cmd(SPI_MODE, 0x0804);
    HAL_Delay(5);
    if (try ++ > 100) {
      break;
    }
  }

  while (vs1053b_read_dreq() == 0) {
  }

  vs1053b_write_cmd(SPI_CLOCKF, 0xc800);

  HAL_Delay(20);
}

uint16_t vs1053b_read_ram(uint16_t address) {
  vs1053b_write_cmd(SPI_WRAMADDR, address);
  uint16_t res;
  res = vs1053b_read_reg(SPI_WRAM);
  return res;
}

void vs1053b_restart_play(void) {
  uint8_t buffer[32];
  uint16_t i;
  uint8_t n;
  for (n = 0; n < 32; n++) {
    buffer[i] = 0;
  }
  uint16_t temp;
  temp = vs1053b_read_reg(SPI_MODE);
  temp |= 1 << 3;
  temp |= 1 << 2;
  temp &= 0 << 12;
  vs1053b_write_cmd(SPI_MODE, temp);
  for (i = 0; i < 2048;) {
    if (vs1053b_write_music(buffer) == 0) {
      i += 32;
      temp = vs1053b_read_reg(SPI_MODE);
      if ((temp & (1 << 3)) == 0) {
        break;
      }
    }
  }
  if (i < 2048) {
    temp = vs1053b_read_ram(0X1E06) & 0xff;
    for (n = 0; n < 32; n++) {
      buffer[n] = temp;
    }
    for (i = 0; i < 2052;) {
      if (vs1053b_write_music(buffer) == 0) {
        i += 32;
      }
    }
  }
}

uint16_t vs1053b_read_reg(uint8_t address) {
  while (vs1053b_read_dreq() == 0) {
  }
  vs1053b_write_pin(XDCS, 1);
  vs1053b_write_pin(XCS, 0);
  uint8_t data = VS_READ_COMMAND;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = address;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  uint8_t temp8b=0;
  uint16_t temp16b=0;
  HAL_SPI_Receive(&hspi1, &temp8b, 1, 5);
  temp16b |= temp8b << 8;
  HAL_SPI_Receive(&hspi1, &temp8b, 1, 5);
  temp16b |= temp8b;
  vs1053b_write_pin(XCS, 1);
  return temp16b;
}

int vs1053b_write_music(uint8_t *buffer) {
  if (vs1053b_read_dreq() == 1) {
    vs1053b_write_pin(XDCS, 0);
    for (int i = 0; i < 32;) {
      if (HAL_OK == HAL_SPI_Transmit(&hspi1, buffer + i, 1, 5)) {
        i += 1;
      }
    }
    // HAL_UART_Transmit(&huart1, (uint8_t *)"s", 1, 1);
    vs1053b_write_pin(XDCS, 1);
  } else {
    return 1;
  }
  return 0;
}

int vs1053b_read_dreq() { return (int)HAL_GPIO_ReadPin(GPIOB, DREQ); }
void vs1053b_write_pin(uint16_t GPIO_Pin, int PinState) {
  if (PinState == 1) {
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin, GPIO_PIN_SET);
  } else if (PinState == 0) {
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin, GPIO_PIN_RESET);
  }
}

int vs1053b_reset() {
  vs1053b_write_pin(REST, 0);
  HAL_Delay(20);
  vs1053b_write_pin(XDCS, 1);
  vs1053b_write_pin(XCS, 1);
  vs1053b_write_pin(REST, 1);
  for (int try = 0; try < 20; try ++) {
    if (vs1053b_read_dreq() == 1) {
      HAL_Delay(20);
      return 0;
    }
    HAL_Delay(1);
  }
  HAL_Delay(20);
  return 1;
}
void vs1053b_write_cmd(uint8_t address, uint16_t data16b) {
  while (vs1053b_read_dreq() == 0) {
  }
  vs1053b_write_pin(XDCS, 1);
  vs1053b_write_pin(XCS, 0);

  uint8_t cmd = VS_WRITE_COMMAND;
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 5);
  HAL_SPI_Transmit(&hspi1, &address, 1, 5);
  uint8_t data8b = data16b >> 8;
  HAL_SPI_Transmit(&hspi1, &data8b, 1, 5);
  data8b = data16b & 0xff;
  HAL_SPI_Transmit(&hspi1, &data8b, 1, 5);

  vs1053b_write_pin(XCS, 1);
}

void vs1053b_write_data(uint8_t data) {
  vs1053b_write_pin(XDCS, 0);
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  vs1053b_write_pin(XDCS, 1);
}

void vs1053b_sine_test() {
  vs1053b_write_cmd(0x0b, 0x2020);
  vs1053b_write_cmd(SPI_MODE, 0x0820);
  while (vs1053b_read_dreq() == 0) {
  }
  vs1053b_write_pin(XDCS, 0);
  uint8_t data;
  data = 0x53;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0xef;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x6e;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x24;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  HAL_Delay(500);
  vs1053b_write_pin(XDCS, 1);

  vs1053b_write_pin(XDCS, 0);
  data = 0x45;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x78;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x69;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x74;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  data = 0x00;
  HAL_SPI_Transmit(&hspi1, &data, 1, 5);
  HAL_Delay(500);
  vs1053b_write_pin(XDCS, 1);
}

int isempty(stack *stackhead) {
  if (stackhead->nums > 0)
    return 0;
  else
    return 1;
}

void pop(stack *stackhead) { stackhead->nums--; }

stack *getData(stack *stackhead, uint8_t type) {
  FRESULT res;
  uint32_t br;
  if (type == 0) {
    res = f_open(&sdfile, "0:/mp3list", FA_READ);
    res = f_lseek(&sdfile, (stackhead->nums - 1) * 60);
    if (res != FR_OK) return NULL;
    res = f_read(&sdfile, stackhead->name, 20, &br);
    res = f_read(&sdfile, stackhead->path, 40, &br);
    if (res != FR_OK) {
      res = f_close(&sdfile);
      return NULL;
    }
    res = f_close(&sdfile);
    return stackhead;
  } else if (type == 1) {
    res = f_open(&sdfile, "0:/txtlist", FA_READ);
    res = f_lseek(&sdfile, (stackhead->nums - 1) * 60);
    if (res != FR_OK) return NULL;
    res = f_read(&sdfile, stackhead->name, 20, &br);
    res = f_read(&sdfile, stackhead->path, 40, &br);
    if (res != FR_OK) {
      res = f_close(&sdfile);
      return NULL;
    }
    res = f_close(&sdfile);
    return stackhead;
  } else if (type == 2) {
    res = f_open(&sdfile, "0:/bmplist", FA_READ);
    res = f_lseek(&sdfile, (stackhead->nums - 1) * 60);
    if (res != FR_OK) return NULL;
    res = f_read(&sdfile, stackhead->name, 20, &br);
    res = f_read(&sdfile, stackhead->path, 40, &br);
    if (res != FR_OK) {
      res = f_close(&sdfile);
      return NULL;
    }
    res = f_close(&sdfile);
    return stackhead;
  }
  return NULL;
}

void push(char *path, char *name, stack *stackhead, uint8_t type) {
  FRESULT res;
  uint32_t bw;
  sprintf(stackhead->path, "%s/%s", path, name);
  if (type == 0) {
    res = f_open(&sdfile, "0:/mp3list", FA_WRITE);
    res = f_lseek(&sdfile, f_size(&sdfile));
    if (res != FR_OK) return;
    res = f_write(&sdfile, name, 20, &bw);
    res = f_write(&sdfile, stackhead->path, 40, &bw);
    res = f_close(&sdfile);
  } else if (type == 1) {
    res = f_open(&sdfile, "0:/txtlist", FA_WRITE);
    res = f_lseek(&sdfile, f_size(&sdfile));
    if (res != FR_OK) return;
    res = f_write(&sdfile, name, 20, &bw);
    res = f_write(&sdfile, stackhead->path, 40, &bw);
    res = f_close(&sdfile);
  } else if (type == 2) {
    res = f_open(&sdfile, "0:/bmplist", FA_WRITE);
    res = f_lseek(&sdfile, f_size(&sdfile));
    if (res != FR_OK) return;
    res = f_write(&sdfile, name, 20, &bw);
    res = f_write(&sdfile, stackhead->path, 40, &bw);
    res = f_close(&sdfile);
  }
}

stack *stackcreate() {
  f_open(&sdfile, "0:/mp3list", FA_CREATE_ALWAYS);
  HAL_Delay(1);
  f_close(&sdfile);

  f_open(&sdfile, "0:/txtlist", FA_CREATE_ALWAYS);
  HAL_Delay(1);
  f_close(&sdfile);

  f_open(&sdfile, "0:/bmplist", FA_CREATE_ALWAYS);
  HAL_Delay(1);
  f_close(&sdfile);

  static stack newstack;
  return &newstack;
}

void spiSpeedChange(uint32_t spiSpeed) {
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = spiSpeed;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}

uint8_t vs1053b_play_music(char *path) {
  FIL sdfile;
  unsigned int bytesread;
	uint16_t tempTime=0;	
	uint16_t decodeTime=0;
	uint8_t volum=180;
	uint8_t tempvol=0;
  uint8_t readbuffer[2048];
  uint32_t blocks = 0;
	uint8_t keySta=4;
	uint16_t fulltime=0;
	uint8_t ispaused=0;
	
  spiSpeedChange(SPI_BAUDRATEPRESCALER_64);
	vs1053b_restart_play();
  HAL_Delay(10);
  vs1053b_soft_reset();
  HAL_Delay(10);
  vs1053b_set_vol(180);
	HAL_Delay(10);
	
  vs1053b_write_cmd(SPI_DECODE_TIME, 0x0000);
  vs1053b_write_cmd(SPI_DECODE_TIME, 0x0000);
	vs1053b_write_cmd(SPI_DECODE_TIME, 0x0000);
  spiSpeedChange(SPI_BAUDRATEPRESCALER_8);
	drawPictu(40,142,"0:/sound");
	drawPictu(20,142,"0:/play");
  if (FR_OK == f_open(&sdfile, path, FA_READ)) {
    HAL_Delay(10);
    while (1) {

			uint8_t i;
			for (i = 0; i < 4; i++) {
      if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
        if (0 == HAL_GPIO_ReadPin(ports[i], keys[i])) {
					break;
				}
      }
    }
			
		keySta=i;
		
		  if (keySta == 0 && volum>1 ){
				volum-=5;
				vs1053b_set_vol(volum);
				//needReprint = 1;
			}else if(keySta == 1 && volum<250 ){
				volum+=5;
				vs1053b_set_vol(volum);
				//needReprint = 1;
			}else if(keySta == 2){
				if(ispaused==0) {
					ispaused=1;
					HAL_Delay(200);
					drawPictu(20,142,"0:/pau");
				}
				else {
					ispaused=0;
					HAL_Delay(200);
					drawPictu(20,142,"0:/play");
				}
			}else if (keySta == 3) {
				spiSpeedChange(SPI_BAUDRATEPRESCALER_64);
				vs1053b_soft_reset();
				vs1053b_set_vol(0);
				spiSpeedChange(SPI_BAUDRATEPRESCALER_8);
				f_close(&sdfile);	
				return 1;
        //sofSta = 1;
        //show_Title();
        //refresh = 1;
        //needReprint = 1;
      }
			if(ispaused)	{
				
				continue;			
			}
      if (FR_OK == f_read(&sdfile, readbuffer, 2048, &bytesread)) {
        if (bytesread == 0) {
          spiSpeedChange(SPI_BAUDRATEPRESCALER_64);
          vs1053b_set_vol(0);
					spiSpeedChange(SPI_BAUDRATEPRESCALER_8);
          break;
        }
        for (blocks = 0; blocks < bytesread;) {
          if (0 == vs1053b_write_music(readbuffer + blocks)) {
            blocks += 32;
          }
        }
		
				spiSpeedChange(SPI_BAUDRATEPRESCALER_64);
				if(fulltime==0)
				fulltime=f_size(&sdfile)/(125*vs1053b_get_HeadInfo());
				
				tempTime=vs1053b_read_reg(SPI_DECODE_TIME);
				spiSpeedChange(SPI_BAUDRATEPRESCALER_8);
				if(decodeTime!=tempTime){
					decodeTime=tempTime;
					show_Mp3(decodeTime,fulltime,volum,ispaused);
				}
				
				if(volum!= tempvol){
					tempvol=volum;
					drawRectangle(60,140,160,20,RGB(0xaa,0xaa,0xaa));
					drawRectangle(60,140,160*volum/250,20,RGB(0xaa,0xaa,0xff));	
				}
				
      } else {
        break;
      }
    }
    f_close(&sdfile);
  } else {
    return 1;	
  }
	return 0;
}

FRESULT searchFile(char *path, stack *filestack, fileNumber *fileNumber) {
  static FILINFO fno;
  int res;
  DIR dir;
  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    while (1) {
      char temppath[40];
      strcpy(temppath, path);
      res = f_readdir(&dir, &fno);

      if (res != FR_OK || fno.fname[0] == 0) break;

      if (fno.fattrib & AM_DIR) {
        if (fno.fname[0] != '.') {
          sprintf(temppath, "%s/%s", path, fno.fname);
          res = searchFile(temppath, filestack, fileNumber);
          if (res != FR_OK) break;
        }
      } else {
        if (fno.fname[strlen(fno.fname) - 1] == '3' &&
            fno.fname[strlen(fno.fname) - 2] == 'P' &&
            fno.fname[strlen(fno.fname) - 3] == 'M' &&
            fno.fname[strlen(fno.fname) - 4] == '.') {
          push(temppath, fno.fname, filestack, 0);
          fileNumber->mp3++;
        } else if (fno.fname[strlen(fno.fname) - 1] == 'T' &&
                   fno.fname[strlen(fno.fname) - 2] == 'X' &&
                   fno.fname[strlen(fno.fname) - 3] == 'T' &&
                   fno.fname[strlen(fno.fname) - 4] == '.') {
          push(temppath, fno.fname, filestack, 1);
          fileNumber->txt++;
        } else if (fno.fname[strlen(fno.fname) - 1] == 'P' &&
                   fno.fname[strlen(fno.fname) - 2] == 'M' &&
                   fno.fname[strlen(fno.fname) - 3] == 'B' &&
                   fno.fname[strlen(fno.fname) - 4] == '.') {
          push(temppath, fno.fname, filestack, 2);
          fileNumber->bmp++;
        }
      }
    }
    f_closedir(&dir);
  }
  return res;
}

void getFile(fileNumber *fileNumber) {
  stack *files = stackcreate();
  fileNumber->mp3 = 0;
  fileNumber->txt = 0;
  fileNumber->bmp = 0;
  static char path[100];
  path[0] = '0';
  path[1] = ':';
  path[2] = 0;
  path[3] = 0;
  int res = searchFile(path, files, fileNumber);
  if (res != FR_OK) {
    return;
  }
}

uint16_t vs1053b_get_HeadInfo(void){
	const uint16_t bitrate[2][16]=
	{ 
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
	{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
	};
	uint16_t HEAD0;
	uint16_t HEAD1;
	HEAD0=vs1053b_read_reg(SPI_HDAT0);
	HEAD1=vs1053b_read_reg(SPI_HDAT1);
	HEAD1>>=3;
  HEAD1=HEAD1&0x03; 
	if(HEAD1==3)	HEAD1=1;
  else HEAD1=0;
  return bitrate[HEAD1][HEAD0>>12];
}

void show_Mp3(uint16_t decodetime,uint16_t fulltime,uint16_t vol,uint8_t ispause){
	drawRectangle(20,180,200,20,RGB(0xaa,0xaa,0xaa));
	drawRectangle(20,180,200*(decodetime)/fulltime,20,RGB(0xaa,0xaa,0xff));	
	char temp[20];
	sprintf(temp,"%d:%02d/%d:%02d",decodetime/60,decodetime%60,fulltime/60,fulltime%60);
	drawEnSentence(120-9*strlen(temp)/2,186,temp,strlen(temp),0x0000);
}
