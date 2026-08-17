/**
 * 延时函数模块（基于 DWT 周期计数器）
 *
 * 使用 Cortex-M3 内核的 DWT->CYCCNT 寄存器计数系统时钟周期。
 * 优点：不占用定时器资源，比软件空循环精确。
 *
 * 主频 72MHz → 1us = 72 个时钟周期
 */

#include "delay.h"

/**
 * @brief 初始化 DWT 周期计数器
 *
 * 使能 DWT 跟踪单元，清零并启动周期计数器。
 * 必须在系统时钟配置完成后调用。
 */
void Delay_Init(void)
{
    /* 使能 DWT 跟踪单元（DEMCR 寄存器的 TRCENA 位） */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;

    /* 清零周期计数器 */
    DWT->CYCCNT = 0;

    /* 使能周期计数器（CTRL 寄存器的 CYCCNTENA 位） */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk;
}

/**
 * @brief 微秒级延时
 * @param nus 延时的微秒数
 *
 * 计算方法：微秒数 × (系统时钟频率 / 1000000) = 需要的时钟周期数
 * 例如 72MHz 时，1us = 72 个周期
 */
void Delay_us(uint32_t nus)
{
    uint32_t start = DWT->CYCCNT;                       /* 记录起始计数值 */
    uint32_t ticks = nus * (SystemCoreClock / 1000000); /* 换算为时钟周期 */
    while ((DWT->CYCCNT - start) < ticks);              /* 等待计数值达到目标 */
}

/**
 * @brief 毫秒级延时
 * @param nms 延时的毫秒数
 */
void Delay_ms(uint32_t nms)
{
    while (nms--)
    {
        Delay_us(1000);  /* 1ms = 1000us */
    }
}
