#ifndef HUSB238_H
#define HUSB238_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Standardadresse des HUSB238
#define HUSB238_I2C_ADDRESS 0x08

#define HUSB238_PD_STATUS0		0x00
#define HUSB238_PD_STATUS1		0x01
#define HUSB238_SRC_PDO_5V		0x02	///< Register value for SRC_PDO_5V in HUSB238
#define HUSB238_SRC_PDO_9V		0x03 	///< Register value for SRC_PDO_9V in HUSB238
#define HUSB238_SRC_PDO_12V		0x04	///< Register value for SRC_PDO_12V in HUSB238
#define HUSB238_SRC_PDO_15V		0x05	///< Register value for SRC_PDO_15V in HUSB238
#define HUSB238_SRC_PDO_18V		0x06	///< Register value for SRC_PDO_18V in HUSB238
#define HUSB238_SRC_PDO_20V		0x07	///< Register value for SRC_PDO_20V in HUSB238
#define HUSB238_SRC_PDO			0x08	///< Register value for SRC_PDO in HUSB238
#define HUSB238_GO_COMMAND		0x09	///< Register value for GO_COMMAND in HUSB238

#define NO_RESPONSE								0b000 ///< No response
#define RESPONE_SUCCESS 						0b001 ///< Success
#define RESPONSE_INVALID_CMD_OR_ARG 			0b011 ///< Invalid command or argument
#define RESPONE_CMD_NOT_SUPPORTED 				0b100 ///< Command not supported
#define RESPONE_TRANSACTION_FAIL_NO_GOOD_CRC 	0b101 ///< Transaction fail. No GoodCRC is received after sending

#define CURRENT5V_DEFAULT	0b00	///< Default current
#define CURRENT5V_1_5_A		0b01	///< 1.5A
#define CURRENT5V_2_4_A 	0b10	///< 2.4A
#define CURRENT5V_3_A 		0b11	///< 3A

#define CURRENT_0_5_A		0b0000		///< 0.5A
#define CURRENT_0_7_A		0b0001		///< 0.7A
#define CURRENT_1_0_A		0b0010		///< 1.0A
#define CURRENT_1_25_A		0b0011		///< 1.25A
#define CURRENT_1_5_A		0b0100		///< 1.5A
#define CURRENT_1_75_A		0b0101		///< 1.75A
#define CURRENT_2_0_A		0b0110		///< 2.0A
#define CURRENT_2_25_A		0b0111		//< 2.25A
#define CURRENT_2_50_A		0b1000		///< 2.50A
#define CURRENT_2_75_A		0b1001		///< 2.75A
#define CURRENT_3_0_A		0b1010		///< 3.0A
#define CURRENT_3_25_A		0b1011		///< 3.25A
#define CURRENT_3_5_A		0b1100		///< 3.5A
#define CURRENT_4_0_A		0b1101		///< 4.0A
#define CURRENT_4_5_A		0b1110		///< 4.5A
#define CURRENT_5_0_A		0b1111		///< 5.0A

//For USER and PD Command to set Voltage
#define PD_NOT_SELECTED		0b0000		///< Not selected
#define PD_SRC_5V			0b0001		///< SRC_PDO_5V
#define PD_SRC_9V			0b0010		///< SRC_PDO_9V
#define PD_SRC_12V			0b0011		///< SRC_PDO_12V
#define PD_SRC_15V			0b1000		///< SRC_PDO_15V
#define PD_SRC_18V			0b1001		///< SRC_PDO_18V
#define PD_SRC_20V			0b1010		///< SRC_PDO_20V

#define MAX_PROFILES	6	///< Maximum number of supported PD profiles

// Funktion zum Schreiben eines Registers
bool husb238_write_register(uint8_t reg, uint8_t value);

// Funktion zum Lesen eines Registers
bool husb238_read_register(uint8_t reg, uint8_t *value);

bool husb238_getCCDirection(void);
bool husb238_isAttached();
uint8_t husb238_getPDRespone();
bool husb238_get5VContractV();
uint8_t husb238_get5VContractA();
uint16_t husb238_getPDSrcVoltage();
uint16_t husb238_getPDSrcCurrent();
uint8_t husb238_getSelectedPD();
bool husb238_isVoltageDetected(uint8_t pd_src);
uint8_t husb238_getSupportedVoltages();
void husb2238_selectPD(uint8_t pd_src);
void husb238_requestPD();
void husb238_reset();

// Funktion zur Initialisierung
int8_t husb238_init(i2c_inst_t *i2c_port);

#endif // HUSB238_H