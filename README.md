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