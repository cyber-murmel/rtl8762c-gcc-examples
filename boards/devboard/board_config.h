#ifndef _BOARDS_DEVBOARD_BOARD_CONFIG_H
#define _BOARDS_DEVBOARD_BOARD_CONFIG_H

#define BOARD_NAME "devboard"
#define MCU_NAME "RTL8762CKF"

#include "pin/pin.h"

#define BOARD_TX_PAD (P3_0)
#define BOARD_RX_PAD (P3_1)

#define LED_PAD (P4_3)

#define BUTTON_PAD P1_7

// static const uint8_t board_pads[] = {
//     H_0,
//     P2_7,
//     P2_6,
//     P2_5,
//     P2_4,
//     P2_3,
//     P2_2,
//     P2_1,
//     P2_0,
//     H_2,
//     H_1,
//     P3_2,
//     P3_3,
//     P3_4,
//     P3_5,
//     P3_6,
//     P1_7,
//     P1_6,
//     P0_0,
//     P0_1,
//     P0_2,
//     P0_4,
//     P0_5,
//     P0_6,
//     P0_7,
//     P4_0,
//     P4_1,
//     P4_2,
//     P4_3,
// };

static const pin_id_t board_pin_ids[] = {
    PIN_ID(H_0),
    PIN_ID_IRQ(P2_7),
    PIN_ID_IRQ(P2_6),
    PIN_ID_IRQ(P2_5),
    PIN_ID_IRQ(P2_4),
    PIN_ID_IRQ(P2_3),
    PIN_ID_IRQ(P2_2),
    PIN_ID_IRQ(P2_1),
    PIN_ID_IRQ(P2_0),
    PIN_ID(H_2),
    PIN_ID(H_1),
    PIN_ID_IRQ(P3_2),
    PIN_ID_IRQ(P3_3),
    PIN_ID_IRQ(P3_4),
    PIN_ID_IRQ(P3_5),
    PIN_ID_IRQ(P3_6),
    PIN_ID_IRQ(P1_7),
    PIN_ID_IRQ(P1_6),
    PIN_ID_IRQ(P0_0),
    PIN_ID_IRQ(P0_1),
    PIN_ID_IRQ(P0_2),
    PIN_ID_IRQ(P0_4),
    PIN_ID_IRQ(P0_5),
    PIN_ID_IRQ(P0_6),
    PIN_ID_IRQ(P0_7),
    PIN_ID(P4_0),
    PIN_ID(P4_1),
    PIN_ID(P4_2),
    PIN_ID(P4_3),
};

#endif // _BOARDS_DEVBOARD_BOARD_CONFIG_H
