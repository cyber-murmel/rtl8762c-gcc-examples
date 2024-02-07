#include <os_msg.h>
#include <os_sched.h>
#include <os_task.h>
#include <platform_utils.h>
#include <rtl876x.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_rcc.h>
#include <rtl876x_wdg.h>

#include "board_config.h"
#include "pin/pin.h"
#include "uart/uart.h"

#define MAIN_TASK_PRIORITY (1)
#define MAIN_TASK_STACK_SIZE (256 * 4)

void* main_task_p;

char tx_buf[16];

uart_t uart = {
    .instance_p = &UART_INSTANCE(0),
    .pads = {
        .tx = -1,
        .rx = -1,
    },
    .baudrate = 115200,
    .parity = NO_PARTY,
    .stop_bits = STOP_BITS_1,
    .word_length = WORD_LENGTH_8,
    .tx_buf = {
        .size = sizeof(tx_buf),
        .data = tx_buf,
    },
};
// prevent `warning: function with qualified void return type called`
#define delay_ms(T) (((void (*)(uint32_t))platform_delay_ms)(T))
void main_task(void* p_param)
{
    uart_init(&uart);
    while (1) {
        for (int idx = 0; idx < (sizeof(pin_instances) / sizeof(pin_instances[0])); idx++) {
            pin_t pin = {
                .instance_p = &pin_instances[idx],
            };
            uart.pads.tx = pin.instance_p->pad;

            uart_pinmux(&uart);

            uart_printf(&uart, "%d\r\n", pin.instance_p->pad);
            uart_flush(&uart);

            pin_deinit(&pin);

            os_delay(10);
        }
    }
}

int main(void)
{
    os_task_create(&main_task_p, "main_task", main_task, 0, MAIN_TASK_STACK_SIZE,
        MAIN_TASK_PRIORITY);

    os_sched_start();
}
