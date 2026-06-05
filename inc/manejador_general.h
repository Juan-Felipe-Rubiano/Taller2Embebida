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
 * @brief Procesa un evento unificado y actualiza las máquinas de estado.
 * 
 * Acepta tanto eventos de botón (EV_SOURCE_BUTTON) como eventos directos
 * a una FSM (EV_SOURCE_DIRECT, originados vía UART). En ambos casos aplica
 * los interlocks de seguridad antes de despachar.
 * 
 * @param sev Evento del sistema a procesar.
 */
void manejador_general(SystemEvent sev);