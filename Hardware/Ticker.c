#include "ticker.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

/* 定义全局变量存储系统时钟滴答数 */
static __IO uint32_t uwTick = 0;

/**
  * @brief  初始化定时器作为系统时基（保持不变）
  */
void Ticker_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(TICK_TIMER_CLK, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;    // 1ms中断一次
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;   // 72MHz/72=1MHz计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TICK_TIMER, &TIM_TimeBaseStructure);

    TIM_ITConfig(TICK_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(TICK_TIMER, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TICK_TIMER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  获取当前系统滴答数
  */
uint32_t GetTick(void)
{
    return uwTick;
}

/**
  * @brief  定时器中断处理函数
  */
void TICK_TIMER_IRQHandler(void)
{
    if (TIM_GetITStatus(TICK_TIMER, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TICK_TIMER, TIM_IT_Update);
        uwTick++;
    }
}

/**
  * @brief  阻塞式毫秒延时
  */
void Delay_MS(uint32_t ms)
{
    uint32_t start = GetTick();
    while ((GetTick() - start) < ms);
}