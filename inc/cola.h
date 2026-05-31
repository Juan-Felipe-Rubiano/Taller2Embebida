/**
 * @file cola.h
 * @brief Implementación de una cola (FIFO) para el manejo de eventos.
 * 
 * Estructura de datos tipo cola circular que permite almacenar los
 * eventos generados por interrupciones o polling para ser procesados
 * posteriormente por las máquinas de estados.
 */
#pragma once
#include "stdint.h"
#include <stdbool.h>

/** 
 * @brief Tamaño máximo del búfer de la cola. 
 */
#define MAX 16

/**
 * @brief Eventos del sistema que pueden ser encolados.
 */
typedef enum {
    ACCEL_PRESS,    /**< Evento: Pedal de acelerador presionado. */
    ACCEL_RELEASE,  /**< Evento: Pedal de acelerador liberado. */
    BRAKE_PRESS,    /**< Evento: Pedal de freno presionado. */
    BRAKE_RELEASE,  /**< Evento: Pedal de freno liberado. */
    REV_TOGGLE,     /**< Evento: Alternar modo de reversa. */
    SEG_TOGGLE      /**< Evento: Alternar sistema de seguridad/armado. */
} EventBtn;

/**
 * @brief Estructura de la cola circular.
 */
typedef struct {
    EventBtn buffer[MAX];  /**< Búfer circular para almacenar los eventos. */
    uint8_t head;       /**< Índice de lectura (cabeza de la cola). */
    uint8_t tail;       /**< Índice de escritura (cola de la cola). */
    uint8_t n;          /**< Número actual de elementos en la cola. */
} Queue_t;

/**
 * @brief Puntero a la estructura de la cola circular.
 */
typedef Queue_t* Queue_p;

/**
 * @brief Inicializa una cola circular.
 * 
 * Restablece los índices y el contador de elementos a cero.
 * 
 * @param q Puntero a la cola a inicializar.
 */
void queue_init(Queue_p q);

/**
 * @brief Encola un nuevo evento en la cola circular.
 * 
 * @param ev Evento a encolar.
 * @param q Puntero a la cola.
 * @return true si el evento se encoló exitosamente, false si la cola está llena.
 */
bool queue_enqueue(EventBtn ev, Queue_p q);

/**
 * @brief Desencola un evento de la cola circular.
 * 
 * @param ev Puntero donde se almacenará el evento extraído.
 * @param q Puntero a la cola.
 * @return true si se extrajo un evento, false si la cola está vacía.
 */

bool queue_dequeue(EventBtn* ev, Queue_p q);