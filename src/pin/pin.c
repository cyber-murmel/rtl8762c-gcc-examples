#include <app_section.h>
#include <platform_utils.h>
#include <rtl876x_gpio.h>
#include <rtl876x_nvic.h>
#include <rtl876x_rcc.h>

#include "pin/pin.h"
#include "rcc/rcc.h"

#include <stddef.h>

const pin_instance_t pin_instances[TOTAL_PIN_NUM] = {
    [P0_0] = PIN_INSTANCE_IRQ(P0_0),
    [P0_1] = PIN_INSTANCE_IRQ(P0_1),
    [P0_2] = PIN_INSTANCE_IRQ(P0_2),
    [P0_3] = PIN_INSTANCE_IRQ(P0_3),
    [P0_4] = PIN_INSTANCE_IRQ(P0_4),
    [P0_5] = PIN_INSTANCE_IRQ(P0_5),
    [P0_6] = PIN_INSTANCE_IRQ(P0_6),
    [P0_7] = PIN_INSTANCE_IRQ(P0_7),
    [P1_0] = PIN_INSTANCE_IRQ(P1_0),
    [P1_1] = PIN_INSTANCE_IRQ(P1_1),
    [P1_2] = PIN_INSTANCE_IRQ(P1_2),
    [P1_3] = PIN_INSTANCE_IRQ(P1_3),
    [P1_4] = PIN_INSTANCE_IRQ(P1_4),
    [P1_5] = PIN_INSTANCE_IRQ(P1_5),
    [P1_6] = PIN_INSTANCE_IRQ(P1_6),
    [P1_7] = PIN_INSTANCE_IRQ(P1_7),
    [P2_0] = PIN_INSTANCE_IRQ(P2_0),
    [P2_1] = PIN_INSTANCE_IRQ(P2_1),
    [P2_2] = PIN_INSTANCE_IRQ(P2_2),
    [P2_3] = PIN_INSTANCE_IRQ(P2_3),
    [P2_4] = PIN_INSTANCE_IRQ(P2_4),
    [P2_5] = PIN_INSTANCE_IRQ(P2_5),
    [P2_6] = PIN_INSTANCE_IRQ(P2_6),
    [P2_7] = PIN_INSTANCE_IRQ(P2_7),
    [P3_0] = PIN_INSTANCE_IRQ(P3_0),
    [P3_1] = PIN_INSTANCE_IRQ(P3_1),
    [P3_2] = PIN_INSTANCE_IRQ(P3_2),
    [P3_3] = PIN_INSTANCE_IRQ(P3_3),
    [P3_4] = PIN_INSTANCE_IRQ(P3_4),
    [P3_5] = PIN_INSTANCE_IRQ(P3_5),
    [P3_6] = PIN_INSTANCE_IRQ(P3_6),
    [P4_0] = PIN_INSTANCE(P4_0),
    [P4_1] = PIN_INSTANCE(P4_1),
    [P4_2] = PIN_INSTANCE(P4_2),
    [P4_3] = PIN_INSTANCE(P4_3),
    [H_0] = PIN_INSTANCE(H_0),
    [H_1] = PIN_INSTANCE(H_1),
    [H_2] = PIN_INSTANCE(H_2),
};

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
        pin_p->instance_p->pad,
        pin_p->mode,
        PAD_IS_PWRON,
        pin_p->pull,
        pin_p->direction,
        pin_p->value);

    Pad_PullConfigValue(pin_p->instance_p->pad, pin_p->pull_config);

    if (SW_MODE == pin_p->mode) {
        Pinmux_Config(pin_p->instance_p->pad, IDLE_MODE);
    } else {
        GPIO_InitTypeDef Gpio_Struct;
        GPIOMode_TypeDef direction = (IN == pin_p->direction) ? GPIO_Mode_IN : GPIO_Mode_OUT;

        GPIO_StructInit(&Gpio_Struct);
        Gpio_Struct.GPIO_Pin = pin_p->instance_p->bit;
        Gpio_Struct.GPIO_Mode = direction;
        if (((GPIOMode_TypeDef)IN == direction) && (NULL != pin_p->interrupt_p)) {
            Gpio_Struct.GPIO_ITCmd = ENABLE;
            Gpio_Struct.GPIO_ITTrigger = pin_p->interrupt_p->level_type;
            Gpio_Struct.GPIO_ITPolarity = pin_p->interrupt_p->polarity;
            Gpio_Struct.GPIO_ITDebounce = pin_p->interrupt_p->debounce;
            Gpio_Struct.GPIO_DebounceTime = pin_p->interrupt_p->debounce_time_ms;
        }
        GPIO_Init(&Gpio_Struct);

        if (((GPIOMode_TypeDef)IN == direction) && (NULL != pin_p->interrupt_p)) {
            // disable interrupt
            GPIO_INTConfig(pin_p->instance_p->bit, DISABLE);

            // configure IRQ
            NVIC_InitTypeDef NVIC_InitStruct;
            NVIC_InitStruct.NVIC_IRQChannel = pin_p->instance_p->irq;
            NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStruct);

            // enable interrupt
            GPIO_MaskINTConfig(pin_p->instance_p->bit, DISABLE);
            GPIO_INTConfig(pin_p->instance_p->bit, ENABLE);
        } else if ((GPIOMode_TypeDef)OUT == direction) {
            GPIO_WriteBit(pin_p->instance_p->bit, pin_p->value);
        }

        Pinmux_Config(pin_p->instance_p->pad, DWGPIO);
    }
}

