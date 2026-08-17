/**
 * ============================================================
 *  有刷直流电机 PWM 调速 —— 主程序
 * ============================================================
 *
 * 硬件平台：STM32F103C8T6（72MHz）
 * 驱动模块：L298N 双 H 桥
 * 通信方式：USART2 串口（PA2/PA3，115200bps）
 *
 * 串口命令（发送后回车）：
 *   F500    → 电机A 正转 50% 速度
 *   F-300   → 电机A 反转 30% 速度
 *   R300    → 电机A 反转 30%（同 F-300）
 *   B700    → 电机B 正转 70% 速度
 *   S       → 全部停止
 *   demo    → 自动演示（加减速 + 正反转）
 *
 * 接线：
 *   PA0  → L298N ENA     PA1  → L298N ENB
 *   PB0  → L298N IN1     PB10 → L298N IN3
 *   PB1  → L298N IN2     PB11 → L298N IN4
 *   PA2  → USB转TTL RXD   PA3  → USB转TTL TXD
 *   GND  → 共地（STM32 + L298N + USB转TTL）
 * ============================================================
 */

#include "main.h"
#include "delay.h"
#include "motor.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*---------- 宏 ----------*/
#define CMD_BUF_SIZE    16

/* 快捷 LED 操作（PC13 低电平点亮） */
#define LED_ON()    GPIO_ResetBits(LED_PORT, LED_PIN)
#define LED_OFF()   GPIO_SetBits(LED_PORT, LED_PIN)

/*---------- 全局变量 ----------*/
static int16_t g_speedA = 0;     /* 电机A 当前速度 */
static int16_t g_speedB = 0;     /* 电机B 当前速度 */

/*---------- 串口接收缓冲区 ----------*/
static char     rx_buf[CMD_BUF_SIZE];
static uint8_t  rx_idx = 0;

/*===========================================================================
 *  系统初始化
 *===========================================================================*/

/**
 * @brief 系统时钟配置：HSE 8MHz → PLL×9 → 72MHz
 */
void SystemClock_Config(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    if (RCC_WaitForHSEStartUp() == ERROR) { while (1); }

    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    FLASH_SetLatency(FLASH_Latency_2);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);

    SystemCoreClock = 72000000;
}

void LED_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(LED_RCC, ENABLE);
    gpio.GPIO_Pin   = LED_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &gpio);
    LED_OFF();
}

/*===========================================================================
 *  命令解析
 *===========================================================================*/

/**
 * @brief 解析并执行一条命令
 *
 * 命令格式：
 *   F<N>    → Motor A 正向 N（0~1000）
 *   F-<N>   → Motor A 反向 N
 *   R<N>    → Motor A 反向 N（别名）
 *   B<N>    → Motor B 正向 N
 *   B-<N>   → Motor B 反向 N
 *   S       → 停止
 *   demo    → 演示模式
 */
static void ParseCmd(const char *cmd)
{
    char  motor;
    int   speed;

    if (!cmd || !cmd[0]) return;

    /* 跳过空格 */
    while (*cmd == ' ') cmd++;
    if (!*cmd) return;

    motor = cmd[0];

    switch (motor)
    {
    case 'F':
    case 'f':
        speed = atoi(&cmd[1]);
        g_speedA = speed;
        Motor_SetSpeed(MOTOR_A, g_speedA);
        printf("[A] 速度 = %d\r\n", g_speedA);
        break;

    case 'R':
    case 'r':
        speed = atoi(&cmd[1]);
        g_speedA = -speed;          /* R = reverse */
        Motor_SetSpeed(MOTOR_A, g_speedA);
        printf("[A] 速度 = %d\r\n", g_speedA);
        break;

    case 'B':
    case 'b':
        speed = atoi(&cmd[1]);
        g_speedB = speed;
        Motor_SetSpeed(MOTOR_B, g_speedB);
        printf("[B] 速度 = %d\r\n", g_speedB);
        break;

    case 'S':
    case 's':
        g_speedA = 0;
        g_speedB = 0;
        Motor_SetSpeed(MOTOR_A, 0);
        Motor_SetSpeed(MOTOR_B, 0);
        printf("[停止] 电机已停转\r\n");
        break;

    default:
        printf("[?] 未知命令: %s\r\n", cmd);
        break;
    }
}

