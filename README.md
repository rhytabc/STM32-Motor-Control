# STM32 电机控制学习项目 · STM32 Motor Control Learning Project

基于 STM32F103C8T6 的完整电机控制学习项目，涵盖**有刷直流电机 PWM 调速**到**无刷直流电机（BLDC）FOC 控制**，最终实现一个**力反馈旋钮**。

*Based on STM32F103C8T6, a complete motor control learning project covering **brushed DC motor PWM speed control** to **brushless DC motor (BLDC) FOC control**, culminating in a **force feedback knob**.*

---

## 📖 项目总览 · Project Overview

| 项目 Project | 电机类型 Motor | 开发环境 Env | 核心技术 Tech |
|---|---|---|---|
| [1. 有刷直流电机 Brushed DC](./1-Brushed-DC-Motor/) | 有刷 DC · Brushed DC | Keil MDK | PWM + H 桥 · PWM + H-Bridge |
| [2. 无刷 SimpleFOC · BLDC SimpleFOC](./2-BLDC-SimpleFOC/) | 无刷 BLDC | Arduino IDE | FOC 磁场定向控制 · Field Oriented Control |
| [3. 力反馈旋钮 · Force Feedback Knob](./3-Force-Feedback-Knob/) | 无刷 BLDC | Arduino IDE | 力矩控制 + PID · Torque Control + PID |

---

## 🛠 硬件清单 · Hardware List

| 硬件 Hardware | 型号 Model | 用途 Purpose |
|---|---|---|
| 主控 MCU | STM32F103C8T6 (Blue Pill) | 所有项目通用 · All projects |
| 有刷电机 Brushed Motor | TT 马达（双轴 1:48）· TT Motor (dual-shaft 1:48) | 项目 1 · Project 1 |
| 有刷驱动 Brushed Driver | L298N 双 H 桥 · L298N dual H-Bridge | 项目 1 · Project 1 |
| 无刷电机 BLDC Motor | 2804 云台电机 + AS5600 编码器 · 2804 gimbal motor + AS5600 encoder | 项目 2、3 |
| 无刷驱动 BLDC Driver | SimpleFOC Mini (DRV8313) | 项目 2、3 |
| 电源 Power | 12V 2A 适配器 · 12V 2A adapter | 无刷供电 · BLDC power |
| 调试 Debug | ST-Link + USB 转 TTL · ST-Link + USB-to-TTL | 烧录 + 串口 · Flash + Serial |

---

## 🎯 学习路线 · Learning Path

```
有刷直流电机 · Brushed DC（入门 Beginner）
    ├── PWM 调速原理 · PWM speed control
    ├── H 桥正反转 · H-Bridge direction
    └── 串口命令 · Serial commands
          ↓
无刷 SimpleFOC · BLDC SimpleFOC（进阶 Intermediate）
    ├── FOC 磁场定向控制 · FOC
    ├── 开环/闭环速度 · Open/closed-loop velocity
    ├── 位置控制 PID · Position control (PID)
    └── 力矩控制 · Torque control
          ↓
力反馈旋钮 · Force Feedback Knob（终极 Final）
    └── 弹簧手感 · Spring feel
```

---

## 🔌 接线 · Wiring

### 有刷电机（项目 1）· Brushed DC (Project 1)

| STM32 | L298N |
|---|---|
| PA0 | ENA |
| PA1 | ENB |
| PB0 | IN1 |
| PB1 | IN2 |
| PB10 | IN3 |
| PB11 | IN4 |

### 无刷电机（项目 2、3）· BLDC (Project 2, 3)

| STM32 | 模块 Module |
|---|---|
| PA0 / PA1 / PA2 | SimpleFOC Mini 三路 PWM · 3-phase PWM |
| PB9 | SimpleFOC Mini EN |
| PB7 / PB6 | AS5600 SDA / SCL |
| PA9 / PA10 | 串口 TX / RX · Serial |

> 详细接线图见 [docs/images](./docs/images/) · *See [docs/images](./docs/images/) for detailed wiring.*

---

## 📦 目录结构 · Directory Structure

```
STM32-Motor-Control/
├── 1-Brushed-DC-Motor/     # 有刷电机 · Brushed DC（Keil）
│   ├── firmware/User/       # 源代码 · Source code
│   └── README.md
├── 2-BLDC-SimpleFOC/        # 无刷电机 · BLDC（Arduino）
│   ├── SimpleFOC_2804.ino
│   └── README.md
├── 3-Force-Feedback-Knob/   # 力反馈旋钮 · Force Feedback Knob
│   ├── ForceFeedbackKnob.ino
│   └── README.md
├── docs/
│   ├── images/              # 接线图 · Wiring images
│   └── videos/              # 演示视频 · Demo videos
└── README.md
```

---

## 📹 效果演示 · Demonstration

### 硬件连接 · Hardware Wiring

![硬件连接图 Wiring Diagram](docs/images/wiring-diagram.jpg)

### 演示视频 · Demo Video

<video width="640" controls>
  <source src="docs/videos/demo.mp4" type="video/mp4">
  你的浏览器不支持视频播放 · <a href="docs/videos/demo.mp4">点击下载 Download</a>
</video>

---

## 📝 使用说明 · Usage

### 环境配置 · Environment Setup

- **有刷电机 Brushed DC**：Keil MDK-ARM v5 + STM32F1xx_DFP 器件包（需勾选 MicroLIB）
- **无刷电机 BLDC**：Arduino IDE + STM32duino 板包 + SimpleFOC 库

### 快速开始 · Quick Start

每个子项目都有独立的 README，包含详细的接线、编译、烧录步骤。
*Each sub-project has its own README with detailed wiring, build, and flashing instructions.*

---

## 📚 参考资料 · References

- [SimpleFOC 官方文档 · Documentation](https://docs.simplefoc.com/)
- [STM32duino](https://github.com/stm32duino/Arduino_Core_STM32)
