#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

int main(void)
{
    pin_t button_pin = {
        .id = &PIN_ID(BUTTON_PAD),
        .direction = IN,
        .mode = PINMUX_MODE,
        .pull = PULL_UP,
        .pull_config = STRONG_PULL,
    };

    pin_t board_pins[(sizeof(board_pin_ids) / sizeof(board_pin_ids[0]))];

    pins_init();

    pin_init(&button_pin);

    for (int idx = 0; idx < (sizeof(board_pin_ids) / sizeof(board_pin_ids[0])); idx++) {
        if (BUTTON_PAD != board_pin_ids[idx].pad) {
            board_pins[idx] = (pin_t) {
                .id = &board_pin_ids[idx],
                .direction = OUT,
                .value = 0,
            };
            pin_init(&board_pins[idx]);
        }
    }

    while (1) {
        for (int idx = 0; idx < (sizeof(board_pin_ids) / sizeof(board_pin_ids[0])); idx++) {
            if (BUTTON_PAD != board_pin_ids[idx].pad) {
                while (pin_get(&button_pin))
                    ;
                pin_set(&board_pins[idx], !pin_get(&board_pins[idx]));
                while (!pin_get(&button_pin))
                    ;
            }
        }
    }
}
