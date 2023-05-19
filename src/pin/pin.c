#include <app_section.h>
#include <platform_utils.h>
#include <rtl876x_gpio.h>
#include <rtl876x_nvic.h>
#include <rtl876x_rcc.h>

#include "pin/pin.h"
#include "rcc/rcc.h"

#include <stddef.h>

static volatile pin_interrupt_handler_t pin_interrupt_handler = NULL;

void pins_init(void)
{
    static const rcc_periph_t GPIO_RCCPeriph = RCC_PERIPH(GPIO);
    rcc_periph_set(&GPIO_RCCPeriph, true);
}

void pin_init(pin_t* pin_p)
{
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
        GPIO_InitTypeDef Gpio_Struct;
        GPIOMode_TypeDef direction = (IN == pin_p->direction) ? GPIO_Mode_IN : GPIO_Mode_OUT;

        GPIO_StructInit(&Gpio_Struct);
        Gpio_Struct.GPIO_Pin = pin_p->id->bit;
        Gpio_Struct.GPIO_Mode = direction;
        if (((GPIOMode_TypeDef)IN == direction) && (NULL != pin_p->interrupt)) {
            Gpio_Struct.GPIO_ITCmd = ENABLE;
            Gpio_Struct.GPIO_ITTrigger = pin_p->interrupt->level_type;
            Gpio_Struct.GPIO_ITPolarity = pin_p->interrupt->polarity;
            Gpio_Struct.GPIO_ITDebounce = pin_p->interrupt->debounce;
            Gpio_Struct.GPIO_DebounceTime = pin_p->interrupt->debounce_time_ms;
        }
        GPIO_Init(&Gpio_Struct);

        if (((GPIOMode_TypeDef)IN == direction) && (NULL != pin_p->interrupt)) {
            // disable interrupt
            GPIO_MaskINTConfig(pin_p->id->bit, ENABLE);
            GPIO_INTConfig(pin_p->id->bit, DISABLE);

            // configure IRQ
            NVIC_InitTypeDef NVIC_InitStruct;
            NVIC_InitStruct.NVIC_IRQChannel = pin_p->id->irq;
            NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStruct);

            // enable interrupt
            GPIO_MaskINTConfig(pin_p->id->bit, DISABLE);
            GPIO_INTConfig(pin_p->id->bit, ENABLE);
        } else if ((GPIOMode_TypeDef)OUT == direction) {
            GPIO_WriteBit(pin_p->id->bit, pin_p->value);
        }

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

void pins_set_interrupt_handler(pin_interrupt_handler_t interrupt_handler)
{
    pin_interrupt_handler = interrupt_handler;
}

static void _generic_pin_interrupt_handler(pin_id_t* id) DATA_RAM_FUNCTION;
static void _generic_pin_interrupt_handler(pin_id_t* id)
{
    uint32_t tick;
    bool state;
    /* disable interrups globally */
    __disable_irq();
    /* store relevant values */
    tick = platform_vendor_tick();
    state = GPIO_ReadInputDataBit(id->bit);
    /* disable interrupt in pin */
    GPIO_INTConfig(id->bit, DISABLE);
    GPIO_MaskINTConfig(id->bit, ENABLE);
    /* mark interrupt as processed */
    GPIO_ClearINTPendingBit(id->bit);
    /* enable interrupt for this pin again */
    GPIO_MaskINTConfig(id->bit, DISABLE);
    GPIO_INTConfig(id->bit, ENABLE);
    /* enable interrupt for this pin again */
    if (NULL != pin_interrupt_handler) {
        pin_interrupt_handler(id, state, tick);
    }
    /* enable interrups again globally */
    __enable_irq();
}

/* attach generic handler to all possible pins */
#define GPIO_HANDLER_NAME(PAD) GPIO##PAD##_Handler
#define GPIO_HANDLER(PAD)                                \
    void GPIO_HANDLER_NAME(PAD)(void) DATA_RAM_FUNCTION; \
    void GPIO_HANDLER_NAME(PAD)(void)                    \
    {                                                    \
        _generic_pin_interrupt_handler(&PIN_ID(PAD));    \
    }

GPIO_HANDLER(P0_0)
GPIO_HANDLER(P0_1)
GPIO_HANDLER(P0_2)
GPIO_HANDLER(P0_3)
GPIO_HANDLER(P0_4)
GPIO_HANDLER(P0_5)
GPIO_HANDLER(P0_6)
GPIO_HANDLER(P0_7)
GPIO_HANDLER(P1_0)
GPIO_HANDLER(P1_1)
GPIO_HANDLER(P1_2)
GPIO_HANDLER(P1_3)
GPIO_HANDLER(P1_4)
GPIO_HANDLER(P1_5)
GPIO_HANDLER(P1_6)
GPIO_HANDLER(P1_7)
GPIO_HANDLER(P2_0)
GPIO_HANDLER(P2_1)
GPIO_HANDLER(P2_2)
GPIO_HANDLER(P2_3)
GPIO_HANDLER(P2_4)
GPIO_HANDLER(P2_5)
GPIO_HANDLER(P2_6)
GPIO_HANDLER(P2_7)
GPIO_HANDLER(P3_0)
GPIO_HANDLER(P3_1)
GPIO_HANDLER(P3_2)
GPIO_HANDLER(P3_3)
GPIO_HANDLER(P3_4)
GPIO_HANDLER(P3_5)
GPIO_HANDLER(P3_6)
