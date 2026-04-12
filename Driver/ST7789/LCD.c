#include "debug.h"
#include "LCD.h"
#include "spi.h"
// #include <stdio.h>
// #include "ASCII.h"
// #include "GB1616.h"	//16*16汉字字模


void LCD_delay(int time){Delay_Ms(time);}

void LCD_SPI_Write(uint8_t data){
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
    SPI_I2S_SendData(SPI1,data);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET);
}

void LCD_SPI_Write_DMA(uint8_t *data,uint16_t len){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET)
        ;
    DMA_Cmd(DMA1_Channel3,DISABLE);//SPI1_DMA_TX
    DMA_ClearFlag(DMA1_FLAG_TC3);//SPI1_DMA_TC_FLAG
    DMA1_Channel3->MADDR = (uint32_t)data;
    DMA1_Channel3->CNTR  = len;

    SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
    DMA_Cmd(DMA1_Channel3,ENABLE);//SPI1_DMA_TX
}

static void WriteComm(u8 cmd){
    LCD_RS(0);
    LCD_SPI_Write(cmd);
}

static void WriteData(u8 data){
    LCD_RS(1);
    LCD_SPI_Write(data);
}

/**********************************************
Lcd初始化函数
***********************************************/
void Lcd_Initialize(void){	
	SPI1_Init();
	SPI_CS(1);
	SPI_CS(0);	
	LCD_delay(100);//等待复位完成（注意：如果你的单片机启动后，没什么事干就直接初始化液晶，要加点延时，
					//你也可以先初始化其它外设，例如串口等，打印点什么消耗时间，以此等待液晶硬件复位完成）
		
	WriteComm(0x11); 
	LCD_delay(120);                //LCD_delay 120ms 
	
	WriteComm(0xb2); 
	WriteComm(0x0c); 
	WriteData(0x0c); 
	WriteData(0x00); 
	WriteData(0x33); 
	WriteData(0x33); 
	WriteComm(0xb7); 
	WriteData(0x35);  
	WriteComm(0xbb); 
	WriteData(0x20); 
	WriteComm(0xc0); 
	WriteData(0x2c); 
	WriteComm(0xc2); 
	WriteData(0x01); 
	WriteComm(0xc3); 
	WriteData(0x0b); 
	WriteComm(0xc4); 
	WriteData(0x20); 
	WriteComm(0xc6); 
	WriteData(0x0f); 
	WriteComm(0xca); 
	WriteData(0x0f); 
	WriteComm(0xc8); 
	WriteData(0x08); 
	WriteComm(0x55); 
	WriteData(0x90); 
	WriteComm(0xd0); 
	WriteData(0xa4); 
	WriteData(0xa1); 
	WriteComm(0xe0); 
	WriteData(0xd0); 
	WriteData(0x00); 
	WriteData(0x03); 
	WriteData(0x08); 
	WriteData(0x0a); 
	WriteData(0x17); 
	WriteData(0x2e); 
	WriteData(0x44); 
	WriteData(0x3f); 
	WriteData(0x29); 
	WriteData(0x10); 
	WriteData(0x0e); 
	WriteData(0x14); 
	WriteData(0x18); 
	WriteComm(0xe1); 
	WriteData(0xd0); 
	WriteData(0x00); 
	WriteData(0x03); 
	WriteData(0x08); 
	WriteData(0x07); 
	WriteData(0x27); 
	WriteData(0x2b); 
	WriteData(0x44); 
	WriteData(0x41); 
	WriteData(0x3c); 
	WriteData(0x1b); 
	WriteData(0x1d); 
	WriteData(0x14); 
	WriteData(0x18); 
	WriteComm(0x36);
	WriteData(0x48);
	WriteComm(0x3a);
	WriteData(0x55);
	WriteComm(0x29);

	WriteComm(0x2a); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0xEF); 

	WriteComm(0x2b); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0xEF); 

	WriteComm(0x2c);

	// display on
	WriteComm(0x29);  	// SLPOUT

	LCD_delay(1);

}
/**********************************************
函数名：开窗函数(操作窗口)

入口参数：XStart x方向的起点
          Xend   x方向的终点
					YStart y方向的起点
          Yend   y方向的终点

这个函数的意义是：开一个矩形框，方便接下来往这个框填充数据
注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角
***********************************************/
/******************************************
函数名：Lcd图像填充
功能：向Lcd指定位置填充图像
入口参数：
					(x,y): 图片左上角起始坐标
					(pic_H,pic_V): 图片的宽高
					 pic  指向存储图片数组的指针
******************************************/
// void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
// {
//   unsigned long i;

