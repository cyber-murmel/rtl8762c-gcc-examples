#ifndef _APP_FLAGS_H_
#define _APP_FLAGS_H_

/** @defgroup  PERIPH_Config Peripheral App Configuration
 * @brief This file is used to config app functions.
 * @{
 */
/*============================================================================*
 *                              Constants
 *============================================================================*/

/** @brief  Config APP LE link number */
#define APP_MAX_LINKS 1
/** @brief  Config APP LE bond number */
#define APP_MAX_BONDS 4
/** @brief  Config DLPS: 0-Disable DLPS, 1-Enable DLPS */
#define F_BT_DLPS_EN 0
/** @brief  Config ANCS Client: 0-Not built in, 1-Open ANCS client function */
#define F_BT_ANCS_CLIENT_SUPPORT 0
#define F_BT_ANCS_APP_FILTER (F_BT_ANCS_CLIENT_SUPPORT & 1)
#define F_BT_ANCS_GET_APP_ATTR (F_BT_ANCS_CLIENT_SUPPORT & 0)
/** @brief  Config ANCS Client debug log: 0-close, 1-open  */
#define F_BT_ANCS_CLIENT_DEBUG (F_BT_ANCS_CLIENT_SUPPORT & 0)

/**
 * @defgroup App_Flags_Realtek_Libs Undocumented Realtek flags.
 * @brief Flags for activating some undocumented features within the Realtek
 * libs. All flags are set to 0 because they were't defined and defaultet to 0.
 * Now they must be defined because of activating -Wundefined within the
 * makefile to prevent configuration failures without any warnings or errors.
 * @{
 */
/** Enable write hardfault record to flash. */
#define ENABLE_WRITE_HARDFAULT_RECORD_TO_FLASH 0
/** Before watchdog system reset, write reset reason to specific flash address.
 */
#define WRITE_REASON_TO_FLASH_BEFORE_RESET_ENABLE 0
/** Internal Realtek lib error dumping of lib on watchdog reset. */
#define DUMP_INFO_BEFORE_RESET_DISABLE 0
/** Internal Realtek lib debug functionality. */
#define ENABLE_FLASH_READ_TURN_OFF_RF 0
/** Internal Realtek lib debug functionality. */
#define ROM_OTA_LINKLOSS_RST 0

#define OTA_BUF_CHECK_EN 1
#define DFU_TEMP_BUFFER_SIZE (2048)

/** Enable Debug Monitor Function (include NVIC Enable and DWT configuration).
 */
#define DEBUG_WATCHPOINT_ENABLE 0

#define F_BT_LE_5_0_AE_ADV_SUPPORT 0
#define F_BT_LE_5_0_AE_SCAN_SUPPORT 0
#define F_BT_LE_6_0_AOA_AOD_SUPPORT 0

/** Enable multibonding configuration.*/
// #define BT_STACK_CONFIG_ENABLE

/** @} */ /* App_Flags_Realtek_Libs */

/** @} */ /* End of group PERIPH_Config */
#endif