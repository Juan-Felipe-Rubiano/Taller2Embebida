/**
 * @file cola.c
 * @brief Implementación de las funciones de la cola circular.
 */
#include "cola.h"

/**
 * @brief Inicializa una cola circular.
 * 
 * @param q Puntero a la cola.
 */
void queue_init(Queue_p q) {
    q->head = q->tail = q->n = 0;
}

/**
 * @brief Inserta un elemento al final de la cola.
 * 
 * Si la cola no ha alcanzado su capacidad máxima definida por MAX, 
 * inserta el evento y actualiza el índice de escritura (tail) de manera circular.
 * 
 * @param ev Evento a insertar.
 * @param q Puntero a la cola.
 * @return true si la inserción fue exitosa, false de lo contrario.
 */
bool queue_enqueue(EventBtn ev, Queue_p q) {
    if(q->n < MAX){
        q->buffer[q->tail] = ev;
        q->tail = (q->tail + 1) % MAX;
        q->n++;
        return true;
    }
    return false;
}

/**
 * @brief Extrae el elemento al frente de la cola.
 * 
 * Si la cola no está vacía, extrae el evento del frente (head) y
 * actualiza el índice de lectura de manera circular.
 * 
 * @param ev Puntero para retornar el evento extraído.
 * @param q Puntero a la cola.
 * @return true si se extrajo exitosamente, false si la cola estaba vacía.
 */
bool queue_dequeue(EventBtn* ev, Queue_p q) {
    if(q->n > 0){
        *ev = q->buffer[q->head];
        q->head = (q->head + 1) % MAX;
        q->n--;
        return true;
    }
    return false;
}