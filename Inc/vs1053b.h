#ifndef VS1053B_H
#define VS1053B_H

#include "fatfs.h"
#include "string.h"
#include "stm32f4xx_hal.h"

#define XDCS GPIO_PIN_4
#define XCS GPIO_PIN_3
#define REST GPIO_PIN_5
#define DREQ GPIO_PIN_6
#define VS_WRITE_COMMAND 0x02
#define VS_READ_COMMAND 0x03

#define SPI_MODE 0x00
#define SPI_STATUS 0x01
#define SPI_BASS 0x02
#define SPI_CLOCKF 0x03
#define SPI_DECODE_TIME 0x04
#define SPI_AUDATA 0x05
#define SPI_WRAM 0x06
#define SPI_WRAMADDR 0x07
#define SPI_HDAT0 0x08
#define SPI_HDAT1 0x09

#define SPI_AIADDR 0x0a
#define SPI_VOL 0x0b
#define SPI_AICTRL0 0x0c
#define SPI_AICTRL1 0x0d
#define SPI_AICTRL2 0x0e
#define SPI_AICTRL3 0x0f
#define SM_DIFF 0x01
#define SM_JUMP 0x02
#define SM_RESET 0x04
#define SM_OUTOFWAV 0x08
#define SM_PDOWN 0x10
#define SM_TESTS 0x20
#define SM_STREAM 0x40
#define SM_PLUSV 0x80
#define SM_DACT 0x100
#define SM_SDIORD 0x200
#define SM_SDISHARE 0x400
#define SM_SDINEW 0x800
#define SM_ADPCM 0x1000
#define SM_ADPCM_HP 0x2000

typedef struct stack
{
	int nums;
  char path[40];
  char name[20];
} stack;

typedef struct fileNumber
{
	uint16_t mp3;
	uint16_t txt;
	uint16_t bmp;
} fileNumber;

extern void getFile(fileNumber*);
extern FRESULT searchFile(char *, stack *,fileNumber *);
extern void pop(stack *stackhead);
extern stack* getData(stack *stackhead,uint8_t type);
extern void push(char *, char *, stack *stackhead,uint8_t type);
extern int isempty(stack *stackhead);
extern struct stack *stackcreate(void);

extern SPI_HandleTypeDef hspi1;
extern void spiSpeedChange(uint32_t);
extern int vs1053b_reset(void);
extern void vs1053b_write_pin(uint16_t, int);
extern int vs1053b_read_dreq(void);
extern void vs1053b_sine_test(void);
extern void vs1053b_write_cmd(uint8_t address, uint16_t data16b);
extern void vs1053b_write_data(uint8_t data);
extern int vs1053b_write_music(uint8_t *);
extern uint16_t vs1053b_read_reg(uint8_t address);
extern uint16_t vs1053b_read_ram(uint16_t address);
extern void vs1053b_soft_reset(void);
extern void vs1053b_set_vol(uint8_t volx);
extern uint8_t vs1053b_play_music(char *);
extern uint16_t vs1053b_get_HeadInfo(void);
extern void show_Mp3(uint16_t decodetime,uint16_t fulltime,uint16_t vol,uint8_t ispause);

#endif
