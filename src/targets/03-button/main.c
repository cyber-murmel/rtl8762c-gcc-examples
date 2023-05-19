#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

#include <string.h>

int main(void)
{
    pin_t board_pins[(sizeof(board_pin_ids) / sizeof(board_pin_ids[0]))];
    pin_t* button_pin_p = &PIN_IN(PIN_ID(BUTTON_PAD));

    pins_init();

    for (int idx = 0; idx < (sizeof(board_pin_ids) / sizeof(board_pin_ids[0])); idx++) {
        if (BUTTON_PAD != board_pin_ids[idx].pad) {
            pin_t* pin_p = &board_pins[idx];
            memcpy(pin_p, &PIN_OUT(board_pin_ids[idx]), sizeof(*pin_p));
            pin_init(pin_p);
        }
    }

    pin_init(button_pin_p);

    while (1) {
        for (int idx = 0; idx < (sizeof(board_pin_ids) / sizeof(board_pin_ids[0])); idx++) {
            if (BUTTON_PAD != board_pin_ids[idx].pad) {
                pin_t* pin_p = &board_pins[idx];

                while (pin_get(button_pin_p))
                    ;

                pin_set(pin_p, !pin_get(pin_p));

                while (!pin_get(button_pin_p))
                    ;
            }
        }
    }
}
