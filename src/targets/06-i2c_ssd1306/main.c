#include <platform_utils.h>
#include <rtl876x_i2c.h>

#include "board_config.h"
#include "i2c/i2c.h"
#include "uart/uart.h"

#include "ssd1306.h"

// prevent `warning: function with qualified void return type called`
#define delay_ms(T) (((void (*)(uint32_t))platform_delay_ms)(T))

i2c_t* i2c = &(i2c_t) {
    .instance_p = &I2C_INSTANCE(0),
    .pads = {
        .scl = H_2,
        .sda = H_1,
    },
    .speed = 100000,
    .mode = I2C_CONTROLLER,
    .address_mode = I2C_ADDRESS_7BIT,
    .target_address = SSD1306_ADDR,
    .ack = I2C_ACK_ENABLE,
    .rx_threshold = 12,
};

char tx_buf[256];

uart_t uart = {
    .instance_p = &UART_INSTANCE(0),
    .pads = {
        .tx = BOARD_TX_PAD,
        .rx = -1,
    },
    .parity = NO_PARTY,
    .stop_bits = STOP_BITS_1,
    .word_length = WORD_LENGTH_8,
    .tx_buf = {
        .size = sizeof(tx_buf),
        .data = tx_buf,
    },
};

typedef struct __attribute__((__packed__)) {
    unsigned int rsvd: 6;
    unsigned int data_ncommand: 1;
    unsigned int co: 1;
} control_byte_t;

struct __attribute__((__packed__)) {
    const control_byte_t control_byte;
    struct __attribute__((__packed__)) {
        const uint8_t cmd;
        uint8_t start_addr;
        uint8_t end_addr;
    } page, column;
} addr_set_sequence = {
    .page = {
        .cmd = PAGEADDR,
        .start_addr = 0,
        .end_addr = 64-1,
    },
    .column = {
        .cmd = COLUMNADDR,
        .start_addr = 0,
        .end_addr = 128-1,
    },
};

struct __attribute__((__packed__)) {
    const control_byte_t control_byte;
    uint8_t data[128 * 64 / 8];
} pix_buf_sequence = {
    .control_byte.data_ncommand = 1,
    .data = {
        0b00000000,
        0b00100000,
        0b01001100,
        0b10001100,
        0b10000000,
        0b10001100,
        0b01001100,
        0b00100000,
    },
};

uint8_t init_sequence[] = {
    0x00, // control byte

    DISPLAYOFF,
    SETDISPLAYCLOCKDIV,
    0x80,
    SETMULTIPLEX,
    64 - 1,
    SETDISPLAYOFFSET,
    0,
    SETSTARTLINE | 0b0,
    CHARGEPUMP,
    0x14, // generate from 3.3V
    MEMORYMODE,
    0,
    SEGREMAP | 0b1,
    COMSCANDEC,
    SETCOMPINS,
    0x12,
    SETCONTRAST,
    0xCF,
    SETPRECHARGE,
    0xF1,
    SETVCOMDETECT,
    0x40,
    DISPLAYALLON_RESUME,
    NORMALDISPLAY,
    DEACTIVATE_SCROLL,
    DISPLAYON,
};

int main(void)
{
    i2c_init(i2c);
    uart_init(&uart);

    I2C_SetSlaveAddress(i2c->instance_p->register_p, SSD1306_ADDR);

    I2C_MasterWrite(i2c->instance_p->register_p, init_sequence, sizeof(init_sequence));
    I2C_MasterWrite(i2c->instance_p->register_p, (uint8_t *) &addr_set_sequence, sizeof(addr_set_sequence));

    I2C_MasterWrite(i2c->instance_p->register_p, (uint8_t *) &pix_buf_sequence, sizeof(pix_buf_sequence));

    while (1) {
    }
}
