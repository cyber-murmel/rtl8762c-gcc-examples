#include <platform_utils.h>
#include <rtl876x.h>
#include <rtl876x_i2c.h>
#include <rtl876x_pinmux.h>

#include "i2c/i2c.h"
#include "rcc/rcc.h"

void i2c_init(i2c_t* i2c_p)
{
    i2c_pinmux(i2c_p);

    rcc_periph_set(&(i2c_p->instance_p->rcc_periph), ENABLE);

    I2C_InitTypeDef I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.I2C_ClockSpeed = i2c_p->speed;
    I2C_InitStruct.I2C_DeviveMode = i2c_p->mode;
    I2C_InitStruct.I2C_AddressMode = i2c_p->address_mode;
    I2C_InitStruct.I2C_SlaveAddress = i2c_p->target_address;
    I2C_InitStruct.I2C_Ack = i2c_p->ack;
    I2C_InitStruct.I2C_RxThresholdLevel = i2c_p->rx_threshold;

    I2C_Init(i2c_p->instance_p->register_p, &I2C_InitStruct);
    I2C_Cmd(i2c_p->instance_p->register_p, ENABLE);
}

void i2c_pinmux(i2c_t* i2c_p)
{
    Pad_Config(i2c_p->pads.scl, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    // Pad_PullConfigValue(i2c_p->pads.scl, PAD_STRONG_PULL);
    Pinmux_Config(i2c_p->pads.scl, i2c_p->instance_p->scl_pin_function);

    Pad_Config(i2c_p->pads.sda, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    // Pad_PullConfigValue(i2c_p->pads.sda, PAD_STRONG_PULL);
    Pinmux_Config(i2c_p->pads.sda, i2c_p->instance_p->sda_pin_function);
}

// int i2c_transfer(i2c_t* i2c_p, uint16_t addr, size_t n, i2c_buf_t bufs[], bool write_nread, bool stop)
// {
//     I2C_SetSlaveAddress(i2c_p->instance_p->register_p, addr);

//     for (i2c_buf_t* buf_p = bufs; buf_p < (bufs + n); buf_p++) {
//     }
// }

bool i2c_probe(i2c_t* i2c_p, uint16_t addr)
{
    uint8_t data = 0;
    I2C_SetSlaveAddress(i2c_p->instance_p->register_p, addr);
    return (I2C_Success == I2C_MasterRead(i2c_p->instance_p->register_p, &data, 1));
}
