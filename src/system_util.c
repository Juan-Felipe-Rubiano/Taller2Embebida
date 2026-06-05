/**
 * @file system_util.c
 * @brief Utilidades del sistema: debounce, lectura de botones, actualizacion de LEDs, displays y alertas.
 */
#include "system_util.h"
#include <stdlib.h>

extern volatile uint32_t msTicks;

extern FSM f_manejo;
extern FSM f_seguridad;
extern FSM f_camaras;
extern FSM f_testigos;

const uint8_t nums[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};


/**
 * @brief Filtra el rebote mecanico de un boton en el pin de GPIOA indicado.
 * @param pin Numero de pin (0-15) a leer en GPIOA.
 * @return 1 si se detecto pulsacion, 2 si se detecto soltura, 0 si no hubo cambio.
 */
uint8_t debounce(uint16_t pin){
    static uint16_t estado_boton = 0xFFFF;//0 si presionado
    static uint32_t ultimo_cambio[16] = {0};
    uint8_t presionado = !(GPIOA->IDATA & (1 << pin));//1 si presionado
    uint8_t soltado = (estado_boton & (1 << pin)) != 0;
    if(presionado){
        if(soltado && (msTicks - ultimo_cambio[pin]) > 20){
            estado_boton &= ~(1 << pin);//marca como presionado
            ultimo_cambio[pin] = msTicks;
            return 1;
        }
    } else {
        if(!soltado && (msTicks - ultimo_cambio[pin]) > 20){
            estado_boton |= (1 << pin);//marca como soltado
            ultimo_cambio[pin] = msTicks;
            return 2; //soltado
        }
    }
    return 0;//no espichado
}


/**
 * @brief Lee los 4 botones fisicos y encola los eventos correspondientes.
 * @param q Puntero a la cola donde se insertaran los eventos detectados.
 */
void enqueue_btn_event(Queue_p q){
    //Casteo a SystemEvent a cada evento de boton con la fuente fisica, diferencia de evento UART
    uint8_t btn_accel = debounce(10);
    if(btn_accel == 1) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=ACCEL_PRESS}, q);
    else if(btn_accel == 2) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=ACCEL_RELEASE}, q);

    uint8_t btn_brake = debounce(11);
    if(btn_brake == 1) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=BRAKE_PRESS}, q);
    else if(btn_brake == 2) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=BRAKE_RELEASE}, q);

    uint8_t btn_rev = debounce(12);
    if(btn_rev == 1) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=REV_TOGGLE}, q);

    uint8_t btn_seg = debounce(15);
    if(btn_seg == 1) queue_enqueue((SystemEvent){.source=EV_SOURCE_BUTTON, .btn=SEG_TOGGLE}, q);
}


/**
 * @brief Bucle de actualizacion del sistema ejecutado en cada ciclo del while(1).
 */
