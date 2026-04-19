/*
 * api_pn532.h
 *
 *  Created on: 17 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_PN532_H_
#define API_INC_API_PN532_H_
#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;


#define PN532_I2C_ADDRESS			0x24		// Una unica direccion para I2C - No configurable
#define PN532_I2C_ADDR_SHIFT      	((uint16_t)(PN532_I2C_ADDRESS << 1))
#define PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET 		0x4A
#define PN532_CONFIG_MODE_IN_AUTO_POLL				 	0x60
#define PN532_CONFIG_DEFAULT_TIMEOUT				 	500 // en ms

#define PN532_MAX_UID_BUFFER		10
/* Lectura en bloque tras byte de estado (evita repetir 0x01 en cada START/STOP). */
#define PN532_READ_ACK_BURST_LEN	32

// DEFINICIONES DE PROTOCOLO
#define PN532_PROTOCOL_READY_BYTE		0x01		// Seguir leyendo la trama
#define PN532_PROTOCOL_STOP_BYTE		0x00		// No seguir leyendo la trama - Enviar Stop
#define PN532_PROTOCOL_00_BYTE		0x00		// Estado de la trama
#define PN532_PROTOCOL_ACK_BYTE		0xFF		// ACK de la trama

#define PN532_PROTOCOL_PREAMBLE_FRAME_1	0x00		// Preambulo de la trama byte 1 de 3
#define PN532_PROTOCOL_PREAMBLE_FRAME_2	0x00		// Preambulo de la trama byte 2 de 3
#define PN532_PROTOCOL_PREAMBLE_FRAME_3	0xFF		// Preambulo de la trama byte 3 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_1	0x00		// ACK trama byte 1 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_2	0xFF		// ACK trama byte 2 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_3	0x00		// ACK trama byte 3 de 3
#define PN532_PROTOCOL_TFI_HOST_TO_PN532		0xD4		// TFI - Identificado de trama host a PN532
#define PN532_PROTOCOL_TFI_PN532_TO_HOST		0xD5		// TFI - Identificado de trama PN532 a host
#define PN532_PROTOCOL_D5_BYTE		0xD5		// TFI de la trama
#define PN532_PROTOCOL_15_BYTE		0x15		// Codigo de respuesta de la trama
#define PN532_PROTOCOL_DCS_BYTE		0xFD		// Checksum de la trama

/**
  * @brief 	estructura principal de control del modulo PN532
  *
  * @param 	i2c_addr_hal - Direccion I2C en formato  con shift
  * @param 	op_timeout_ms - Timeout maximo de operaciones con el PN532 (ms).
  * @param 	card_present- Indica si hay tarjeta/tag detectado.
  * @param 	uid_len - Cantidad de bytes validos del UID.
  * @param 	uid - Almacena el UID del tag (hasta PN532_MAX_UID_BUFFER bytes)
  */
typedef struct {
    uint16_t i2c_addr_hal;
    uint32_t op_timeout_ms;
    bool     card_present;
    uint8_t  uid_len;
    uint8_t  uid[PN532_MAX_UID_BUFFER];
    uint8_t  read_mode;
} PN532_Handler_t;

typedef enum {
    PN532_OK = 0,
    PN532_ERR_NO_CARD,
    PN532_ERR_I2C,
    PN532_ERR_TIMEOUT,
    PN532_ERR_PROTOCOL,
    PN532_ERR_OTRO,
} PN532_Status_t;

/**
  * @brief 	Modos de trabajo del modulo
  *
  * @param 	pn532_mode_InListPassiveTarget Lectura pasiva: Configura el modulo para esperar
  * 		la orden de lectura. (se debe respetar un tiempo de espera antes de consultar si
  * 		hay dato valido)
  * @param 	pn532_mode_InAutoPoll Lectura activa continua: Configura el modulo en lectura continua
  */
typedef enum
{
	pn532_mode_InListPassiveTarget   	= PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET,
	pn532_mode_InAutoPoll    			= PN532_CONFIG_MODE_IN_AUTO_POLL,
} PN532_CONFIG_MODE_T;


/**
  * @brief 	Inicializa el unico PN532_Handler_t interno (valores por defecto).
  * @param 	modo - Modo de lectura (InListPassiveTarget o InAutoPoll).
  *         El desarrollo esta solo para el modo InListPassiveTarget.
  * @retval PN532_OK o PN532_ERR_OTRO si el modo no es valido.
  */
PN532_Status_t PN532_init_module(PN532_CONFIG_MODE_T modo);

/**
  * @brief 	SAM Normal: camino feliz Ready -> ACK 6B -> Ready -> respuesta D5 15 00. read_mode InListPassiveTarget.
  */
PN532_Status_t PN532_config_module(void);

/**
  * @brief 	Lee y valida trama ACK (buffer interno). PN532_OK si ACK valido, error de protocolo o I2C si no.
  */
PN532_Status_t PN532_read_ack(void);

#endif /* API_INC_API_PN532_H_ */
