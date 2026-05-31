/**
 * @file manejo.h
 * @brief Máquina de estados para el sistema de tracción y manejo.
 * 
 * Controla los modos de conducción del vehículo, como la aceleración,
 * control de crucero, frenado y reversa.
 */
#pragma once

#include "common.h"

/**
 * @brief Estados posibles de conducción.
 */
typedef enum {
    ST_IDLE,    /**< Vehículo detenido o en ralentí. */
    ST_ACCEL,   /**< Vehículo acelerando. */
    ST_CRUISE,  /**< Velocidad de crucero activa. */
    ST_BRAKE,   /**< Vehículo frenando. */
    ST_REV,     /**< Vehículo en reversa. */
    STD__N      /**< Número total de estados de manejo. */
} State_drive;

/**
 * @brief Eventos relacionados con la conducción.
 */
typedef enum {
    EV_GAS_PRESS,       /**< Pedal de acelerador presionado. */
    EV_GAS_RELEASE,     /**< Pedal de acelerador liberado. */
    EV_BRAKE_PRESS,     /**< Pedal de freno presionado. */
    EV_BRAKE_RELEASE,   /**< Pedal de freno liberado. */
    EV_REV_TOGGLE,      /**< Alternar modo de reversa. */
    EVD__N              /**< Número total de eventos de manejo. */
} Event_drive;

/**
 * @brief Tabla de transiciones de la FSM de manejo.
 */
static const Cell table_drive[STD__N][EVD__N] = {
    [ST_IDLE] = {
        [EV_GAS_PRESS] = {ST_ACCEL, action_handler},
        [EV_REV_TOGGLE] = {ST_REV, action_handler},
        [EV_GAS_RELEASE] = {ST_IDLE, NULL},
        [EV_BRAKE_PRESS] = {ST_IDLE, NULL},
        [EV_BRAKE_RELEASE] = {ST_IDLE, NULL},
    },
    [ST_ACCEL] = {
        [EV_GAS_RELEASE] = {ST_CRUISE, action_handler},
        [EV_BRAKE_PRESS] = {ST_BRAKE, action_handler},
        [EV_GAS_PRESS] = {ST_ACCEL, NULL},
        [EV_BRAKE_RELEASE] = {ST_ACCEL, NULL},
        [EV_REV_TOGGLE] = {ST_ACCEL, NULL}
    },
    [ST_CRUISE] = {
        [EV_BRAKE_PRESS] = {ST_BRAKE, action_handler},
        [EV_GAS_PRESS] = {ST_ACCEL, action_handler},
        [EV_GAS_RELEASE] = {ST_CRUISE, NULL},
        [EV_BRAKE_RELEASE] = {ST_CRUISE, NULL},
        [EV_REV_TOGGLE] = {ST_CRUISE, NULL}
    },
    [ST_BRAKE] = {
        [EV_BRAKE_RELEASE] = {ST_IDLE, action_handler},
        [EV_GAS_PRESS] = {ST_BRAKE, NULL},
        [EV_GAS_RELEASE] = {ST_BRAKE, NULL},
        [EV_BRAKE_PRESS] = {ST_BRAKE, NULL},
        [EV_REV_TOGGLE] = {ST_BRAKE, NULL}
    },
    [ST_REV] = {
        [EV_REV_TOGGLE] = {ST_IDLE, action_handler},
        [EV_GAS_PRESS] = {ST_REV, NULL},
        [EV_GAS_RELEASE] = {ST_REV, NULL},
        [EV_BRAKE_PRESS] = {ST_REV, NULL},
        [EV_BRAKE_RELEASE] = {ST_REV, NULL},
    }
};