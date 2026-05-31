/**
 * @file system_util.h
 * @brief Utilidades del sistema, configuración y rutinas de actualización.
 * 
 * Define constantes para representar los caracteres en el display
 * de 7 segmentos y declara las funciones principales de configuración,
 * lectura de pines (con anti-rebote) y bucle de actualización.
 */
#pragma once
#include "apm32f10x.h"
#include "apm32_config.h"
#include "cola.h"
#include "manejador_general.h"

/**
 * @name Códigos de caracteres para el display de 7 segmentos
 * @{
 */
#define MARCHA_P 0x73   /**< Representación del carácter 'P' (Parking). */
#define MARCHA_r 0x50   /**< Representación del carácter 'r' (Reversa). */
#define MARCHA_A 0x77   /**< Representación del carácter 'A' (Avance/Drive). */
/** @} */

/**
 * @brief Realiza lectura de un botón con filtro anti-rebote (debounce).
 * 
 * @param pin Número del pin (GPIO) a leer.
 * @return 1 si el botón fue presionado de manera estable, 0 en caso contrario.
 */
uint8_t debounce(uint16_t pin);

/**
 * @brief Lee el estado de los botones y encola eventos.
 * 
 * Verifica si algún botón ha sido presionado (utilizando debounce)
 * y, de ser así, inserta el evento correspondiente en la cola.
 * 
 * @param q Puntero a la cola donde se almacenarán los eventos.
 */
void enqueue_btn_event(Queue_p q);

/**
 * @brief Bucle principal de actualización del sistema.
 * 
 * Se encarga de procesar los eventos pendientes en la cola,
 * actualizar las pantallas, testigos luminosos y estado general.
 */
void update_system(void);

/**
 * @brief Configuración inicial del hardware.
 * 
 * Inicializa los relojes, periféricos (GPIO, timers, etc.)
 * y establece los estados iniciales del sistema.
 */
void config(void);