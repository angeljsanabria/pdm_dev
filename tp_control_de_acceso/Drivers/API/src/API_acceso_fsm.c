/*
 * API_acceso_fsm.c
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#include <string.h>

#include "API_acceso_fsm.h"
#include "API_accesos_output.h"
#include "bsp_button.h"

/*
 * Estructura principal de control del acceso
 */
static acceso_t acceso;

void acceso_FSM_init(void)
{
	(void)memset(&acceso, 0, sizeof(acceso));
	acceso.st_actual = ACCESO_FSM_INIT;
	delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
}

void acceso_FSM_update(void)
{
	switch (acceso.st_actual) {
	case ACCESO_FSM_INIT:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.retry_count = 0;
			acceso.pending_authorization = 0;
			acceso.valid_card = 0;
			acceso.len_card = 0;
			(void)memset(acceso.value_card, 0, sizeof(acceso.value_card));
			acceso.lastStNFC = PN532_init_module(pn532_mode_InListPassiveTarget);
			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
		}
		acceso_cerrar_todos();
		if (delayRead(&acceso.delay_fsm)) {
			delayStop(&acceso.delay_fsm);
			acceso.st_actual = ACCESO_FSM_CONFIG_PN532;
		}
		break;

	case ACCESO_FSM_CONFIG_PN532:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.lastStNFC  = PN532_config_module();
			if (acceso.lastStNFC  != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
			delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
		}
		if (delayRead(&acceso.delay_fsm)) {
			acceso.lastStNFC = PN532_read_ack();

			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			} else {
				delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
				acceso.st_actual = ACCESO_FSM_IDLE;
			}
		}
		break;

	case ACCESO_FSM_IDLE:
		if (delayRead(&acceso.delay_fsm)) {
			acceso.st_actual = ACCESO_FSM_POLL_TO_READ_CARD;
		}
		break;

	case ACCESO_FSM_POLL_TO_READ_CARD:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.lastStNFC = PN532_polling_send();
			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
			acceso.retry_count = 0;
			delayInit(&acceso.delay_fsm, 500);
			acceso.st_actual = ACCESO_FSM_POLL_READ_ACK;
		}
		break;

	case ACCESO_FSM_POLL_READ_ACK: {
		uint8_t irq_ack;

		irq_ack = BSP_Button_Read();
		if ((delayRead(&acceso.delay_fsm) != 0) || (irq_ack)) {
			acceso.lastStNFC = PN532_read_ack();
			if (acceso.lastStNFC == PN532_OK) {
				acceso.retry_count = 0;
				delayInit(&acceso.delay_fsm, 500);
				acceso.st_actual = ACCESO_FSM_PROCESS_AND_VALIDATE_DATA;
			} else {
				acceso.retry_count++;
				if (acceso.retry_count <= ACCESO_MAX_RETRIES) {
					delayInit(&acceso.delay_fsm, 200);
				} else {
					acceso.retry_count = 0;
					acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				}
			}
		}
	}
		break;

	case ACCESO_FSM_PROCESS_AND_VALIDATE_DATA: {
		uint8_t len_uid = 0;
		uint8_t irq_in;

		irq_in = BSP_Button_Read();
		if ((delayRead(&acceso.delay_fsm) != 0) || (irq_in == 0)) {
			acceso.lastStNFC = PN532_read_inlist_response();
			if (acceso.lastStNFC == PN532_OK) {
				acceso.lastStNFC = PN532_save_read_uid_card(acceso.value_card, &len_uid);
				if (acceso.lastStNFC == PN532_OK) {
					acceso.len_card = len_uid;
					acceso.valid_card = 1;
					acceso.st_actual = ACCESO_FSM_SENT_TO_AUTHORIZER;
				} else {
					acceso.valid_card = 0;
					acceso.len_card = 0;
					acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				}
			} else if (acceso.lastStNFC == PN532_ERR_NO_CARD) {
				acceso.valid_card = 0;
				acceso.len_card = 0;
				(void)memset(acceso.value_card, 0, sizeof(acceso.value_card));
				delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
				acceso.st_actual = ACCESO_FSM_IDLE;
			} else {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			}
		}
	}
		break;

	case ACCESO_FSM_SENT_TO_AUTHORIZER:
		acceso.st_actual = ACCESO_FSM_IDLE;
		break;
	case ACCESO_FSM_WAIT_RX:
		break;
	case ACCESO_FSM_EXEC_ERROR_LED_SEQ:
		delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
		acceso.st_actual = ACCESO_FSM_IDLE;
		break;
	case ACCESO_FSM_EXEC_ACCESO:
		break;
	default:
		acceso.st_actual = ACCESO_FSM_INIT;
		break;
	}
}
