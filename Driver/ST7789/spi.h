#ifndef __SPI_H_
#define __SPI_H_
#include "debug.h"

#define GPIO_SPI1     GPIOA
#define RCC_SPI1      RCC_APB2Periph_GPIOA

#define SPI1_CS       GPIO_Pin_4
#define SPI1_SCK      GPIO_Pin_5
#define LCD_DC        GPIO_Pin_6
#define SPI1_MOSI     GPIO_Pin_7

																					  
void SPI1_Init(void);			 //³õÊ¼»¯SPI¿Ú
		 
#endif