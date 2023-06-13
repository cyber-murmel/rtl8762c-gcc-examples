#include <platform_utils.h>

#include "board_config.h"
#include "i2c/i2c.h"
#include "uart/uart.h"

// prevent `warning: function with qualified void return type called`
#define delay_ms(T) (((void (*)(uint32_t))platform_delay_ms)(T))

i2c_t* i2c0 = &(i2c_t) {
    .instance_p = &I2C_INSTANCE(0),
    .pads = {
        .scl = H_2,
        .sda = H_1,
    },
    .speed = 100000,
    .mode = I2C_CONTROLLER,
    .address_mode = I2C_ADDRESS_7BIT,
    .target_address = 0x50,
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

int main(void)
{
    i2c_init(i2c0);
    uart_init(&uart);

    delay_ms(2000);

    while (1) {
        uart_print(&uart, "\ec"); // clear screen
        uart_print(&uart, "scanning i2c\r\n  ");

        for (uint16_t i = 0; i < 16; i++) {
            uart_printf(&uart, "  %x", i);
        }
        uart_print(&uart, "\r\n");

        uart_print(&uart, "0    ");
        for (uint16_t addr = 1; addr < 128; addr++) {
            if (0 == addr % 16) {
                uart_printf(&uart, "\r\n%x ", addr / 16);
            }
            if (i2c_probe(i2c0, addr)) {
                uart_printf(&uart, " %02x", addr);
            } else {
                uart_print(&uart, " --");
            }
        }
        uart_print(&uart, "\r\n");
        uart_flush(&uart);

        delay_ms(1000);

        for (int i = 0; i < 5; i++) {
            uart_print(&uart, ".");
            uart_flush(&uart);
            delay_ms(1000);
        }
    }
}
