#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

#include <string.h>

static pin_t board_pins[(sizeof(pin_instances) / sizeof(pin_instances[0]))];
static volatile int pin_idx = 0;

void button_interrupt_handler(uint8_t pad, bool state, uint32_t tick)
{
    if (BUTTON_PAD != pin_instances[pin_idx].pad) {
        pin_t* pin_p = &board_pins[pin_idx];
        pin_set(pin_p, !pin_get(pin_p));
    }

    if (pin_idx < ((sizeof(pin_instances) / sizeof(pin_instances[0])) - 1)) {
        pin_idx++;
    } else {
        pin_idx = 0;
    }
}

int main(void)
{
    pin_t button_pin = PIN_IN_IRQ(&PIN_INSTANCE_IRQ(BUTTON_PAD));

    pins_init();

    for (int idx = 0; idx < (sizeof(pin_instances) / sizeof(pin_instances[0])); idx++) {
        if (BUTTON_PAD != pin_instances[idx].pad) {
            pin_t* pin_p = &board_pins[idx];

            *pin_p = PIN_OUT(&pin_instances[idx]);
            pin_p->value = 0;

            pin_init(pin_p);
        }
    }

    pins_set_interrupt_handler(button_interrupt_handler);

    pin_init(&button_pin);
    __enable_irq();

    while (1) {
    }
}
