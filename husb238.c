#include "husb238.h"

//for PD
#define UNATTACHED 			0b0000		///< Unattached
#define PD_5V				0b0001		///< 5V
#define PD_9V				0b0010		///< 9V
#define PD_12V				0b0011		///< 12V
#define PD_15V				0b0100		///< 15V
#define PD_18V				0b0101		///< 18V
#define PD_20V				0b0110		///< 20V

// Globale I2C-Instanz
static i2c_inst_t *i2c_instance = NULL;

// Struktur für das Power Delivery (PD) Profil
typedef struct {
    uint8_t voltage;   ///< Voltagelevel in Volt, z.B. 5, 9, 12, 15, 18, 20V
    uint16_t current;  ///< Current in Milliampere (mA), z.B. 500, 1000, 2000 (für 0.5A, 1A, 2A)
	uint8_t power;	 ///< Power in Watt (W), z.B. 5, 9, 12, 15, 18, 20W
} PDProfile;

PDProfile supported_profiles[MAX_PROFILES]; ///< Array für die unterstützten PD-Profile

/**************************************************************************/
/**
 * @brief Parses the provided current value and returns the corresponding current in milliamps.
 *
 * @param current The current value to parse (e.g., 0.5A, 1.0A, etc.).
 * 
 * @return uint16_t
 *         The corresponding current value in milliamps (e.g., 500 for 0.5A, 1000 for 1.0A, etc.), 
 *         or 0 if an invalid current value is provided.
 *
 * @details This function takes a predefined current value (such as CURRENT_0_5_A, CURRENT_1_0_A, etc.) 
 * and converts it to the corresponding current in milliamps. If the input current value is not valid, 
 * it returns 0.
 *
 * Usage:
 * - Use this function to convert a predefined current constant into the corresponding current in milliamps.
 *
 * Example:
 * ```
 * uint16_t current = parse_current(CURRENT_1_0_A);
 * if (current > 0) {
 *     // Successfully parsed current in milliamps
 * } else {
 *     // Invalid current
 * }
 * ```
 */
/**************************************************************************/
static uint16_t parse_current(uint8_t current)
{
	switch (current)
	{
	case CURRENT_0_5_A:
		return 500;
	case CURRENT_0_7_A:
		return 700;
	case CURRENT_1_0_A:
		return 1000;
	case CURRENT_1_25_A:
		return 1250;
	case CURRENT_1_5_A:
		return 1500;
	case CURRENT_1_75_A:
		return 1750;
	case CURRENT_2_0_A:
		return 2000;
	case CURRENT_2_25_A:
		return 2250;
	case CURRENT_2_50_A:
		return 2500;
	case CURRENT_2_75_A:
		return 2750;
	case CURRENT_3_0_A:
		return 3000;
	case CURRENT_3_25_A:
		return 3250;
	case CURRENT_3_5_A:
		return 3500;
	case CURRENT_4_0_A:
		return 4000;
	case CURRENT_4_5_A:
		return 4500;
	case CURRENT_5_0_A:
		return 5000;
	default:
		return 0;
	}
}

/**************************************************************************/
/**
 * @brief Parses the provided voltage value and returns the corresponding Power Delivery (PD) source voltage.
 *
 * @param voltage The voltage value to parse (e.g., 5V, 9V, etc.).
 * 
 * @return uint8_t
 *         Corresponding PD source voltage as a defined constant, or `PD_NOT_SELECTED` if no valid voltage is found.
 *
 * @details This function takes a voltage value (such as PD_5V, PD_9V, etc.) and returns the 
 * appropriate Power Delivery source voltage constant based on the provided input. If the 
 * input voltage does not match any of the defined voltage options, it returns the constant 
 * `PD_NOT_SELECTED`.
 *
 * Usage:
 * - Use this function to convert a given voltage value into the corresponding Power Delivery 
 *   source voltage setting for the HUSB238 device.
 *
 * Example:
 * ```
 * uint8_t pd_voltage = parse_voltage(PD_5V);
 * if (pd_voltage != PD_NOT_SELECTED) {
 *     // Successfully parsed voltage
 * } else {
 *     // Invalid voltage
 * }
 * ```
 */
/**************************************************************************/
static uint8_t parse_voltage(uint8_t voltage)
{
	switch (voltage)
	{
	case PD_5V:
		return PD_SRC_5V;
	case PD_9V:
		return PD_SRC_9V;
	case PD_12V:
		return PD_SRC_12V;
	case PD_15V:
		return PD_SRC_15V;
	case PD_18V:
		return PD_SRC_18V;
	case PD_20V:
		return PD_SRC_20V;
	default:
		return PD_NOT_SELECTED;
	}
}