/*===========================================================================
 *  演示模式
 *===========================================================================*/

/**
 * @brief 自动演示：正转加速 → 减速 → 反转加速 → 停止
 */
static void Demo(void)
{
    int16_t s;

    printf("\r\n===== 演示开始 =====\r\n");

    /* 正转 0→1000 加速（每 20ms 加 20） */
    printf("正转加速中...  ");
    for (s = 0; s <= 1000; s += 20)
    {
        Motor_SetSpeed(MOTOR_A, s);
        Delay_ms(20);
    }

    /* 保持 2 秒 */
    printf("保持...  ");
    Delay_ms(2000);

    /* 正转 1000→0 减速 */
    printf("减速...  ");
    for (s = 1000; s >= 0; s -= 20)
    {
        Motor_SetSpeed(MOTOR_A, s);
        Delay_ms(20);
    }

    /* 反转 0→-1000 加速 */
    printf("反转加速...  ");
    for (s = 0; s >= -1000; s -= 20)
    {
        Motor_SetSpeed(MOTOR_A, s);
        Delay_ms(20);
    }

    /* 保持 2 秒 */
    printf("保持...  ");
    Delay_ms(2000);

    /* 反转 -1000→0 减速 */
    printf("减速停止...  ");
    for (s = -1000; s <= 0; s += 20)
    {
        Motor_SetSpeed(MOTOR_A, s);
        Delay_ms(20);
    }

    Motor_SetSpeed(MOTOR_A, 0);
    printf("完成!\r\n");
    printf("===== 演示结束 =====\r\n\r\n");
}

/*===========================================================================
 *  主函数
 *===========================================================================*/

int main(void)
{
    /*---- 硬件初始化 ----*/
    SystemClock_Config();
    Delay_Init();
    LED_Init();
    USART2_Init();
    Motor_Init();

    /*---- 打印启动信息 ----*/
    printf("\r\n");
    printf("========================================\r\n");
    printf("  有刷直流电机 PWM 调速\r\n");
    printf("========================================\r\n");
    printf("  MCU   : STM32F103C8T6 @ %luMHz\r\n", SystemCoreClock / 1000000);
    printf("  驱动  : L298N 双 H 桥\r\n");
    printf("  PWM   : TIM2, 18kHz\r\n");
    printf("========================================\r\n");
    printf("\r\n");
    printf("命令说明：\r\n");
    printf("  F<数字>   电机A 正转（如 F500）\r\n");
    printf("  F-<数字>  电机A 反转（如 F-300）\r\n");
    printf("  R<数字>   电机A 反转（同 F-）\r\n");
    printf("  B<数字>   电机B 正转（如 B700）\r\n");
    printf("  S         停止\r\n");
    printf("  demo      自动演示\r\n");
    printf("========================================\r\n\r\n");

    LED_ON();     /* LED 亮，表示就绪 */

    /*---- 主循环 ----*/
    while (1)
    {
        /* 处理串口接收 */
        if (USART2_IsDataReady())
        {
            char ch = (char)USART2_ReadByte();

            if (ch == '\r' || ch == '\n')
            {
                if (rx_idx > 0)
                {
                    rx_buf[rx_idx] = '\0';   /* 字符串结束 */

                    if (strncmp(rx_buf, "demo", 4) == 0)
                    {
                        Demo();
                    }
                    else
                    {
                        ParseCmd(rx_buf);
                    }

                    rx_idx = 0;               /* 清空缓冲区 */
                }
            }
            else if (rx_idx < CMD_BUF_SIZE - 1)
            {
                rx_buf[rx_idx++] = ch;
            }
        }
    }
}
