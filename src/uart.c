#include "uart.h"
#include "cola.h"

extern FSM f_manejo;
extern FSM f_seguridad;
extern FSM f_camaras;
extern FSM f_testigos;

//Arreglos de UART que solo leía un caracter
#define RX_BUF_SIZE 64
static volatile char rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;
//

/**
 * @brief  Configures UART1 (PA9=TX, PA10=RX) at 115200 baud (assuming 72MHz PCLK2)
 */
void UART_Init(uint32_t baudrate) {

    // Habilitar relojes: AFIO (bit 0), GPIOB (bit 3), USART1 (bit 14)
    RCM->APB2CLKEN |= (1 << 0) | (1 << 3) | (1 << 14);
    // Remapeando USART1 a PB6/PB7
    AFIO->REMAP1_B.USART1RMP = 1;
    GPIOB->CFGLOW &= ~(0x0F << 24); // PB6 ahora RX
    GPIOB->CFGLOW |= (0x0B << 24); // PB6 ahora TX
    GPIOB->CFGLOW &= ~(0x0F << 28);
    GPIOB->CFGLOW |= (0x04 << 28);

    // Configure Baudrate
    // BRR = PCLK2 / BaudRate
    // If PCLK2 = 72MHz and Baud = 115200 -> BRR = 625 = 0x271
    uint32_t pclk2 = 72000000;
    USART1->BR = pclk2 / baudrate;

    // Configure Control Register (8N1, TX enable, RX enable, USART enable)
    USART1->CTRL1 = (1 << 13) | (1 << 5) |(1 << 3) | (1 << 2); // UE, TE, RE

    NVIC->ISER[1] |= (1 << 5);
}

void UART_Tx(char c) {
    while (!(USART1->STS & (1 << 7))); // Wait for TXE
    USART1->DATA = c;
}

// Retargeting printf to UART1
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        UART_Tx(ptr[i]);
    }
    return len;
}


void UART_hw_event_listener(Queue_p q){
    static char rx_message[3];
    static uint8_t rx_idx = 0;

    while(rx_tail != rx_head) {
        char c = rx_buf[rx_tail]; //Lectura del dato recibido
        rx_tail = (rx_tail + 1) & (RX_BUF_SIZE - 1);
        UART_Tx(c);
        if(c == '\n' || c == '\r') {
            if(rx_idx > 0) {
                rx_message[rx_idx] = '\0';
                int n = rx_message[1] - '0';

                if(n >= 0 && n <= 9) {
                    printf("\r\n[UART RX] Procesando Comando '%s'...\r\n", rx_message);
                    FSM* fsm_destino = NULL;
                    switch(rx_message[0]) {
                        case 'M': case 'm': fsm_destino = &f_manejo;    break;
                        case 'S': case 's': fsm_destino = &f_seguridad; break;
                        case 'C': case 'c': fsm_destino = &f_camaras;   break;
                        case 'T': case 't': fsm_destino = &f_testigos;  break;
                        default:
                            printf("[UART RX ERROR] FSM invalida '%c'. Use M, S, C o T.\r\n", rx_message[0]);
                    }
                    if(fsm_destino != NULL) {
                        SystemEvent sev = {
                            .source = EV_SOURCE_UART, //En este caso si lo mandamos como evento UART
                            .direct = { .fsm = fsm_destino, .ev = n }
                        };
                        queue_enqueue(sev, q);
                    }
                } else {
                    printf("Comando no reconocido: %s\n", rx_message);
                }
                rx_idx = 0; //Reiniciar índice para el próximo mensaje
            }
        } else if(rx_idx < sizeof(rx_message) - 1) {
            rx_message[rx_idx++] = c; //Guardar
        }
    }
}

//Arreglos de UART que solo leía un caracter
void USART1_IRQHandler(void){
    if(USART1->STS & (1 << 5)) {
        char c = (char)(USART1->DATA & 0xFF);
        uint8_t next = (rx_head + 1) & (RX_BUF_SIZE - 1);
        if(next != rx_tail) {
            rx_buf[rx_head] = c;
            rx_head = next;
        }
    }

    if(USART1->STS & (1 << 3)) {
        (void)USART1->DATA;
    }
}
//