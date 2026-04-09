# Release note (mini): UART de datos + parser de comandos (5_practica)

## Resumen

Capa **`API_uart_data`** sobre **USART3**, maquina de estados del **parser de lineas** (`API_cmdparser`), y enlace con la aplicacion (LED, errores por UART).

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

## API UART de datos (`API_uart_data.h` / `.c`)

- `bool_t uartInit(void)` Configura **USART3** (baudios, 8N1, FIFO deshabilitado tras ajustes) e inicializa el periferico. Devuelve **false** si falla `HAL_UART_Init` o la configuracion extendida.
- `void uartSendString(uint8_t *pstring)` Envia una cadena **terminada en `\0`**. No envia si puntero nulo, longitud 0 o `strlen >= UART_DATA_MAX_SIZE`.
- `void uartSendStringSize(uint8_t *pstring, uint16_t size)` Envia **exactamente** `size` bytes (eco local, datos binarios cortos, etc.).
- `bool_t uartReceiveStringSize(uint8_t *pstring, uint16_t size)` Recibe **size** bytes con timeout; **true** si llegaron todos a tiempo, **false** si timeout o error HAL.
- `void API_uart_data_test(void)` Mensaje de prueba al arranque (`"Iniciado"` por TX).

Handler global: **`UART_HandleTypeDef huart3`** (definido en `API_uart_data.c`). GPIO y reloj: **`bsp_uart3.c`** / **`bsp_uart3.h`**.

---

## Parser de comandos (`API_cmdparser`)

- **Fin de linea:** **LF** (`0x0A`, `ENDSTR` en `API_uart_data.h`).
- **Longitud maxima de linea:** **64** bytes (`CMD_MAX_LINE`; el terminador se reemplaza por `'\0'` antes de interpretar).
- **Comandos** (comparacion no sensible a mayusculas tras normalizar):  
  `HELP`, `STATUS`, y subcomandos LED: `LED ON`, `LED OFF`, `LED TOGGLE`.
- **Eco:** cada caracter recibido se reenvia por la misma UART (`_echoLastChar`).

### Respuestas y acciones (trazado en `cmdPoll` → `CMD_EXEC`)

| Comando reconocido | Accion / respuesta por UART |
|--------------------|-----------------------------|
| **HELP** | Bloque de texto de ayuda (lineas con `\r\n`, listado de comandos). |
| **STATUS** | *Previsto por `_printStatus()`:* texto `STATUS: Led Encendido` o `STATUS: Led Apagado` segun `ledIsOn()`. *Comprobar en firmware que el ramo `CMD_TO_EXE_STATUS` invoque `_printStatus()` si se desea salida en consola.* |
| **LED ON** | Enciende LED de aplicacion (`ledOn()` → BSP LED3). |
| **LED OFF** | Apaga LED (`ledOff()`). |
| **LED TOGGLE** | Conmuta LED (`ledToggle()`). |

### Errores (`CMD_ERROR` → `_printCmdErr()`)

Mensajes en texto plano terminados en `\r\n`:

| Estado interno | Texto |
|----------------|--------|
| `CMD_ERR_OVERFLOW` | `ERROR: overflow` |
| `CMD_ERR_SYNTAX` | `ERROR: syntax` |
| `CMD_ERR_ARG` | `ERROR: arg` |
| Otros (p. ej. `CMD_ERR_UNKNOW`) | `ERROR: unknown` |

---

## Dependencias de aplicacion (main / BSP)

- LED usuario: **BSP_LED3** (control On/Off/Toggle; estado legible vía `BSP_LED_IsOn` / `ledIsOn()` en `main`).
- El parser usa `ledOn`, `ledOff`, `ledToggle`, `ledIsOn` segun enlace del proyecto.


