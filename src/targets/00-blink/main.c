#include <platform_utils.h>

#include "board_config.h"
#include "pin/pin.h"

// prevent `warning: function with qualified void return type called`
#define delay_ms(T) (((void (*)(uint32_t)) platform_delay_ms)(T))

int main(void)
{
    pin_t led_pin = {
        .id = PIN_ID(LED_PAD),
        .direction = OUT,
    };

    pins_init();

    pin_init(&led_pin);

    while (1) {
        pin_set(&led_pin, !pin_get(&led_pin));
        delay_ms(1000);
    }
}
