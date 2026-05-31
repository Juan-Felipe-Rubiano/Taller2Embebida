/**
 * @file manejador_general.h
 * @brief Orquestador principal de las máquinas de estado del sistema.
 * 
 * Contiene la definición de la función que recibe eventos de los
 * periféricos (como botones) y los distribuye a las máquinas de estado
 * correspondientes (manejo, seguridad, cámaras, testigos).
 */
#pragma once

#include "cola.h"
#include "camaras.h"
#include "manejo.h"
#include "seguridad.h"
#include "testigos.h"

/**
 * @brief Procesa un evento de botón y actualiza las máquinas de estado.
 * 
 * Esta función toma un evento extraído de la cola de eventos y
 * determina a qué máquinas de estado debe ser enviado, ejecutando
 * las transiciones necesarias en el sistema general.
 * 
 * @param ev Evento de botón a procesar.
 */
void manejador_general(EventBtn ev);