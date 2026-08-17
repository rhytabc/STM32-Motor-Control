#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>

/**
 * USART2 串口通信模块
 *
 * PA2 (TX) / PA3 (RX)，115200bps 8N1
 */

#define USART_BAUDRATE      115200

void    USART2_Init(void);
void    USART2_SendByte(uint8_t data);
void    USART2_SendString(const char *str);
void    USART2_SendBuf(const uint8_t *buf, uint16_t len);
uint8_t USART2_IsDataReady(void);
uint8_t USART2_ReadByte(void);
int     fputc(int ch, FILE *f);

#endif
