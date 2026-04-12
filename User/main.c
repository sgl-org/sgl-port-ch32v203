/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "debug.h"
#include "sgl.h"
#include "components/sgl_timer.h"
#include "LCD.h"
#include "touch_CTP.h"
/* Global typedef */

/* Global define */

/* Global Variable */
static sgl_color_t panel_buffer0[LCD_W*10],panel_buffer1[LCD_W*10];

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM2_IRQHandler(void){
    if(likely(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)){
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        sgl_tick_inc(1);
    }
}

void TIM2_Init(uint16_t arr,uint16_t psc){
    NVIC_InitTypeDef NVIC_InitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

    NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2,ENABLE);
}

void TIM3_Init(uint16_t arr,uint16_t psc){
    NVIC_InitTypeDef NVIC_InitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

    NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3,ENABLE);
}

void DMA1_Channel3_IRQHandler(void){
    if(likely(DMA_GetITStatus(DMA1_IT_TC3)!=RESET)){
        DMA_Cmd(DMA1_Channel3,DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC3);
        SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,DISABLE);
        while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET);
        // SPI_I2S_ReceiveData(SPI1);

        // LCD_CS_Set();
        sgl_fbdev_flush_ready();
    }
}

void panel_flush_area(sgl_area_t *area,sgl_color_t *src){
    uint16_t width=area->x2-area->x1+1;
    uint16_t height=area->y2-area->y1+1;
    // printf("area:\r\n");
    SPI_CS(0);
    BlockWrite(area->x1,area->x2,area->y1,area->y2);
    LCD_RS(1);
    LCD_SPI_Write_DMA((uint8_t*)src,width*height*sizeof(sgl_color_t));
}
void TIM3_IRQHandler(void){
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        uint8_t pressed=0;
        uint8_t buf[16];
        u16 touchX=0,touchY=0;

        CTP_Read_Reg((uint8_t*)&buf, 8);

        if((buf[3]==0x80) && (buf[4]>1)){
            pressed=1;
            touchX=buf[4];
            touchY=buf[6];
        }

        sgl_event_pos_input(touchX,touchY,pressed);
    }
}

void SDI_puts(const char *str){
    printf("%s",str);
}

void setup_ui(void);

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    SDI_Printf_Enable();
    printf("SystemClk:%dMHz\r\n",SystemCoreClock/1000000);

    Lcd_Initialize();
    Touch_I2C1_Init();
    TIM2_Init(100-1,1440-1);
    TIM3_Init(1000-1,1440-1);
    sgl_fbinfo_t fbinfo={
        .xres=LCD_W,
        .yres=LCD_H,
        .flush_area=panel_flush_area,
        .buffer[0]=panel_buffer0,
        .buffer[1]=panel_buffer1,
        .buffer_size=SGL_ARRAY_SIZE(panel_buffer0)
    };
    sgl_logdev_register(SDI_puts);
    sgl_fbdev_register(&fbinfo);
    sgl_init();
    setup_ui();
    
    sgl_mm_monitor_t mn=sgl_mm_get_monitor();
    uint8_t inte=mn.used_rate>>8,deci=mn.used_rate&0xff;
    printf("SGL Heap Stat\r\n");
    printf("Tot %u byte,\tUsed %u byte,\tRate %u.%02u %%\r\n",mn.total_size,mn.used_size,inte,deci);
    while(1){
        sgl_task_handler();
        sgl_timer_handler();
        // Touch_Test();
        // Delay_Ms(10);
    }
}
sgl_obj_t *slid0;
sgl_obj_t *g_pie,*pie;
sgl_timer_t *tim_mem;
sgl_anim_t *g_pie_anim,*anim_slid0;
/* 折线图演示：固定显示的点个数 */
#define LINECHART_POINT_NUM 6
/* 折线图对象指针 */
static sgl_obj_t *g_linechart = NULL;
/* 温度数据数组（整数表示，单位可以自定义理解） */
static int32_t    g_line_temp_buf[LINECHART_POINT_NUM];
/* 湿度数据数组（整数表示，单位可以自定义理解） */
static int32_t    g_line_humi_buf[LINECHART_POINT_NUM];
/* 折线图数据更新用的相位（用于生成演示用的波形） */
static float      g_line_phase = 0.0f;
void anim_slid_cb(sgl_anim_t *anim,int32_t value){
    SGL_ASSERT(unlikely(anim!=NULL&&anim->data!=NULL));
    sgl_slider_set_value(anim->data,value);
}
void anim_slid_finish_cb(sgl_anim_t *anim){
	SGL_ASSERT(unlikely(anim!=NULL&&anim->data!=NULL));
	
	int32_t temp;
	temp = anim->start_value;
	anim->start_value = anim->end_value;
	anim->end_value = temp;
}
static void piechart_anim_path(sgl_anim_t *anim, int32_t value){
    sgl_obj_t *pie = (sgl_obj_t *)anim->data;
    if (!pie) return;

    /* 将动画值映射到 10~60 之间，用作 slice0 的数值 */
    const int32_t min_v = 10;
    const int32_t max_v = 60;
    int32_t v = min_v + (value * (max_v - min_v) / 1000);
    if (v < 0) v = 0;

    sgl_piechart_set_slice_value(pie, 0, v);
}

