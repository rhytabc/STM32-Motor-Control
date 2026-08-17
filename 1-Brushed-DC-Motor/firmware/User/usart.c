/**
 * USART2 串口通信模块
 *
 * PA2 = USART2_TX, PA3 = USART2_RX, 115200bps 8N1
 */

#include "usart.h"

void USART2_Init(void)
{
    GPIO_InitTypeDef  gpio;
    USART_InitTypeDef usart;

    /* USART2 在 APB1 上，GPIOA 在 APB2 上 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* PA2: USART2_TX */
    gpio.GPIO_Pin   = GPIO_Pin_2;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* PA3: USART2_RX */
    gpio.GPIO_Pin   = GPIO_Pin_3;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate            = USART_BAUDRATE;
    usart.USART_WordLength          = USART_WordLength_8b;
    usart.USART_StopBits            = USART_StopBits_1;
    usart.USART_Parity              = USART_Parity_No;
    usart.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_Cmd(USART2, ENABLE);
}

void USART2_SendByte(uint8_t data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}

void USART2_SendString(const char *str)
{
    while (*str) USART2_SendByte((uint8_t)*str++);
}

void USART2_SendBuf(const uint8_t *buf, uint16_t len)
{
    while (len--) USART2_SendByte(*buf++);
}

uint8_t USART2_IsDataReady(void)
{
    return (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) ? 1 : 0;
}

uint8_t USART2_ReadByte(void)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART2);
}

int fputc(int ch, FILE *f)
{
    (void)f;
    USART2_SendByte((uint8_t)ch);
    return ch;
}
