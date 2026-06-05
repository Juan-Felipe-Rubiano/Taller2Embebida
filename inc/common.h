/**
 * @file common.h
 * @brief Estructuras y definiciones comunes para las máquinas de estado.
 * 
 * Contiene la definición de los tipos fundamentales utilizados por
 * todas las máquinas de estado finito (FSM) del sistema, como
 * las acciones, las celdas de transición y el despachador.
 */
#pragma once
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Puntero a función que representa una acción de transición.
 * @param name Nombre de la FSM o contexto que ejecuta la acción.
 */
typedef void (*Action)(const char* name);

/**
 * @brief Celda de transición para una FSM.
 * 
 * Define el siguiente estado y la acción a ejecutar cuando ocurre
 * un evento específico en el estado actual.
 */
typedef struct {
    int next;   /**< Siguiente estado tras procesar el evento. */
    Action act; /**< Acción a ejecutar en la transición (puede ser NULL). */
} Cell;

/**
 * @brief Estructura de la máquina de estados finitos (FSM).
 * 
 * Almacena el contexto, dimensiones de la tabla de transiciones
 * y el estado actual de la máquina.
 */
typedef struct {
    const char* name;    /**< Nombre o identificador de la máquina de estados. */
    int current_state;   /**< Estado actual en el que se encuentra la FSM. */
    int n;               /**< Número total de estados. */
    int m;               /**< Número total de eventos. */
    const Cell *table;   /**< Puntero a la tabla de transición aplanada. */
} FSM;


/**
 * @brief Procesa un evento en la máquina de estados.
 * 
 * @param fsm Puntero a la máquina de estados.
 * @param ev Evento a procesar.
 */
void fsm_dispatch(FSM* fsm, int ev);

/**
 * @brief Manejador de acción por defecto.
 * 
 * Función genérica utilizada cuando no se requiere una lógica compleja.
 * 
 * @param name Nombre de la FSM.
 */
void action_handler(const char* name);