/**************************************************************************/
/**
 * @brief Writes a value to a specified register on the HUSB238 device.
 *
 * @param reg The register address to write to.
 * @param value The value to write to the register.
 * 
 * @return bool
 *         `true` if the write operation was successful, 
 *         `false` if the write operation failed.
 *
 * @details This function writes a value to a specified register on the HUSB238 device using 
 * I2C communication. It sends both the register address and the value to the device. If the 
 * write operation is successful, the function returns `true`; otherwise, it returns `false`.
 *
 * Usage:
 * - Use this function to write a value to a specific register of the HUSB238 device.
 *
 * Example:
 * ```
 * if (husb238_write_register(HUSB238_GO_COMMAND, 0x01)) {
 *     // Successfully wrote to register
 * } else {
 *     // Failed to write to register
 * }
 * ```
 */
/**************************************************************************/
bool husb238_write_register(uint8_t reg, uint8_t value)
{
	uint8_t buffer[2] = {reg, value};
	int result = i2c_write_blocking(i2c_instance, HUSB238_I2C_ADDRESS, buffer, 2, false);
	return result == 2;
}

/**************************************************************************/
/**
 * @brief Reads a value from a specified register on the HUSB238 device.
 *
 * @param reg The register address to read from.
 * @param value Pointer to a variable where the register value will be stored.
 * 
 * @return bool
 *         `true` if the register value was successfully read, 
 *         `false` if an error occurred during the read operation.
 *
 * @details This function performs an I2C write operation to select the register address 
 * and then reads the corresponding value from the register. The register address is passed 
 * as a parameter, and the value from the register is stored in the provided `value` pointer.
 * If the read and write operations are successful, the function returns `true`; otherwise, 
 * it returns `false`.
 *
 * Usage:
 * - Use this function to read the value of a specific register from the HUSB238 device.
 *
 * Example:
 * ```
 * uint8_t reg_value = 0;
 * if (husb238_read_register(HUSB238_PD_STATUS1, &reg_value)) {
 *     // Successfully read register value
 * } else {
 *     // Failed to read register value
 * }
 * ```
 */
/**************************************************************************/
bool husb238_read_register(uint8_t reg, uint8_t *value)
{
	int result = i2c_write_blocking(i2c_instance, HUSB238_I2C_ADDRESS, &reg, 1, true);
	if (result != 1)
	{
		return false;
	}

	result = i2c_read_blocking(i2c_instance, HUSB238_I2C_ADDRESS, value, 1, false);
	return result == 1;
}

/**************************************************************************/
/**
 * @brief Retrieves the USB Type-C Configuration Channel (CC) direction from the HUSB238 device.
 *
 * @return bool
 *         `true` if CC2 is connected, `false` if CC1 is connected.
 *
 * @details This function reads the 7th bit of the `HUSB238_PD_STATUS1` register to determine
 * the CC direction. The CC direction indicates which Configuration Channel (CC1 or CC2) is
 * active and connected. If the bit is set (1), it indicates that CC2 is connected; if the bit is 
 * cleared (0), CC1 is connected.
 *
 * Usage:
 * - Use this function to identify the active Configuration Channel (CC) in a USB Type-C connection.
 *
 * Example:
 * ```
 * if (husb238_getCCDirection()) {
 *     // CC2 is connected
 * } else {
 *     // CC1 is connected
 * }
 * ```
 */
/**************************************************************************/
bool husb238_getCCDirection(void)
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS1, &reg_value);
	return (reg_value>>7);
}

/**************************************************************************/
/**
 * @brief Checks if the HUSB238 device is attached to a Power Delivery (PD) source or sink.
 *
 * @return bool
 *         `true` if the device is attached (indicating a connection has been established), 
 *         `false` otherwise.
 *
 * @details This function reads the 6th bit of the `HUSB238_PD_STATUS1` register to determine 
 * the attachment status. If the attachment status bit is set (1), the device is considered 
 * attached to a PD source or sink. If the bit is cleared (0), there is no attachment.
 *
 * Usage:
 * - Use this function to check whether the HUSB238 is physically connected to a PD source or sink.
 *
 * Example:
 * ```
 * if (husb238_isAttached()) {
 *     // Device is attached
 * } else {
 *     // Device is not attached
 * }
 * ```
 */
