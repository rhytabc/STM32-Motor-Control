# STM32 电机控制学习项目（从有刷到无刷 FOC）

基于 STM32F103C8T6 的完整电机控制学习项目，涵盖**有刷直流电机 PWM 调速**到**无刷直流电机（BLDC）FOC 控制**，最终实现一个**力反馈旋钮**。

> 一个从零到一的嵌入式电机控制学习记录，代码完整、注释详细、可复现。

---

## 📖 项目总览

| 项目 | 电机类型 | 开发环境 | 核心技术 | 状态 |
|---|---|---|---|---|
| [1. 有刷直流电机](./1-Brushed-DC-Motor/) | 有刷 DC | Keil MDK | PWM + H 桥 | ✅ 完成 |
| [2. 无刷电机 SimpleFOC](./2-BLDC-SimpleFOC/) | 无刷 BLDC | Arduino IDE | FOC 磁场定向控制 | ✅ 完成 |
| [3. 力反馈旋钮](./3-Force-Feedback-Knob/) | 无刷 BLDC | Arduino IDE | 力矩控制 + PID | ✅ 完成 |

---

## 🛠 硬件清单

| 硬件 | 型号 | 用途 |
|---|---|---|
| 主控 | STM32F103C8T6 (Blue Pill) | 所有项目通用 |
| 有刷电机 | TT 马达（双轴 1:48） | 项目 1 |
| 有刷驱动 | L298N 双 H 桥 | 项目 1 |
| 无刷电机 | 2804 云台电机 + AS5600 编码器 | 项目 2、3 |
| 无刷驱动 | SimpleFOC Mini (DRV8313) | 项目 2、3 |
| 电源 | 12V 2A 适配器 | 无刷供电 |
| 调试 | ST-Link + USB 转 TTL | 烧录 + 串口 |

---

## 🎯 学习路线

```
有刷直流电机（入门）
    ├── PWM 调速原理
    ├── H 桥正反转控制
    └── 串口命令控制
          ↓
无刷电机 SimpleFOC（进阶）
    ├── FOC 磁场定向控制原理
    ├── 开环/闭环速度控制
    ├── 位置控制（PID）
    └── 力矩控制
          ↓
力反馈旋钮（终极项目）
    └── 弹簧手感实现
```

---

## 🔌 接线说明

### 有刷电机（项目 1）

| STM32 | L298N |
|---|---|
| PA0 | ENA |
| PA1 | ENB |
| PB0 | IN1 |
| PB1 | IN2 |
| PB10 | IN3 |
| PB11 | IN4 |

### 无刷电机（项目 2、3）

| STM32 | 模块 |
|---|---|
| PA0 / PA1 / PA2 | SimpleFOC Mini 三路 PWM |
| PB9 | SimpleFOC Mini EN |
| PB7 / PB6 | AS5600 SDA / SCL |
| PA9 / PA10 | 串口 TX / RX |

> 详细接线图见 [docs/images](./docs/images/)

---

## 📦 目录结构

```
STM32-Motor-Control/
├── 1-Brushed-DC-Motor/     # 有刷电机（Keil）
│   ├── firmware/User/       # 源代码
│   └── README.md
├── 2-BLDC-SimpleFOC/        # 无刷电机（Arduino）
│   ├── SimpleFOC_2804.ino
│   └── README.md
├── 3-Force-Feedback-Knob/   # 力反馈旋钮
│   ├── ForceFeedbackKnob.ino
│   └── README.md
├── docs/
│   ├── images/              # 硬件连接图片
│   └── videos/              # 演示视频
└── README.md
```

---

## 📹 效果演示

<!-- TODO: 硬件连接图片 -->

<!-- TODO: 演示视频 -->

---

## 📝 使用说明

### 环境配置

- **有刷电机**：Keil MDK-ARM v5 + STM32F1xx_DFP 器件包（需勾选 MicroLIB）
- **无刷电机**：Arduino IDE + STM32duino 板包 + SimpleFOC 库

### 快速开始

每个子项目都有独立的 README，包含详细的接线、编译、烧录步骤。

---

## 📚 参考资料

- [SimpleFOC 官方文档](https://docs.simplefoc.com/)
- [STM32duino](https://github.com/stm32duino/Arduino_Core_STM32)

---

## ⚠️ 免责声明

本项目仅供学习交流，操作时注意用电安全，12V 电源请勿反接。
