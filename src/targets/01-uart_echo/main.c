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
#include "uart/uart.h"

#define MAIN_TASK_PRIORITY (1)
#define MAIN_TASK_STACK_SIZE (256 * 4)
#define RX_QUEUE_LEN (256)

void* main_task_p;
void* rx_queue;

void rx_cb(uart_t* uart_p);
char rx_buf[256], tx_buf[256];

uart_t uart = {
    .instance_p = &UART_INSTANCE(0),
    .pads = {
        .tx = BOARD_TX_PAD,
        .rx = BOARD_RX_PAD,
    },
    .baudrate = 115200,
    .parity = NO_PARTY,
    .stop_bits = STOP_BITS_1,
    .word_length = WORD_LENGTH_8,
    .rx_trigger_level = 29,
    .idle_time = IDLE_TIME_2,
    .rx_buf = {
        .size = sizeof(rx_buf),
        .data = rx_buf,
    },
    .tx_buf = {
        .size = sizeof(tx_buf),
        .data = tx_buf,
    },
    .rx_cb = rx_cb,
};

void main_task(void* p_param)
{
    if (false == os_msg_queue_create(&rx_queue, RX_QUEUE_LEN, sizeof(char))) {
        // the queue was not created successfully
    }

    uart_init(&uart);

    uart_print(&uart, "main task started\r\n");

    while (1) {
        char c;
        if (os_msg_recv(rx_queue, &c, 0xFFFFFFFF) == true) {
            uart_printn(&uart, &c, 1);
        }
    }
}

void rx_cb(uart_t* uart_p)
{
    for (unsigned int i = 0; i < uart_p->rx_buf.count; i++) {
        if (false == os_msg_send(rx_queue, &uart_p->rx_buf.data[i], 0)) {
            // couldn't send to the queue
        }
    }
    uart_p->rx_buf.count = 0;
}

int main(void)
{
    os_task_create(&main_task_p, "main_task", main_task, 0, MAIN_TASK_STACK_SIZE,
        MAIN_TASK_PRIORITY);

    os_sched_start();
}
