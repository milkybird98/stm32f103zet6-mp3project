#ifndef LCDILI9341_H
#define LCDILI9341_H

#include "stdlib.h"
#include "math.h"
#include "ctype.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "string.h"

#define ILI9341_TFTWIDTH   240
#define ILI9341_TFTHEIGHT  320

#define ILI9341_NOP        0x00
#define ILI9341_SWRESET    0x01
#define ILI9341_RDDID      0x04
#define ILI9341_RDDST      0x09

#define ILI9341_SLPIN      0x10
#define ILI9341_SLPOUT     0x11
#define ILI9341_PTLON      0x12
#define ILI9341_NORON      0x13

#define ILI9341_RDMODE     0x0A
#define ILI9341_RDMADCTL   0x0B
#define ILI9341_RDPIXFMT   0x0C
#define ILI9341_RDIMGFMT   0x0D
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF     0x20
#define ILI9341_INVON      0x21
#define ILI9341_GAMMASET   0x26
#define ILI9341_DISPOFF    0x28
#define ILI9341_DISPON     0x29

#define ILI9341_CASET      0x2A
#define ILI9341_PASET      0x2B
#define ILI9341_RAMWR      0x2C
#define ILI9341_RAMRD      0x2E

#define ILI9341_PTLAR      0x30
#define ILI9341_MADCTL     0x36
#define ILI9341_VSCRSADD   0x37
#define ILI9341_PIXFMT     0x3A

#define ILI9341_FRMCTR1    0xB1
#define ILI9341_FRMCTR2    0xB2
#define ILI9341_FRMCTR3    0xB3
#define ILI9341_INVCTR     0xB4
#define ILI9341_DFUNCTR    0xB6

#define ILI9341_PWCTR1     0xC0
#define ILI9341_PWCTR2     0xC1
#define ILI9341_PWCTR3     0xC2
#define ILI9341_PWCTR4     0xC3
#define ILI9341_PWCTR5     0xC4
#define ILI9341_VMCTR1     0xC5
#define ILI9341_VMCTR2     0xC7

#define ILI9341_RDID1      0xDA
#define ILI9341_RDID2      0xDB
#define ILI9341_RDID3      0xDC
#define ILI9341_RDID4      0xDD

#define ILI9341_GMCTRP1    0xE0
#define ILI9341_GMCTRN1    0xE1

#define RAND_SEED 151

typedef struct BITMAP_HEADER{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
	  uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
}BITMAP_HEADER;

extern SPI_HandleTypeDef hspi2;

extern void lcdInit(void);
extern void lcdRestart(void);
extern void writeCommand(uint8_t cmd);
extern void spiWrite(uint8_t data);
extern void spiWrite16(uint16_t data);
extern void drawCnWord(uint16_t x, uint16_t y, char *character, uint16_t color, uint16_t backcolor, FIL file_ogj);
extern void drawCnSentence(uint16_t x,uint16_t y,char *character,uint8_t length,uint16_t color, uint16_t backcolor);
extern void drawEnWord(uint16_t x, uint16_t y, char character, uint16_t color);
extern void drawEnSentence(uint16_t x,uint16_t y,char *character,uint8_t length,uint16_t color);
extern void drawRectangle(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
extern void drawPoint(uint16_t x,uint16_t y,uint16_t color);
extern void drawLine(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,uint16_t color);
extern void drawCircul(uint16_t x,uint16_t y,uint16_t r,uint16_t color);
extern void drawPictu(uint16_t x,uint16_t y,const char *path);

extern uint16_t RGB(uint8_t r, uint8_t g, uint8_t b);
extern uint16_t getrRGB(void);

#endif
