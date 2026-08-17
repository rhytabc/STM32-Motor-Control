<div align="center">

**简体中文** · [English](./README_EN.md)

</div>

# 无刷电机 SimpleFOC 控制

基于 STM32F103C8T6 + SimpleFOC 库的无刷直流电机（BLDC）FOC 磁场定向控制，支持开环/闭环速度控制、位置控制、力矩控制。

## 硬件

- STM32F103C8T6 (Blue Pill)
- 2804 云台电机 + AS5600 磁编码器（一体机）
- SimpleFOC Mini 驱动板（DRV8313）
- 12V 2A 电源

## 接线

| STM32 | 模块 | 说明 |
|---|---|---|
| PA0 | SimpleFOC Mini IN1 | 三相 PWM |
| PA1 | SimpleFOC Mini IN2 | 三相 PWM |
| PA2 | SimpleFOC Mini IN3 | 三相 PWM |
| PB9 | SimpleFOC Mini EN | 使能 |
| PB7 | AS5600 SDA | I2C 数据 |
| PB6 | AS5600 SCL | I2C 时钟 |
| PA9 / PA10 | USB 转 TTL | 串口 |

> SimpleFOC Mini 的 VIN/GND 接 12V 电源，GND 与 STM32 共地。

## 环境配置

1. Arduino IDE 添加 STM32 板包（STM32duino）
2. 安装 **SimpleFOC** 库（库管理器搜 SimpleFOC）
3. 板子选 `Generic STM32F103C series`，Variant 选 `STM32F103C8`
4. Upload method 选 `OpenOCD STLink (SWD)`

## 串口命令

波特率 115200：

| 命令 | 功能 |
|---|---|
| `O` | 开环速度模式 |
| `C` | 闭环速度模式（需校准） |
| `T5` | 目标速度 5 rad/s |
| `A1.57` | 目标角度 90°（位置控制） |
| `M0.3` | 力矩 0.3 Nm（力矩控制） |

## 核心原理

SimpleFOC 库封装了 FOC 的复杂数学：

```
目标 → PID → 电压 → SVPWM → 三相 PWM → 电机
              ↑                        ↓
              └──── 编码器反馈角度 ←────┘
```

你只需关注 `motor.move(目标)`，中间的 Clarke/Park 变换、SVPWM、PID 全由库完成。

## 注意

- 闭环模式首次需 `motor.initFOC()` 校准传感器零位
- 位置控制若震动，调小 `P_angle.P` 或加大 `P_angle.D`
- 2804 电机极对数默认 7，如电机型号不同需修改 `MOTOR_PP`