/* 饼图动画完成回调：来回往返（10->60->10->...） */
static void piechart_anim_finished(sgl_anim_t *anim){
    int32_t tmp = anim->start_value;
    anim->start_value = anim->end_value;
    anim->end_value   = tmp;
}
// void tim_mem_cb(const sgl_timer_t *timer, void *user_data){
//     sgl_mm_monitor_t mn=sgl_mm_get_monitor();

//     uint8_t inte=mn.used_rate>>8,deci=mn.used_rate&0xff;
//     printf("SGL Heap Stat\r\n");
//     printf("Tot %u byte,\tUsed %u byte,\tRate %u.%02u %%\r\n",mn.total_size,mn.used_size,inte,deci);

//     sgl_piechart_set_slice_value(pie,1,sgl_slider_get_value(slid0)>>2);
// }

void setup_ui(void){
    // tim_mem=sgl_timer_create();
    // sgl_timer_setup(tim_mem,tim_mem_cb,500,-1,NULL);

    // slid0=sgl_slider_create(NULL);
    // sgl_obj_set_pos(slid0, 30, 30);
    // sgl_obj_set_size(slid0, 180, 30);
    // sgl_slider_set_radius(slid0,8);
    // sgl_slider_set_knob_color(slid0,sgl_rgb(0x30, 0x44, 0xDF));
    // sgl_slider_set_track_color(slid0,sgl_rgb(0x61,0x61,0x61));
    // sgl_slider_set_fill_color(slid0,sgl_rgb(0x32,0x34,0xB6));
    
    // anim_slid0=sgl_anim_create();
    // sgl_anim_set_data(anim_slid0,slid0);
    // sgl_anim_set_path(anim_slid0,anim_slid_cb,SGL_ANIM_PATH_LINEAR);
    // sgl_anim_set_start_value(anim_slid0,0);				// 起始值
    // sgl_anim_set_end_value(anim_slid0,100);					// 结束值
    // sgl_anim_set_act_delay(anim_slid0,0);						// 动画无延迟
    // sgl_anim_set_act_duration(anim_slid0,3000);
    // sgl_anim_set_finish_cb(anim_slid0,anim_slid_finish_cb);			// 绑定完成回调
	// sgl_anim_set_auto_free(anim_slid0);
    // sgl_anim_start(anim_slid0,SGL_ANIM_REPEAT_LOOP);

    pie = sgl_piechart_create(NULL);
    if(pie){
        sgl_obj_set_pos(pie, 20, 5);
        sgl_obj_set_size(pie, 180, 80);
        sgl_piechart_set_alpha(pie, 255);
        sgl_piechart_set_smooth(pie, 0);
        sgl_piechart_set_radius(pie, 30);
        //sgl_piechart_set_inner_radius_rate(pie, 30);
        sgl_piechart_set_slice_count(pie, 4);
        sgl_piechart_set_slice(pie, 0, 40, sgl_rgb(0xFF, 0x63, 0x84), "Slice A");
        sgl_piechart_set_slice(pie, 1, 25, sgl_rgb(0x36, 0xA2, 0xEB), "Slice B");
        sgl_piechart_set_slice(pie, 2, 20, sgl_rgb(0xFF, 0xC1, 0x07), "Slice C");
        sgl_piechart_set_slice(pie, 3, 15, sgl_rgb(0x4B, 0xC0, 0x81), "Slice D");
        sgl_piechart_enable_legend(pie, true);
        sgl_piechart_set_legend_pos(pie, SGL_PIECHART_LEGEND_POS_RIGHT);
        sgl_piechart_set_legend_dir(pie, SGL_PIECHART_LEGEND_DIR_VERTICAL);
        sgl_piechart_set_legend_font(pie, &consolas14);
        sgl_piechart_enable_open_anim(pie, true); 
        sgl_piechart_set_open_anim_path(pie, SGL_ANIM_PATH_EASE_IN_OUT);
        /* 记录全局饼图对象，方便动画回调里使用 */

        g_pie = pie;

        /* 创建饼图扇形大小动画：在 10~60 之间来回变化 slice 0 的值 */
        g_pie_anim = sgl_anim_create();
        if (g_pie_anim) {
            sgl_anim_set_data(g_pie_anim, g_pie);
            sgl_anim_set_start_value(g_pie_anim, 10);
            sgl_anim_set_end_value(g_pie_anim, 900);
            sgl_anim_set_act_duration(g_pie_anim, 4000);
            sgl_anim_set_path(g_pie_anim, piechart_anim_path, SGL_ANIM_PATH_EASE_IN);
            sgl_anim_set_finish_cb(g_pie_anim, piechart_anim_finished);
            sgl_anim_start(g_pie_anim, SGL_ANIM_REPEAT_LOOP);
        }
    }
    /* 折线图演示：初始化温度和湿度数组（固定 6 个点） */
    for (uint16_t i = 0; i < LINECHART_POINT_NUM; i++) {
        /* 每个温度点设为 23+i，方便观察变化 */
        g_line_temp_buf[i] = 23 + i;
        /* 每个湿度点设为 40+i，方便观察变化 */
        g_line_humi_buf[i] = 40 + i;
    }

    /* 为 X 轴准备 6 个标签字符串，这里仅作为示例 */
    static const char *line_x_labels[LINECHART_POINT_NUM] = {
        "P1", "P2", "P3", "P4", "P5", "P6",
    };

    /* 创建折线图控件 */
    sgl_obj_t *linechart = sgl_linechart_create(NULL);
    if (linechart) {
        /* 保存折线图对象指针到全局变量，方便在主循环中更新 */
        g_linechart = linechart;
        /* 设置折线图左上角位置 */
        sgl_obj_set_pos(linechart, 10, 80);
        /* 设置折线图宽高 */
        sgl_obj_set_size(linechart, 220, 155);

        /* 设置折线图背景颜色 */
        sgl_linechart_set_bg_color(linechart, sgl_rgb(255, 255, 255));
        /* 设置折线图背景透明度 */
        sgl_linechart_set_bg_alpha(linechart, 255);
        /* 设置折线图边框颜色 */
        sgl_linechart_set_border_color(linechart, sgl_rgb(255, 255, 255));
        sgl_linechart_enable_axis_ticks(linechart, SGL_LINECHART_AXIS_X, true);
        sgl_linechart_enable_axis_ticks(linechart, SGL_LINECHART_AXIS_Y, true);
        /* 设置 X 轴范围为 0~(LINECHART_POINT_NUM-1)，步长为 1 */
        sgl_linechart_set_axis_range(linechart, SGL_LINECHART_AXIS_X, 0, LINECHART_POINT_NUM - 1);
        /* 设置 X 轴刻度步长为 1 */
        sgl_linechart_set_axis_step(linechart, SGL_LINECHART_AXIS_X, 1);
        /* 关闭 X 轴自动缩放，使用固定范围 */
        sgl_linechart_enable_axis_auto_scale(linechart, SGL_LINECHART_AXIS_X, false);
        /* 打开 X 轴标签显示 */
        sgl_linechart_enable_axis_labels(linechart, SGL_LINECHART_AXIS_X, true);

        /* 开启 Y 轴自动缩放，让 Y 范围根据数据自动调整 */
        sgl_linechart_enable_axis_auto_scale(linechart, SGL_LINECHART_AXIS_Y, true);
        /* 设置 Y 轴自动刻度大约分成 4 段 */
        sgl_linechart_set_axis_auto_divisions(linechart, SGL_LINECHART_AXIS_Y, 4);
        /* 打开 Y 轴标签显示 */
        sgl_linechart_enable_axis_labels(linechart, SGL_LINECHART_AXIS_Y, true);

        /* 打开 X 轴和 Y 轴网格线 */
        sgl_linechart_enable_axis_grid(linechart, SGL_LINECHART_AXIS_X, true);
        sgl_linechart_enable_axis_grid(linechart, SGL_LINECHART_AXIS_Y, true);
        /* 把网格线设置为虚线 */
        sgl_linechart_set_axis_grid_style(linechart, SGL_LINECHART_AXIS_X, 1);
        sgl_linechart_set_axis_grid_style(linechart, SGL_LINECHART_AXIS_Y, 1);
        /* 设置网格线颜色和透明度 */
        sgl_linechart_set_axis_grid_color(linechart, SGL_LINECHART_AXIS_X, sgl_rgb(60, 60, 60), 80);
        sgl_linechart_set_axis_grid_color(linechart, SGL_LINECHART_AXIS_Y, sgl_rgb(60, 60, 60), 80);

        /* 设置 X 轴和 Y 轴标签字体 */
        sgl_linechart_set_axis_label_font(linechart, SGL_LINECHART_AXIS_X, &consolas14);
        sgl_linechart_set_axis_label_font(linechart, SGL_LINECHART_AXIS_Y, &consolas14);
        /* 设置 X 轴和 Y 轴标签字体颜色和透明度 */
        sgl_linechart_set_axis_label_color(linechart, SGL_LINECHART_AXIS_X, sgl_rgb(0,0,0), 255);
        sgl_linechart_set_axis_label_color(linechart, SGL_LINECHART_AXIS_Y, sgl_rgb(0,0,0), 255);

        /* 设置 X 轴标签字符串数组，这里使用 "P1"~"P6" 作为示例 */
        sgl_linechart_set_x_labels(linechart, line_x_labels, LINECHART_POINT_NUM);

        /* 配置折线图有两条曲线：温度和湿度 */
        sgl_linechart_set_series_count(linechart, 2);

        /* 绑定温度曲线，只传 Y 数据数组，X 自动使用 0~5 */
        sgl_linechart_set_series_y_array(linechart, 0, g_line_temp_buf, LINECHART_POINT_NUM);
        /* 设置温度曲线为“折线+圆形点”的组合 */
        sgl_linechart_set_series_mode(linechart, 0, SGL_LINECHART_SERIES_MODE_LINE_AND_POINT);
        /* 设置温度曲线颜色为蓝色 */
        sgl_linechart_set_series_line_color(linechart, 0, sgl_rgb(0, 122, 255));
        /* 设置温度曲线线宽为 2（逻辑宽度） */
        sgl_linechart_set_series_line_width(linechart, 0, 2);
        /* 设置温度曲线的点样式为圆形，半径为 3 像素 */
        sgl_linechart_set_series_point_style(linechart, 0, SGL_LINECHART_POINT_SHAPE_CIRCLE, 3);
        /* 打开温度曲线下方的填充区域 */
        sgl_linechart_enable_series_fill(linechart, 0, true);
        /* 设置温度曲线下方填充颜色和透明度 */
        sgl_linechart_set_series_fill_color(linechart, 0, sgl_rgb(0, 122, 255), 60);

        /* 绑定湿度曲线，只传 Y 数据数组，X 自动使用 0~5 */
        sgl_linechart_set_series_y_array(linechart, 1, g_line_humi_buf, LINECHART_POINT_NUM);
        /* 设置湿度曲线为“折线+方形点”的组合 */
        sgl_linechart_set_series_mode(linechart, 1, SGL_LINECHART_SERIES_MODE_LINE_AND_POINT);
        /* 设置湿度曲线颜色为红色 */
        sgl_linechart_set_series_line_color(linechart, 1, sgl_rgb(255, 64, 64));
        /* 设置湿度曲线线宽为 2（逻辑宽度） */
        sgl_linechart_set_series_line_width(linechart, 1, 2);
        /* 设置湿度曲线的点样式为方形，半径为 3 像素 */
        sgl_linechart_set_series_point_style(linechart, 1, SGL_LINECHART_POINT_SHAPE_SQUARE, 3);
        /* 打开湿度曲线下方的填充区域 */
        sgl_linechart_enable_series_fill(linechart, 1, false);
        /* 设置湿度曲线下方填充颜色和透明度 */
        sgl_linechart_set_series_fill_color(linechart, 1, sgl_rgb(255, 64, 64), 50);

        /* 设置折线图整体透明度为不透明 */
        sgl_linechart_set_alpha(linechart, 255);
        /* 打开折线图载入时的渐显动画 */
        sgl_linechart_enable_open_anim(linechart, true);
        /* 设置折线图动画方向为从左向右进入 */
        sgl_linechart_set_open_anim_dir(linechart, SGL_LINECHART_OPEN_ANIM_FROM_TOP);
        /* 使用缓动函数控制折线图打开动画的进度，使动画效果更平滑 */
        sgl_linechart_set_open_anim_path(linechart, SGL_ANIM_PATH_EASE_IN_OUT);
    }
}