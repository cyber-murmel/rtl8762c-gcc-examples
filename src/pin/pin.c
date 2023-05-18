#include <rtl876x_gpio.h>
#include <rtl876x_rcc.h>

#include "pin/pin.h"
#include "rcc/rcc.h"

void pins_init(void)
{
    static const rcc_periph_t GPIO_RCCPeriph = RCC_PERIPH(GPIO);
    rcc_periph_set(&GPIO_RCCPeriph, true);
}

void pin_init(pin_t* pin_p)
{
    GPIO_InitTypeDef Gpio_Struct;

    if ((IN == pin_p->direction) && (SW_MODE == pin_p->mode)) {
        // SW mode pins are output only
        return;
    }

    Pad_Config(
        pin_p->id->pad,
        pin_p->mode,
        PAD_IS_PWRON,
        pin_p->pull,
        pin_p->direction,
        pin_p->value);

    Pad_PullConfigValue(pin_p->id->pad, pin_p->pull_config);

    if (SW_MODE == pin_p->mode) {
        Pinmux_Config(pin_p->id->pad, IDLE_MODE);
    } else {
        GPIOMode_TypeDef direction = (IN == pin_p->direction) ? GPIO_Mode_IN : GPIO_Mode_OUT;

        GPIO_StructInit(&Gpio_Struct);
        Gpio_Struct.GPIO_Pin = pin_p->id->bit;
        Gpio_Struct.GPIO_Mode = direction;
        GPIO_Init(&Gpio_Struct);

        GPIO_WriteBit(pin_p->id->bit, pin_p->value);

        Pinmux_Config(pin_p->id->pad, DWGPIO);
    }
}

bool pin_get(pin_t* pin_p)
{
    if (SW_MODE == pin_p->mode) {
        return pin_p->value;
    } else {
        if (IN == pin_p->direction) {
            return (SET == GPIO_ReadInputDataBit(pin_p->id->bit));
        } else {
            return (SET == GPIO_ReadOutputDataBit(pin_p->id->bit));
        }
    }
}

void pin_set(pin_t* pin_p, bool value)
{
    if (SW_MODE == pin_p->mode) {
        Pad_OutputControlValue(pin_p->id->pad, value);
        pin_p->value = value;
    } else {
        GPIO_WriteBit(pin_p->id->bit, value);
    }
}
