/**
 * @file camaras.h
 * @brief Máquina de estados para el control de cámaras del vehículo.
 * 
 * Administra las diferentes vistas (trasera, frontal) y advertencias
 * de colisión en función de la marcha y obstáculos detectados.
 */
#pragma once

#include "common.h"

/**
 * @brief Estados posibles de la máquina de cámaras.
 */
typedef enum {
    ST_CAM_OFF,         /**< Cámaras apagadas. */
    ST_REAR_VIEW,       /**< Vista trasera activada. */
    ST_FRONT_VIEW,      /**< Vista frontal activada. */
    ST_COLLISION_WARN,  /**< Advertencia de colisión inminente. */
    STC__N              /**< Número total de estados de la cámara. */
} State_camaras;

/**
 * @brief Eventos que disparan transiciones en las cámaras.
 */
typedef enum {
    EV_GEAR_REV,        /**< Vehículo en reversa. */
    EV_GEAR_DRIVE,      /**< Vehículo en marcha hacia adelante. */
    EV_OBJ_FRONT,       /**< Objeto detectado al frente. */
    EV_OBJ_CRIT,        /**< Objeto en distancia crítica. */
    EV_CAM_TIMEOUT,         /**< Tiempo de espera agotado. */
    EVC__N              /**< Número total de eventos. */
} Event_camaras;

/**
 * @brief Tabla de transiciones de la FSM de cámaras.
 */
static const Cell table_camaras[STC__N][EVC__N] = {
    [ST_CAM_OFF] = {
        [EV_GEAR_REV] = {ST_REAR_VIEW, action_handler},
        [EV_OBJ_FRONT] = {ST_FRONT_VIEW, action_handler},
        [EV_GEAR_DRIVE] = {ST_CAM_OFF, NULL},
        [EV_OBJ_CRIT] = {ST_CAM_OFF, NULL},
        [EV_CAM_TIMEOUT] = {ST_CAM_OFF, NULL}
    },
    [ST_REAR_VIEW] = {
        [EV_GEAR_DRIVE] = {ST_CAM_OFF, action_handler},
        [EV_OBJ_CRIT] = {ST_COLLISION_WARN, action_handler},
        [EV_GEAR_REV] = {ST_REAR_VIEW, NULL},
        [EV_OBJ_FRONT] = {ST_REAR_VIEW, NULL},
        [EV_CAM_TIMEOUT] = {ST_CAM_OFF, action_handler}
    },
    [ST_FRONT_VIEW] = {
        [EV_GEAR_DRIVE] = {ST_CAM_OFF, action_handler},
        [EV_OBJ_FRONT] = {ST_COLLISION_WARN, action_handler},
        [EV_GEAR_REV] = {ST_FRONT_VIEW, NULL},
        [EV_OBJ_CRIT] = {ST_COLLISION_WARN, action_handler},
        [EV_CAM_TIMEOUT] = {ST_FRONT_VIEW, NULL}
    },
    [ST_COLLISION_WARN] = {
        [EV_CAM_TIMEOUT] = {ST_CAM_OFF, action_handler},
        [EV_GEAR_REV] = {ST_COLLISION_WARN, NULL},
        [EV_GEAR_DRIVE] = {ST_COLLISION_WARN, NULL},
        [EV_OBJ_FRONT] = {ST_COLLISION_WARN, NULL},
        [EV_OBJ_CRIT] = {ST_COLLISION_WARN, NULL}
    }
};