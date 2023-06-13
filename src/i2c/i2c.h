#include <rtl876x.h>
#include <rtl876x_i2c.h>
#include <rtl876x_pinmux.h>

#include "rcc/rcc.h"

#include <stdint.h>

#ifndef _I2C_H
#define _I2C_H

typedef enum {
    I2C0_CLK_PIN = I2C0_CLK,
    I2C0_DAT_PIN = I2C0_DAT,
    I2C1_CLK_PIN = I2C1_CLK,
    I2C1_DAT_PIN = I2C1_DAT,
} i2c_pin_function_t;

#define I2C0_CLK 5
#define I2C0_DAT 6
#define I2C1_CLK 7
#define I2C1_DAT 8

typedef struct {
    const uint8_t index;
    I2C_TypeDef* register_p;
    const rcc_periph_t rcc_periph;
    const IRQn_Type irq_channel;
    const i2c_pin_function_t scl_pin_function, sda_pin_function;
} i2c_instance_t;

#define I2C_INSTANCE(INDEX)                                \
    ((i2c_instance_t) {                                    \
        .index = INDEX,                                    \
        .register_p = (I2C_TypeDef*)I2C##INDEX##_REG_BASE, \
        .rcc_periph = RCC_PERIPH(I2C##INDEX),              \
        .irq_channel = I2C##INDEX##_IRQn,                  \
        .scl_pin_function = I2C##INDEX##_CLK_PIN,          \
        .sda_pin_function = I2C##INDEX##_DAT_PIN,          \
    })

typedef struct {
    uint8_t scl;
    uint8_t sda;
} i2c_pads_t;

typedef enum {
    I2C_TARGET = I2C_DeviveMode_Slave,
    I2C_CONTROLLER = I2C_DeviveMode_Master,
} i2c_mode_t;

typedef enum {
    I2C_ADDRESS_7BIT = I2C_AddressMode_7BIT,
    I2C_ADDRESS_10BIT = I2C_AddressMode_10BIT,
} i2c_address_mode_t;

typedef enum {
    I2C_ACK_DISABLE = I2C_Ack_Disable,
    I2C_ACK_ENABLE = I2C_Ack_Enable,
} i2c_ack_t;

typedef struct {
    const i2c_instance_t* instance_p;
    // I2C_TypeDef* peripheral;
    i2c_pads_t pads;
    uint32_t speed;
    i2c_mode_t mode;
    i2c_address_mode_t address_mode;
    uint16_t target_address;
    i2c_ack_t ack;
    uint32_t rx_threshold;
} i2c_t;

typedef struct {
    size_t len;
    uint8_t* buf;
} i2c_buf_t;

void i2c_init(i2c_t* i2c_p);
void i2c_pinmux(i2c_t* i2c_p);
int i2c_transfer(i2c_t* i2c_p, uint16_t addr, size_t n, i2c_buf_t bufs[], bool write_nread, bool stop);
bool i2c_probe(i2c_t* i2c_p, uint16_t addr);

#endif //_I2C_H
