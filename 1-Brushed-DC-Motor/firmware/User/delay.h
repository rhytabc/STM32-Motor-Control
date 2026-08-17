#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

/**
 * 延时函数模块（基于 DWT 周期计数器）
 *
 * 使用 Cortex-M3 内核自带的 DWT（数据观察点与跟踪）单元
 * 中的周期计数器来实现微秒级精确延时。
 * 72MHz 主频下，72个时钟周期 = 1微秒。
 */

void Delay_Init(void);           /* 初始化 DWT 周期计数器 */
void Delay_us(uint32_t nus);     /* 微秒级延时 */
void Delay_ms(uint32_t nms);     /* 毫秒级延时 */

#endif
