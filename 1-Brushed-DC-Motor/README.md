<div align="center">

**简体中文** · [English](./README_EN.md)

</div>

# 有刷直流电机 PWM 调速

基于 STM32F103C8T6 + L298N 的有刷直流电机控制，支持 PWM 调速、正反转、串口命令控制。

## 硬件

- STM32F103C8T6 最小系统板
- TT 马达（双轴 1:48，3-6V）
- L298N 双 H 桥驱动模块
- USB 转 TTL（串口调试）

## 接线

| STM32 | L298N | 说明 |
|---|---|---|
| PA0 | ENA | 电机 A PWM（TIM2_CH1） |
| PA1 | ENB | 电机 B PWM（TIM2_CH2） |
| PB0 | IN1 | 电机 A 方向 |
| PB1 | IN2 | 电机 A 方向 |
| PB10 | IN3 | 电机 B 方向 |
| PB11 | IN4 | 电机 B 方向 |
| PA2 | — | 串口 TX（USART2） |
| PA3 | — | 串口 RX（USART2） |

> ⚠️ 拔掉 L298N 上 ENA/ENB 的跳线帽，否则 PWM 无法调速。
> 电机接 OUT1/OUT2（电机 A），供电接 12V 端子（USB 5V 也能驱动）。

## 编译烧录

1. Keil MDK-ARM v5 新建工程，选 STM32F103C8
2. RTE 勾选：CMSIS CORE、Device StdPeriph Drivers（GPIO/RCC/USART/TIM/FLASH/Framework）
3. 添加 `firmware/User/` 下所有 `.c` 文件 + `startup_stm32f10x_md.s` + `system_stm32f10x.c`
4. C/C++ → Define：`USE_STDPERIPH_DRIVER,STM32F10X_MD`；Include Path：`..\User`
5. 勾选 **Use MicroLIB**（printf 串口输出必需）
6. 编译烧录

## 串口命令

波特率 115200，命令格式：

| 命令 | 功能 |
|---|---|
| `F500` | 电机 A 正转 50% |
| `F-300` | 电机 A 反转 30% |
| `R300` | 电机 A 反转（同 F-） |
| `B700` | 电机 B 正转 70% |
| `S` | 停止 |
| `demo` | 自动演示（加减速 + 正反转） |

## 核心原理

- **PWM 调速**：TIM2 输出 18kHz PWM，占空比 0~1000 对应速度 0~100%
- **H 桥方向**：IN1/IN2 电平组合决定电流方向（正转/反转/刹车/滑行）

```c
// 电机速度设置：-1000 ~ +1000（负 = 反转）
Motor_SetSpeed(MOTOR_A, 500);   // 半速正转
Motor_SetSpeed(MOTOR_A, -800);  // 80% 反转
```
