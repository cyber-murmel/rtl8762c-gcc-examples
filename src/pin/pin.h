#include <rtl876x.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>

#ifndef _PIN_H
#define _PIN_H

typedef struct {
    const uint8_t pad;
    const uint32_t bit;
    const IRQn_Type irq;
} pin_instance_t;

typedef enum {
    IN = PAD_OUT_DISABLE,
    OUT = PAD_OUT_ENABLE,
} pin_direction_t;

typedef enum {
    PULL_UP = PAD_PULL_UP,
    PULL_DOWN = PAD_PULL_DOWN,
    PULL_NONE = PAD_PULL_NONE,
} pin_pull_t;

typedef enum {
    WEAK_PULL = PAD_WEAK_PULL,
    STRONG_PULL = PAD_STRONG_PULL,
} pin_pull_config_t;

typedef enum {
    SW_MODE = PAD_SW_MODE,
    PINMUX_MODE = PAD_PINMUX_MODE,
} pin_mode_t;

typedef enum {
    LEVEL = GPIO_INT_Trigger_LEVEL,
    HIGH = GPIO_INT_POLARITY_ACTIVE_HIGH,
} pin_interrupt_level_type_t;

typedef enum {
    LOW = GPIO_INT_POLARITY_ACTIVE_LOW,
    EDGE = GPIO_INT_Trigger_EDGE,
    BOTH_EDGE = GPIO_INT_BOTH_EDGE,
} pin_interrupt_polarity_t;

typedef enum {
    DEBOUNCE_DISABLE = GPIO_INT_DEBOUNCE_DISABLE,
    DEBOUNCE_ENABLE = GPIO_INT_DEBOUNCE_ENABLE,
} pin_interrupt_debounce_t;

typedef struct {
    const IRQn_Type irq;
    pin_interrupt_level_type_t level_type;
    pin_interrupt_polarity_t polarity;
    pin_interrupt_debounce_t debounce;
    uint32_t debounce_time_ms;
} pin_interrupt_t;

typedef struct {
    const pin_instance_t* instance_p;
    pin_direction_t direction;
    pin_pull_t pull;
    pin_pull_config_t pull_config;
    pin_mode_t mode;
    bool value;
    pin_interrupt_t* interrupt_p;
} pin_t;

typedef void (*pin_interrupt_handler_t)(const uint8_t pad, bool state, uint32_t tick);

extern pin_t all_pins[];

extern const pin_instance_t pin_instances[TOTAL_PIN_NUM];

#define IRQn_hlp(PAD) GPIO##PAD##_IRQn
#define IRQn(PAD) IRQn_hlp(PAD)

#define GPIO_BIT(PAD)                                                 \
    ((PAD <= P3_6)                                                    \
            ? BIT(PAD)                                                \
            : (((PAD <= P4_3) && (PAD >= P4_0))                       \
                    ? BIT(PAD - 4)                                    \
                    : (((PAD == H_0) || (PAD == H_1) || (PAD == H_2)) \
                            ? BIT(PAD - 11)                           \
                            : 0xFF)))

#define PIN_INSTANCE(PAD)     \
    ((pin_instance_t) {       \
        .pad = PAD,           \
        .bit = GPIO_BIT(PAD), \
    })

#define PIN_INSTANCE_IRQ(PAD) \
    ((pin_instance_t) {       \
        .pad = PAD,           \
        .bit = GPIO_BIT(PAD), \
        .irq = IRQn(PAD) })

#define PIN_OUT(INSTANCE_P)       \
    ((pin_t) {                    \
        .instance_p = INSTANCE_P, \
        .mode = SW_MODE,          \
        .direction = OUT,         \
    })

#define PIN_IN(INSTANCE_P)          \
    ((pin_t) {                      \
        .instance_p = INSTANCE_P,   \
        .mode = PINMUX_MODE,        \
        .direction = IN,            \
        .pull = PULL_UP,            \
        .pull_config = STRONG_PULL, \
    })

#define PIN_IN_IRQ(INSTANCE_P)               \
    ((pin_t) {                               \
        .instance_p = INSTANCE_P,            \
        .mode = PINMUX_MODE,                 \
        .direction = IN,                     \
        .pull = PULL_UP,                     \
        .pull_config = STRONG_PULL,          \
        .interrupt_p = &((pin_interrupt_t) { \
            .level_type = EDGE,              \
            .polarity = LOW,                 \
            .debounce = DEBOUNCE_ENABLE,     \
            .debounce_time_ms = 10,          \
        }),                                  \
    })

void pins_init();
void pin_init(pin_t* pin_p);
void pin_deinit(pin_t* pin_p);
void pin_configure_output(pin_t* pin_p);
void pin_configure_input(pin_t* pin_p);
void pin_configure_input_interrupt(pin_t* pin_p);
bool pin_get(pin_t* pin_p);
void pin_set(pin_t* pin_p, bool value);
void pins_set_interrupt_handler(pin_interrupt_handler_t interrupt_handler);

#endif //_PIN_H
