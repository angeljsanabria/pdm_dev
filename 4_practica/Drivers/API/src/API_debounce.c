/*
 * API_debounce.c
 *
 *  Created on: 26 mar 2026
 *      Author: angel-dev
 */
#include "API_debounce.h"
#include "API_delay.h"
#include "bsp_button.h"

#define DELAY_DEBOUNCE 40


typedef enum{
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING,
} debounceState_t;


static debounceState_t estadoActual = 0;
static bool_t isPress;
static delay_t	delayFSM = {0};
static bool_t changeLed;



bool_t readKey(void){
	isPress = BSP_Button_Read();

	bool_t aux = isPress;

	if(isPress){
		isPress = false;
	}

	return aux;
}


void debounceFSM_init(void){

	estadoActual = BUTTON_UP;
	changeLed = false;
	delayInit(&delayFSM, DELAY_DEBOUNCE);

}


void debounceFSM_update(void){
	// Leer la entrada
	switch (estadoActual) {
		case BUTTON_UP:
			// Condicion de cambio;
			if(BSP_Button_Read()){		// si fue press
				estadoActual = BUTTON_FALLING;
				(void)delayRead(&delayFSM);

			}

		break;
		case BUTTON_FALLING:

			if(delayRead(&delayFSM)){

				if (BSP_Button_Read()){
					estadoActual = BUTTON_DOWN;
				}else{
					estadoActual = BUTTON_UP;
				}
			}

		break;

		case BUTTON_DOWN:
			// Condicion de cambio;
			if(!BSP_Button_Read()){		// si fue press
				estadoActual = BUTTON_RISING;
				delayRead(&delayFSM);
			}
		break;

		case BUTTON_RISING:
			if(delayRead(&delayFSM)){

				if (BSP_Button_Read()){
					estadoActual = BUTTON_UP;
					if(isPress){
						//isPress = false;
						// usar el setter
						// setear el periodo a 500ms
					}
					else{
						//isPress = true;
						// usar el setter
						// setear el periodo a 100ms
					}
				}else{
					estadoActual = BUTTON_DOWN;
				}
			}
		break;

		default:
			estadoActual = BUTTON_UP;

	}

}
