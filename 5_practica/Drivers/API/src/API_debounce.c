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


static debounceState_t estadoActual;
static bool_t isPress;
static delay_t	delayFSM = {0};


static void setIsPress(bool_t v)
{
	isPress = v;
}


bool_t readKey(void)
{
	bool_t out = isPress;
	if (isPress) {
		isPress = false;
	}
	return out;
}


void debounceFSM_init(void){

	estadoActual = BUTTON_UP;
	isPress = false;
	delayInit(&delayFSM, DELAY_DEBOUNCE);

}


void debounceFSM_update(void){

	switch (estadoActual) {
		case BUTTON_UP:
			// Condicion de cambio;
			if(BSP_Button_Read()){		// si fue press
				estadoActual = BUTTON_FALLING;
				(void)delayRead(&delayFSM);

			}

		break;
		case BUTTON_FALLING:
			// Condiciones de cambio;
			if(delayRead(&delayFSM)){
				if (BSP_Button_Read()){
					estadoActual = BUTTON_DOWN;
					setIsPress(true);
					buttonPressed();
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
			// Condiciones de cambio;
			if(delayRead(&delayFSM)){
				if (BSP_Button_Read()){
					estadoActual = BUTTON_DOWN;
				}else{
					estadoActual = BUTTON_UP;
					setIsPress(false);
					buttonReleased();
				}
			}
		break;

		default:
			estadoActual = BUTTON_UP;
			break;
	}
	return;
}
