#ifndef  __TOUCH_CTP_H__
#define  __TOUCH_CTP_H__

#include "debug.h"

/* ================= 硬件 I2C1 版本 ================= */
/*
I2C1 默认引脚：
PB6 -> SCL
PB7 -> SDA
*/

#define CTP_ADDR        0x2a

/* 对外接口 */

void Touch_I2C1_Init(void);

/* 寄存器读写 */
uint8_t CTP_Write_Reg(uint8_t startaddr, uint8_t *pbuf, uint32_t len);
uint8_t CTP_Read_Reg(uint8_t *pbuf, uint32_t len);

/* GUI 接口 */
int GUI_TOUCH_X_MeasureX(void);
int GUI_TOUCH_X_MeasureY(void);

/* 测试函数 */
void Touch_Test(void);
uint32_t touch_get_point(void);

#endif