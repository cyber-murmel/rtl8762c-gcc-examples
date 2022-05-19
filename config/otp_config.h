/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    otp_config.h
* @brief   Update Configuration in APP
* @date    2017.6.6
* @version v1.0
* *************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor
*Corporation</center></h2>
* *************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef OTP_CONFIG_H
#define OTP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x_wdg.h"
/*============================================================================*
 *                        debug configuration
 *============================================================================*/
/** @brief just for debug */
#define SYSTEM_TRACE_ENABLE 0

/*============================================================================*
 *                        flash configuration
 *============================================================================*/
/** @brief support for puran flash*/
#define FTL_APP_CALLBACK_ENABLE 0
/** @brief as recommended by Realtek*/
#define DISABLE_FTL_REATURE 0
/** @brief enable BP, set lock level depend on flash layout and selected flash
 * id */
#define FLASH_BLOCK_PROTECT_ENABLE 1
/** @brief modify delay time for wakeup flash from power down mode to standby
 * mode*/
#define AFTER_TOGGLE_CS_DELAY 6

/*============================================================================*
 *                        platform configuration
 *============================================================================*/
/** @brief default enable swd pinmux */
#define SWD_PINMUX_ENABLE 1
/** @brief default disable watch dog in rom */
#define ROM_WATCH_DOG_ENABLE 0
/** @brief set wdg mode, default reset all */
#define ROM_WATCH_DOG_MODE RESET_ALL
/** @brief as recommended by Realtek*/
#define XTAL_40M_DELAY_TIME 0

/*============================================================================*
 *                        upperstack configuration
 *============================================================================*/
// add more here

/*============================================================================*
 *                        app configuration
 *============================================================================*/
#define OTA_TIMEOUT_TOTAL 240
#define OTA_TIMEOUT_WAIT4_CONN 60
#define OTA_TIMEOUT_WAIT4_IMAGE_TRANS 200
#define OTA_TIMEOUT_CTITTV 0xFF
// add more here

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group OTP_CONFIG */
#endif
