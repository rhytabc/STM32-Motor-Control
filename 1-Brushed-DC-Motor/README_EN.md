<div align="center">

[简体中文](./README.md) · **English**

</div>

# Brushed DC Motor PWM Speed Control

Brushed DC motor control based on STM32F103C8T6 + L298N, supporting PWM speed control, forward/reverse rotation, and serial command control.

## Hardware

- STM32F103C8T6 minimum system board
- TT Motor (dual-shaft 1:48, 3-6V)
- L298N dual H-Bridge driver module
- USB-to-TTL (serial debugging)

## Wiring

| STM32 | L298N | Description |
|---|---|---|
| PA0 | ENA | Motor A PWM (TIM2_CH1) |
| PA1 | ENB | Motor B PWM (TIM2_CH2) |
| PB0 | IN1 | Motor A direction |
| PB1 | IN2 | Motor A direction |
| PB10 | IN3 | Motor B direction |
| PB11 | IN4 | Motor B direction |
| PA2 | — | Serial TX (USART2) |
| PA3 | — | Serial RX (USART2) |

> ⚠️ Remove the ENA/ENB jumper caps on L298N, otherwise PWM speed control won't work.
> Connect motor to OUT1/OUT2 (Motor A), power to the 12V terminal (USB 5V also works).

## Build & Flash

1. Create a new Keil MDK-ARM v5 project, select STM32F103C8
2. In RTE, enable: CMSIS CORE, Device StdPeriph Drivers (GPIO/RCC/USART/TIM/FLASH/Framework)
3. Add all `.c` files under `firmware/User/` + `startup_stm32f10x_md.s` + `system_stm32f10x.c`
4. C/C++ → Define: `USE_STDPERIPH_DRIVER,STM32F10X_MD`; Include Path: `..\User`
5. Enable **Use MicroLIB** (required for printf serial output)
6. Build and flash

## Serial Commands

Baud rate 115200:

| Command | Function |
|---|---|
| `F500` | Motor A forward 50% |
| `F-300` | Motor A reverse 30% |
| `R300` | Motor A reverse (same as F-) |
| `B700` | Motor B forward 70% |
| `S` | Stop |
| `demo` | Auto demo (accel/decel + forward/reverse) |

## Core Principles

- **PWM speed control**: TIM2 outputs 18kHz PWM, duty cycle 0~1000 corresponds to speed 0~100%
- **H-Bridge direction**: IN1/IN2 level combination determines current direction (forward/reverse/brake/coast)

```c
// Set motor speed: -1000 ~ +1000 (negative = reverse)
Motor_SetSpeed(MOTOR_A, 500);   // Half speed forward
Motor_SetSpeed(MOTOR_A, -800);  // 80% reverse
```
