# Programacion de microcontroladores de la CESE

## Placa de desarrollo

Estoy usando una NUCELO-L4R5ZI (No consegui la sugerida para el Curso).

- NUCELO-L4R5ZI
- STM32L4R5ZITx
- LQFP144
- Flash: 2048 KB
- Ram1, 2, 3: 640 KB, 64 KB, 384 KB

## Practicas y revisiones

### Base proyect

- Hice un proyecto base con el asistente de STM32CubeID, pero lo restructure para que me quede
mas comodo.
- Clock

  | SYSCLK | 20 MHz |
  | ------ | ------ |

- BSP y funcion del periferico

  | Periferico | PIN  |
  | ---------- | ---- |
  | LED2       | PB7  |
  | LED3       | PB14 |
  | BTN_USR    | PC13 |


### Practica 2 (entregable)

- Se incluyo un #define PUNTO_3 para habilitar la feature opcional

### Practica 3 (entregable)

- Se implmento un driver/APIU de delay no bloqueante
- Estructura:
  - `Drivers/API/inc/API_delay.h`: interfaz del modulo
  - `Drivers/API/src/API_delay.c`: implementacion del delay no bloqueante

### Practica 4 (entregable)

#### Punto 1

En el enunciado, el **Punto 1** pide implementar la FSM/MEF y funciones como `debounceFSM_init` / `debounceFSM_update` en `main.c`. En esta entrega **esa logica no aparece en `main.c`** porque en el **Punto 2** se movio todo al modulo `API_debounce` (`API_debounce.h` y `API_debounce.c`).

Se dejaron en `main.c` solo las **acciones de aplicacion** que el enunciado asocia al pulsador, para poder **ver en placa** los eventos de la MEF:

```c
void buttonPressed(void)
{
	BSP_LED_On(BSP_LED3);
	return;
}

void buttonReleased(void)
{
	BSP_LED_Off(BSP_LED3);
	return;
}
```

`buttonPressed` y `buttonReleased` son llamadas desde la MEF en `API_debounce.c` cuando hay **pulsacion estable** y **suelto estable**. Se usa **LED3** (en esta placa, **LED rojo**) como indicador de esos eventos.

#### Punto 2

Archivos del modulo anti-rebote/debounce:

- `Drivers/API/inc/API_debounce.h`: Prototipos y comentarios de las funciones de la API. |
- `Drivers/API/src/API_debounce.c`:  FSM/MEF, `readKey`, temporizacion 40 ms con `API_delay`, llamadas a `buttonPressed` / `buttonReleased`.

La **documentacion** de las funciones publicas esta en **`API_debounce.h`**.

El **uso del resultado de la FSM** en la aplicacion se ve en `main.c` con **`if (readKey())`**: al detectarse una pulsacion debounceada se alterna el periodo del parpadeo del **LED2** (**LED azul** en esta NUCLEO) mediante `API_delay` (`delayWrite` / `delayRead` + `BSP_LED_Toggle(BSP_LED2)`).

```c
		if(readKey()){	// Si se detecto un evento, cambio el tipo de delay
			if (ptrD)	ptrD = 0;
			else		ptrD = 1;

			delayWrite(&delayL1, wDelay[ptrD]);
		}
```
