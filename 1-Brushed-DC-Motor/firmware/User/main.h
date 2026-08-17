#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"

/**
 * 主程序头文件
 *
 * 项目：有刷直流电机 PWM 调速
 * 芯片：STM32F103C8T6
 * 控制：串口命令调速 + 正反转
 */

/*---------- 板载 LED ----------*/
#define LED_PORT    GPIOC
#define LED_PIN     GPIO_Pin_13
#define LED_RCC     RCC_APB2Periph_GPIOC

/*---------- 函数声明 ----------*/
void SystemClock_Config(void);
void LED_Init(void);

#endif
