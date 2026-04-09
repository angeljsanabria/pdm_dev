/*
 * API_cmdparser.c
 *
 *  Created on: 8 abr 2026
 *      Author: angel-dev
 */
#include "API_cmdparser.h"
#include <string.h>


/**
  * @brief Comandos soportados
  */
#define CMD_INPUT_HELP			"HELP"
#define CMD_INPUT_STATUS		"STATUS"
#define CMD_INPUT_LED_ON		"ON"
#define CMD_INPUT_LED_OFF		"OFF"
#define CMD_INPUT_LED_TOGGLE	"TOGGLE"


/**
  * @brief Definiciones de ayuda
  */
#define CMD_INPUT_MIN_LENGTH	4
#define MINUSCULA_A					0x61
#define MINUSCULA_Z					0x7A
#define MINUSCULA_TO_MAYUS_OFFSET	0x20

typedef enum{
	CMD_IDLE,
	CMD_RECEIVING,
	CMD_PROCESS,
	CMD_EXEC,
	CMD_ERROR,
} parserCmdState_t;

enum {
	CMD_TO_EXE_NONE		= 0,
	CMD_TO_EXE_HELP,
	CMD_TO_EXE_STATUS,
	CMD_TO_EXE_LED_ON,
	CMD_TO_EXE_LED_OFF,
	CMD_TO_EXE_LED_TOGGLE,
};

static parserCmdState_t currentSt;
static uint16_t ptrRxBuff;
static uint16_t cmdToExe;
static cmd_status_t cmdStatus;
static uint8_t lastCharRead;
static uint8_t rxBuff[CMD_MAX_LINE] = {0};


void _echoLastChar(void){
	uartSendStringSize(&lastCharRead, 1);
}
/**
 * @brief 	Lectura de un unico caracteres de la uart de Datos
 *
 * @retval	true si se recibio un caracter
 * @retval	false si no recibio un caracter
 *
 */
static bool_t _readOneChar(void){
	if(uartReceiveStringSize(&rxBuff[ptrRxBuff], 1) == true){
		lastCharRead = rxBuff[ptrRxBuff];
		_echoLastChar();
		ptrRxBuff++;
		return true;
	}
	return false;
}

/**
 * @brief	Coloco el '0' para poder usar las funciones de string, reemplazando el retorno de carro
 * 			Aseguro el string en el Input Data
 */
static void _addNullToInputStr(void)
{
	if(ptrRxBuff > 0){
		ptrRxBuff--;
		rxBuff[ptrRxBuff] = 0x00;
	}
}

/**
 * @brief 	trasformo el comando ingresado a mayusculas
 * 			busco si hay algun caracter en minuscula y le aplico el offset
 * 			correspondiente para transformar a su mayuscula
 *
 */
static void _inputCmdToLowerCase(void){
	for (uint16_t loop = 0; loop < ptrRxBuff; loop++){
		if ((rxBuff[loop] >= MINUSCULA_A) && (rxBuff[loop] <= MINUSCULA_Z)){
			rxBuff[loop] =  rxBuff[loop] - (uint8_t)MINUSCULA_TO_MAYUS_OFFSET;
		}
	}
}

/**
 * @brief 	Check si hay alguno de los comandos validos listados
 * 			busco si hay algun caracter en minuscula y le aplico el offset
 * 			correspondiente para transformar a su mayuscula
 *
 * @retval 	cmd_status_t CMD_OK si hay algun comando valido a ejecutar
 */
static cmd_status_t _checkInputCmd(){

	char* aux;
	if (strstr((char *)rxBuff, CMD_INPUT_HELP) != NULL) {
		cmdToExe = CMD_TO_EXE_HELP;
		return CMD_OK;
	}

	if (strstr((char *)rxBuff, CMD_INPUT_STATUS) != NULL) {
		cmdToExe = CMD_TO_EXE_STATUS;
		return CMD_OK;
	}

	aux = strstr((char *)rxBuff, "LED");

	if(aux == NULL){
		return CMD_ERR_UNKNOW;
	}

	if (strstr(aux, CMD_INPUT_LED_ON) != NULL) {
		cmdToExe = CMD_TO_EXE_LED_ON;
		return CMD_OK;
	}

	if (strstr(aux, CMD_INPUT_LED_OFF) != NULL) {
		cmdToExe = CMD_TO_EXE_LED_OFF;
		return CMD_OK;
	}

	if (strstr(aux, CMD_INPUT_LED_TOGGLE) != NULL) {
		cmdToExe = CMD_TO_EXE_LED_TOGGLE;
		return CMD_OK;
	}

	return CMD_ERR_SYNTAX;

}

