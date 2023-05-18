#ifndef _BOARDS_DEVBOARD_BOARD_CONFIG_H
#define _BOARDS_DEVBOARD_BOARD_CONFIG_H

#define BOARD_NAME "devboard"
#define MCU_NAME "RTL8762CKF"

#include "pin/pin.h"

#define BOARD_TX_PAD (P3_0)
#define BOARD_RX_PAD (P3_1)

#define LED_PAD (P4_3)

#define BUTTON_PAD (P1_7)

static const pin_id_t board_pin_ids[] = {
    PIN_ID(H_0),
    PIN_ID(P2_7),
    PIN_ID(P2_6),
    PIN_ID(P2_5),
    PIN_ID(P2_4),
    PIN_ID(P2_3),
    PIN_ID(P2_2),
    PIN_ID(P2_1),
    PIN_ID(P2_0),
    PIN_ID(H_2),
    PIN_ID(H_1),
    PIN_ID(P3_2),
    PIN_ID(P3_3),
    PIN_ID(P3_4),
    PIN_ID(P3_5),
    PIN_ID(P3_6),
    PIN_ID(P1_7),
    PIN_ID(P1_6),
    PIN_ID(P0_0),
    PIN_ID(P0_1),
    PIN_ID(P0_2),
    PIN_ID(P0_4),
    PIN_ID(P0_5),
    PIN_ID(P0_6),
    PIN_ID(P0_7),
    PIN_ID(P4_0),
    PIN_ID(P4_1),
    PIN_ID(P4_2),
    PIN_ID(P4_3),
};

#endif // _BOARDS_DEVBOARD_BOARD_CONFIG_H
