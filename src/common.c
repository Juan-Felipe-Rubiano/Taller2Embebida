#include "common.h"
/* --- Acciones Generales --- */
void action_handler(const char* name) {
    
}

/* --- Motor de Despacho --- */
void fsm_dispatch(FSM* fsm, int ev) {
    if (ev >= fsm-> m || fsm->current_state >= fsm->n) return;
 
    const Cell *c = &fsm->table[fsm->current_state * fsm->m +ev];
   
    fsm->current_state = c->next;
    if (c->act) {
        c->act(fsm->name);
    }
}