// 	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
// 	LCD_RS(1);
// 	for (i = 0; i < pic_H*pic_V*2; i++)
// 	{
// 		SPI_I2S_SendData(SPI1, pic[i]);
// 	}	
	
//	for (i = 0; i < pic_H*pic_V*2; i+=2)//把16位图变成24位图，也可以直接取模成24位图
//	{
//		SPI_I2S_SendData(SPI1, pic[i]|0X7);
//		SPI_I2S_SendData(SPI1, (pic[i]<<5)|(pic[i+1]>>3)|0X3);
//		SPI_I2S_SendData(SPI1, (pic[i+1]<<3)|0X7);
//	}
	
//	for (i = 0; i < pic_H*pic_V*3; i++)//如果直接取模成24位图，则程序改以下以形式
//	{
//		SPI_I2S_SendData(SPI1, pic[i]);
//	}
// }
//=============== 在x，y 坐标上打一个颜色为Color的点 ===============//
void BlockWrite(u16 xs,u16 xe,u16 ys,u16 ye){
    WriteComm(0x2a);
    WriteData(xs>>8);WriteData(xs);
    WriteData(xe>>8);WriteData(xe);

    WriteComm(0x2b);
    WriteData(ys>>8);WriteData(ys);
    WriteData(ye>>8);WriteData(ye);

    WriteComm(0x2c);
}

void Lcd_ColorBox(u16 xs,u16 ys,u16 xl,u16 yl,u16 c){
    u32 i;
    BlockWrite(xs,xs+xl-1,ys,ys+yl-1);
    LCD_RS(1);
    for(i=0;i<xl*yl;i++){
        LCD_SPI_Write(c>>8);
		LCD_SPI_Write(c);
    }
}

void DrawPixel(u16 x,u16 y,u16 c){
    BlockWrite(x,x,y,y);
    LCD_RS(1);
    LCD_SPI_Write(c>>8);
	LCD_SPI_Write(c);
}

void LCD_draw_HLine(u16 x0,u16 x1,u16 y,u32 c){
    u16 i;
    BlockWrite(x0,x1,y,y);
    LCD_RS(1);
    for(i=0;i<=x1-x0;i++){
        SPI_I2S_SendData(SPI1,c>>8);SPI_WAIT();
        SPI_I2S_SendData(SPI1,c);SPI_WAIT();
    }
}

void LCD_draw_VLine(u16 x,u16 y0,u16 y1,u32 c){
    u16 i;
    BlockWrite(x,x,y0,y1);
    LCD_RS(1);
    for(i=0;i<=y1-y0;i++){
        SPI_I2S_SendData(SPI1,c>>8);SPI_WAIT();
        SPI_I2S_SendData(SPI1,c);SPI_WAIT();
    }
}
/**********电池电量显示**********
x,y :      电池显示位置坐标
power_per: 电池显示的百分比，范围为0~4
*********************************/
// void draw_Bat(u16 x,u16 y,u8 power_per)
// {
// 	u8 i,j;
// 	LCD_draw_VLine(x,y,y+5,BLACK);
// 	LCD_draw_VLine(x,y+5+10,y+5+10+5,BLACK);
	
