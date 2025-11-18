//
// Created by 31910 on 2025/5/11.
//

#ifndef UNTITLED4_TICKER_H
#define UNTITLED4_TICKER_H

#include "stm32f10x.h"

/* 配置使用哪个定时器作为时基 */
#define TICK_TIMER            TIM2
#define TICK_TIMER_CLK        RCC_APB1Periph_TIM2
#define TICK_TIMER_IRQn       TIM2_IRQn
#define TICK_TIMER_IRQHandler TIM2_IRQHandler

// 初始化与滴答获取接口
void Ticker_Init(void);
uint32_t GetTick(void);

// 新增时间管理函数
void Delay_MS(uint32_t ms);
uint8_t Is_Timeout(uint32_t startTick, uint32_t timeout);
uint32_t Get_Time_Diff(uint32_t start, uint32_t end);

// UI组件适配接口
uint8_t UI_Timer_Check(void* component, uint32_t interval);

#endif // __TICKER_H__