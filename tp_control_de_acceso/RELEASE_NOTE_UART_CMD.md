# Release note (mini): 

## Resumen


---

## Hardware y pinout (USART3)

| Parametro | Valor |
|-----------|--------|
| Periferico | **USART3** (`huart3`, `USART3`) |
| Placa de referencia (BSP) | **NUCLEO-L4R5ZI** |
| **TX** | **PD8** (AF7 USART3) |
| **RX** | **PD9** (AF7 USART3) |
| Puerto GPIO | **GPIOD** |

---

## Parametros de linea (UART de datos)

| Parametro | Valor en firmware |
|-----------|-------------------|
| Baudios | **57600** |
| Bits de datos | **8** |
| Paridad | **ninguna** (8N1) |
| Bits de parada | **1** |
| Timeout RX (`HAL_UART_Receive`) | **10 ms** (`UART_DATA_MIN_RX_TIMEOUT`) |
| Tamano max. util para send (proteccion API) | **256** (`UART_DATA_MAX_SIZE`) |

---

## API UART de datos (`API_uart_data.h` / `.c`) _TODO Actualizar

- `bool_t uartInit(void)` Configura **USART3** (baudios, 8N1, FIFO deshabilitado tras ajustes) e inicializa el periferico. Devuelve **false** si falla `HAL_UART_Init` o la configuracion extendida.
- `void uartSendString(uint8_t *pstring)` Envia una cadena **terminada en `\0`**. No envia si puntero nulo, longitud 0 o `strlen >= UART_DATA_MAX_SIZE`.
- `void uartSendStringSize(uint8_t *pstring, uint16_t size)` Envia **exactamente** `size` bytes (eco local, datos binarios cortos, etc.).
- `bool_t uartReceiveStringSize(uint8_t *pstring, uint16_t size)` Recibe **size** bytes con timeout; **true** si llegaron todos a tiempo, **false** si timeout o error HAL.
- `void API_uart_data_test(void)` Mensaje de prueba al arranque (`"Iniciado"` por TX).

Handler global: **`UART_HandleTypeDef huart3`** (definido en `API_uart_data.c`). GPIO y reloj: **`bsp_uart3.c`** / **`bsp_uart3.h`**.

---




angel-dev@192 tp_control_de_acceso % tree -L 4 -C
.
├── BSP
│   ├── Inc
│   │   ├── bsp.h
│   │   ├── bsp_button.h
│   │   ├── bsp_i2c1.h
│   │   ├── bsp_leds.h
│   │   └── bsp_uart3.h
│   └── Src
│       ├── bsp.c
│       ├── bsp_button.c
│       ├── bsp_i2c1.c
│       ├── bsp_leds.c
│       └── bsp_uart3.c
├── Core
│   ├── Inc
│   │   ├── main.h
│   │   ├── stm32l4xx_hal_conf.h
│   │   └── stm32l4xx_it.h
│   ├── Src
│   │   ├── main.c
│   │   ├── stm32l4xx_hal_msp.c
│   │   ├── stm32l4xx_it.c
│   │   ├── syscalls.c
│   │   └── sysmem.c
├── Drivers
│   ├── API
│   │   ├── inc
│   │   │   ├── API_acceso_fsm.h
│   │   │   ├── API_accesos_output.h
│   │   │   ├── API_cmd_data_parser.h
│   │   │   ├── API_debounce.h
│   │   │   ├── API_delay.h
│   │   │   ├── API_i2c_1.h
│   │   │   ├── API_pn532.h
│   │   │   └── API_uart_data.h
│   │   └── src
│   │       ├── API_acceso_fsm.c
│   │       ├── API_accesos_output.c
│   │       ├── API_cmd_data_parser.c
│   │       ├── API_delay.c
│   │       ├── API_i2c_1.c
│   │       ├── API_pn532.c
│   │       └── API_uart_data.c

