#include "common.h"
#include <stdio.h>
#include <string.h>

//Para imprimir en UART
const char* state_str_manejo[] = {"IDLE", "ACCEL", "CRUISE", "BRAKE", "REV"};
const char* state_str_seguridad[] = {"DISARMED", "ARMED", "PRE_ALARM", "ALARM_ACTIVE"};
const char* state_str_camaras[] = {"CAM_OFF", "REAR_VIEW", "FRONT_VIEW", "COLLISION_WARN"};
const char* state_str_testigos[] = {"OK", "ENGINE_FAULT", "FUEL_LOW", "OIL_LOW", "P_BRAKE"};

const char* event_str_manejo[] = {"GAS_PRESS", "GAS_RELEASE", "BRAKE_PRESS", "BRAKE_RELEASE", "REV_TOGGLE"};
const char* event_str_seguridad[] = {"LOCK", "UNLOCK", "INTRUSION_L1", "INTRUSION_L2", "SEG_TIMEOUT"};
const char* event_str_camaras[] = {"GEAR_REV", "GEAR_DRIVE", "OBJ_FRONT", "OBJ_CRIT", "CAM_TIMEOUT"};
const char* event_str_testigos[] = {"ENGINE_ERR", "FUEL_CRIT", "OIL_CRIT", "P_BRAKE_ACT", "RESET", "REFUEL", "MAINTENANCE", "P_BRAKE_REL"};

/* --- Acciones Generales --- */
void action_handler(const char* name) {
    
}

/* --- Motor de Despacho --- */
void fsm_dispatch(FSM* fsm, int ev) {
    if (ev >= fsm-> m || fsm->current_state >= fsm->n) return;

    int cur = fsm->current_state;
 
    const Cell *c = &fsm->table[fsm->current_state * fsm->m +ev];
   
    fsm->current_state = c->next;

    //Más de impresiones

    const char* str_ev = "N/A";
    const char* str_cur = "N/A";
    const char* str_new = "N/A";

    if(strcmp(fsm->name, "MANEJO") == 0) {
        str_ev = event_str_manejo[ev];
        str_cur = state_str_manejo[cur];
        str_new = state_str_manejo[fsm->current_state];
    }
    else if(strcmp(fsm->name, "SEGURIDAD") == 0) {
        str_ev = event_str_seguridad[ev];
        str_cur = state_str_seguridad[cur];
        str_new = state_str_seguridad[fsm->current_state];
    }
    else if(strcmp(fsm->name, "TESTIGOS") == 0) {
        str_ev = event_str_testigos[ev];
        str_cur = state_str_testigos[cur];
        str_new = state_str_testigos[fsm->current_state];
    }
    else if(strcmp(fsm->name, "CAMARAS") == 0) {
        str_ev = event_str_camaras[ev];
        str_cur = state_str_camaras[cur];
        str_new = state_str_camaras[fsm->current_state];
    }

    printf("[FSM %s]: Evento detectado: %s , Estado anterior: %s, Nuevo estado: %s\n", fsm->name, str_ev, str_cur, str_new);

    if (c->act) {
        c->act(fsm->name);
    }
}