/**************************************************************************/
bool husb238_isAttached()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS1, &reg_value);
	return (reg_value>>6) & 0x01;
}

/**************************************************************************/
/**
 * @brief Retrieves the Power Delivery (PD) response status from the HUSB238 device.
 *
 * @return uint8_t
 *         The PD response status as one of the following values:
 *         - `NO_RESPONSE`: No response received.
 *         - `RESPONE_SUCCESS`: Successful response.
 *         - `RESPONSE_INVALID_CMD_OR_ARG`: Invalid command or argument.
 *         - `RESPONE_CMD_NOT_SUPPORTED`: Command not supported by the device.
 *         - `RESPONE_TRANSACTION_FAIL_NO_GOOD_CRC`: Transaction failed due to no GoodCRC being received.
 *
 * @details This function reads the 3rd to 5th bits of the `HUSB238_PD_STATUS1` register, 
 * which represent the PD response status. It then returns the response status as a value
 * based on the defined constants.
 *
 * Usage:
 * - Use this function to check the response status of a Power Delivery transaction.
 *
 * Example:
 * ```
 * uint8_t response = husb238_getPDRespone();
 * if (response == RESPONE_SUCCESS) {
 *     // PD response successful
 * } else if (response == RESPONSE_INVALID_CMD_OR_ARG) {
 *     // Invalid command or argument
 * }
 * ```
 */
/**************************************************************************/
uint8_t husb238_getPDRespone()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS1, &reg_value);
	return (reg_value>>3) & 0x07;
}

/**************************************************************************/
/**
 * @brief Checks the status of the 5V contract voltage from the HUSB238 device.
 *
 * @return bool
 *         `true` if the 5V contract voltage is active, `false` otherwise.
 *
 * @details This function reads the 2nd bit of the `HUSB238_PD_STATUS1` register to determine
 * the 5V contract voltage status. The function returns `true` if the bit is set (indicating
 * that the 5V contract voltage is active), and `false` otherwise.
 *
 * Usage:
 * - Use this function to check if the 5V contract voltage is being negotiated or is active.
 *
 * Example:
 * ```
 * if (husb238_get5VContractV()) {
 *     // 5V contract voltage is active
 * } else {
 *     // 5V contract voltage is not active
 * }
 * ```
 */
/**************************************************************************/
bool husb238_get5VContractV()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS1, &reg_value);
	return (reg_value>>2) & 0x01;
}

/**************************************************************************/
/**
 * @brief Retrieves the current for the 5V contract from the HUSB238 device.
 *
 * @return uint8_t
 *         The current for the 5V contract as an HUSB238_5VCurrentContract enum value.
 *
 * @details This function reads the bottom two bits (0-1) of the `HUSB238_PD_STATUS1` register,
 * which represent the 5V contract current. The current is returned as an enum value corresponding
 * to the possible 5V contract currents (e.g., `CURRENT_0_5_A`, `CURRENT_1_0_A`, etc.).
 *
 * Usage:
 * - Use this function to determine the current associated with a 5V Power Delivery contract.
 *
 * Example:
 * ```
 * uint8_t current = husb238_get5VContractA();
 * if (current == CURRENT_1_0_A) {
 *     // 1A 5V contract current
 * }
 * ```
 */
/**************************************************************************/
uint8_t husb238_get5VContractA()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS1, &reg_value);
	return reg_value & 0x03;
}

/**************************************************************************/
/**
 * @brief Retrieves the source voltage of the currently selected Power Delivery (PD) profile.
 *
 * @return uint16_t
 *         The source voltage in volts (V) for the active PD profile.
 *
 * @details This function reads the `HUSB238_PD_STATUS0` register and extracts bits 4-7,
 * which represent the source voltage setting. The extracted value is then parsed using 
 * the `parse_voltage()` function to convert it into a voltage value in volts.
 *
 * Usage:
 * - Use this function to get the voltage level of the selected PD profile.
 *
 * Example:
 * ```
 * uint16_t voltage = husb238_getPDSrcVoltage();
 * printf("Source Voltage: %u V\n", voltage);
 * ```
 */
/**************************************************************************/
uint16_t husb238_getPDSrcVoltage()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS0, &reg_value);
	return parse_voltage((reg_value>>4) & 0x0F);
}

