/**
 * @file testigos.h
 * @brief Máquina de estados para el panel de testigos/luces del vehículo.
 * 
 * Controla el encendido de los indicadores de fallo de motor, bajo
 * nivel de aceite o combustible, y el freno de mano.
 */
#pragma once

#include "common.h"

/**
 * @brief Estados posibles de los testigos del panel.
 */
typedef enum {
    ST_OK,              /**< Todo funcionando correctamente (testigos apagados). */
    ST_ENGINE_FAULT,    /**< Testigo de fallo de motor encendido. */
    ST_FUEL_LOW,        /**< Testigo de bajo combustible encendido. */
    ST_OIL_LOW,         /**< Testigo de bajo nivel de aceite encendido. */
    ST_P_BRAKE,         /**< Testigo de freno de mano encendido. */
    STT__N              /**< Número total de estados de los testigos. */
} State_testigos;

/**
 * @brief Eventos del panel de testigos.
 */
typedef enum {
    EV_ENGINE_ERR,      /**< Falla de motor detectada. */
    EV_FUEL_CRIT,       /**< Nivel crítico de combustible alcanzado. */
    EV_OIL_CRIT,        /**< Presión/nivel de aceite crítico. */
    EV_P_BRAKE_ACT,     /**< Freno de mano activado. */
    EV_RESET,           /**< Reset general / limpieza de advertencias. */
    EV_REFUEL,          /**< Reabastecimiento de combustible detectado. */
    EV_MAINTENANCE,     /**< Mantenimiento realizado (reinicio de aceite). */
    EV_P_BRAKE_REL,     /**< Freno de mano liberado. */
    EVT__N              /**< Número total de eventos de testigos. */
} Event_testigos;

/**
 * @brief Tabla de transiciones de la FSM de testigos.
 */
static const Cell table_testigos[STT__N][EVT__N] = {
    [ST_OK] = {
        [EV_ENGINE_ERR] = {ST_ENGINE_FAULT, action_handler},
        [EV_FUEL_CRIT] = {ST_FUEL_LOW, action_handler},
        [EV_OIL_CRIT] = {ST_OIL_LOW, action_handler},
        [EV_P_BRAKE_ACT] = {ST_P_BRAKE, action_handler},
        [EV_RESET] = {ST_OK, NULL},
        [EV_REFUEL] = {ST_OK, NULL},
        [EV_MAINTENANCE] = {ST_OK, NULL},
        [EV_P_BRAKE_REL] = {ST_OK, NULL}
    },
    [ST_ENGINE_FAULT] = {
        [EV_RESET] = {ST_OK, action_handler},
        [EV_ENGINE_ERR] = {ST_ENGINE_FAULT, NULL},
        [EV_FUEL_CRIT] = {ST_ENGINE_FAULT, NULL},
        [EV_OIL_CRIT] = {ST_ENGINE_FAULT, NULL},
        [EV_P_BRAKE_ACT] = {ST_ENGINE_FAULT, NULL},
        [EV_REFUEL] = {ST_ENGINE_FAULT, NULL},
        [EV_MAINTENANCE] = {ST_ENGINE_FAULT, NULL},
        [EV_P_BRAKE_REL] = {ST_ENGINE_FAULT, NULL}
    },
    [ST_FUEL_LOW] = {
        [EV_RESET] = {ST_FUEL_LOW, NULL},
        [EV_ENGINE_ERR] = {ST_FUEL_LOW, NULL},
        [EV_FUEL_CRIT] = {ST_FUEL_LOW, NULL},
        [EV_OIL_CRIT] = {ST_FUEL_LOW, NULL},
        [EV_P_BRAKE_ACT] = {ST_FUEL_LOW, NULL},
        [EV_REFUEL] = {ST_OK, action_handler},
        [EV_MAINTENANCE] = {ST_FUEL_LOW, NULL},
        [EV_P_BRAKE_REL] = {ST_FUEL_LOW, NULL}
    },
    [ST_OIL_LOW] = {
        [EV_RESET] = {ST_OIL_LOW, NULL},
        [EV_ENGINE_ERR] = {ST_OIL_LOW, NULL},
        [EV_FUEL_CRIT] = {ST_OIL_LOW, NULL},
        [EV_OIL_CRIT] = {ST_OIL_LOW, NULL},
        [EV_P_BRAKE_ACT] = {ST_OIL_LOW, NULL},
        [EV_REFUEL] = {ST_OIL_LOW, NULL},
        [EV_MAINTENANCE] = {ST_OK, action_handler},
        [EV_P_BRAKE_REL] = {ST_OIL_LOW, NULL}
    },
    [ST_P_BRAKE] = {
        [EV_RESET] = {ST_P_BRAKE, NULL},
        [EV_ENGINE_ERR] = {ST_P_BRAKE, NULL},
        [EV_FUEL_CRIT] = {ST_P_BRAKE, NULL},
        [EV_OIL_CRIT] = {ST_P_BRAKE, NULL},
        [EV_P_BRAKE_ACT] = {ST_P_BRAKE, NULL},
        [EV_REFUEL] = {ST_P_BRAKE, NULL},
        [EV_MAINTENANCE] = {ST_P_BRAKE, NULL},
        [EV_P_BRAKE_REL] = {ST_OK, action_handler}
    }
};