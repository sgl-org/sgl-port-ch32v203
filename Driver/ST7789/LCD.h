#include "debug.h"

#define LCD_W 240
#define LCD_H 240

#define SPI_CS(a)	\
						if (a)	\
						GPIO_SetBits(GPIOA,GPIO_Pin_4);	\
						else		\
						GPIO_ResetBits(GPIOA,GPIO_Pin_4)

#define LCD_RS(a)	\
						if (a)	\
						GPIO_SetBits(GPIOA,GPIO_Pin_6);	\
						else		\
						GPIO_ResetBits(GPIOA,GPIO_Pin_6)

// #define Lcd_Light_ON   GPIOA->BSRR = GPIO_Pin_2;
// #define Lcd_Light_OFF  GPIOA->BRR  = GPIO_Pin_2;

//*************  24位色（1600万色）定义 *************//
//#define WHITE          0xFFFFFF
//#define BLACK          0x000000
//#define BLUE           0x0000FF
//#define BLUE2          0x3F3FFF
//#define RED            0xFF0000
//#define MAGENTA        0xFF00FF
//#define GREEN          0x00FF00
//#define CYAN           0x00FFFF
//#define YELLOW         0xFFFF00

//*************  18位色（26万色）定义 *************//
//#define White          0x3FFFF
//#define Black          0x00000
//#define Blue           0x3F000
//#define Blue2          0x3F3CF
//#define Red            0x0003F
//#define Magenta        0x3F03F
//#define Green          0x0FFC0
//#define Cyan           0x3FFC0
//#define Yellow         0x0FFFF						

////*************  16位色定义 *************//
#define WHITE						0xFFFF
#define BLACK						0x0000	  
#define BLUE						0x001F  
#define BRED						0XF81F
#define GRED						0XFFE0
#define GBLUE						0X07FF
#define RED							0xF800
#define MAGENTA					0xF81F
#define GREEN						0x07E0
#define CYAN						0x7FFF
#define YELLOW					0xFFE0
#define BROWN						0XBC40 //棕色
#define BRRED						0XFC07 //棕红色
#define GRAY						0X8430 //灰色

//Lcd初始化及其低级控制函数
void LCD_delay(int time);
void LCD_SPI_Write_DMA(uint8_t *data,uint16_t len);
void Lcd_Initialize(void);
//Lcd高级控制函数
void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);
void DrawPixel(u16 x, u16 y, u16 Color);
void BlockWrite(u16 xs,u16 xe,u16 ys,u16 ye);