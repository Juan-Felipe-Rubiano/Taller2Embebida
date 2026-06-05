/**
 * @file manejador_general.c
 * @brief Orquestador central de eventos: distribuye cada EventBtn a las FSMs correspondientes
 *        y aplica las reglas de interbloqueo del sistema.
 */
#include "manejador_general.h"

extern FSM f_manejo;
extern FSM f_seguridad;
extern FSM f_camaras;
extern FSM f_testigos;


/**
 * @brief Procesa un evento unificado y actualiza las FSMs del sistema.
 * @param sev Evento del sistema extraido de la cola.
 */
void manejador_general(SystemEvent sev){

    // Para eventos enviados con UART
    if(sev.source == EV_SOURCE_UART) {
        FSM* fsm = sev.direct.fsm;
        int ev = sev.direct.ev;

        if(fsm == &f_manejo && f_seguridad.current_state == ST_ALARM_ACTIVE) {
            printf("[MNGR] Bloqueado: alarma activa, no se puede controlar el manejo.\r\n");
            return;
        }
        if(fsm == &f_camaras && f_seguridad.current_state == ST_ARMED) {
            printf("[MNGR] Bloqueado: sistema armado, camaras deshabilitadas.\r\n");
            return;
        }
        if(fsm == &f_seguridad && ev == EV_LOCK && f_manejo.current_state != ST_IDLE) {
            printf("[MNGR] Bloqueado: vehiculo en movimiento, no se puede armar.\r\n");
            return;
        }

        fsm_dispatch(fsm, ev);

        if(f_seguridad.current_state == ST_ALARM_ACTIVE && f_manejo.current_state != ST_IDLE)
            f_manejo.current_state = ST_IDLE;
        if(f_seguridad.current_state == ST_ARMED && f_camaras.current_state != ST_CAM_OFF)
            f_camaras.current_state = ST_CAM_OFF;
        return;
    }

    // Forma anterior, responde a botones reales
    EventBtn ev = sev.btn;

    if(ev == ACCEL_PRESS) {
        if(f_seguridad.current_state == ST_ARMED) {
            fsm_dispatch(&f_seguridad, EV_INTRUSION_L1);
            return;
        }
        if(f_seguridad.current_state == ST_PRE_ALARM) {
            fsm_dispatch(&f_seguridad, EV_INTRUSION_L2);
            return;
        }
    }

    switch(ev){
        case ACCEL_PRESS:
            if(f_seguridad.current_state == ST_ALARM_ACTIVE || f_testigos.current_state == ST_ENGINE_FAULT) break;
            fsm_dispatch(&f_manejo, EV_GAS_PRESS);
            break;            
        case ACCEL_RELEASE:
            fsm_dispatch(&f_manejo, EV_GAS_RELEASE);
            break;
        case BRAKE_PRESS:
            fsm_dispatch(&f_manejo, EV_BRAKE_PRESS);
            break;
        case BRAKE_RELEASE:
            fsm_dispatch(&f_manejo, EV_BRAKE_RELEASE);
            break;
        case REV_TOGGLE:
            if(f_seguridad.current_state != ST_ALARM_ACTIVE){
                if(f_manejo.current_state == ST_IDLE){
                    fsm_dispatch(&f_manejo, EV_REV_TOGGLE);
                    if(f_manejo.current_state == ST_REV) fsm_dispatch(&f_camaras, EV_GEAR_REV);
                }else if(f_manejo.current_state == ST_REV){
                    fsm_dispatch(&f_manejo, EV_REV_TOGGLE);
                    fsm_dispatch(&f_camaras, EV_GEAR_DRIVE);
                } 
            }
            break;
        case SEG_TOGGLE:
            if(f_seguridad.current_state == ST_DISARMED){
                if(f_manejo.current_state == ST_IDLE) fsm_dispatch(&f_seguridad, EV_LOCK);
            } 
            else{
                fsm_dispatch(&f_seguridad, EV_UNLOCK);
            } 
            break;
    }

    if(f_seguridad.current_state == ST_ALARM_ACTIVE && f_manejo.current_state != ST_IDLE)
        f_manejo.current_state = ST_IDLE;
    if(f_seguridad.current_state == ST_ARMED && f_camaras.current_state != ST_CAM_OFF)
        f_camaras.current_state = ST_CAM_OFF;
}