/**************************************************************************/
/**
 * @brief Retrieves the source current of the currently selected Power Delivery (PD) profile.
 *
 * @return uint16_t
 *         The source current in milliamps (mA) for the active PD profile.
 *
 * @details This function reads the `HUSB238_PD_STATUS0` register and extracts bits 0-3, 
 * which represent the source current setting. The extracted value is then parsed 
 * using the `parse_current()` function to convert it into milliamps.
 *
 * Usage:
 * - Use this function to get the current capacity of the selected PD profile.
 *
 * Example:
 * ```
 * uint16_t current = husb238_getPDSrcCurrent();
 * printf("Source Current: %u mA\n", current);
 * ```
 */
/**************************************************************************/
uint16_t husb238_getPDSrcCurrent()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_PD_STATUS0, &reg_value);
	return parse_current(reg_value & 0x0F);
}

/**************************************************************************/
/**
 * @brief Retrieves the currently selected Power Delivery (PD) profile.
 *
 * @return uint8_t
 *         The currently selected PD profile as a 4-bit value (0-15).
 *         This value corresponds to the voltage profile currently in use.
 *
 * @details This function reads the `HUSB238_SRC_PDO` register and extracts bits 4-7,
 * which indicate the selected PD source profile. The result can be matched against
 * predefined PD profiles (e.g., `PD_5V`, `PD_9V`, etc.).
 *
 * Usage:
 * - Use this function to check which PD profile is currently active.
 *
 * Example:
 * ```
 * uint8_t current_profile = husb238_getSelectedPD();
 * if (current_profile == PD_5V) {
 *     // 5V profile is selected
 * }
 * ```
 */
/**************************************************************************/
uint8_t husb238_getSelectedPD()
{
	uint8_t reg_value = 0;
	husb238_read_register(HUSB238_SRC_PDO, &reg_value);
	return (reg_value>>4) & 0x0F;
}

/**************************************************************************/
/**
 * @brief Checks if a specific voltage profile is detected and supported by the HUSB238.
 *
 * @param pd_src The voltage profile to check, specified as a `uint8_t` value
 *               corresponding to a PD source voltage (e.g., `PD_SRC_5V`, `PD_SRC_9V`, etc.).
 *
 * @return bool
 *         `true` if the specified voltage profile is detected and supported.
 *         `false` otherwise.
 *
 * @details This function iterates through the `supported_voltage` array, which contains
 * the voltage profiles detected by the `husb238_getSupportedVoltages()` function. If the
 * specified voltage (`pd_src`) matches any entry in the array, the function returns `true`.
 *
 * Usage:
 * - Call this function to verify if a specific voltage is available before attempting
 *   to select or request a power delivery profile.
 *
 * Example:
 * ```
 * if (husb238_isVoltageDetected(PD_SRC_9V)) {
 *     // 9V profile is available
 * } else {
 *     // 9V profile is not supported
 * }
 * ```
 */
/**************************************************************************/
bool husb238_isVoltageDetected(uint8_t pd_src)
{
	for(uint8_t i = 0; i < MAX_PROFILES; i++)
	{
		if(supported_profiles[i].voltage == pd_src)
		{
			return true;
		}
	}
	return false;
}

/**************************************************************************/
/**
 * @brief Retrieves the supported voltage profiles from the HUSB238 device.
 *
 * This function reads the registers corresponding to different voltage profiles
 * (from 5V to 20V) and identifies which profiles are supported by the device.
 *
 * @return uint8_t
 *         The number of supported voltage profiles detected.
 *
 * @details The function performs the following steps:
 * 1. Iterates over the source power delivery output (PDO) registers, from `HUSB238_SRC_PDO_5V`
 *    to `HUSB238_SRC_PDO_20V`.
 * 2. Reads the content of each register to determine if the voltage profile is supported
 *    by checking the 7th bit (support flag).
 * 3. If supported, stores the voltage value (parsed using `parse_voltage`) and current value 
 *    (parsed using `parse_current`) into the global `supported_voltage` and `supported_current`
 *    arrays, respectively.
 * 4. Increments the support count for each detected profile.
 *
 * Usage:
 * - This function should be called after initializing the device to determine available
 *   power delivery options.
 *
 * Example:
 * ```
 * uint8_t num_profiles = husb238_getSupportedVoltages();
 * if (num_profiles > 0) {
 *     // Device supports power profiles
 * }
 * ```
 */
