/**
 * SimpleFOC 2804 无刷电机控制
 *
 * 硬件：
 *   STM32F103C8T6 (Blue Pill)
 *   2804 云台电机 + AS5600 磁编码器 (I2C)
 *   SimpleFOC Mini 驱动板 (DRV8313)
 *
 * 接线：
 *   编码器 AS5600: SDA→PB7, SCL→PB6
 *   驱动板: IN1→PA0, IN2→PA1, IN3→PA2, EN→PB9
 *   串口: PA9(TX), PA10(RX), 115200
 *
 * 串口命令：
 *   T20    → 目标速度 20 rad/s
 *   T0     → 停止
 *   A3.14  → 目标角度 3.14 rad (180°)
 *   O      → 开环模式（无传感器，直接转）
 *   C      → 闭环速度模式
 */

#include <SimpleFOC.h>

// ==================== 电机配置 ====================
// 2804 云台电机：7 对极，相电阻 ~13Ω
#define MOTOR_PP        7       // 极对数（不确定就先用7，跑起来再调）

// ==================== 编码器 ====================
// AS5600 磁编码器，I2C 接口
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

// ==================== 驱动板 ====================
// SimpleFOC Mini (DRV8313) 三路 PWM + 使能
BLDCDriver3PWM driver = BLDCDriver3PWM(PA0, PA1, PA2, PB9);

// ==================== 电机对象 ====================
BLDCMotor motor = BLDCMotor(MOTOR_PP);

// ==================== 串口命令 ====================
Commander commander = Commander(Serial);
float target_velocity = 0;
float target_angle = 0;
float target_torque = 0;

void doTarget(char* cmd)   { commander.scalar(&target_velocity, cmd); }
void doTorque(char* cmd){
    commander.scalar(&target_torque, cmd);
    if (motor.controller != MotionControlType::torque) {
        motor.controller = MotionControlType::torque;
        Serial.println("力矩控制模式就绪！");
    }
}
void doAngle(char* cmd){
    commander.scalar(&target_angle, cmd);
    if (motor.controller != MotionControlType::angle) {
        motor.controller = MotionControlType::angle;
        Serial.println("正在校准传感器...");
        motor.initFOC();
        Serial.println("位置控制模式就绪！");
    }
}
void doOpenLoop(char* cmd) { motor.controller = MotionControlType::velocity_openloop; motor.move(target_velocity); Serial.println("开环速度模式"); }
void doClosedLoop(char* cmd){
    motor.controller = MotionControlType::velocity;
    Serial.println("正在校准传感器...");
    motor.initFOC();
    motor.move(0);
    Serial.println("闭环速度模式就绪！");
}

void setup()
{
    // ---- 1. 串口 ----
    Serial.begin(115200);
    delay(500);
    Serial.println("\n========================================");
    Serial.println("  SimpleFOC 2804 无刷电机控制");
    Serial.println("========================================");
    Serial.println("命令: T<速度> | A<角度> | O(开环) | C(闭环)");
    Serial.println("========================================\n");

    // ---- 2. 编码器初始化 ----
    sensor.init();
    motor.linkSensor(&sensor);
    Serial.println("[OK] AS5600 编码器");

    // ---- 3. 驱动初始化 ----
    driver.voltage_power_supply = 12;   // 12V 供电
    driver.voltage_limit       = 6;     // 先用 6V 限流，安全
    driver.init();
    motor.linkDriver(&driver);
    Serial.println("[OK] DRV8313 驱动");

    // ---- 4. 控制模式（先开环，安全） ----
    motor.controller = MotionControlType::velocity_openloop;
    motor.voltage_limit = 6;

    // ---- 5. PID 参数（先给默认值，后面再调） ----
    motor.PID_velocity.P = 0.2;
    motor.PID_velocity.I = 20;
    motor.PID_velocity.D = 0.001;
    motor.LPF_velocity.Tf = 0.01;       // 速度低通滤波

    motor.P_angle.P = 20;
    motor.velocity_limit = 40;           // 限速 40 rad/s

    // ---- 6. 启动 ----
    motor.useMonitoring(Serial);
    motor.init();

    Serial.println("[OK] 电机就绪");
    Serial.println("\n发送 O 进入开环 → 发送 T5 试试转起来\n");

    // ---- 串口命令绑定 ----
    commander.add('T', doTarget, "速度 rad/s");
    commander.add('A', doAngle,  "角度 rad");
    commander.add('M', doTorque, "力矩 Nm");
    commander.add('O', doOpenLoop, "开环");
    commander.add('C', doClosedLoop, "闭环");

    delay(1000);
}

void loop()
{
    motor.loopFOC();

    // 根据模式执行不同控制
    switch (motor.controller)
    {
    case MotionControlType::velocity_openloop:
        motor.move(target_velocity);
        break;
    case MotionControlType::velocity:
        motor.move(target_velocity);
        break;
    case MotionControlType::angle:
        motor.move(target_angle);
        break;
    case MotionControlType::torque:
        motor.move(target_torque);
        break;
    default:
        break;
    }

    commander.run();
}
