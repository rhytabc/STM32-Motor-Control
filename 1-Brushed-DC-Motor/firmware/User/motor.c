/**
 * ============================================================
 *  有刷直流电机驱动 —— PWM + H桥方向控制
 * ============================================================
 *
 * H 桥真值表（以 Motor A 为例）：
 *   IN1=1  IN2=0  → 正转（电流 左→右）
 *   IN1=0  IN2=1  → 反转（电流 右→左）
 *   IN1=1  IN2=1  → 刹车（电机短路制动）
 *   IN1=0  IN2=0  → 滑行（自由停止）
 *
 * PWM 配置：
 *   TIM2, 通道1(PA0) 对应 Motor A 的 ENA
 *   TIM2, 通道2(PA1) 对应 Motor B 的 ENB
 *   频率 = 72MHz / (3+1) / (999+1) = 18kHz
 * ============================================================
 */

#include "motor.h"
#include "stm32f10x_tim.h"

/*---------- 引脚定义 ----------*/
/* Motor A */
#define A_ENA_PORT      GPIOA
#define A_ENA_PIN       GPIO_Pin_0
#define A_IN1_PORT      GPIOB
#define A_IN1_PIN       GPIO_Pin_0
#define A_IN2_PORT      GPIOB
#define A_IN2_PIN       GPIO_Pin_1

/* Motor B */
#define B_ENB_PORT      GPIOA
#define B_ENB_PIN       GPIO_Pin_1
#define B_IN3_PORT      GPIOB
#define B_IN3_PIN       GPIO_Pin_10
#define B_IN4_PORT      GPIOB
#define B_IN4_PIN       GPIO_Pin_11

/*---------- 宏：快捷操作方向引脚 ----------*/
#define A_IN1_H()   GPIO_SetBits(A_IN1_PORT, A_IN1_PIN)
#define A_IN1_L()   GPIO_ResetBits(A_IN1_PORT, A_IN1_PIN)
#define A_IN2_H()   GPIO_SetBits(A_IN2_PORT, A_IN2_PIN)
#define A_IN2_L()   GPIO_ResetBits(A_IN2_PORT, A_IN2_PIN)
#define B_IN3_H()   GPIO_SetBits(B_IN3_PORT, B_IN3_PIN)
#define B_IN3_L()   GPIO_ResetBits(B_IN3_PORT, B_IN3_PIN)
#define B_IN4_H()   GPIO_SetBits(B_IN4_PORT, B_IN4_PIN)
#define B_IN4_L()   GPIO_ResetBits(B_IN4_PORT, B_IN4_PIN)

/*---------- 内部函数：设置指定通道的 PWM 占空比 ----------*/
static void SetPWM(uint16_t ch, uint16_t duty)
{
    switch (ch)
    {
    case 1: TIM_SetCompare1(TIM2, duty); break;
    case 2: TIM_SetCompare2(TIM2, duty); break;
    }
}

/**
 * @brief 初始化电机驱动
 */
void Motor_Init(void)
{
    GPIO_InitTypeDef  gpio;
    TIM_TimeBaseInitTypeDef  tim;
    TIM_OCInitTypeDef  oc;

    /*---- 1. 使能时钟 ----*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /*---- 2. 配置 PWM 引脚 PA0/PA1 为复用推挽输出 ----*/
    gpio.GPIO_Pin   = A_ENA_PIN | B_ENB_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /*---- 3. 配置方向引脚 PB0/PB1/PB10/PB11 为推挽输出 ----*/
    gpio.GPIO_Pin   = A_IN1_PIN | A_IN2_PIN | B_IN3_PIN | B_IN4_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    /* 初始输出低电平 */
    A_IN1_L(); A_IN2_L();
    B_IN3_L(); B_IN4_L();

    /*---- 4. 配置 TIM2 时基：18kHz PWM ----*/
    TIM_TimeBaseStructInit(&tim);
    tim.TIM_Prescaler         = 3;          /* 72MHz / 4 = 18MHz */
    tim.TIM_Period            = 999;        /* 18MHz / 1000 = 18kHz */
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &tim);

    /*---- 5. 配置 PWM 输出通道 ----*/
    TIM_OCStructInit(&oc);
    oc.TIM_OCMode      = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_Pulse       = 0;                 /* 初始占空比 0 */

    TIM_OC1Init(TIM2, &oc);                 /* CH1 → PA0 */
    TIM_OC2Init(TIM2, &oc);                 /* CH2 → PA1 */

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    /*---- 6. 启动 TIM2 ----*/
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief 设置电机速度
 * @param motor  MOTOR_A 或 MOTOR_B
 * @param speed  -1000 ~ +1000，正=正转，负=反转，0=停
 */
void Motor_SetSpeed(uint8_t motor, int16_t speed)
{
    uint16_t duty;
    uint8_t  forward;

    /* 限幅 */
    if (speed > 1000)  speed = 1000;
    if (speed < -1000) speed = -1000;

    if (speed >= 0)
    {
        forward = 1;
        duty = (uint16_t)speed;
    }
    else
    {
        forward = 0;
        duty = (uint16_t)(-speed);
    }

    if (motor == MOTOR_A)
    {
        if (duty == 0)
        {
            /* 占空比 0 → 滑行 */
            A_IN1_L(); A_IN2_L();
        }
        else if (forward)
        {
            A_IN1_H(); A_IN2_L();           /* 正转 */
        }
        else
        {
            A_IN1_L(); A_IN2_H();           /* 反转 */
        }
        SetPWM(1, duty);
    }
    else  /* MOTOR_B */
    {
        if (duty == 0)
        {
            B_IN3_L(); B_IN4_L();
        }
        else if (forward)
        {
            B_IN3_H(); B_IN4_L();           /* 正转 */
        }
        else
        {
            B_IN3_L(); B_IN4_H();           /* 反转 */
        }
        SetPWM(2, duty);
    }
}

/**
 * @brief 刹车（IN1=IN2=1，电机两端短路制动）
 */
void Motor_Brake(uint8_t motor)
{
    if (motor == MOTOR_A)
    {
        SetPWM(1, 0);
        A_IN1_H(); A_IN2_H();
    }
    else
    {
        SetPWM(2, 0);
        B_IN3_H(); B_IN4_H();
    }
}

/**
 * @brief 滑行（IN1=IN2=0，H桥全关，电机自由旋转）
 */
void Motor_Coast(uint8_t motor)
{
    if (motor == MOTOR_A)
    {
        SetPWM(1, 0);
        A_IN1_L(); A_IN2_L();
    }
    else
    {
        SetPWM(2, 0);
        B_IN3_L(); B_IN4_L();
    }
}
