/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "stdio.h"
#include "core_cm4.h"   /* Para ITM_SendChar */

#include "bsp.h"
#include "clock_config.h"


/**
 * @brief Descomentar #define PUNTO_3 para habilitar la feature del punto 3.
 */
#define PUNTO_3

#ifdef PUNTO_3
#define REPEAT_MAX 5
#define PATRONES_MAX 3
#endif


int _write(int le, char *ptr, int len)
{
int DataIdx;
for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
	ITM_SendChar(*ptr++);
	}
return len;
}


// Funciones de delay no bloqueante
void delayInit( delay_t * delay, tick_t duration ){
	if(delay == NULL) 	return;

	delay->running = false;
	delay->duration = duration;
	delay->startTime = 0;

	return;
}

bool_t delayRead( delay_t * delay ){
	if(delay == NULL) 	return false;

	if(delay->running){
		if((HAL_GetTick() - delay->startTime) >= delay->duration){
			delay->running = false;
			return true;
		}
	}else{
		delay->running = true;
		delay->startTime = HAL_GetTick();
	}

	return false;
}

void delayWrite( delay_t * delay, tick_t duration ){
	if(delay == NULL) 	return;

	delay->duration = duration;

	return;
}

// Usa por defecto la freq HAL_TICK_FREQ_1KHZ -> 1ms de tick
enum delayTicksMs{
	delayTicks_100ms	= 100,
	delayTicks_200ms	= 200,
	delayTicks_1s		= 1000,
};


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{


	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	// Ver si se esta inciando HAL_InitTick()


	/* Configure the system clock to 120MHz */
	Clock_Config();


	/* Initialize Board Support Package (LEDs, buttons, etc.) */
	BSP_Init();

	printf("INICIADO \r\n");

	delay_t	delayL1 = {0};
	bool tout = true;		// indica un timeout de delay

	delayInit(&delayL1, delayTicks_100ms);
#ifdef PUNTO_3
	uint8_t repeat = REPEAT_MAX;
	uint8_t patrones = PATRONES_MAX;
	const tick_t  arrDelays[PATRONES_MAX] = {delayTicks_1s, delayTicks_200ms, delayTicks_100ms};
#endif

	while (1)
	{
#ifdef PUNTO_3
		if(tout){
			tout = false;
			if(repeat >= (REPEAT_MAX*2)){		// *2 por encendido y apagado
				repeat = 0;
				if(patrones >= PATRONES_MAX){
					patrones = 0;
				}
				delayWrite(&delayL1, arrDelays[patrones]);
				patrones++;
			}
			repeat++;
		}
#endif
		if(delayRead(&delayL1)){
			tout = true;
			BSP_LED_Toggle(BSP_LED2);
		}

	}

	return 0;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
