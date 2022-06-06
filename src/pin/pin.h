#include <rtl876x.h>
#include <rtl876x_pinmux.h>

#ifndef _PIN_H
#define _PIN_H

#define PIN_ID(PAD)                                                       \
    {                                                                     \
        .pad = PAD,                                                       \
        .bit = ((PAD <= P3_6)                                             \
                ? BIT(PAD)                                                \
                : (((PAD <= P4_3) && (PAD >= P4_0))                       \
                        ? BIT(PAD - 4)                                    \
                        : (((PAD == H_0) || (PAD == H_1) || (PAD == H_2)) \
                                ? BIT(PAD - 11)                           \
                                : 0xFF))),                                \
    }

typedef struct {
    const uint8_t pad;
    const uint32_t bit;
} pin_id_t;

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
    PINMUX_MODE = PAD_PINMUX_MODE,
    SW_MODE = PAD_SW_MODE,
} pin_mode_t;

typedef struct {
    const pin_id_t id;
    pin_direction_t direction;
    pin_pull_t pull;
    pin_mode_t mode;
    bool value;
} pin_t;

void pins_init();
void pin_init(pin_t* pin_p);
bool pin_get(pin_t* pin_p);
void pin_set(pin_t* pin_p, bool value);

#endif //_PIN_H
