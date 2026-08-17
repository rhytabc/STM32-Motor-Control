#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"
#include <stdint.h>

/**
 * ============================================================
 *  有刷直流电机驱动模块
 * ============================================================
 *
 * 支持：L298N / TB6612 / L293D 等双H桥驱动
 *
 * 引脚（Motor A）：
 *   PA0 (TIM2_CH1) → ENA   PWM 调速
 *   PB0            → IN1   方向控制
 *   PB1            → IN2   方向控制
 *
 * 引脚（Motor B）：
 *   PA1 (TIM2_CH2) → ENB   PWM 调速
 *   PB10           → IN3   方向控制
 *   PB11           → IN4   方向控制
 *
 * PWM：TIM2, 18kHz, 1000 级分辨率
 * speed 范围：-1000（全速反转） ~ 0（停） ~ +1000（全速正转）
 * ============================================================
 */

#define MOTOR_A     0
#define MOTOR_B     1

void Motor_Init(void);
void Motor_SetSpeed(uint8_t motor, int16_t speed);   /* -1000 ~ +1000 */
void Motor_Brake(uint8_t motor);                      /* 刹车 */
void Motor_Coast(uint8_t motor);                      /* 滑行 */

#endif
