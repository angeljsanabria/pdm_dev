/*
 * API_debounce.h
 *
 *  Created on: 26 mar 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_delay.h"

/**
 * @brief
 * @param
 * @param
 */
void debounceFSM_init(void);		// debe cargar el estado inicial


/**
 * @brief
 * @param
 * @param
 */
void debounceFSM_update(void);		// debe leer las entradas, resolver la lógica de
								// transición de estados y actualizar las salidas

bool_t readKey(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
