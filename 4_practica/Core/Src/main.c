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
#include "API_delay.h"
#include "API_debounce.h"

/**
 * @brief Definiciones de patrones y repeticiones de cada uno
 */
#define PATRONES_MAX 4
#define REPEAT_MAX 1



int _write(int le, char *ptr, int len)
{
int DataIdx;
for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
	ITM_SendChar(*ptr++);
	}
return len;
}



// Usa por defecto la freq HAL_TICK_FREQ_1KHZ -> 1ms de tick
//enum delayTicksMs{
//	delayTicks_100ms	= 100,
//	delayTicks_200ms	= 200,
//	delayTicks_1s		= 1000,
//};

enum delayTicksMs{
	delayTicks_1		= 100,
	delayTicks_2		= 500,
};


/**
 * @brief
 * @param
 * @param
 */
void buttonPressed();			// debe encender el LED

/**
 * @brief
 * @param
 * @param
 */
void buttonReleased();			// debe apagar el LED

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


	delay_t delayL1 = {0};
	const tick_t wDelay[2] = {100, 500};
	uint8_t ptrD = 0;
	delayInit(&delayL1, wDelay[ptrD]);


	debounceFSM_init();


	while (1)
	{
		debounceFSM_update();


		if(1){
			if (ptrD){
				ptrD = 0;
			}else{
				ptrD = 1;
			}

			delayWrite(&delayL1, wDelay[ptrD]);
		}

		if(delayRead(&delayL1)){
		   BSP_LED_Toggle(BSP_LED2);
		}

	}

	return 0;
}


void buttonPressed(){

}

void buttonReleased(){

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