/**************************************************************************/
uint8_t husb238_getSupportedVoltages()
{
	uint8_t reg_value = 0;
	uint8_t support_cnt =0;
	for(uint8_t i = HUSB238_SRC_PDO_5V; i < HUSB238_SRC_PDO_20V+1; i++)
	{
		husb238_read_register(i, &reg_value);
		if(((reg_value >> 7) & 0x01) == 1)
		{
			supported_profiles[support_cnt].voltage = parse_voltage(i-1);
			supported_profiles[support_cnt].current = parse_current(reg_value & 0x0F);
			switch (i-1)
			{
			case PD_5V:
				supported_profiles[support_cnt].power = 5 * supported_profiles[support_cnt].current;
				break;
			case PD_9V:
				supported_profiles[support_cnt].power = 9 * supported_profiles[support_cnt].current;
				break;
			case PD_12V:
				supported_profiles[support_cnt].power = 12 * supported_profiles[support_cnt].current;
				break;
			case PD_15V:
				supported_profiles[support_cnt].power = 15 * supported_profiles[support_cnt].current;
				break;
			case PD_18V:
				supported_profiles[support_cnt].power = 18 * supported_profiles[support_cnt].current;
				break;
			case PD_20V:
				supported_profiles[support_cnt].power = 20 * supported_profiles[support_cnt].current;
				break;
			default:
				supported_profiles[support_cnt].power = 0;
				break;
			}
			support_cnt++;
		}
	}
	return support_cnt;
}

/**************************************************************************/
/*!
	@brief  Selects a PD output.
	@param pd The PD selection as an HUSB238_PDOelection enum value.
	@details This function writes to bits 4-7 of the SRC_PDO register to select
   a PD.
*/
/**************************************************************************/
void husb2238_selectPD(uint8_t pd_src)
{
	husb238_write_register(HUSB238_SRC_PDO, pd_src << 4);
}

/**************************************************************************/
/*!
	@brief  Requests a PD.
	@details This function writes 0b00001 to the GO_COMMAND register to request
   a PD.
*/
/**************************************************************************/
void husb238_requestPD()
{
	husb238_write_register(HUSB238_GO_COMMAND, 0b00001);
}

/**************************************************************************/
/**
 * @brief Resets the HUSB238 USB-PD controller.
 *
 * This function triggers a reset operation on the HUSB238 device by writing the 
 * appropriate value to the `GO_COMMAND` register. The reset command clears the 
 * device's current state and prepares it for new operations.
 *
 * @details The function writes `0b10000` to the lower 5 bits of the `GO_COMMAND` register, 
 * as defined in the HUSB238 specification, to initiate the reset process.
 *
 * Usage:
 * - Call this function if the device needs to be reinitialized or if it is in an
 *   undefined state.
 */
/**************************************************************************/
void husb238_reset()
{
	// Create an Adafruit_I2CRegisterBits object for the bottom 5 bits (0-4) of
	// GO_COMMAND
	// Write 0b10000 to the bottom 5 bits
	husb238_write_register(HUSB238_GO_COMMAND, 0b10000);
}

/**************************************************************************/
/**
 * @brief Initializes the HUSB238 USB-PD controller.
 *
 * This function configures the I2C interface and performs necessary checks to ensure the 
 * HUSB238 is ready for operation. It verifies device attachment, Power Delivery (PD) 
 * response, and supported voltage profiles.
 *
 * @param i2c_port Specifies the I2C hardware instance to use (0 for i2c0, 1 for i2c1).
 *
 * @return int8_t
 *         >0: The number of supported voltage profiles if initialization is successful.
 *          0: The HUSB238 is not attached or not responding.
 *         -1: The Power Delivery (PD) response check failed.
 *         -2: No valid voltage profiles are detected.
 *
 * This function performs the following steps:
 * 1. Configures the I2C interface with the specified pins and communication settings.
 * 2. Checks if the HUSB238 device is attached using `husb238_isAttached()`.
 * 3. Validates the PD response using `husb238_getPDRespone()`.
 * 4. Retrieves the number of supported voltage profiles using `husb238_getSupportedVoltages()`.
 */
/**************************************************************************/
int8_t husb238_init(i2c_inst_t *i2c_port)
{
	// Wähle die I2C-Instanz
	i2c_instance = i2c_port;

	if(!husb238_isAttached())
	{
		return 0;
	}

	if(husb238_getPDRespone() != RESPONE_SUCCESS)
	{
		return -1;
	}

	uint8_t num_voltage = husb238_getSupportedVoltages();
	if(num_voltage == 0)
	{
		return -2;
	}
	return num_voltage;
}