/**
 * @file seguridad.h
 * @brief Máquina de estados para el sistema de alarma y seguridad.
 * 
 * Gestiona el armado, desarmado, y alertas del sistema contra intrusiones.
 */
#pragma once

#include "common.h"

/**
 * @brief Estados posibles del sistema de seguridad.
 */
typedef enum {
    ST_DISARMED,      /**< Sistema desarmado. */
    ST_ARMED,         /**< Sistema armado. */
    ST_PRE_ALARM,     /**< Pre-alarma (intrusión menor). */
    ST_ALARM_ACTIVE,  /**< Alarma disparada (intrusión mayor). */
    STS__N            /**< Número total de estados de seguridad. */
} State_seguridad;

/**
 * @brief Eventos del sistema de seguridad.
 */
typedef enum {
    EV_LOCK,            /**< Comando de bloqueo/armado. */
    EV_UNLOCK,          /**< Comando de desbloqueo/desarmado. */
    EV_INTRUSION_L1,    /**< Detección de intrusión nivel 1. */
    EV_INTRUSION_L2,    /**< Detección de intrusión nivel 2 (grave). */
    EV_SEG_TIMEOUT,         /**< Tiempo de espera agotado. */
    EVS__N              /**< Número total de eventos de seguridad. */
} Event_seguridad;

/**
 * @brief Tabla de transiciones de la FSM de seguridad.
 */
static const Cell table_seguridad[STS__N][EVS__N] = {
    [ST_DISARMED] = {
        [EV_LOCK] = {ST_ARMED, action_handler},
        [EV_UNLOCK] = {ST_DISARMED, NULL},
        [EV_INTRUSION_L1] = {ST_DISARMED, NULL},
        [EV_INTRUSION_L2] = {ST_DISARMED, NULL},
        [EV_SEG_TIMEOUT] = {ST_DISARMED, NULL}
    },
    [ST_ARMED] = {
        [EV_UNLOCK] = {ST_DISARMED, action_handler},
        [EV_INTRUSION_L1] = {ST_PRE_ALARM, action_handler},
        [EV_INTRUSION_L2] = {ST_ALARM_ACTIVE, action_handler},
        [EV_LOCK] = {ST_ARMED, NULL},
        [EV_SEG_TIMEOUT] = {ST_ARMED, NULL}
    },
    [ST_PRE_ALARM] = {
        [EV_SEG_TIMEOUT] = {ST_ARMED, action_handler},
        [EV_INTRUSION_L2] = {ST_ALARM_ACTIVE, action_handler},
        [EV_UNLOCK] = {ST_PRE_ALARM, NULL},
        [EV_INTRUSION_L1] = {ST_PRE_ALARM, NULL},
        [EV_LOCK] = {ST_PRE_ALARM, NULL}
    },
    [ST_ALARM_ACTIVE] = {
        [EV_UNLOCK] = {ST_DISARMED, action_handler},
        [EV_SEG_TIMEOUT] = {ST_ALARM_ACTIVE, NULL},
        [EV_LOCK] = {ST_ALARM_ACTIVE, NULL},
        [EV_INTRUSION_L1] = {ST_ALARM_ACTIVE, NULL},
        [EV_INTRUSION_L2] = {ST_ALARM_ACTIVE, NULL}
    }
};