/**
 * 力反馈旋钮 —— 弹簧手感
 *
 * 把 2804 电机变成一个"会自动回中"的旋钮。
 * 拧它，它弹回；松手，它停在中间。
 *
 * 原理：
 *   力 = -K × (当前角度 - 中心角度)
 *   K = 弹簧刚度（越大越硬）
 *
 * 接线（跟之前一样）：
 *   编码器 AS5600: SDA→PB7, SCL→PB6
 *   驱动板: IN1→PA0, IN2→PA1, IN3→PA2, EN→PB9
 *   串口: PA9(TX), PA10(RX), 115200
 *
 * 串口命令：
 *   K10    → 设置弹簧刚度 K = 10
 *   Z      → 把当前位置设为中心（回中点）
 */

#include <SimpleFOC.h>

#define MOTOR_PP        7

// 编码器 + 驱动 + 电机
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
BLDCDriver3PWM driver = BLDCDriver3PWM(PA0, PA1, PA2, PB9);
BLDCMotor motor = BLDCMotor(MOTOR_PP);

// 弹簧参数
float spring_K = 5.0;      // 弹簧刚度
float center_angle = 0;    // 回中角度

// 串口命令
Commander commander = Commander(Serial);

void doK(char* cmd) { commander.scalar(&spring_K, cmd); Serial.print("K="); Serial.println(spring_K); }
void doZero(char* cmd){ center_angle = motor.shaft_angle; Serial.print("中心设为: "); Serial.println(center_angle); }

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("\n========================================");
    Serial.println("  力反馈旋钮 —— 弹簧手感");
    Serial.println("========================================");
    Serial.println("命令: K<刚度> | Z(设中心)");
    Serial.println("========================================\n");

    // 编码器
    sensor.init();
    motor.linkSensor(&sensor);

    // 驱动（电压限制跟之前能工作的配置一致）
    driver.voltage_power_supply = 12;
    driver.voltage_limit = 6;
    driver.init();
    motor.linkDriver(&driver);

    // 先用电压限制 6V 校准
    motor.voltage_limit = 6;
    motor.controller = MotionControlType::torque;

    motor.init();

    // 关键：力矩模式必须校准传感器零位，否则力方向是错的
    Serial.println("正在校准传感器...");
    motor.initFOC();
    Serial.println("校准完成！");

    // 把当前角度设为中心
    center_angle = motor.shaft_angle;

    Serial.println("[OK] 旋钮就绪，拧动电机轴试试！");
    Serial.println("拧开 → 它弹回；松手 → 停在中间\n");

    commander.add('K', doK, "弹簧刚度");
    commander.add('Z', doZero, "设中心");

    delay(1000);
}

void loop()
{
    motor.loopFOC();

    // 当前角度（initFOC 后 shaft_angle 有效）
    float angle = motor.shaft_angle;

    // 角度差（归一化到 -π ~ π，处理环绕）
    float diff = angle - center_angle;
    while (diff > PI)  diff -= 2 * PI;
    while (diff < -PI) diff += 2 * PI;

    // 弹簧力：偏离中心越远，往回拉的力越大
    float torque = -spring_K * diff;

    motor.move(torque);

    commander.run();
}
