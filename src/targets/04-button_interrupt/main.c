#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

#include <string.h>

static pin_t board_pins[(sizeof(board_pin_ids) / sizeof(board_pin_ids[0]))];
static volatile int pin_idx = 0;

void button_interrupt_handler(pin_id_t* id, bool state, uint32_t tick)
{
    if (BUTTON_PAD != board_pin_ids[pin_idx].pad) {
        pin_t* pin_p = &board_pins[pin_idx];
        pin_set(pin_p, !pin_get(pin_p));
    }

    if (pin_idx < ((sizeof(board_pin_ids) / sizeof(board_pin_ids[0])) - 1)) {
        pin_idx++;
    } else {
        pin_idx = 0;
    }
}

int main(void)
{
    pin_t button_pin = PIN_IN_IRQ(PIN_ID_IRQ(BUTTON_PAD));

    pins_init();

    for (int idx = 0; idx < (sizeof(board_pin_ids) / sizeof(board_pin_ids[0])); idx++) {
        if (BUTTON_PAD != board_pin_ids[idx].pad) {
            pin_t* pin_p = &board_pins[idx];
            memcpy(pin_p, &PIN_OUT(board_pin_ids[idx]), sizeof(*pin_p));
            pin_init(pin_p);
        }
    }

    pins_set_interrupt_handler(button_interrupt_handler);

    pin_init(&button_pin);
    __enable_irq();

    while (1) {
    }
}