/**
 * @brief 	Ejecuto el HELP
 *
 */
static void _printHelp(void){
	uartSendString("########################\r\n");
	uartSendString("HELP DE COMAND PARSER\r\n");
	uartSendString("Funciones soportadas (no case sensitive):\r\n");
	uartSendString("HELP: Imprime esta ayuda\r\n");
	uartSendString("Status: Imprime el estado del LED a controlar\r\n");
	uartSendString("LED ON: ENCIENDE EL LED\r\n");
	uartSendString("LED OFF: APAGA EL LED\r\n");
	uartSendString("LED TOGGLE: INVIERTE EL ESTADO ACTUAL DEL LED\r\n");
	uartSendString("########################\r\n");
}

/**
 * @brief 	Ejecuto el STATUS
 *
 */
static void _printStatus(void){

	if(ledIsOn()){
		uartSendString("STATUS: Led Encendido\r\n");
	}else{
		uartSendString("STATUS: Led Apagado\r\n");
	}

}

/**
 * @brief 	Imprime por la uart de datos  los  mensajes de error segun cmdStatus (solo errores).
 */
static void _printCmdErr(void)
{
	if (cmdStatus == CMD_ERR_OVERFLOW) {
		uartSendString("ERROR: overflow\r\n");
	} else if (cmdStatus == CMD_ERR_SYNTAX) {
		uartSendString("ERROR: syntax\r\n");
	} else if (cmdStatus == CMD_ERR_ARG) {
		uartSendString("ERROR: arg\r\n");
	} else {
		uartSendString("ERROR: unknown\r\n");
	}
}

/**
 * @brief 	FUNCIONES PUBLICAS
 * 			documentadas en el .h
 */
void cmdParserInit(void){
	currentSt = CMD_IDLE;
	ptrRxBuff = 0;
	lastCharRead = 0;
	cmdStatus = CMD_OK;
	cmdToExe = CMD_TO_EXE_NONE;
	return;
}



void cmdPoll(void){

	switch (currentSt) {
	case CMD_IDLE:
		//
		if(_readOneChar()){
			if((lastCharRead == CMD_TERMINADOR) || (lastCharRead == CMD_NO_INIT_1) || (lastCharRead == CMD_NO_INIT_2)){
				ptrRxBuff = 0;
			}
			else{
				currentSt = CMD_RECEIVING;
			}
		}

		break;

	case CMD_RECEIVING:
		if (ptrRxBuff >= CMD_MAX_LINE) {
			cmdStatus = CMD_ERR_OVERFLOW;
			currentSt = CMD_ERROR;
			break;
		}
		if(_readOneChar()){
			if(lastCharRead == CMD_TERMINADOR){
				if(ptrRxBuff <= CMD_INPUT_MIN_LENGTH){
					cmdStatus = CMD_ERR_SYNTAX;
					currentSt  = CMD_ERROR;
					break;
				}
				currentSt  = CMD_PROCESS;
				break;
			}
			if(ptrRxBuff >= CMD_MAX_LINE){
				cmdStatus = CMD_ERR_OVERFLOW;
				currentSt  = CMD_ERROR;
				break;
			}
		}
		break;

	case CMD_PROCESS:
		_addNullToInputStr();
		_inputCmdToLowerCase();
		cmdStatus = _checkInputCmd();
		if(cmdStatus == CMD_OK)		currentSt = CMD_EXEC;
		else						currentSt = CMD_ERROR;

		break;

	case CMD_EXEC:
		//
		if(cmdToExe == CMD_TO_EXE_HELP){
			_printHelp();
		}
		else if(cmdToExe == CMD_TO_EXE_STATUS){
			_printStatus();
		}
		else if(cmdToExe == CMD_TO_EXE_LED_ON){
			ledOn();
		}
		else if(cmdToExe == CMD_TO_EXE_LED_OFF){
			ledOff();
		}
		else if(cmdToExe == CMD_TO_EXE_LED_TOGGLE){
			ledToggle();
		}else{
			cmdStatus = CMD_ERR_UNKNOW;
			currentSt = CMD_ERROR;
			break;
		}



		ptrRxBuff = 0;
		cmdToExe = CMD_TO_EXE_NONE;
		currentSt = CMD_IDLE;
		break;

	case CMD_ERROR:
		_printCmdErr();

		ptrRxBuff = 0;
		cmdToExe = CMD_TO_EXE_NONE;
		currentSt = CMD_IDLE;
		break;

	default:
		currentSt = CMD_IDLE;
		break;

	}
}
