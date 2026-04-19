/*
 * API_acceso_fsm.c
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#include <string.h>

#include "API_acceso_fsm.h"
#include "API_accesos_output.h"

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
	{
		PN532_Status_t st;

		if (!delayIsRunning(&acceso.delay_fsm)) {
			st = PN532_config_module();
			acceso.lastStNFC = st;
			if (st != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
			delayInit(&acceso.delay_fsm, ACCESO_DELAY_AFTER_CONFIG_MS);
		}
		if (delayRead(&acceso.delay_fsm)) {
			st = PN532_read_ack();
			acceso.lastStNFC = st;
			if (st != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			} else {
				acceso.st_actual = ACCESO_FSM_IDLE;
			}
		}
	}
		break;
	case ACCESO_FSM_IDLE:
		break;
	case ACCESO_FSM_POLL_TO_READ_CARD:
		break;
	case ACCESO_FSM_PROCESS_AND_VALIDATE_DATA:
		break;
	case ACCESO_FSM_SENT_TO_AUTHORIZER:
		break;
	case ACCESO_FSM_WAIT_RX:
		break;
	case ACCESO_FSM_EXEC_ERROR_LED_SEQ:
		break;
	case ACCESO_FSM_EXEC_ACCESO:
		break;
	default:
		acceso.st_actual = ACCESO_FSM_INIT;
		break;
	}
}
