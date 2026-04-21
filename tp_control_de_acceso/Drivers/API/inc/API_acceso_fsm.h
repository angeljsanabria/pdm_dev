/*
 * API_acceso_fsm.h
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_ACCESO_FSM_H_
#define API_INC_API_ACCESO_FSM_H_

#include <stdint.h>

#include "API_pn532.h"
#include "API_delay.h"
#include "API_uart_data.h"

#define ACCESO_DELAY_DEFAULT		1000

#define ACCESO_MAX_VALUE_CARD		PN532_MAX_UID_BUFFER

#define ACCESO_MAX_RETRIES			2

typedef enum {
	ACCESO_FSM_INIT = 0,
	ACCESO_FSM_CONFIG_PN532,
	ACCESO_FSM_IDLE,
	ACCESO_FSM_POLL_TO_READ_CARD,
	ACCESO_FSM_POLL_READ_ACK,
	ACCESO_FSM_PROCESS_AND_VALIDATE_DATA,
	ACCESO_FSM_SENT_TO_AUTHORIZER,
	ACCESO_FSM_WAIT_RX,
	ACCESO_FSM_EXEC_ERROR_LED_SEQ,
	ACCESO_FSM_EXEC_ACCESO,
} accesoState_t;

typedef struct {
	accesoState_t st_actual;
	delay_t delay_fsm;
	uint8_t pending_authorization;
	uint8_t valid_card;
	uint8_t len_card;
	uint8_t value_card[ACCESO_MAX_VALUE_CARD];
	uint8_t retry_count;
	PN532_Status_t lastStNFC;
} acceso_t;

void acceso_FSM_init(void);
void acceso_FSM_update(void);

#endif /* API_INC_API_ACCESO_FSM_H_ */
