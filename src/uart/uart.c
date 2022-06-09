/**
 * @file UART.c
 * @ingroup UARTGroup
 * @author marble
 * @date 2021-05-31
 *
 */

#include <os_msg.h>
#include <os_sync.h>
#include <rtl876x_nvic.h>
#include <rtl876x_pinmux.h>

#include "rcc/rcc.h"
#include "uart/uart.h"

#define NUM_UARTS (3)

static uart_t* uarts[NUM_UARTS];

static inline int uart_to_idx(uart_t* uart_p)
{
    switch ((size_t)(uart_p->id.register_p)) {
    case UART0_REG_BASE: {
        return 0;
    } break;
    case UART1_REG_BASE: {
        return 1;
    } break;
    case UART2_REG_BASE: {
        return 2;
    } break;
    default: {
        return -1;
    } break;
    }
}

/**
 * @brief Initialize the UART abstraction layer
 *
 * @param uart_p Reference to UART describing struct
 * @param msg_q_p Message queue handle to publish UART events to
 */
bool uart_init(uart_t* uart_p)
{
    int uart_idx = uart_to_idx(uart_p);

    if (-1 == uart_idx) {
        // invalid UART index
        return false;
    }

    uarts[uart_idx] = uart_p;

    if (true != os_mutex_create(&(uart_p->mutex_p))) {
        // the mutex was not created successfully
        return false;
    }

    Pad_Config(uart_p->tx_pad, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
        PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(uart_p->rx_pad, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
        PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pinmux_Config(uart_p->tx_pad, uart_p->id.tx_pin_function);
    Pinmux_Config(uart_p->rx_pad, uart_p->id.rx_pin_function);

    // initialize UART
    rcc_periph_set(&(uart_p->id.rcc_periph), ENABLE);

    /* uart init */
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.parity = uart_p->parity;
    UART_InitStruct.stopBits = uart_p->stop_bits;
    UART_InitStruct.wordLen = uart_p->word_length;
    UART_InitStruct.rxTriggerLevel = uart_p->rx_trigger_level;
    UART_InitStruct.idle_time = uart_p->idle_time;

    UART_Init(uart_p->id.register_p, &UART_InitStruct);

    // enable rx interrupt and line status interrupt
    UART_INTConfig(UART, UART_INT_RD_AVA, ENABLE);
    UART_INTConfig(UART, UART_INT_IDLE, ENABLE);

    /*  Enable UART IRQ  */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = uart_p->id.irq_channel;
    NVIC_InitStruct.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);

    return true;
}

void uart_flush(const uart_t* uart_p)
{
    while (UART_GetFlagState(uart_p->id.register_p,
               UART_FLAG_THR_TSR_EMPTY)
        != SET)
        ;
}

/**
 * @details Flush UART, copy data into buffer, start GDMA transmission and
 * return
 *
 * @todo find sensible mutex timeout
 *
 */
bool uart_printn(const uart_t* uart_p, const char* str, size_t vCount)
{
    size_t i = 0;
    if (!os_mutex_take(uart_p->mutex_p, 0xffffffff)) {
        return false;
    }

    uart_flush(uart_p);

    /* send block bytes(16 bytes) */
    for (i = 0; i < (vCount / UART_TX_FIFO_SIZE); i++) {
        UART_SendData(uart_p->id.register_p, (const uint8_t*)str + (UART_TX_FIFO_SIZE * i), UART_TX_FIFO_SIZE);
        /* wait tx fifo empty */
        uart_flush(uart_p);
    }

    /* send left bytes */
    UART_SendData(uart_p->id.register_p, (const uint8_t*)str + (UART_TX_FIFO_SIZE * i), vCount % UART_TX_FIFO_SIZE);

    os_mutex_give(uart_p->mutex_p);

    return true;
}

int uart_printf(const uart_t* uart_p, char* fmt, ...)
{
    int len;
    va_list argptr;

    va_start(argptr, fmt);

    len = vsnprintf(uart_p->tx_buf.data, uart_p->tx_buf.size, fmt, argptr);
    if (len > 0) {
        if (!uart_printn(uart_p, uart_p->tx_buf.data, len)) {
            return -1;
        }
    }

    va_end(argptr);

    return len;
}

static void _genericUARTHandler(int idx)
{
    uart_t* uart_p = uarts[idx];
    if (NULL == uart_p) {
        return;
    }

    UART_TypeDef* Register_p = uart_p->id.register_p;
    uint16_t rx_len = 0;
    /* diable interrups globally to prevent cascades */
    __disable_irq();
    /* Read interrupt id */
    uint32_t int_status = UART_GetIID(Register_p);
    /* Disable interrupt */
    UART_INTConfig(Register_p, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);

    if (UART_GetFlagState(Register_p, UART_FLAG_RX_IDLE) == SET) {
        /* Clear flag */
        UART_INTConfig(Register_p, UART_INT_IDLE, DISABLE);
        /* Send msg to app task */
        if (NULL != uart_p->rx_cb) {
            uart_p->rx_cb(uart_p);
        }
        /* IO_UART_DLPS_Enter_Allowed = true; */
        UART_INTConfig(Register_p, UART_INT_IDLE, ENABLE);
    }

    switch (int_status & 0x0E) {
    /* Rx time out(0x0C). */
    case UART_INT_ID_RX_TMEOUT:
        rx_len = UART_GetRxFIFOLen(Register_p);
        UART_ReceiveData(UART, (uint8_t*)&uart_p->rx_buf.data[uart_p->rx_buf.count], rx_len);
        uart_p->rx_buf.count += rx_len;
        break;
    /* Receive line status interrupt(0x06). */
    case UART_INT_ID_LINE_STATUS:
        break;
    /* Rx data valiable(0x04). */
    case UART_INT_ID_RX_LEVEL_REACH:
        rx_len = UART_GetRxFIFOLen(Register_p);
        UART_ReceiveData(UART, (uint8_t*)&uart_p->rx_buf.data[uart_p->rx_buf.count], rx_len);
        uart_p->rx_buf.count += rx_len;
        break;
    /* Tx fifo empty(0x02), not enable. */
    case UART_INT_ID_TX_EMPTY:
        /* Do nothing */
        break;
    default:
        break;
    }
    /* enable interrupt again */
    UART_INTConfig(Register_p, UART_INT_RD_AVA, ENABLE);
    /* enable interrups again globally */
    __enable_irq();
}

/* attach generic handler to all possible pins */
#define UART_HANDLER(IDX)          \
    void UART##IDX##_Handler(void) \
    {                              \
        _genericUARTHandler(IDX);  \
    }

UART_HANDLER(0)
UART_HANDLER(1)
UART_HANDLER(2)
