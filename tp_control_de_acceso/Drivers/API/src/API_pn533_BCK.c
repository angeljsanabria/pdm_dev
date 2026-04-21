/*
 * API_pn532.c
 *
 *  Created on: 17 abr 2026
 *      Author: angel-dev
 */
#include "API_pn532.h"
#include <string.h>
#include "API_i2c_1.h"

/*
 * SAMConfiguration minima (0x14, modo Normal 0x01). Trama completa minima.
 */


static const uint8_t PN532_POLLING_READ[] = {
	PN532_PROTOCOL_PREAMBLE_FRAME_1, PN532_PROTOCOL_PREAMBLE_FRAME_2, PN532_PROTOCOL_PREAMBLE_FRAME_3,
    0x04, 0xFC,
	PN532_PROTOCOL_TFI_HOST_TO_PN532,
	PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET,
	0x01, 	// 1 Sola tarjeta
	0x00,
    0xE1,
    0x00	// 106 kbps (ISO14443A → MIFARE)
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
	uint8_t cola_pendiente_i2c;

	n = 0;
	found = 0;
	cola_pendiente_i2c = 1;

	// Devuelve varios 0x01, revisar hasta que encuentre el preambulo
	if (found == 0) {
		if (I2C1_Read(pn532.i2c_addr_hal, buff_rx, PN532_READ_ACK_BUFFER_LEN,
			      pn532.op_timeout_ms) != HAL_OK) {
			return PN532_ERR_I2C;
		}
		n = 0;
		while (n < PN532_READ_ACK_BUFFER_LEN) {
			n++;
			if (n < 3) {
				continue;
			}
			i = (uint8_t)(n - 1);
			if ((buff_rx[i - 2] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
			    && (buff_rx[i - 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
			    && (buff_rx[i] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
				found = 1;
				cola_pendiente_i2c = 0;
				break;
			}
		}
	}
	if (found == 0) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint8_t)(n + 3) > PN532_READ_ACK_BUFFER_LEN) {  // No tengo espacio para la respuesta ack
		return PN532_ERR_PROTOCOL;
	}

	if (cola_pendiente_i2c != 0) {
		if (I2C1_Read(pn532.i2c_addr_hal, &buff_rx[n], 3, pn532.op_timeout_ms) != HAL_OK) {
			return PN532_ERR_I2C;
		}
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


/*
 * Respuesta InListPassiveTarget desde PN532_PROTOCOL_TFI_PN532_TO_HOST
 * y comando 
 */
static PN532_Status_t pn532_read_inlist_response(void)
{
	uint8_t buff_rx[PN532_READ_ACK_BUFFER_LEN];
	uint16_t n;
	uint8_t i;
	uint8_t found;
	uint16_t p;
	uint8_t len_field;
	uint8_t count_cards;
	uint8_t uid_len;
	uint16_t sum;
	uint8_t k;

	if (I2C1_Read(pn532.i2c_addr_hal, buff_rx, PN532_READ_ACK_BUFFER_LEN,
		      pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	found = 0;
	p = 0;count_cardscount_cardscount_cards
	n = 0;
	while (n < PN532_READ_ACK_BUFFER_LEN) {
		n++;
		if (n < 3) {
			continue;
		}
		i = (uint8_t)(n - 1);
		if ((buff_rx[i - 2] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
		    && (buff_rx[i - 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
		    && (buff_rx[i] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
			found = 1;
			p = (uint16_t)(i - 2);
			break;
		}
	}
	if (found == 0) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint16_t)(p + 5) >= PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	len_field = buff_rx[p + 3];
	// Comprobar que el checksum es correcto LCS se elgije para que la suma sea 
	if ((uint16_t)(buff_rx[p + 3] + buff_rx[p + 4]) != (uint16_t)PN532_PROTOCOL_LCS_16BITS_VALIDADOR) {
		return PN532_ERR_PROTOCOL;
	}
	if ((uint16_t)(p + 5 + (uint16_t)len_field) > PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	if (buff_rx[p + 5] != PN532_PROTOCOL_TFI_PN532_TO_HOST) {
		return PN532_ERR_PROTOCOL;
	}
	if (buff_rx[p + 6] != PN532_PROTOCOL_CMD_INLIST_PASSIVE) {
		return PN532_ERR_PROTOCOL;
	}

	// Cantidad de tarjetas leidas
	count_cards = buff_rx[p + 7];
	if (count_cards == 0) {
		pn532.card_present = false;
		pn532.uid_len = 0;
		(void)memset(pn532.uid, 0, sizeof(pn532.uid));
		return PN532_ERR_NO_CARD;
	}

	if ((uint16_t)(p + 12) >= PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	uid_len = buff_rx[p + 11];
	if (uid_len == 0 || uid_len > PN532_MAX_UID_BUFFER) {
		return PN532_ERR_PROTOCOL;
	}
	if ((uint16_t)(p + 12 + (uint16_t)uid_len) > PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	(void)memcpy(pn532.uid, &buff_rx[p + 12], uid_len);
	pn532.uid_len = uid_len;
	pn532.card_present = true;

	sum = 0;
	for (k = 0; k < len_field; k++) {
		sum = (uint16_t)(sum + buff_rx[p + 5 + k]);
	}
	if ((sum & 0xFFU) != 0U) {
		return PN532_ERR_PROTOCOL;
	}

	return PN532_OK;
}

PN532_Status_t PN532_read_card_uid(uint8_t *uid, uint8_t *len)
{
	if ((uid == NULL) || (len == NULL)) {
		return PN532_ERR_OTRO;
	}
	if (pn532.card_present == false) {
		return PN532_ERR_NO_CARD;
	}
	*len = pn532.uid_len;
	(void)memcpy(uid, pn532.uid, (size_t)pn532.uid_len);
	return PN532_OK;
}

PN532_Status_t PN532_polling_read(void)
{
	if (I2C1_Write(pn532.i2c_addr_hal,
		       (uint8_t *)PN532_POLLING_READ,
		       (uint16_t)sizeof(PN532_POLLING_READ),
		       pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	return pn532_read_inlist_response();
}

PN532_Status_t PN532_config_module(void)
{
	// Solo config de lectura tipo polling
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
