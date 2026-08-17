<div align="center">

[简体中文](./README.md) · **English**

</div>

# BLDC Motor Control with SimpleFOC

Brushless DC motor (BLDC) FOC field oriented control based on STM32F103C8T6 + SimpleFOC library, supporting open/closed-loop velocity control, position control, and torque control.

## Hardware

- STM32F103C8T6 (Blue Pill)
- 2804 gimbal motor + AS5600 magnetic encoder (integrated)
- SimpleFOC Mini driver (DRV8313)
- 12V 2A power supply

## Wiring

| STM32 | Module | Description |
|---|---|---|
| PA0 | SimpleFOC Mini IN1 | 3-phase PWM |
| PA1 | SimpleFOC Mini IN2 | 3-phase PWM |
| PA2 | SimpleFOC Mini IN3 | 3-phase PWM |
| PB9 | SimpleFOC Mini EN | Enable |
| PB7 | AS5600 SDA | I2C data |
| PB6 | AS5600 SCL | I2C clock |
| PA9 / PA10 | USB-to-TTL | Serial |

> Connect SimpleFOC Mini VIN/GND to 12V power, share GND with STM32.

## Environment Setup

1. Add STM32 board package in Arduino IDE (STM32duino)
2. Install **SimpleFOC** library (Library Manager → search SimpleFOC)
3. Board: `Generic STM32F103C series`, Variant: `STM32F103C8`
4. Upload method: `OpenOCD STLink (SWD)`

## Serial Commands

Baud rate 115200:

| Command | Function |
|---|---|
| `O` | Open-loop velocity mode |
| `C` | Closed-loop velocity mode (needs calibration) |
| `T5` | Target velocity 5 rad/s |
| `A1.57` | Target angle 90° (position control) |
| `M0.3` | Torque 0.3 Nm (torque control) |

## Core Principles

The SimpleFOC library encapsulates the complex FOC math:

```
Target → PID → Voltage → SVPWM → 3-phase PWM → Motor
              ↑                            ↓
              └──── Encoder feedback angle ←┘
```

You only need to call `motor.move(target)`; the Clarke/Park transforms, SVPWM, and PID are all handled by the library.

## Notes

- Closed-loop mode requires `motor.initFOC()` to calibrate sensor zero position
- If position control vibrates, decrease `P_angle.P` or increase `P_angle.D`
- 2804 motor pole pairs default to 7; change `MOTOR_PP` for different motors