// 	LCD_draw_HLine(x,x+28,y,BLACK);
// 	LCD_draw_HLine(x,x+28,y+20,BLACK);
	
// 	LCD_draw_VLine(x+28,y,y+5,BLACK);
// 	LCD_draw_VLine(x+28,y+5+10,y+5+10+5,BLACK);
	
// 	LCD_draw_HLine(x+28,x+28+3,y+5,BLACK);
// 	LCD_draw_HLine(x+28,x+28+3,y+5+10,BLACK);
	
// 	LCD_draw_VLine(x+28+3,y+5,y+5+10,BLACK);
	
// 	for(j=0;j<power_per;j++)
// 	{
// 		BlockWrite(x+3,x+3+4,y+3,y+3+14);
// 		LCD_RS(1);
// 		for(i=0;i<6*15;i++)
// 			{
// 				SPI_I2S_SendData(SPI1, BLACK>>8);
// 				SPI_I2S_SendData(SPI1, BLACK);				
// 			}
// 		x=x+6;
// 	}
// }
// /**********8*16字体 ASCII码 显示*************
// (x,y): 显示字母的起始坐标
// num:   要显示的字符:" "--->"~"
// fColor 前景色
// bColor 背景色
// flag:  有背景色(1)无背景色(0)
// *********************************************/
// void SPILCD_ShowChar(unsigned short x,unsigned short y,unsigned char num, unsigned int fColor, unsigned int bColor,unsigned char flag) 
// {       
// 	unsigned char temp;
// 	unsigned int pos,i,j;  

// 	num=num-' ';//得到偏移后的值
// 	i=num*16; 	
// 	for(pos=0;pos<16;pos++)
// 		{
// 			temp=nAsciiDot[i+pos];	//调通调用ASCII字体
// 			for(j=0;j<8;j++)
// 		   {                 
// 		        if(temp&0x80)
// 							DrawPixel(x+j,y,fColor);
// 						else 
// 							if(flag) DrawPixel(x+j,y,bColor); //如果背景色标志flag为1
// 							temp<<=1; 
// 		    }
// 			 y++;
// 		}		 
// }  

/**********写一个16x16的汉字*****************
(x,y): 显示汉字的起始坐标
c[2]:  要显示的汉字
fColor 前景色
bColor 背景色
flag:  有背景色(1)无背景色(0)
*********************************************/
// void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor,unsigned char flag)
// {
// 	unsigned int i,j,k;
// 	unsigned short m;
// 	for (k=0;k<64;k++) { //64标示自建汉字库中的个数，循环查询内码
// 	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
// 			{ 
//     	for(i=0;i<32;i++) 
// 			{
// 				m=codeGB_16[k].Msk[i];
// 				for(j=0;j<8;j++) 
// 				{		
// 					if((m&0x80)==0x80) {
// 						DrawPixel(x+j,y,fColor);
// 						}
// 					else {
// 						if(flag) DrawPixel(x+j,y,bColor);
// 						}
// 					m=m<<1;
// 				} 
// 				if(i%2){y++;x=x-8;}
// 				else x=x+8;
// 		  }
// 		}  
// 	  }	
// 	}
/**********显示一串字*****************
(x,y): 字符串的起始坐标
*s:    要显示的字符串指针
fColor 前景色
bColor 背景色
flag:  有背景色(1)无背景色(0)
*********************************************/
// void LCD_PutString(unsigned short x, unsigned short y, char *s, unsigned int fColor, unsigned int bColor,unsigned char flag) 
// 	{
// 		unsigned char l=0;
// 		while(*s) 
// 			{
// 				if( (unsigned char)*s < 0x80) 
// 						{
// 							SPILCD_ShowChar(x+l*8,y,*s,fColor,bColor,flag);
// 							s++;l++;
// 						}
// 				// else
// 				// 		{
// 				// 			PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor,flag);
// 				// 			s+=2;l+=2;
// 				// 		}
// 			}
// 	}