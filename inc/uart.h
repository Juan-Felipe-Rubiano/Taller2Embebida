#ifndef __UART_H
#define __UART_H

#include "apm32f10x.h"
#include "cola.h"
#include <stdio.h>

void UART_Init(uint32_t baudrate);
void UART_Tx(char c);

void UART_hw_event_listener(Queue_p q);
//Arreglos de UART que solo leía un caracter
void USART1_IRQHandler(void);
//

#endif