void pin_deinit(pin_t* pin_p)
{
    GPIO_MaskINTConfig(pin_p->instance_p->bit, ENABLE);
    GPIO_INTConfig(pin_p->instance_p->bit, DISABLE);

    Pad_Config(pin_p->instance_p->pad, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE,
        PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pinmux_Config(pin_p->instance_p->pad, IDLE_MODE);
}

bool pin_get(pin_t* pin_p)
{
    if (SW_MODE == pin_p->mode) {
        return pin_p->value;
    } else {
        if (IN == pin_p->direction) {
            return (SET == GPIO_ReadInputDataBit(pin_p->instance_p->bit));
        } else {
            return (SET == GPIO_ReadOutputDataBit(pin_p->instance_p->bit));
        }
    }
}

void pin_set(pin_t* pin_p, bool value)
{
    if (SW_MODE == pin_p->mode) {
        Pad_OutputControlValue(pin_p->instance_p->pad, value);
        pin_p->value = value;
    } else {
        GPIO_WriteBit(pin_p->instance_p->bit, value);
    }
}

void pins_set_interrupt_handler(pin_interrupt_handler_t interrupt_handler)
{
    pin_interrupt_handler = interrupt_handler;
}

static void _generic_pin_interrupt_handler(const uint8_t pad, const uint32_t bit) DATA_RAM_FUNCTION;
static void _generic_pin_interrupt_handler(const uint8_t pad, const uint32_t bit)
{
    uint32_t tick;
    bool state;
    /* disable interrups globally */
    __disable_irq();
    /* store relevant values */
    tick = platform_vendor_tick();
    state = GPIO_ReadInputDataBit(bit);
    /* disable interrupt in pin */
    GPIO_INTConfig(bit, DISABLE);
    GPIO_MaskINTConfig(bit, ENABLE);
    /* mark interrupt as processed */
    GPIO_ClearINTPendingBit(bit);
    /* enable interrupt for this pin again */
    GPIO_MaskINTConfig(bit, DISABLE);
    GPIO_INTConfig(bit, ENABLE);
    /* enable interrupt for this pin again */
    if (NULL != pin_interrupt_handler) {
        pin_interrupt_handler(pad, state, tick);
    }
    /* enable interrups again globally */
    __enable_irq();
}

/* attach generic handler to all possible pins */
#define GPIO_HANDLER_NAME(PAD) GPIO##PAD##_Handler
#define GPIO_HANDLER(PAD)                                   \
    void GPIO_HANDLER_NAME(PAD)(void) DATA_RAM_FUNCTION;    \
    void GPIO_HANDLER_NAME(PAD)(void)                       \
    {                                                       \
        _generic_pin_interrupt_handler(PAD, GPIO_BIT(PAD)); \
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