void update_system(void){
    //Bateria
    static uint32_t tiempo_bateria = 0;
    static float carga_bateria = 99.0f;

    if(msTicks - tiempo_bateria >= 300) {
        tiempo_bateria = msTicks;
        if(f_manejo.current_state == ST_ACCEL || f_manejo.current_state == ST_REV) carga_bateria -= 1.0f;
        else if(f_manejo.current_state == ST_CRUISE) carga_bateria -= 0.5f;
        else if(f_manejo.current_state == ST_BRAKE) carga_bateria += 1.5f;
        else if(f_manejo.current_state == ST_IDLE) carga_bateria -= 0.05f;
    }
    if(carga_bateria < 0.0f) carga_bateria = 0.0f;
    if(carga_bateria > 99.0f) carga_bateria = 99.0f;

    if(carga_bateria <= 20.0f && f_testigos.current_state == ST_OK)
        fsm_dispatch(&f_testigos, EV_FUEL_CRIT);
    else if(carga_bateria > 20.0f && f_testigos.current_state == ST_FUEL_LOW)
        fsm_dispatch(&f_testigos, EV_REFUEL);
    static uint32_t entrada_pre_alarm = 0;
    static int ultimo_estado_seg = -1;
    if(f_seguridad.current_state != ultimo_estado_seg) {
        if(f_seguridad.current_state == ST_PRE_ALARM) entrada_pre_alarm = msTicks;
        ultimo_estado_seg = f_seguridad.current_state;
    }
    if(f_seguridad.current_state == ST_PRE_ALARM && (msTicks - entrada_pre_alarm >= 5000))
        fsm_dispatch(&f_seguridad, EV_SEG_TIMEOUT);

    static uint32_t ultimo_chequeo_motor = 0;
    if(msTicks - ultimo_chequeo_motor >= 5000) {
        ultimo_chequeo_motor = msTicks;
        if(f_testigos.current_state == ST_OK && (rand() % 100 < 10))
            fsm_dispatch(&f_testigos, EV_ENGINE_ERR);
    }

    static uint32_t entrada_falla_motor = 0;
    static int ultimo_estado_test = -1;
    if(f_testigos.current_state != ultimo_estado_test) {
        if(f_testigos.current_state == ST_ENGINE_FAULT) entrada_falla_motor = msTicks;
        ultimo_estado_test = f_testigos.current_state;
    }
    if(f_testigos.current_state == ST_ENGINE_FAULT && (msTicks - entrada_falla_motor >= 10000))
        fsm_dispatch(&f_testigos, EV_RESET);

    //Leds
    uint8_t leds = 0;
    uint8_t parpadeo_lento = (msTicks / 500) % 2;
    uint8_t parpadeo_rapido = (msTicks / 150) % 2;
    if(f_seguridad.current_state == ST_ARMED) {
        if(parpadeo_lento) leds |= 0x03;
    }
    else if(f_seguridad.current_state == ST_PRE_ALARM) {
        if(parpadeo_rapido) leds |= 0x03;
    }
    else if(f_seguridad.current_state == ST_ALARM_ACTIVE) leds |= 0x03;
    
    if(f_testigos.current_state == ST_ENGINE_FAULT) leds |= 0x04;
    if(f_testigos.current_state == ST_FUEL_LOW) leds |= 0x08;

    if(f_manejo.current_state == ST_ACCEL || f_manejo.current_state == ST_REV) leds |= 0x10;

    if(f_manejo.current_state == ST_BRAKE) leds |= 0x20;

    GPIOB->ODATA &= ~((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15));
    GPIOB->ODATA |= ((leds & 0x0F) << 12);
    GPIOA->ODATA &= ~((1 << 8) | (1 << 9));
    GPIOA->ODATA |= (((leds >> 4) & 0x03) << 8);

    //7 segmentos
    uint8_t marcha_act = MARCHA_P;
    if(f_manejo.current_state == ST_ACCEL || f_manejo.current_state == ST_CRUISE) marcha_act = MARCHA_A;
    if(f_manejo.current_state == ST_REV) marcha_act = MARCHA_r;

    uint8_t carga_redondeada = (uint8_t)(carga_bateria / 10.0f);
    if(carga_redondeada > 9) carga_redondeada = 9;
    uint8_t bateria = nums[carga_redondeada];

    GPIOB->ODATA |= (1 << 8) | (1 << 9);
    GPIOA->ODATA = (GPIOA->ODATA & ~0xFF) | (~bateria & 0xFF);
    GPIOB->ODATA &= ~(1 << 8);
    delay_ms(5);

    GPIOB->ODATA |= (1 << 8) | (1 << 9);
    GPIOA->ODATA = (GPIOA->ODATA & ~0xFF) | (~marcha_act & 0xFF);
    GPIOB->ODATA &= ~(1 << 9);
    delay_ms(5);

    GPIOB->ODATA |= (1 << 8) | (1 << 9);
}


/**
 * @brief Configuracion inicial de GPIO y perifericos del sistema.
 */
void config(void){
    RCM->APB2CLKEN |= (1 << 2) | (1 << 3) | (1 << 0);
    AFIO->REMAP1_B.SWJCFG = 0x2; //Desactivar JTAG real, sin afectar SWD;
    GPIOA->CFGLOW = 0x33333333; // PA0-7
    GPIOA->CFGHIG = 0x88888833; // PA8-15
    GPIOB->CFGHIG = 0x33334433; // PB8-15
    //botones
    GPIOA->ODATA |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 15);
}