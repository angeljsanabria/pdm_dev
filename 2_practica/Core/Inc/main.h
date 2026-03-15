/**
 * @file 	main.h
 * @author 	Angel
 * @brief 	Delays no bloqueantes
 * @version 0.1
 * @date 	2026-03-15
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


#include <stdint.h>
#include <stdbool.h>


typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Estructura de control para delays no bloqueantes.
 */
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;


/**
 * @brief Inicializa una estructura de delay.
 * @param delay Puntero a la estructura delay_t a inicializar.
 * @param duration Duración del delay en ticks.
 */
void delayInit( delay_t * delay, tick_t duration );

/**
 * @brief Comprueba si el tiempo del delay ha expirado.
 * @param delay Puntero a la estructura de delay.
 * @return bool_t Retorna true si el tiempo expiró, false en caso contrario.
 */
bool_t delayRead( delay_t * delay );

/**
 * @brief Actualiza la duración de un delay existente.
 * @param delay Puntero a la estructura de delay.
 * @param duration Nueva duración en ticks.
 */
void delayWrite( delay_t * delay, tick_t duration );

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);



#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
