#include "touch_CTP.h"
#include "LCD.h"

void Touch_I2C1_Init(void){
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef   I2C_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // PB6 SCL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB7 SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C1);

    I2C_InitStructure.I2C_ClockSpeed = 400000;     // 400kHz Fast Mode
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}
void GUI_TOUCH_X_ActivateX(void) {}
void GUI_TOUCH_X_ActivateY(void) {}

uint8_t CTP_Read_Reg(uint8_t *pbuf, uint32_t len){
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    // ===== 写寄存器地址 =====
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, CTP_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
    I2C_SendData(I2C1, 0x00);        // 起始寄存器
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // ===== 重新启动读 =====
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, CTP_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while(len){
        if(len == 1){
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){
            *pbuf++ = I2C_ReceiveData(I2C1);
            len--;
        }
    }
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return 0;
}


void Touch_Test(void){
	// char i;
	u8 buf[16];
	u16 touchX=0,touchY=0;
	CTP_Read_Reg((uint8_t*)&buf, 8);//一般小屏读一个点就够了，节省资源
	if((buf[3]==0x80) && (buf[4]>1)){
		touchX = buf[4] ;//x坐标
		touchY = buf[6];//y坐标
		Lcd_ColorBox(touchX,touchY,4,4,0xf800);
		// printf("touch %d,%d\r\n",touchX,touchY);
	}
}
uint32_t touch_get_point(void){
	u8 buf[16];
	u16 touchX=0,touchY=0;
	CTP_Read_Reg((uint8_t*)&buf, 8);//一般小屏读一个点就够了，节省资源
	if((buf[3]==0x80) && (buf[4]>1)){
		touchX = buf[4] ;//x坐标
		touchY = buf[6];//y坐标
		// printf("touch %d,%d\r\n",touchX,touchY);
	}
	return ((uint32_t)touchX<<16)+touchY;
}