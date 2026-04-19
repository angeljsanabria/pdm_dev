/*
 * API_pn532.c
 *
 *  Created on: 17 abr 2026
 *      Author: angel-dev
 */
#include "API_pn532.h"
#include <string.h>
#include "stm32l4xx_hal.h"
#include "API_i2c_1.h"

/*
 * SAMConfiguration minima (0x14, modo Normal 0x01). Trama completa minima.
 */
static const uint8_t PN532_SAM_NORMAL_FRAME[] = {
	0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17, 0x00
};

static PN532_Handler_t pn532;

PN532_Status_t PN532_init_module(PN532_CONFIG_MODE_T modo)
{
	pn532.i2c_addr_hal = PN532_I2C_ADDR_SHIFT;
	pn532.op_timeout_ms = PN532_CONFIG_DEFAULT_TIMEOUT;
	pn532.card_present = false;
	pn532.uid_len = 0;
	if (modo == pn532_mode_InListPassiveTarget) {
		pn532.read_mode = pn532_mode_InListPassiveTarget;
	} else if (modo == pn532_mode_InAutoPoll) {
		pn532.read_mode = pn532_mode_InAutoPoll;
	} else {
		return PN532_ERR_OTRO;
	}
	(void)memset(pn532.uid, 0x00, (size_t)PN532_MAX_UID_BUFFER);
	return PN532_OK;
}

PN532_Status_t PN532_read_ack(void)
{
	uint8_t buff_rx[PN532_READ_ACK_BUFFER_LEN] = {0};
	uint8_t n;
	uint8_t i;
	uint8_t found;

	n = 0;
	found = 0;
	while (n < PN532_READ_ACK_BUFFER_LEN) {
		if (I2C1_Read(pn532.i2c_addr_hal, &buff_rx[n], 1, pn532.op_timeout_ms) != HAL_OK) {
			return PN532_ERR_I2C;
		}
		HAL_Delay(50);
		n++;
		if (n < 3) {
			continue;
		}
		i = (uint8_t)(n - 1);
		if ((buff_rx[i - 2] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
		    && (buff_rx[i - 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
		    && (buff_rx[i] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
			found = 1;
			break;
		}
	}
	if (found == 0) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint8_t)(n + 3) > PN532_READ_ACK_BUFFER_LEN) {  // No tengo espacio para la respuesta ack
		return PN532_ERR_PROTOCOL;
	}

	if (I2C1_Read(pn532.i2c_addr_hal, &buff_rx[n], 3, pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	n = (uint8_t)(n + 3);
	if ((buff_rx[n - 3] != PN532_PROTOCOL_ACK_FRAME_BYTE_1)
	    || (buff_rx[n - 2] != PN532_PROTOCOL_ACK_FRAME_BYTE_2)
	    || (buff_rx[n - 1] != PN532_PROTOCOL_ACK_FRAME_BYTE_3)) {
		return PN532_ERR_PROTOCOL;
	}
	
	return PN532_OK;
}

static PN532_Status_t PN532_config_module_normal(void)
{
	if (I2C1_Write(pn532.i2c_addr_hal,
		       (uint8_t *)PN532_SAM_NORMAL_FRAME,
		       (uint16_t)sizeof(PN532_SAM_NORMAL_FRAME),
		       pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	return PN532_OK;
}

PN532_Status_t PN532_config_module(void)
{
	return PN532_config_module_normal();
}


//
//uint8_t PN532_Read_Card(PN532_Handle *nfc, uint8_t *uid_out) {
//    I2C1_Write(nfc->dev_addr, (uint8_t*)CMD_INLIST, sizeof(CMD_INLIST), nfc->timeout);
//
//    uint8_t status = 0;
//    uint32_t start_tick = HAL_GetTick();
//
//    // Reemplazamos HAL_Delay por un bucle de espera activa no bloqueante
//    // hasta que el chip esté listo o pase el timeout del dispositivo
//    while (status != 0x01) {
//        I2C1_Read(nfc->dev_addr, &status, 1, 10);
//
//        if ((HAL_GetTick() - start_tick) > nfc->timeout) {
//            return 0; // Timeout: no hubo tarjeta o el chip no responde
//        }
//    }
//
//    // Ahora que status es 0x01, leemos la trama completa de forma segura
//    uint8_t rx_buf[32];
//    I2C1_Read(nfc->dev_addr, rx_buf, 32, nfc->timeout);
//    // ... procesar UID ...
//}
