#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

#include <string.h>

int main(void)
{
    pin_t board_pins[(sizeof(pin_instances) / sizeof(pin_instances[0]))];
    pin_t* button_pin_p = &PIN_IN(&PIN_INSTANCE(BUTTON_PAD));

    pins_init();

    for (int idx = 0; idx < (sizeof(pin_instances) / sizeof(pin_instances[0])); idx++) {
        if (BUTTON_PAD != pin_instances[idx].pad) {
            pin_t* pin_p = &board_pins[idx];

            *pin_p = PIN_OUT(&pin_instances[idx]);
            pin_p->value = 0;

            pin_init(pin_p);
        }
    }

    pin_init(button_pin_p);

    while (1) {
        for (int idx = 0; idx < (sizeof(pin_instances) / sizeof(pin_instances[0])); idx++) {
            if (BUTTON_PAD != pin_instances[idx].pad) {
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
